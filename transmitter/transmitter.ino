#include <RH_ASK.h>
#include <LedControl.h>
#include <SPI.h> 

// radio transmitter
RH_ASK driver;
bool flag = 0;

// led matrix
// note: use .setColumn() when setting rows because 
// of the physical orientation of the matrix in this project
const uint8_t DIN = 8;
const uint8_t CS = 10;
const uint8_t CLK = 9;
LedControl lc = LedControl(DIN,CLK,CS,0);
uint8_t image[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // just for display
uint8_t image_no_cursor[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // to send over radio

// joystick
const uint8_t SW = 2; // SW
const uint8_t X = A1; // VRx
const uint8_t Y = A0; // VRy
uint8_t posx=3,
        posy=4;
uint16_t last_switch_time=0;

// send matrix button
const uint8_t btn = 3;
uint16_t last_btn_time=0;

void inline displayImage()
{
  for(int i = 0; i < 8; i++)
  {
      // displaying the cursor + the drawn image
      uint8_t var = image[i];
      var |=image_no_cursor[i];
      lc.setColumn(0,i, var);
  }
}


void setup() {

  // led matrix setup
  lc.shutdown(0,false);     
  lc.setIntensity(0,15);       
  lc.clearDisplay(0);
  image[posx] |= (1<<(8-posy-1)); //counting colums from left to right, from 0
  displayImage();
  
  // joystick setup
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW), toggleISR, FALLING);

  // send matrix button
  pinMode(btn, INPUT);
  attachInterrupt(digitalPinToInterrupt(btn), sendISR, FALLING);

 
  //Serial.begin(9600);


  driver.init();

}


void toggleISR(){
  int time_now = millis();
  if(time_now - last_btn_time > 250)
  {
    //Serial.println("-----Interrupt-----TOGGLE-LED------");
    image_no_cursor[posx] ^= (1<<(8-posy-1));
    last_btn_time = time_now;
  }
}

void sendISR(){
   int time_now = millis();
   if(time_now - last_switch_time > 250)
   {
     //Serial.println("-----Interrupt-----SEND----------");
     flag = 1;
     last_switch_time = time_now;
   }
}


void loop() {
  uint_fast16_t valX = analogRead(X), 
                valY = analogRead(Y);
  //Serial.println(valX);
  //Serial.println(valY);
  // up
  if(valY < 20 && (valX > 450 && valX<600))
  {
    // check for boundaries
    if(posx < 7){
      image[posx] &= ~(1<<(8-posy-1));
      ++posx;
      image[posx] |= (1<<(8-posy-1));
    }
    else{
      image[posx] &= ~(1<<(8-posy-1));
      posx = 0;
      image[posx] |= (1<<(8-posy-1));
    }
    //Serial.print("Position X: ");
    //Serial.println(posx);
  }
  // right
  if(valY > 1000 && (valX > 450 && valX<600))
  {
      if(posx > 0)
      {
       image[posx] &= ~(1<<(8-posy-1));
       --posx;
       image[posx] |= (1<<(8-posy-1));
      }
      else
      {
       image[posx] &= ~(1<<(8-posy-1));
       posx = 7;
       image[posx] |= (1<<(8-posy-1));
      }
      //Serial.print("Position X: ");
      //Serial.println(posx);
  }
  //down
  if(valX < 20 && (valY > 450 && valY<600))
  {
    // check for boundaries
    if(posy > 0){
      image[posx] &= ~(1<<(8-posy-1));
      --posy;
      image[posx] |= (1<<(8-posy-1));
    }
    else{
      image[posx] &= ~(1<<(8-posy-1));
      posy = 7;
      image[posx] |= (1<<(8-posy-1));
    }
    //Serial.print("Position Y: ");
    //Serial.println(posy);
  }
  //left
  if(valX >1000 && (valY > 450 && valY<600))
  {
    // check for boundaries
    if(posy < 7){
      image[posx] &= ~(1<<(8-posy-1));
      ++posy;
      image[posx] |= (1<<(8-posy-1));
    }
    else{
      image[posx] &= ~(1<<(8-posy-1));
      posy = 0;
      image[posx] |= (1<<(8-posy-1));
    }
    //Serial.print("Position Y: ");
    //Serial.println(posy);
  }
  if(flag)
  {
      const uint8_t* c = image_no_cursor;
     driver.send(c, 8);
     driver.waitPacketSent();
     for(int i = 0; i < 8; i++) image_no_cursor[i] = 0x00;
     flag = 0;
  }


  displayImage();
}
