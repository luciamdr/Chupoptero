#define espera 1000

#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4


//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK); //define screen as an object

Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//ints
int PWM = 0;
int potentiometer = 0;
int sensor = 0;
//int light = LOW;


//floats
float voltageSensor = 0;
float pressure = 0;
float start = 0;
float timePassed = 0;
float previous = 0;

//bool
bool pulsado = false;
// these are a required variables for the graphing functions
bool Redraw1 = true;
bool Redraw2 = true;
bool Redraw3 = true;
bool Redraw4 = true;
double ox , oy ;

char buff[50];
char buff2[50];

byte count;
byte sensorArray[128];
byte drawHeight;

/*following functions controls scrolling direction (left/right) and drawing mode (dot/filled)
  These commands are NOT case sensitive, code understands in both capitals and non-capitals of these commands to make it more user friendly.
*/

char filled = 'F'; //decide either filled or dot display (D=dot, any else filled)
char drawDirection = 'L'; //decide drawing direction, from right or from left (L=from left to right, any else from right to left)
char slope = 'W'; //slope colour of filled mode white or black slope (W=white, any else black. Well, white is blue in this dispay but you get the point)


void setup() {

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Initialize the display. Address 0x3C (0x7A)
  
  display.display();
  //delay(2000);
  display.clearDisplay();
  display.display();
  for (count = 0; count <= 128; count++) //Set all elements to zero
  {
    sensorArray[count] = 0;
  }

  
  pinMode(5,OUTPUT); //Set D5 to be an output to switch on/off the motor through the transistor
  pinMode(2,INPUT); //Set D2 to be the input from the push button
  //pinMode(4,OUTPUT); //Set pin D4 to be the output to switch on/off the heat/light through the transistor


     

  delay(1000);
}


void loop() {
  
  if(pulsador()) //If pulsador returns true, motor function is executed
  {
    motor();
  }

}

bool pulsador ()
{

    if(digitalRead(2) == HIGH)
    {
      delay(250);
      return true;
    } else {
      return false;
    }
        
}

void motor () {

 
 //Potentiometer read sets value for the PWM function
 potentiometer = analogRead(A1);
 PWM = (potentiometer*0.21)+40; //pulse width modulation to get sth below 5v (get average between pulses)

 //Motor on with the value from the PWM trhoug D5-transistor
 analogWrite(5,PWM); 
 delay(200);

  //Light to provide heat
 // analogWrite(4,HIGH);
  

 //Counter
 start = millis();
 while (digitalRead(2)!= HIGH)
 {
    previous = timePassed;
    timePassed = round((millis()-start)/1000);
    
    if (previous != start)
    {
      
      //Sensor Read
      sensor = analogRead(A0);
      voltageSensor = sensor*(5.1/1024);
      pressure = ((voltageSensor/5.1)+0.095)/0.009; //From sensor datasheet

  drawHeight = 150; //Pressure max //map(pressure, 0, 1023, 0, 32 )
  sensorArray[0] = drawHeight;

  for (count = 1; count <= 80; count++ )
  {
    if (filled == 'D' || filled == 'd')
    {
      if (drawDirection == 'L' || drawDirection == 'l')
      {
        display.drawPixel(count, 32 - sensorArray[count - 1], WHITE);
      }
      else //else, draw dots from right to left
      {
        display.drawPixel(80 - count, 32 - sensorArray[count - 1], WHITE);
      }
    }


    

    else
    {
      if (drawDirection == 'L' || drawDirection == 'l')
      {
        if (slope == 'W' || slope == 'w')
        {
          display.drawLine(count, 32, count, 32 - sensorArray[count - 1], WHITE);
        }
        else
        {
          display.drawLine(count, 1, count, 32 - sensorArray[count - 1], WHITE);

        }
      }



      else
      {
        if (slope == 'W' || slope == 'w')
        {
          display.drawLine(80 - count, 32, 80 - count, 32 - sensorArray[count - 1], WHITE);
        }
        else
        {
          display.drawLine(80 - count, 1, 80 - count, 32 - sensorArray[count - 1], WHITE);
        }
      }
    }
  }

  drawAxises();
  display.display();  //needed before anything is displayed
  display.clearDisplay(); //clear before new drawing

  for (count = 80; count >= 2; count--) // count down from 160 to 2
  {
    sensorArray[count - 1] = sensorArray[count - 2];
  }


    } 
 }
analogWrite (5,0);
timePassed = 0;
start = 0;
delay(250);
}

void drawAxises()  //separate to keep stuff neater. This controls ONLY drawing background!
{
  display.setCursor(90, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(pressure);
  display.setCursor(90, 8);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("kPa");


  display.drawLine(0, 0, 0, 32, WHITE);
  display.drawLine(80, 0, 80, 32, WHITE);

  for (count = 0; count < 40; count += 10)
  {
    display.drawLine(80, count, 75, count, WHITE);
    display.drawLine(0, count, 5, count, WHITE);
  }

  for (count = 10; count < 80; count += 10)
  {
    display.drawPixel(count, 0 , WHITE);
    display.drawPixel(count, 31 , WHITE);
  }
}
