/*
  SparkFun RTK Control Test Sketch

  License: MIT. Please see LICENSE.md for more details

  ESP32-WROVER-IE Pin Allocations:
  D0  : Boot + Boot Button
  D1  : Serial TX (CH340 RX)
  D2  : STAT LED
  D3  : Serial RX (CH340 TX)
  D4  : SD CS
  D5  : Unused - via 74HC4066 switch
  D12 : SDA2 - Qwiic OLED - via 74HC4066 switch
  D13 : Serial1 TX - LARA_TXDI
  D14 : Serial1 RX - LARA RXDO
  D15 : SCL2 - Qwiic OLED - via 74HC4066 switch
  D16 : N/C
  D17 : N/C
  D18 : SPI SCK
  D19 : SPI POCI
  D21 : I2C SDA
  D22 : I2C SCL
  D23 : SPI PICO
  D25 : GNSS Time Pulse
  D26 : LARA Power On
  D27 : Ethernet Chip Select
  D32 : PWREN
  D33 : Ethernet Interrupt
  A34 : LARA Network Indicator
  A35 : Board Detect (3.0V)
  A36 : SD Card Detect
*/

#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3

SFE_UBLOX_GNSS_SUPER theGNSS;

const int SD_CS = 4; // Chip select for the microSD card
const int ETHERNET_CS = 27; // Chip select for the WizNet 5500
const int PWREN = 32; // 3V3_SW and SDIO Enable
const int STAT_LED = 2;
const int SERIAL1_TX = 13;
const int SERIAL1_RX = 14;
const int SCL_2 = 15;
const int SDA_2 = 12;
const int ETHERNET_INT = 33;
const int GNSS_INT = 25;

void setup()
{
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(ETHERNET_CS, OUTPUT);
  digitalWrite(ETHERNET_CS, HIGH);
  pinMode(PWREN, OUTPUT);
  digitalWrite(PWREN, HIGH);

  pinMode(STAT_LED, OUTPUT);

  delay(1000);

  Serial.begin(115200);
  Serial.println("SparkFun RTK - Test Sketch");

  //theGNSS.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  while (!theGNSS.begin(Wire)) // Start the GNSS on Wire
  {
    Serial.println("GNSS not detected. Retrying...");
  }

  theGNSS.setI2COutput(COM_TYPE_UBX); //Set the SPI port to output UBX only (turn off NMEA noise)

  theGNSS.newCfgValset(VAL_LAYER_RAM); // Create a new Configuration Interface VALSET message. Apply the changes in RAM only (not BBR).

  theGNSS.addCfgValset(UBLOX_CFG_HW_ANT_CFG_SHORTDET, 1); // Enable antenna short detection
  theGNSS.addCfgValset(UBLOX_CFG_HW_ANT_CFG_OPENDET, 1); // Enable antenna open detection

  if (theGNSS.sendCfgValset() == false)
  {
    Serial.println(F("VALSET failed!"));
  }
}

void loop()
{
  static UBX_MON_HW_data_t hwStatus;

  if (theGNSS.getHWstatus(&hwStatus)) // Read the hardware status
  {
    Serial.print(F("Antenna status: "));
    switch (hwStatus.aStatus)
    {
      case (0):
        Serial.println(F("INIT"));
        break;
      case (1):
        Serial.println(F("DON'T KNOW"));
        break;
      case (2):
        Serial.println(F("OK"));
        break;
      case (3):
        Serial.println(F("SHORT"));
        break;
      case (4):
        Serial.println(F("OPEN"));
        break;
    }
  }
}