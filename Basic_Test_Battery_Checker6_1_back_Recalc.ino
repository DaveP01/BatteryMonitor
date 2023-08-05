//////////////////////////////////////////////////////////////////////////
// Lipo battery checker by D.Paton. 2016 Oled library by Hobby_Tronics,	 //
// based on the Sparkfun Microview code.							    //
// Lipo Checker will display voltages and percentages. Press the button //
// once and the percentage will display in large font. Hold the button  //
// down for 3 seconds and it will start a safe discharge to storage     //
// cycle.													                                			//
// Note: The above functionality has now changed somewhat. See the      //
// see the README.MD file in the repo for more information.             //
// I still need to work out the creep and how to resolve it.            //
// Seems to be in the region of 0.01x volts. As of 13/04/22 I now       //
// suspect that is is due to thermal variation in the resistor and      //
// other components //
//////////////////////////////////////////////////////////////////////////


/* To change voltage from 3 decimals to 2 decimal places use Line 486 as an example "oled.print (cell1, 2);" = 2 instead of 3 digits.
 *  Also need to sort out the voltage/percentage calculations as it varies from module to module by up to 6% !!!
 *  Update: Variations between modules due to using resistor values of 10% accuracy instead of the original build using 1% accuracy resistors!
 *  Lastly, the damned logo. I've found the bmp and used the tool to get the hex values, loaded them into logo.h but it does not work.
 *  Question is, is it because the wrong logo file is being compiled or because it doesn't use the logo.h file at all.
 *  Only way to test is to rename all the logo files and see what happens. If nothing changes then perhaps it's not using the logo at all for some reason.
 *  Must look at the instructions again to see what is being linked and if the library is actually being used.
 *  Logo issue now resolved I think.
 */

#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <HT_SSD1306.h>  // Include the HT_SSD1306 library

//////////////////////////
// SSD1306 Definition //
//////////////////////////
#define PIN_RESET 9  // Connect RST to pin 9
#define PIN_DC    8  // Connect DC to pin 8
#define PIN_CS    10 // Connect CS to pin 10
#define DC_JUMPER 1
// Also connect pin 13 to SCK and pin 11 to MOSI

//////////////////////////////////
// SSD1306 Object Declaration //
//////////////////////////////////
// Declare a SSD1306 object. The parameters include:
// 1 - Reset pin: Any digital pin
// 2 - D/C pin: Any digital pin (SPI mode only)
// 3 - CS pin: Any digital pin (SPI mode only, 10 recommended)
#include "arduino.h"

//
//
void readADC ();
void calcPercent();
void displayPercent();
void shapeBattery();
void oneBar();
void twoBar();
void threeBar();
void fourBar();
void fiveBar();
void sixBar();
void sevenBar();
void eightBar();
void nineBar();
void tenBar();
void readADC2 ();
void calcPercent2();
void shapeBattery2();
void oneBar2();
void twoBar2();
void threeBar2();
void fourBar2();
void fiveBar2();
void sixBar2();
void sevenBar2();
void eightBar2();
void nineBar2();
void tenBar2();
void readADC3 ();
void calcPercent3();
void dischargePercent();

SSD1306 oled(PIN_RESET, PIN_DC, PIN_CS);
//HT_SSD1306 oled(PIN_RESET, DC_JUMPER); // Example I2C declaration

// I2C is great, but will result in a much slower update rate. The
// slower framerate may be a worthwhile tradeoff, if you need more
// pins, though.

float aRead5 = 0.000;
float aRead4 = 0.000;
float aRead3 = 0.000;
float aRead2 = 0.000;
float aRead1 = 0.000;
float aRead0 = 0.000;

float cell1 = 0.000;
float cell2 = 0.000;
float cell3 = 0.000;
float cell4 = 0.000; // this was "float cell4 = 0 - 000;", don't know why, perhaps mistype but was working
float cell5 = 0.000;
float cell6 = 0.000;

float p;
float minVolt = 3.209;
float maxVolt = 4.199;

int cellCount = 0;
float cellPercent = 0;

//****** this is the setup of the run once routine
bool runOnce = false;


//**************************************************************************************
int inPin = 2;  // the pin number for input (for me a push button) was pin2
int dChrgPin = 4;	//the pin used to discharge the pack.....slowly
int testPin3 = 3; // these following are for the v1.8 black board jst to test the LEDs are working**********!!!!!!!!
int testPin5 = 5; // delete these when put to good use
int testPin6 = 6; // same
int testPin7 = 7; // same

int current;         // Current state of the button
// (LOW is pressed b/c i'm using the pullup resistors)
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = LOW;
unsigned long firstTime; // how long since the button was first pressed
//************************************************************************************
//******************************************************************************************
//**********************************************************************************************

void setup()
{

  /* add setup code here */
  // These three lines of code are all you need to initialize the
  // OLED and print the splash screen.
  // Before you can start using the OLED, call begin() to init
  // all of the pins and configure the OLED.
  oled.begin();
  // clear(ALL) will clear out the OLED's graphic memory.
  // clear(PAGE) will clear the Arduino's display buffer.
  oled.clear(ALL);  // Clear the display's memory (gets rid of artifacts)
  digitalWrite(inPin, LOW);  // Turn on 20k pulldown resistor to simplify switch input  //*************************************************************************************

  pinMode(dChrgPin, OUTPUT);
  pinMode(testPin3, OUTPUT);
  pinMode(testPin5, OUTPUT);
  pinMode(testPin6, OUTPUT);
  pinMode(testPin7, OUTPUT);

}

void loop()
/* add main program code here */
{
  //**** runONCE is a single shot to stop the main loop, readADC starts the battery checking process  ************

  if (!runOnce)
  {

    runOnce = true;

    oled.logo();
    oled.display();
    /////// Switching on all LEDs at boot time
    digitalWrite(testPin3, HIGH);
    digitalWrite(testPin5, HIGH);
    digitalWrite(testPin6, HIGH);
    digitalWrite(testPin7, HIGH);
    digitalWrite(dChrgPin, HIGH);

  }

  //*********** Digital momentary Switch section here *******************
  current = digitalRead(inPin);


  // if the button state changes to pressed, remember the start time
  if (current == HIGH && previous == LOW && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;

  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held > 50) {

    //if (current == LOW && secs_held > prev_secs_held) {
    //	ledblink(1, 50, ledPin); // Each second the button is held blink the indicator led
    //}

    // check if the button was released since we last checked
    if (current == LOW && previous == HIGH) {
      // HERE YOU WOULD ADD VARIOUS ACTIONS AND TIMES FOR YOUR OWN CODE
      // ===============================================================================

      // Button pressed for less than 1 second show std voltage display
      if (secs_held <= 0) {
        readADC();
        digitalWrite(dChrgPin, LOW);
        digitalWrite(testPin3, LOW);
        digitalWrite(testPin5, LOW);
        digitalWrite(testPin6, LOW);
        digitalWrite(testPin7, LOW);
      }

      // If the button was held for 0.5 to 2 seconds show large graphic display
      if (secs_held >= 0.5 && secs_held < 2) {
          oled.clear(PAGE);
          oled.setFontType(0);
          oled.setCursor (1, 1);
          oled.print ("Discharge options:");
          oled.setCursor(1, 15);
          oled.print ("Press button for");
          oled.setCursor(1, 25);
          oled.print ("3 seconds = 50%");
          oled.setCursor(1, 45);
          oled.print ("Press button for");
          oled.setCursor(1, 55);
          oled.print ("5 seconds = 30%");
          oled.display();
       // readADC2();
       // digitalWrite(dChrgPin, LOW);
       // digitalWrite(testPin3, LOW);
       // digitalWrite(testPin5, LOW);
       // digitalWrite(testPin6, LOW);
       // digitalWrite(testPin7, LOW);
      }

      // Button held for 3 or more seconds, discharge the connected pack
      // NOTE the LED comes on BUT i NEED TO RESET THE FONTS FROM THE LIBRARY AS ONLY FONT(1) WORKS!!!!!!!
      if (secs_held >= 2 && secs_held < 5)  {

         
do
        {

          readADC3();
          oled.clear(PAGE);
          oled.setFontType(0);
          oled.setCursor (40, 1);
          oled.print ("DISCHARGING");
          oled.setCursor(40, 15);
          oled.print ("TO 50%");
          oled.display();
          delay(1000);
          //calcPercent();
          dischargePercent();
          oled.setFontType(3);
          displayPercent();
          oled.display();
          //  delay(500);
          digitalWrite(dChrgPin, HIGH);
          digitalWrite(testPin3, HIGH);
          digitalWrite(testPin5, HIGH);
          digitalWrite(testPin6, HIGH);
          digitalWrite(testPin7, HIGH);
          delay(500);
          digitalWrite(testPin3, LOW);
          delay(500);
          digitalWrite(testPin5, LOW);
          delay(500);
          digitalWrite(testPin6, LOW);
          delay(500);
          digitalWrite(testPin7, LOW);
          delay(500);


        } while (cellPercent > 50);  // If the pack discharges to 50% then stop the discharge

        digitalWrite(dChrgPin, LOW);
        oled.clear(PAGE);
        oled.setFontType(1);
        oled.setCursor(1, 10);
        oled.print ("Battery at");
        oled.setCursor(1, 25);
        oled.print("storage charge");
        oled.display();

        while (true) {                       // Flash all the LEDs to show that discharge has ended
          digitalWrite(testPin3, HIGH);
          digitalWrite(testPin5, HIGH);
          digitalWrite(testPin6, HIGH);
          digitalWrite(testPin7, HIGH);
          delay(500);
          digitalWrite(testPin3, LOW);
          digitalWrite(testPin5, LOW);
          digitalWrite(testPin6, LOW);
          digitalWrite(testPin7, LOW);
          delay(500);
        }

        if (cellPercent < 50) {
          oled.clear(PAGE);
          oled.setFontType(1);
          oled.setCursor(1, 10);
          oled.print ("Pack voltage");
          oled.setCursor(1, 25);
          oled.print ("too low to");
          oled.setCursor(1, 40);
          oled.print ("discharge to 50%");
          oled.display();
          digitalWrite(dChrgPin, LOW);
        }


      }
      // ====================================

      // Button held for 3 or more seconds, discharge the connected pack
      // NOTE the LED comes on BUT i NEED TO RESET THE FONTS FROM THE LIBRARY AS ONLY FONT(1) WORKS!!!!!!!
      if (secs_held >= 5)  {

        do
        {

          readADC3();
          oled.clear(PAGE);
          oled.setFontType(0);
          oled.setCursor (40, 1);
          oled.print ("DISCHARGING");
          oled.setCursor(40, 15);
          oled.print ("TO 30%");
          oled.display();
          delay(1000);
          //calcPercent();
          dischargePercent();
          oled.setFontType(3);
          displayPercent();
          oled.display();
          //  delay(500);

          digitalWrite(dChrgPin, HIGH);
          digitalWrite(testPin3, HIGH);
          digitalWrite(testPin5, HIGH);
          digitalWrite(testPin6, HIGH);
          digitalWrite(testPin7, HIGH);
          delay(500);
          digitalWrite(testPin3, LOW);
          delay(500);
          digitalWrite(testPin5, LOW);
          delay(500);
          digitalWrite(testPin6, LOW);
          delay(500);
          digitalWrite(testPin7, LOW);
          delay(500);

        } while (cellPercent > 30);  // If the pack discharges to 60% then stop the discharge


        digitalWrite(dChrgPin, LOW);
        oled.clear(PAGE);
        oled.setFontType(1);
        oled.setCursor(1, 10);
        oled.print ("Battery at");
        oled.setCursor(1, 25);
        oled.print("storage charge");
        oled.display();

        while (true) {                       // Flash all the LEDs to show that discharge has ended
          digitalWrite(testPin3, HIGH);
          digitalWrite(testPin5, HIGH);
          digitalWrite(testPin6, HIGH);
          digitalWrite(testPin7, HIGH);
          delay(500);
          digitalWrite(testPin3, LOW);
          digitalWrite(testPin5, LOW);
          digitalWrite(testPin6, LOW);
          digitalWrite(testPin7, LOW);
          delay(500);
        }

        if (cellPercent < 30) {
          oled.clear(PAGE);
          oled.setFontType(1);
          oled.setCursor(1, 10);
          oled.print ("Pack voltage");
          oled.setCursor(1, 25);
          oled.print ("too low to");
          oled.setCursor(1, 40);
          oled.print ("discharge to 30%");
          oled.display();
          digitalWrite(dChrgPin, LOW);
        }

      }
      // ====================================
    }
  }

  previous = current;
  prev_secs_held = secs_held;

  //********************************************

}

void readADC () {
  oled.clear(ALL);
  oled.clear(DISPLAY);
  ////////////////////////////////
  // Read voltages from the ADC //
  ////////////////////////////////

  aRead5 = analogRead(A5);
  cell6 = aRead5 * 0.004225;	//4883

  aRead4 = analogRead(A4);
  cell5 = aRead4 * 0.004115;	// 4863

  aRead3 = analogRead(A3);
  cell4 = aRead3 * 0.004175;  // 4863 multiplier to account for inconsistencies in voltage dividers

  aRead2 = analogRead(A2);
  cell3 = aRead2 * 0.004200;	// multiplier to account for inconsistencies in voltage dividers

  aRead1 = analogRead(A1);
  cell2 = aRead1 * 0.004129; //0.004446;	// multiplier to account for inconsistencies in voltage dividers

  aRead0 = analogRead(A0);
  cell1 = aRead0 * 0.004895; //0.004863;

  // Decide how many cells in pack //

  if (aRead5 > 0.1) {
    cellCount = 6;
  }
  else if (aRead4 > 0.1) {
    cellCount = 5;
  }
  else if (aRead3 > 0.1) {
    cellCount = 4;
  }
  else if (aRead2 > 0.1) {
    cellCount = 3;
  }
  else if (aRead1 > 0.1) {
    cellCount = 2;
  }
  else 	cellCount = 1;

  // Display battery, voltages and percentage
  switch (cellCount) {
    case 1:
      oled.clear(PAGE);
      p = (cell1);
      calcPercent();
      oled.setFontType(1);
      oled.setCursor(31, 0);
      oled.print (cell1, 3);
      displayPercent();
      oled.display();
      break;

    case 2:
      oled.clear(PAGE);
      p = (cell1 + cell2);
      calcPercent();
      oled.setFontType(1);
      oled.setCursor(31, 0);
      oled.print (cell1, 3);

      oled.setCursor(31, 11);
      oled.print (cell2, 3);
      displayPercent();
      oled.display();
      break;

    case 3:
      oled.clear(PAGE);
      p = (cell1 + cell2 + cell3);
      calcPercent();
      oled.setFontType(1);
      oled.setCursor(31, 0); // Print Cell1 voltage
      oled.print (cell1, 3);

      oled.setCursor(31, 11); // Print Cell2 voltage
      oled.print (cell2, 3);

      oled.setCursor(31, 22);  // Print Cell3 voltage 37
      oled.print (cell3, 3);

      displayPercent();
      oled.display();
      break;

    case 4:
      oled.clear(PAGE);
      p = (cell1 + cell2 + cell3 + cell4);
      calcPercent();
      oled.setFontType(1);
      oled.setCursor(31, 0); // Print Cell1 voltage 5
      oled.print (cell1, 3);

                            //  oled.setCursor(65, 0); // Print Cell1 voltage 5
                            //    oled.print ("v");
      
      oled.setCursor(31, 11); // Print Cell2 voltage 21
      oled.print (cell2, 3);

      oled.setCursor(31, 22);  // Print Cell3 voltage 37
      oled.print (cell3, 3);

      oled.setCursor(31, 33); // 53
      oled.print (cell4, 3);
      displayPercent();
      oled.display();

      break;

    case 5:
      oled.clear(PAGE);
      p = (cell1 + cell2 + cell3 + cell4 + cell5);
      calcPercent();
      oled.setFontType(0);
      oled.setCursor(31, 0); // Print Cell1 voltage 5
      oled.print (cell1, 3);

      oled.setCursor(31, 11); // Print Cell2 voltage 21
      oled.print (cell2, 3);

      oled.setCursor(31, 22);  // Print Cell3 voltage 37
      oled.print (cell3, 3);

      oled.setCursor(31, 33); // 53
      oled.print (cell4, 3);

      oled.setCursor(31, 44); // 53
      oled.print (cell5, 3);
      displayPercent();
      oled.display();
      break;

    case 6:
      oled.clear(PAGE);
      p = (cell1 + cell2 + cell3 + cell4 + cell5 + cell6);
      calcPercent();
      oled.setFontType(0);
      oled.setCursor(31, 0); // Print Cell1 voltage 5
      oled.print (cell1, 3);

      oled.setCursor(31, 11); // Print Cell2 voltage 21
      oled.print (cell2, 3);

      oled.setCursor(31, 22);  // Print Cell3 voltage 37
      oled.print (cell3, 3);

      oled.setCursor(31, 33); // 53
      oled.print (cell4, 3);

      oled.setCursor(31, 44); // 53
      oled.print (cell5, 3);

      oled.setCursor(31, 55); // 53
      oled.print (cell6, 3);

      displayPercent();
      oled.display();
      break;

    default:
      p = 0;
      oled.clear(ALL);
      oled.clear(DISPLAY);
      oled.print ("No pack connected");
      oled.display();
      break;
  }
}

void calcPercent() { // These pack percentage calculations are based on cellCount!!!!
  if (p > cellCount * 4.199) {
    cellPercent = 100;
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
    sevenBar();
    eightBar();
    nineBar();
    tenBar();
  }

  else if ((p <= 4.199 * cellCount) && (p >= 4.085 * cellCount)) {			// 90% to 100% scale   p = ((p - minVolt*cellCount)/(
    //p = ((p - 12.255)/(0.342) * 10 +90);
    cellPercent = ((p - 4.085 * cellCount) / (4.199 * cellCount - 4.085 * cellCount) * 10 + 90);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
    sevenBar();
    eightBar();
    nineBar();
    tenBar();
  }

  else if ((p <= 4.084 * cellCount) && (p >= 3.999 * cellCount))			// 80% to 89.99% scale (upper was 12.228 increased to
  { // p=((p - 11.997)/ (0.231)*10 +80);			// 12.254 to close gap to next limit of 12.255 in lower 100% scale)
    cellPercent = ((p - 3.999 * cellCount) / (4.084 * cellCount - 3.999 * cellCount) * 10 + 80);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
    sevenBar();
    eightBar();
    nineBar();
  }

  else if ((p <= 3.998 * cellCount) && (p >= 3.936 * cellCount))		// 70% to 79.99% scale (upper was 11.976 increased to
  { //p=((p - 11.808)/(0.168) *10 +70);		// 11.996 to close gap to next limit of 11.997 in lower 100% scale)
    cellPercent = ((p - 3.936 * cellCount) / (3.998 * cellCount - 3.936 * cellCount) * 10 + 70);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
    sevenBar();
    eightBar();
  }

  else if ((p <= 3.935 * cellCount) && (p >= 3.883 * cellCount))							// 60 - 69.99%
  { //p=((p - 11.649)/ (0.141) *10 +60);
    cellPercent = ((p - 3.883 * cellCount) / (3.935 * cellCount - 3.883 * cellCount) * 10 + 60);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
    sevenBar();
  }

  else if ((p <= 3.882 * cellCount) && (p >= 3.839 * cellCount))						// 50 - 59.99%
  { //p=((p - 11.517)/(0.117) *10 +50);
    cellPercent = ((p - 3.839 * cellCount) / (3.882 * cellCount - 3.839 * cellCount) * 10 + 50);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
    sixBar();
  }

  else if ((p <= 3.838 * cellCount) && (p >= 3.812 * cellCount))					// 40 - 49.99%
  { //p=((p - 11.436)/(0.072) *10 +40);
    cellPercent = ((p - 3.812 * cellCount) / (3.838 * cellCount - 3.812 * cellCount) * 10 + 40);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
    fiveBar();
  }

  else if ((p <= 3.811 * cellCount) && (p >= 3.791 * cellCount))				// 30 - 39.99%
  { //p=((p - 11.373)/(0.054) *10 +30);
    cellPercent = ((p - 3.791 * cellCount) / (3.811 * cellCount - 3.791 * cellCount) * 10 + 30);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
    fourBar();
  }

  else if ((p <= 3.790 * cellCount) && (p >= 3.747 * cellCount))		// 20 - 29.99%
  { //p=((p - 11.241)/(0.117) *10 +20);
    cellPercent = ((p - 3.747 * cellCount) / (3.790 * cellCount - 3.747 * cellCount) * 10 + 20);
    shapeBattery();
    oneBar();
    twoBar();
    threeBar();
  }

  else if ((p <= 3.746 * cellCount) && (p >= 3.683 * cellCount))		// 10 - 19.99%
  { //p=((p - 11.049)/(0.171) *10 +10);
    cellPercent = ((p - 3.683 * cellCount) / (3.746 * cellCount - 3.683 * cellCount) * 10 + 10);
    shapeBattery();
    oneBar();
    twoBar();
  }

  else if ((p <= 3.682 * cellCount) && (p >= 3.209 * cellCount))		// 0 - 9.99%
  { //p=((p - 9.627)/(1.278) *100);
    cellPercent = ((p - 3.209 * cellCount) / (3.682 * cellCount - 3.209 * cellCount));
    shapeBattery();
    oneBar();
  }


  else {
    cellPercent = 0;
    oled.clear(DISPLAY);
    oled.clear(ALL);
    shapeBattery();
  }
}

void displayPercent() {
  oled.setFontType(3);
  oled.setCursor(77, 10); // Print percentage
  oled.print(cellPercent, 0);
  oled.setFontType(1);
  oled.print("%");
  oled.display ();
  //delay (3000);
}

void shapeBattery() {
  // Draw the battery
  //oled.clear(PAGE);  // Clear the page

  oled.rect(5, 0, 20, 3); // Battery top 10
  oled.rect(0, 3, 30, 61); // Battery body 5

  oled.display();
  delay(1000);
}
void oneBar() {
  oled.rectFill(2, 58, 26, 5 ); // 1st bar
  oled.display();
  delay(50);
}

void twoBar() {
  oled.rectFill(2, 52, 26, 5 ); // 2nd bar
  oled.display();
  delay(50);
}

void threeBar() {
  oled.rectFill(2, 46, 26, 5); // 3rd bar
  oled.display();
  delay(50);
}

void fourBar() {
  oled.rectFill(2, 40, 26, 5); // 4th bar
  oled.display();
  delay(50);
}

void fiveBar() {
  oled.rectFill(2, 34, 26, 5); // 5th bar
  oled.display();
  delay(50);
}

void sixBar() {
  oled.rectFill(2, 28, 26, 5); // 6th bar
  oled.display();
  delay(50);
}

void sevenBar() {
  oled.rectFill(2, 22, 26, 5); // 7th bar
  oled.display();
  delay(50);
}

void eightBar() {
  oled.rectFill(2, 16, 26, 5); // 8th bar
  oled.display();
  delay(50);
}

void nineBar() {
  oled.rectFill(2, 10, 26, 5); // 9th bar
  oled.display();
  delay(50);
}

void tenBar() {
  oled.rectFill(2, 4, 26, 5); // 10th bar
  oled.display();
  delay(50);
}

void readADC2 () {
  //oled.clear(ALL);
  // oled.clear(DISPLAY);
  ////////////////////////////////
  // Read voltages from the ADC //
  ////////////////////////////////

  aRead5 = analogRead(A5);
  cell6 = aRead5 * 0.004225;	//4883

  aRead4 = analogRead(A4);
  cell5 = aRead4 * 0.004115;	// 4863

  aRead3 = analogRead(A3);
  cell4 = aRead3 * 0.004175;  // 4863 multiplier to account for inconsistencies in voltage dividers

  aRead2 = analogRead(A2);
  cell3 = aRead2 * 0.004200;	// multiplier to account for inconsistencies in voltage dividers

  aRead1 = analogRead(A1);
  cell2 = aRead1 * 0.004129; //0.004446;	// multiplier to account for inconsistencies in voltage dividers

  aRead0 = analogRead(A0);
  cell1 = aRead0 * 0.004895; //0.004863;

  // Decide how many cells in pack //

  if (aRead5 > 0.1) {
    cellCount = 6;
  }
  else if (aRead4 > 0.1) {
    cellCount = 5;
  }
  else if (aRead3 > 0.1) {
    cellCount = 4;
  }
  else if (aRead2 > 0.1) {
    cellCount = 3;
  }
  else if (aRead1 > 0.1) {
    cellCount = 2;
  }
  else 	cellCount = 1;

  // Display battery, voltages and percentage
  switch (cellCount) {
    case 1:
      p = (cell1);
      calcPercent2();
      break;

    case 2:
      p = (cell1 + cell2);
      calcPercent2();
      break;

    case 3:
      p = (cell1 + cell2 + cell3);
      calcPercent2();
      break;

    case 4:
      p = (cell1 + cell2 + cell3 + cell4);
      calcPercent2();
      break;

    case 5:
      p = (cell1 + cell2 + cell3 + cell4 + cell5);
      calcPercent2();
      break;

    case 6:
      p = (cell1 + cell2 + cell3 + cell4 + cell5 + cell6);
      calcPercent2();
      break;

    default:
      p = 0;
      break;
  }
}

// may not need "cellPercent" references in calcPercent2 function % as not displayed
void calcPercent2() { // These pack percentage calculations are based on cellCount!!!!
  if (p > cellCount * 4.199) {
    cellPercent = 100;
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
    sevenBar2();
    eightBar2();
    nineBar2();
    tenBar2();
  }

  else if ((p <= 4.199 * cellCount) && (p >= 4.085 * cellCount)) {			// 90% to 100% scale   p = ((p - minVolt*cellCount)/(
    cellPercent = ((p - 4.085 * cellCount) / (4.199 * cellCount - 4.085 * cellCount) * 10 + 90);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
    sevenBar2();
    eightBar2();
    nineBar2();
    tenBar2();
  }

  else if ((p <= 4.084 * cellCount) && (p >= 3.999 * cellCount))			// 80% to 89.99% scale (upper was 12.228 increased to
  { // p=((p - 11.997)/ (0.231)*10 +80);			                        // 12.254 to close gap to next limit of 12.255 in lower 100% scale)
    cellPercent = ((p - 3.999 * cellCount) / (4.084 * cellCount - 3.999 * cellCount) * 10 + 80);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
    sevenBar2();
    eightBar2();
    nineBar2();
  }

  else if ((p <= 3.998 * cellCount) && (p >= 3.936 * cellCount))		// 70% to 79.99% scale (upper was 11.976 increased to
  { //p=((p - 11.808)/(0.168) *10 +70);		// 11.996 to close gap to next limit of 11.997 in lower 100% scale)
    cellPercent = ((p - 3.936 * cellCount) / (3.998 * cellCount - 3.936 * cellCount) * 10 + 70);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
    sevenBar2();
    eightBar2();
  }

  else if ((p <= 3.935 * cellCount) && (p >= 3.883 * cellCount))							// 60 - 69.99%
  { //p=((p - 11.649)/ (0.141) *10 +60);
    cellPercent = ((p - 3.883 * cellCount) / (3.935 * cellCount - 3.883 * cellCount) * 10 + 60);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
    sevenBar2();
  }

  else if ((p <= 3.882 * cellCount) && (p >= 3.839 * cellCount))						// 50 - 59.99%
  { //p=((p - 11.517)/(0.117) *10 +50);
    cellPercent = ((p - 3.839 * cellCount) / (3.882 * cellCount - 3.839 * cellCount) * 10 + 50);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
    sixBar2();
  }

  else if ((p <= 3.838 * cellCount) && (p >= 3.812 * cellCount))					// 40 - 49.99%
  { //p=((p - 11.436)/(0.072) *10 +40);
    cellPercent = ((p - 3.812 * cellCount) / (3.838 * cellCount - 3.812 * cellCount) * 10 + 40);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
    fiveBar2();
  }

  else if ((p <= 3.811 * cellCount) && (p >= 3.791 * cellCount))				// 30 - 39.99%
  { //p=((p - 11.373)/(0.054) *10 +30);
    cellPercent = ((p - 3.791 * cellCount) / (3.811 * cellCount - 3.791 * cellCount) * 10 + 30);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
    fourBar2();
  }

  else if ((p <= 3.790 * cellCount) && (p >= 3.747 * cellCount))		// 20 - 29.99%
  { //p=((p - 11.241)/(0.117) *10 +20);
    cellPercent = ((p - 3.747 * cellCount) / (3.790 * cellCount - 3.747 * cellCount) * 10 + 20);
    shapeBattery2();
    oneBar2();
    twoBar2();
    threeBar2();
  }

  else if ((p <= 3.746 * cellCount) && (p >= 3.683 * cellCount))		// 10 - 19.99%
  { //p=((p - 11.049)/(0.171) *10 +10);
    cellPercent = ((p - 3.683 * cellCount) / (3.746 * cellCount - 3.683 * cellCount) * 10 + 10);
    shapeBattery2();
    oneBar2();
    twoBar2();
  }

  else if ((p <= 3.682 * cellCount) && (p >= 3.209 * cellCount))		// 0 - 9.99%
  { //p=((p - 9.627)/(1.278) *100);
    cellPercent = ((p - 3.209 * cellCount) / (3.682 * cellCount - 3.209 * cellCount));
    shapeBattery2();
    oneBar2();
  }


  else {
    cellPercent = 0;
    oled.clear(DISPLAY);
    oled.clear(ALL);
    shapeBattery2();

  }
}


void shapeBattery2() {
  // Draw the full screen battery
  //oled.clear(PAGE);  // Clear the page

  oled.clear(PAGE);  // Clear the page
  // Draw an outline of the screen:
  oled.rect(0, 0, oled.getLCDWidth() - 1, oled.getLCDHeight());
  // Draw the center line
  oled.rectFill(oled.getLCDWidth() / 2 - 2, 0, 5, oled.getLCDHeight());

  oled.display();
  delay(1000);
}



// Drawing the rectangles
// co-ordinates are:
// top left corner, x left to right, y top down.
// give it a x/y coordinate to start at, then a width and a height.

void oneBar2() {
  oled.rectFill(116, 2, 11, 60 ); // 1st bar
  oled.display();
  delay(50);
}

void twoBar2() {
  oled.rectFill(104, 2, 11, 60 ); // 2nd bar
  oled.display();
  delay(50);
}

void threeBar2() {
  oled.rectFill(92, 2, 11, 60); // 3rd bar
  oled.display();
  delay(50);
}

void fourBar2() {
  oled.rectFill(80, 2, 11, 60); // 4th bar
  oled.display();
  delay(50);
}

void fiveBar2() {
  oled.rectFill(68, 2, 11, 60); // 5th bar
  oled.display();
  delay(50);
}

void sixBar2() {
  oled.rectFill(50, 2, 11, 60); // 6th bar
  oled.display();
  delay(50);
}

void sevenBar2() {
  oled.rectFill(38, 2, 11, 60); // 7th bar
  oled.display();
  delay(50);
}

void eightBar2() {
  oled.rectFill(26, 2, 11, 60); // 8th bar
  oled.display();
  delay(50);
}

void nineBar2() {
  oled.rectFill(14, 2, 11, 60); // 9th bar
  oled.display();
  delay(50);
}

void tenBar2() {
  oled.rectFill(2, 2, 11, 60); // 10th bar
  oled.display();
  delay(50);
}


void readADC3 () {
  oled.clear(ALL);
  oled.clear(DISPLAY);
  ////////////////////////////////
  // Read voltages from the ADC //
  ////////////////////////////////

  aRead5 = analogRead(A5);
  cell6 = aRead5 * 0.004225;	//4883

  aRead4 = analogRead(A4);
  cell5 = aRead4 * 0.004115;	// 4863

  aRead3 = analogRead(A3);
  cell4 = aRead3 * 0.004175;  // 4863 multiplier to account for inconsistencies in voltage dividers

  aRead2 = analogRead(A2);
  cell3 = aRead2 * 0.004200;	// multiplier to account for inconsistencies in voltage dividers

  aRead1 = analogRead(A1);
  cell2 = aRead1 * 0.004129; //0.004446;	// multiplier to account for inconsistencies in voltage dividers

  aRead0 = analogRead(A0);
  cell1 = aRead0 * 0.004895; //0.004863;

  // Decide how many cells in pack //

  if (aRead5 > 0.1) {
    cellCount = 6;
  }
  else if (aRead4 > 0.1) {
    cellCount = 5;
  }
  else if (aRead3 > 0.1) {
    cellCount = 4;
  }
  else if (aRead2 > 0.1) {
    cellCount = 3;
  }
  else if (aRead1 > 0.1) {
    cellCount = 2;
  }
  else 	cellCount = 1;

  // Display battery, voltages and percentage
  switch (cellCount) {
    case 1:
      p = (cell1);
      calcPercent3();
      break;

    case 2:
      p = (cell1 + cell2);
      calcPercent3();
      break;

    case 3:
      p = (cell1 + cell2 + cell3);
      calcPercent3();
      break;

    case 4:
      p = (cell1 + cell2 + cell3 + cell4);
      calcPercent3();
      break;

    case 5:
      p = (cell1 + cell2 + cell3 + cell4 + cell5);
      calcPercent3();
      break;

    case 6:
      p = (cell1 + cell2 + cell3 + cell4 + cell5 + cell6);
      calcPercent3();
      break;

    default:
      p = 0;
      break;
  }
}

void calcPercent3() { // These pack percentage calculations are based on cellCount!!!!
  if (p > cellCount * 4.199) {
    cellPercent = 100;

  }

  else if ((p <= 4.199 * cellCount) && (p >= 4.085 * cellCount)) {			// 90% to 100% scale   p = ((p - minVolt*cellCount)/(
    //p = ((p - 12.255)/(0.342) * 10 +90);
    cellPercent = ((p - 4.085 * cellCount) / (4.199 * cellCount - 4.085 * cellCount) * 10 + 90);

  }

  else if ((p <= 4.084 * cellCount) && (p >= 3.999 * cellCount))			// 80% to 89.99% scale (upper was 12.228 increased to
  { // p=((p - 11.997)/ (0.231)*10 +80);			// 12.254 to close gap to next limit of 12.255 in lower 100% scale)
    cellPercent = ((p - 3.999 * cellCount) / (4.084 * cellCount - 3.999 * cellCount) * 10 + 80);

  }

  else if ((p <= 3.998 * cellCount) && (p >= 3.936 * cellCount))		// 70% to 79.99% scale (upper was 11.976 increased to
  { //p=((p - 11.808)/(0.168) *10 +70);		// 11.996 to close gap to next limit of 11.997 in lower 100% scale)
    cellPercent = ((p - 3.936 * cellCount) / (3.998 * cellCount - 3.936 * cellCount) * 10 + 70);

  }

  else if ((p <= 3.935 * cellCount) && (p >= 3.883 * cellCount))							// 60 - 69.99%
  { //p=((p - 11.649)/ (0.141) *10 +60);
    cellPercent = ((p - 3.883 * cellCount) / (3.935 * cellCount - 3.883 * cellCount) * 10 + 60);

  }

  else if ((p <= 3.882 * cellCount) && (p >= 3.839 * cellCount))						// 50 - 59.99%
  { //p=((p - 11.517)/(0.117) *10 +50);
    cellPercent = ((p - 3.839 * cellCount) / (3.882 * cellCount - 3.839 * cellCount) * 10 + 50);

  }

  else if ((p <= 3.838 * cellCount) && (p >= 3.812 * cellCount))					// 40 - 49.99%
  { //p=((p - 11.436)/(0.072) *10 +40);
    cellPercent = ((p - 3.812 * cellCount) / (3.838 * cellCount - 3.812 * cellCount) * 10 + 40);

  }

  else if ((p <= 3.811 * cellCount) && (p >= 3.791 * cellCount))				// 30 - 39.99%
  { //p=((p - 11.373)/(0.054) *10 +30);
    cellPercent = ((p - 3.791 * cellCount) / (3.811 * cellCount - 3.791 * cellCount) * 10 + 30);

  }

  else if ((p <= 3.790 * cellCount) && (p >= 3.747 * cellCount))		// 20 - 29.99%
  { //p=((p - 11.241)/(0.117) *10 +20);
    cellPercent = ((p - 3.747 * cellCount) / (3.790 * cellCount - 3.747 * cellCount) * 10 + 20);

  }

  else if ((p <= 3.746 * cellCount) && (p >= 3.683 * cellCount))		// 10 - 19.99%
  { //p=((p - 11.049)/(0.171) *10 +10);
    cellPercent = ((p - 3.683 * cellCount) / (3.746 * cellCount - 3.683 * cellCount) * 10 + 10);

  }

  else if ((p <= 3.682 * cellCount) && (p >= 3.209 * cellCount))		// 0 - 9.99%
  { //p=((p - 9.627)/(1.278) *100);
    cellPercent = ((p - 3.209 * cellCount) / (3.682 * cellCount - 3.209 * cellCount));

  }


  else {
    cellPercent = 0;

  }
}



void dischargePercent() { // These pack percentage calculations are based on cellCount!!!!
  if (p > cellCount * 4.199) {
    cellPercent = 100;

    shapeBattery();
    tenBar();
    nineBar();
    eightBar();
    sevenBar();
    sixBar();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();
    // oled.clear(calcPercent);   ***I tried to use this to clear just the battery, didn't work as expected here


  }

  else if ((p <= 4.199 * cellCount) && (p >= 4.085 * cellCount)) {			// 90% to 100% scale   p = ((p - minVolt*cellCount)/(
    cellPercent = ((p - 4.085 * cellCount) / (4.199 * cellCount - 4.085 * cellCount) * 10 + 90);
    shapeBattery();

    eightBar();
    sevenBar();
    sixBar();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();


  }

  else if ((p <= 4.084 * cellCount) && (p >= 3.999 * cellCount))			// 80% to 89.99% scale (upper was 12.228 increased to
  { // p=((p - 11.997)/ (0.231)*10 +80);			// 12.254 to close gap to next limit of 12.255 in lower 100% scale)
    cellPercent = ((p - 3.999 * cellCount) / (4.084 * cellCount - 3.999 * cellCount) * 10 + 80);
    shapeBattery();
    eightBar();
    sevenBar();
    sixBar();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();

  }

  else if ((p <= 3.998 * cellCount) && (p >= 3.936 * cellCount))		// 70% to 79.99% scale (upper was 11.976 increased to
  { //p=((p - 11.808)/(0.168) *10 +70);		// 11.996 to close gap to next limit of 11.997 in lower 100% scale)
    cellPercent = ((p - 3.936 * cellCount) / (3.998 * cellCount - 3.936 * cellCount) * 10 + 70);
    shapeBattery();
    sevenBar();
    sixBar();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();


  }

  else if ((p <= 3.935 * cellCount) && (p >= 3.883 * cellCount))							// 60 - 69.99%
  { //p=((p - 11.649)/ (0.141) *10 +60);
    cellPercent = ((p - 3.883 * cellCount) / (3.935 * cellCount - 3.883 * cellCount) * 10 + 60);
    shapeBattery();
    sixBar();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();
  }

  else if ((p <= 3.882 * cellCount) && (p >= 3.839 * cellCount))						// 50 - 59.99%
  { //p=((p - 11.517)/(0.117) *10 +50);
    cellPercent = ((p - 3.839 * cellCount) / (3.882 * cellCount - 3.839 * cellCount) * 10 + 50);
    shapeBattery();
    fiveBar();
    fourBar();
    threeBar();
    twoBar();
    oneBar();
  }

  else if ((p <= 3.838 * cellCount) && (p >= 3.812 * cellCount)) // 40 - 49.99%
  { //p=((p - 11.436)/(0.072) *10 +40);
    cellPercent = ((p - 3.812 * cellCount) / (3.838 * cellCount - 3.812 * cellCount) * 10 + 40);
    shapeBattery();
    fourBar();
    threeBar();
    twoBar();
    oneBar();
  }

  else if ((p <= 3.811 * cellCount) && (p >= 3.791 * cellCount)) // 30 - 39.99%
  { //p=((p - 11.373)/(0.054) *10 +30);
    cellPercent = ((p - 3.791 * cellCount) / (3.811 * cellCount - 3.791 * cellCount) * 10 + 30);
    shapeBattery();
    threeBar();
    twoBar();
    oneBar();
  }

  else if ((p <= 3.790 * cellCount) && (p >= 3.747 * cellCount)) // 20 - 29.99%
  { //p=((p - 11.241)/(0.117) *10 +20);
    cellPercent = ((p - 3.747 * cellCount) / (3.790 * cellCount - 3.747 * cellCount) * 10 + 20);
    shapeBattery();
    twoBar();
    oneBar();
  }

}
