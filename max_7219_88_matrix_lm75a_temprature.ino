#include  <SPI.h>
#include <Wire.h>
#define CS 10

// LM75A address  
#define DS1631_ADDRESS  0x4B

// MAX7219 Control registers
#define DECODE_MODE  9 
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B 
#define SHUTDOWN 0x0C
#define  DISPLAY_TEST 0x0F
#define NUM_LEN 10 

byte leter [][8]={
                {
                B11100000,
                B10100000,
                B10100000,
                B10100000,
                B11100000,
                0x00, 0x00}, //0 
                {
                B11000000,
                B01000000,
                B01000000,
                B01000000,
                B11100000,
                0x00, 0x00}, //1 

                {
                B11100000,
                B00100000,
                B11100000,
                B10000000,
                B11100000,
                0x00, 0x00}, //2 

                {
                B11100000,
                B00100000,
                B11100000,
                B00100000,
                B11100000,
                0x00, 0x00}, //3

                {
                B10100000,
                B10100000,
                B11100000,
                B00100000,
                B00100000,
                0x00, 0x00}, //4 

                {
                B11100000,
                B10000000,
                B11100000,
                B00100000,
                B11100000,
                0x00, 0x00}, //5

                {
                B11100000,
                B10000000,
                B11100000,
                B10100000,
                B11100000,
                0x00, 0x00}, //6

                {
                B11100000,
                B00100000,
                B00100000,
                B00100000,
                B00100000,
                0x00, 0x00}, //7

                {
                B11100000,
                B10100000,
                B11100000,
                B10100000,
                B11100000,
                0x00, 0x00}, //8

                {
                B11100000,
                B10100000,
                B11100000,
                B00100000,
                B11100000,
                0x00, 0x00}, //9

                };

void  SendData(uint8_t address, uint8_t value) {  
  digitalWrite(CS, LOW);   
  SPI.transfer(address);      // Send address.
  SPI.transfer(value);        //  Send the value.
  digitalWrite(CS, HIGH); // Finish transfer.
}


void setup()  {

  Serial.begin(115200);
  // Serial.print("MOSI Pin: ");
  // Serial.println(MOSI);
  // Serial.print("MISO Pin: ");
  // Serial.println(MISO);
  // Serial.print("SCK Pin: ");
  // Serial.println(SCK);
  // Serial.print("SS Pin: ");
  // Serial.println(SS);  


  pinMode(CS, OUTPUT);  
  SPI.setBitOrder(MSBFIRST);   // Most significant  bit first 
  SPI.begin();                 // Start SPI
  SendData(DISPLAY_TEST,  0x01);       // Run test - All LED segments lit.
  delay(1000);
  SendData(DISPLAY_TEST,  0x00);           // Finish test mode.
  SendData(DECODE_MODE, 0x00);             // Disable BCD mode. 
  SendData(INTENSITY, 0x02);               // Use intensity.  
  SendData(SCAN_LIMIT, 0x0f);              // Scan all digits.
  SendData(SHUTDOWN,  0x01);               // Turn on chip.
}

void loop()  {
  int int_temp;
  Wire.beginTransmission(DS1631_ADDRESS);
  Wire.write(0xAC); // Access Configuration Register
  Wire.endTransmission();
  Wire.requestFrom(DS1631_ADDRESS, 1);
  uint8_t config = Wire.read();

  // Write the configuration byte to registers 0x51 and 0x22
  Wire.beginTransmission(DS1631_ADDRESS);
  Wire.write(0x51); // Configuration Register 1
  Wire.write(config);
  Wire.endTransmission();
  delay(10); // Delay for writing

  Wire.beginTransmission(DS1631_ADDRESS);
  Wire.write(0x22); // Configuration Register 2
  Wire.write(config);
  Wire.endTransmission();
  delay(10); // Delay for writing

  // Read the raw temperature data (2 bytes)
  Wire.beginTransmission(DS1631_ADDRESS);
  Wire.write(0xAA); // Start temperature conversion and read
  Wire.endTransmission();
  delay(10);
  
  Wire.requestFrom(DS1631_ADDRESS, 2);
  uint8_t raw_temp[2];
  if (Wire.available() >= 2) {
    raw_temp[0] = Wire.read();
    raw_temp[1] = Wire.read();
  }
  
  int_temp = raw_temp[0];
  // Calculate temperature in Celsius
  int16_t t_msb = (raw_temp[0] >= 0x80) ? (raw_temp[0] - 255) : raw_temp[0];
  float temperature = t_msb + (raw_temp[1] >> 4) * 0.0625;
  
  // Print temperature
  // Serial.print("Temperature ");
  // Serial.print(temperature);
  // Serial.println(" °C");

  // int decilams = (temperature - int_temp) * 100;
  // Serial.println(decilams);
  
 
  for (int i=1;i<9;i++){      
      int tensDigit = int_temp / 10;
      int onesDigit = int_temp % 10;
      byte line = leter[tensDigit][i-1]  | (leter[onesDigit][i-1] >> 4) ;
      SendData(i, line);
      // SendData(7, (decilams / 10) );
      // SendData(8, (decilams % 10) );
  }

//Blink 
SendData(8, 1);
delay(499);
SendData(8, 0);
delay(499);

}