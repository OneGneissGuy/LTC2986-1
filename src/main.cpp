#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"

#include "LTC2986-1_configuration_constants.h"
#include "LTC2986-1_support_functions.h"
#include "LTC2986-1_table_coeffs.h"

//#define CHIP_SELECT 10//CS is DIGITAL PIN 10 on LINDUINO/ARDUINO UNO

uint8_t CHIP_SELECT = 10; //CS is DIGITAL PIN 10 on LINDUINO/ARDUINO UNO
// Function prototypes
void configure_channels();
void configure_global_parameters();

void setup()
{
  // put your setup code here, to run once:
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  pinMode(CHIP_SELECT, OUTPUT); // Configure chip select pin on Linduino

  Serial.begin(115200); // Initialize the serial port to the PC

  print_title();
//Uncomment discover_dem_board if using Linduino DC2608
  // Serial.println("Discovering board LTC2986-1");
  // char demo_name[] = "DC2508"; // Demo Board Name stored in QuikEval EEPROM
  // discover_demo_board(demo_name);
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
      SENSOR_TYPE__TYPE_E_THERMOCOUPLE |
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
  //Sample the LTC 2986-1 continuously
  measure_channel(CHIP_SELECT, 1, TEMPERATURE); // Ch 1: Type E Thermocouple
  //measure_channel(CHIP_SELECT, 2, TEMPERATURE); // Ch 2: Off-Chip Diode
  delay(200);
}