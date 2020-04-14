#include <Arduino.h>
#include <stdint.h>
#include "LT_SPI.h"
#include <SPI.h>
#include "LTC2986-1_configuration_constants.h"
#include "LTC2986-1_support_functions.h"
#include "LTC2986-1_table_coeffs.h"

uint8_t CHIP_SELECT = 10;
long previousMillis = 0; // will store last time LED was updated
long elapsedMillis = 0;

// Function prototypes
void configure_channels();
void configure_global_parameters();

void setup()
{
  spi_enable(SPI_CLOCK_DIV16);

  Serial.begin(115200); // Initialize the serial port to the PC

  Serial.println("configuring LTC2986-1");
  Serial.println("configuring channels");
  configure_channels();
  Serial.println("configuring global params");
  configure_global_parameters();
  Serial.println("Begin main loop...");
}

void configure_channels()
{
  uint32_t channel_assignment_data;

  // ----- Channel 1: Assign Type E Thermocouple -----
  channel_assignment_data =
      SENSOR_TYPE__TYPE_K_THERMOCOUPLE |
      TC_COLD_JUNCTION_CH__2 |
      TC_SINGLE_ENDED |
      TC_OPEN_CKT_DETECT__NO |
      TC_OPEN_CKT_DETECT_CURRENT__10UA;
  assign_channel(CHIP_SELECT, 1, channel_assignment_data);
  // ----- Channel 2: Assign Off-Chip Diode to Read Cold Junction-----
  channel_assignment_data =
      SENSOR_TYPE__OFF_CHIP_DIODE |
      DIODE_SINGLE_ENDED |
      DIODE_NUM_READINGS__3 |
      DIODE_AVERAGING_OFF |
      DIODE_CURRENT__20UA_80UA_160UA |
      (uint32_t)0x100C49 << DIODE_IDEALITY_FACTOR_LSB; // diode - ideality factor(eta): 1.00299930572509765625
  assign_channel(CHIP_SELECT, 2, channel_assignment_data);
}
void configure_global_parameters()
{
  // -- Set global parameters
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xF0, TEMP_UNIT__C | REJECTION__50_60_HZ);
  // -- Set any extra delay between conversions (in this case, 0*100us)
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xFF, 0);
}

void loop()
{

  unsigned long currentMillis = millis();
  elapsedMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  //Sample the LTC 2986-1 continuously
  measure_channel(CHIP_SELECT, 1, TEMPERATURE); // Ch 1: Type K Thermocouple
  measure_channel(CHIP_SELECT, 2, TEMPERATURE); // Ch 2: Off-Chip Diode
                                                //delay(200);
  Serial.print("Time elapsed for measurement (ms) = ");
  Serial.println(elapsedMillis);
  
}