#include <LiquidCrystal.h>
#include <RH_ASK.h>
#include <SPI.h> 
#include <LedControl.h>

// led matrix
const uint8_t DIN = 8;
const uint8_t CS = 9;
const uint8_t CLK = 10;
LedControl lc = LedControl(DIN,CLK,CS,0);
 
// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

// display
const uint8_t RS = 2;
const uint8_t E = 3;
const uint8_t D4 = 4;
const uint8_t D5 = 5;
const uint8_t D6 = 6;
const uint8_t D7 = 7;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
 
void setup()
{

 lcd.begin(16, 2);
 
 // Setup led mat.
 lc.shutdown(0,false);     
 lc.setIntensity(0,15);       
 lc.clearDisplay(0);
 

 rf_driver.init();

}
uint8_t buf[8];
uint8_t buflen = sizeof(buf);
uint8_t i = 0;
void loop()
{
    // Set buffer to size of expected message
    // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen))
    {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Verifying...");
      delay(1000);
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Message");
      lcd.setCursor(4,1);
      lcd.print("Received");
      
      for(i=0; i < 8; i++)
         lc.setColumn(0,i,buf[i]);      
    }
}
