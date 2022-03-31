#include <Wire.h>
 
#define LEDPIN D3
#define DISPLAY_ADDRESS1 0x72 //This is the default address of the OpenLCD

const int sampleWindow = 50; // Sample window width in mS 
unsigned int audioSignal;

int cycles = 0;
 
void setup() 
{
  Wire.begin(); //Join the bus as master

  //By default .begin() will set I2C SCL to Standard Speed mode of 100kHz
  //Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz

  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);

  //Send the reset command to the display - this forces the cursor to return to the beginning of the display
  Wire.beginTransmission(DISPLAY_ADDRESS1);
  Wire.write('|'); //Put LCD into setting mode
  Wire.write('-'); //Send clear display command
  Wire.endTransmission();
}
 

void loop() 
{
  i2cSendValue("Percolator!!!"); //Send the four characters to the display
  delay(50); //The maximum update rate of OpenLCD is about 100Hz (10ms). A smaller delay will cause flicker
  i2cSendValue(" ");
  
  unsigned long start= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  
  // collect data for 50 miliseconds
  while (millis() - start < sampleWindow)
  {
   audioSignal = analogRead(0);
      if (audioSignal < 1024)  //This is the max of the 10-bit ADC so this loop will include all readings
      {
         if (audioSignal > signalMax)
         {
           signalMax = audioSignal;  // save just the max levels
         }
      else if (audioSignal < signalMin)
        {
         signalMin = audioSignal;  // save just the min levels
         }
     }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
  
  int ledPower;          // Voltage supplied to LED
  ledPower = volts * 100;
  Serial.println(ledPower);

  if (volts >=0.28)    // if voltage is above a certain threshold
  {
  //turn on LED
    digitalWrite(LEDPIN, ledPower);
  }
  else
  {
    //turn LED off
    digitalWrite(LEDPIN, LOW);
  }
}

//Given a number, i2cSendValue chops up an integer into four values and sends them out over I2C
void i2cSendValue(char* value)
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1

  Wire.write('|'); //Put LCD into setting mode
  Wire.write('-'); //Send clear display command

//  Wire.print("Percolator!!!");
  Wire.print(value);

  Wire.endTransmission(); //Stop I2C transmission
}
