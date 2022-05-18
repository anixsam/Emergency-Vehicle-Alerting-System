#include <RH_ASK.h>
#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LedControl.h>

// Traffic leds connects from 22 to 24
// OLED Display => VCC to 5v , GND to GND , SDA to 20 , SCK to 21
// RF Reciever => VCC to 5v , GND to GND , Data to 11


/* --------------------OLED Init------------------------- */
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/*--------------------------------------------------------*/


/* --------------------LED MATRIX INIT------------------------- */
const int DIN_PIN = 4; //DIN TO 4
const int CS_PIN = 2; // CS TO 2
const int CLK_PIN = 3;// CLK TO 3

const uint8_t IMAGES[][8] = {
{
  0b01111110,
  0b11000001,
  0b10100001,
  0b10010001,
  0b10001001,
  0b10000101,
  0b10000011,
  0b01111110
},
{
  0b11111111,
  0b10000001,
  0b10000001,
  0b10011001,
  0b10011001,
  0b10000001,
  0b10000001,
  0b11111111
}};

const int IMAGES_LEN = sizeof(IMAGES)/8;

LedControl matDisplay = LedControl(DIN_PIN, CLK_PIN, CS_PIN);
/*--------------------------------------------------------*/
RH_ASK rf_driver;


int Signal[4][3] = {
  {22 ,23 ,24 },
  {25 ,26 ,27 },
  {28 ,29 ,30},
  {31,32,33}
};

int redDelay = 2000; //Delay for Red
int yellowDelay = 500; //Delay for yellow

char *rec_signal;  //Signal Recieved

int i=0;

void setup() 
{
  rf_driver.init();
  
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } //OLED Initialisation

  scrolltext("Emergency Vehicle Alerting System");
  delay(500); // Display Initialisation Delay

  display.clearDisplay();

  matDisplay.clearDisplay(0);
  matDisplay.shutdown(0, false);
  matDisplay.setIntensity(0, 5);
  
  for (int i = 0; i < 3; i++) 
  {
    pinMode(Signal[0][i],OUTPUT);
    pinMode(Signal[1][i],OUTPUT);
    pinMode(Signal[2][i],OUTPUT);
    pinMode(Signal[3][i],OUTPUT);
  }
}



void loop() {

  uint8_t buf[1];
    uint8_t buflen = sizeof(buf);
    // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen))
    {
      rec_signal = (char*)buf;
      // Message received with valid checksum
      Serial.print("Message Received: ");
      Serial.println(rec_signal[0]);         
    }

    if(rec_signal[0] == 'l' || rec_signal[0] == 'r' || rec_signal[0] == 's' || rec_signal[0] == 'v')
    {
      digitalWrite(Signal[0][0],HIGH);
      digitalWrite(Signal[1][0],HIGH);
      digitalWrite(Signal[2][0],HIGH);
      digitalWrite(Signal[3][0],HIGH);
      scrolltext("Emergency Vehicle"); 
      showDisplay(IMAGES[0]); 
    }
    else
    {
      normalTraffic();
      scrolltext("Car Infotainment");
      showDisplay(IMAGES[1]); 
    }
  
}

void normalTraffic()
{

  if(i==4)
    i=0;
  
  for(int j=0;j<4;j++)
  {
    if(i==j)
    {
      digitalWrite(Signal[j][2],HIGH); 
    }
     digitalWrite(Signal[j][0],HIGH);
  }
  
  digitalWrite(Signal[i][0],LOW);
  
  delay(redDelay);
  
 
  digitalWrite(Signal[i][1],HIGH);
  digitalWrite(Signal[i][2],LOW);
  
  delay(yellowDelay);
  
  digitalWrite(Signal[i][1],LOW);
    
  i++;
  
}


void scrolltext(char* text)
{
  display.clearDisplay();

  display.setTextSize(1.9); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.display();      // Show initial text
  delay(500);

  // Scroll Right
  display.startscrollright(0x00, 0x0F);
  delay(500);
  display.stopscroll();
}


//Function to display the matrix

void showDisplay(const byte* image)
{
 for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      matDisplay.setLed(0, i, j, bitRead(image[i], 7 - j));
    }
  }
  delay(200); //Delay to clear the output
 matDisplay.clearDisplay(0);
}
