/* Arduino real time clock and calendar with 2 alarm functions and temperature monitor using DS3231
   Read DS3231 RTC datasheet to understand the code
   Time & date parameters can be set using two push buttons connected to pins 9 (B1) & 10 (B2).
   Alarm1 and alarm2 can be set using two push buttons connected to 11 (B3) & 10 (B2).
   Pin 12 becomes high when alarm occurred and button B2 returns it to low and
   turns the occurred alarm OFF.
   DS3231 interrupt pin is connected to Arduino external interrupt pin 2.
*/

// include LCD library code
#include <LiquidCrystal.h>

// include Wire library code (needed for I2C protocol devices)
#include <Wire.h>

// LCD module connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

const int button1   =  9;                   // button1 pin number
const int button2   = 10;                   // button2 pin number
const int button3   = 11;                   // button3 pin number
const int alarm_pin = 12;                   // Alarms pin number

void setup() {
  pinMode(9,  INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  digitalWrite(alarm_pin, LOW);
  // set up the LCD's number of columns and rows
  lcd.begin(20, 4);
  Wire.begin();                                 // Join i2c bus
  attachInterrupt(digitalPinToInterrupt(2), Alarm, FALLING);
}

// Variables declaration
bool alarm1_status, alarm2_status;
char Time[]     = "  :  :  ",
                  calendar[] = "      /  /20  ",
                               alarm1[]   = "  :  ", alarm2[]   = "  :  ",
                                   temperature[] = "Θερ:  .  C";
byte  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;

void Alarm() {
  digitalWrite(alarm_pin, HIGH);
}
void DS3231_read() {                            // Function to read time & calendar data
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0);                                // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS3231 and release I2C bus at end of reading
  second = Wire.read();                         // Read seconds from register 0
  minute = Wire.read();                         // Read minuts from register 1
  hour   = Wire.read();                         // Read hour from register 2
  day    = Wire.read();                         // Read day from register 3
  date   = Wire.read();                         // Read date from register 4
  month  = Wire.read();                         // Read month from register 5
  year   = Wire.read();                         // Read year from register 6
}
void alarms_read_display() {                    // Function to read and display alarm1, alarm2 and temperature data
  byte control_reg, temperature_lsb;
  char temperature_msb;
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0x08);                             // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 11);                   // Request 11 bytes from DS3231 and release I2C bus at end of reading
  alarm1_minute = Wire.read();                  // Read alarm1 minutes
  alarm1_hour   = Wire.read();                  // Read alarm1 hours
  Wire.read();                                  // Skip alarm1 day/date register
  alarm2_minute = Wire.read();                  // Read alarm2 minutes
  alarm2_hour   = Wire.read();                  // Read alarm2 hours
  Wire.read();                                  // Skip alarm2 day/date register
  control_reg = Wire.read();                    // Read the DS3231 control register
  status_reg  = Wire.read();                    // Read the DS3231 status register
  Wire.read();                                  // Skip aging offset register
  temperature_msb = Wire.read();                // Read temperature MSB
  temperature_lsb = Wire.read();                // Read temperature LSB
  // Convert BCD to decimal
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
  // End conversion
  alarm1[8]     = alarm1_minute % 10  + 48;
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bitRead(control_reg, 0);      // Read alarm1 interrupt enable bit (A1IE) from DS3231 control register
  alarm2_status = bitRead(control_reg, 1);      // Read alarm2 interrupt enable bit (A2IE) from DS3231 control register
  if (temperature_msb < 0) {
    temperature_msb = abs(temperature_msb);
    temperature[2] = '-';
  }
  else
    temperature[2] = ' ';
  temperature_lsb >>= 6;
  temperature[4] = temperature_msb % 10  + 48;
  temperature[3] = temperature_msb / 10  + 48;
  if (temperature_lsb == 0 || temperature_lsb == 2) {
    temperature[7] = '0';
    if (temperature_lsb == 0) temperature[6] = '0';
    else                     temperature[6] = '5';
  }
  if (temperature_lsb == 1 || temperature_lsb == 3) {
    temperature[7] = '5';
    if (temperature_lsb == 1) temperature[6] = '2';
    else                     temperature[6] = '7';
  }
  temperature[8]  = 223;                        // Put the degree symbol ---> Δες το ASCII code 167 ή το 248 για σύμβολο βαθμού κελσίου
  lcd.setCursor(0, 0);
  lcd.print(temperature);                       // Display temperature

  lcd.setCursor(3, 3);
  lcd.print(alarm1);                            // Display alarm1
  
  lcd.setCursor(8, 3);
  if (alarm1_status)  lcd.print("●");         // If A1IE = 1 print 'ON'  ☼ ● ○
  else               lcd.print("○");          // If A1IE = 0 print 'OFF'
  
  lcd.setCursor(11, 3);
  lcd.print(alarm2);                            // Display alarm2
  
  lcd.setCursor(16, 3);
  if (alarm2_status)  lcd.print("●");         // If A2IE = 1 print 'ON'
  else               lcd.print("○");          // If A2IE = 0 print 'OFF'
}
void calendar_display() {                       // Function to display calendar
  switch (day) {
    case 1:  strcpy(calendar, "Κυριακή   /  /20  "); break;   //Sun
    case 2:  strcpy(calendar, "Δευτέρα   /  /20  "); break;   //Mon 
    case 3:  strcpy(calendar, "Τρίτη   /  /20  "); break;   //Tue
    case 4:  strcpy(calendar, "Τετάρτη   /  /20  "); break;   //Wed
    case 5:  strcpy(calendar, "Πέμπτη   /  /20  "); break;   //Thu
    case 6:  strcpy(calendar, "Παρασκευή   /  /20  "); break;   //Fri
    case 7:  strcpy(calendar, "Σάββατο   /  /20  "); break;   //Sat
    default: strcpy(calendar, "Σάββατο   /  /20  ");
  }
  calendar[13] = year  % 10 + 48;
  calendar[12] = year  / 10 + 48;
  calendar[8]  = month % 10 + 48;
  calendar[7]  = month / 10 + 48;
  calendar[5]  = date  % 10 + 48;
  calendar[4]  = date  / 10 + 48;
  lcd.setCursor(0, 2);                          //Στην τρίτη σειρά βρίσκεται η ημερομηνία
  lcd.print(calendar);                          // Display calendar
}
void DS3231_display() {
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
  // End conversion
  Time[7]     = second % 10  + 48;
  Time[6]     = second / 10  + 48;
  Time[4]     = minute % 10  + 48;
  Time[3]     = minute / 10  + 48;
  Time[1]     = hour   % 10  + 48;
  Time[0]     = hour   / 10  + 48;
  calendar_display();                           // Call calendar display function
  lcd.setCursor(6, 1);
  lcd.print(Time);                              // Display time
}
void Blink() {
  byte j = 0;
  while (j < 10 && (digitalRead(button1) || i >= 5) && digitalRead(button2) && (digitalRead(button3) || i < 5)) {
    j++;
    delay(25);
  }
}
byte edit(byte x, byte y, byte parameter) {
  char text[3];
  while (!digitalRead(button1) || !digitalRead(button3));   // Wait until button B1 is released
  while (true) {
    while (!digitalRead(button2)) {                         // If button B2 is pressed
      parameter++;
      if (((i == 0) || (i == 5)) && parameter > 23)         // If hours > 23 ==> hours = 0
        parameter = 0;
      if (((i == 1) || (i == 6)) && parameter > 59)         // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if (i == 2 && parameter > 31)                         // If date > 31 ==> date = 1
        parameter = 1;
      if (i == 3 && parameter > 12)                         // If month > 12 ==> month = 1
        parameter = 1;
      if (i == 4 && parameter > 99)                         // If year > 99 ==> year = 0
        parameter = 0;
      if (i == 7 && parameter > 1)                          // For alarms ON or OFF (1: alarm ON, 0: alarm OFF)
        parameter = 0;
      lcd.setCursor(x, y);
      if (i == 7) {                                         // For alarms ON & OFF
        if (parameter == 1)  lcd.print("●");
        else                lcd.print("○");
      }
      else {
        sprintf(text, "%02u", parameter);
        lcd.print(text);
      }
      if (i >= 5) {
        DS3231_read();                          // Read data from DS3231
        DS3231_display();                       // Display DS3231 time and calendar
      }
      delay(200);                               // Wait 200ms
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                            // Print two spaces
    if (i == 7) lcd.print(" ");                 // Print space (for alarms ON & OFF)
    Blink();                                    // Call Blink function
    lcd.setCursor(x, y);
    if (i == 7) {                               // For alarms ON & OFF
      if (parameter == 1)  lcd.print("●");
      else                lcd.print("○");
    }
    else {
      sprintf(text, "%02u", parameter);
      lcd.print(text);
    }
    Blink();
    if (i >= 5) {
      DS3231_read();
      DS3231_display();
    }
    if ((!digitalRead(button1) && i < 5) || (!digitalRead(button3) && i >= 5)) {
      i++;                                      // Increment 'i' for the next parameter
      return parameter;                         // Return parameter value and exit
    }
  }
}

void loop() {
  if (!digitalRead(button1)) {                  // If B1 button is pressed
    i = 0;
    hour   = edit(6, 1, hour);
    minute = edit(9, 0, minute);
    while (!digitalRead(button1));            // Wait until button B1 released
    while (true) {
      while (!digitalRead(button2)) {         // If button B2 button is pressed
        day++;                                // Increment day
        if (day > 7) day = 1;
        calendar_display();                   // Call display_calendar function
        lcd.setCursor(0, 2);
        lcd.print(calendar);                  // Display calendar
        delay(200);
      }
      lcd.setCursor(0, 2);
      lcd.print("         ");                       // Print 9 spaces
      Blink();
      lcd.setCursor(0, 2);
      lcd.print(calendar);                    // Print calendar
      Blink();                                // Call Blink function
      if (!digitalRead(button1))              // If button B1 is pressed
        break;
    }
    date = edit(10, 2, date);                  // Edit date
    month = edit(13, 2, month);                // Edit month
    year = edit(18, 2, year);                 // Edit year
    // Convert decimal to BCD
    minute = ((minute / 10) << 4) + (minute % 10);
    hour = ((hour / 10) << 4) + (hour % 10);
    date = ((date / 10) << 4) + (date % 10);
    month = ((month / 10) << 4) + (month % 10);
    year = ((year / 10) << 4) + (year % 10);
    // End conversion
    // Write time & calendar data to DS3231 RTC
    Wire.beginTransmission(0x68);             // Start I2C protocol with DS3231 address
    Wire.write(0);                            // Send register address
    Wire.write(0);                            // Reset sesonds and start oscillator
    Wire.write(minute);                       // Write minute
    Wire.write(hour);                         // Write hour
    Wire.write(day);                          // Write day
    Wire.write(date);                         // Write date
    Wire.write(month);                        // Write month
    Wire.write(year);                         // Write year
    Wire.endTransmission();                   // Stop transmission and release the I2C bus
    delay(200);
  }
  if (!digitalRead(button3)) {                // If B3 button is pressed
    while (!digitalRead(button3));            // Wait until button B3 released
    i = 5;
    alarm1_hour   = edit(3,  3, alarm1_hour);
    alarm1_minute = edit(6,  3, alarm1_minute);
    alarm1_status = edit(8,  3, alarm1_status);
    i = 5;
    alarm2_hour   = edit(11,  3, alarm2_hour);
    alarm2_minute = edit(14,  3, alarm2_minute);
    alarm2_status = edit(16, 3, alarm2_status);
    alarm1_minute = ((alarm1_minute / 10) << 4) + (alarm1_minute % 10);
    alarm1_hour   = ((alarm1_hour   / 10) << 4) + (alarm1_hour % 10);
    alarm2_minute = ((alarm2_minute / 10) << 4) + (alarm2_minute % 10);
    alarm2_hour   = ((alarm2_hour   / 10) << 4) + (alarm2_hour % 10);
    // Write alarms data to DS3231
    Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
    Wire.write(7);                              // Send register address (alarm1 seconds)
    Wire.write(0);                              // Write 0 to alarm1 seconds
    Wire.write(alarm1_minute);                  // Write alarm1 minutes value to DS3231
    Wire.write(alarm1_hour);                    // Write alarm1 hours value to DS3231
    Wire.write(0x80);                           // Alarm1 when hours, minutes, and seconds match
    Wire.write(alarm2_minute);                  // Write alarm2 minutes value to DS3231
    Wire.write(alarm2_hour);                    // Write alarm2 hours value to DS3231
    Wire.write(0x80);                           // Alarm2 when hours and minutes match
    Wire.write(4 | alarm1_status | (alarm2_status << 1));      // Write data to DS3231 control register (enable interrupt when alarm)
    Wire.write(0);                              // Clear alarm flag bits
    Wire.endTransmission();                     // Stop transmission and release the I2C bus
    delay(200);                                 // Wait 200ms
  }
  if (!digitalRead(button2) && digitalRead(alarm_pin)) {       // When button B2 pressed with alarm (Reset and turn OFF the alarm)
    digitalWrite(alarm_pin, LOW);               // Turn OFF the alarm indicator
    Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
    Wire.write(0x0E);                           // Send register address (control register)
    // Write data to control register (Turn OFF the occurred alarm and keep the other as it is)
    Wire.write(4 | (!bitRead(status_reg, 0) & alarm1_status) | ((!bitRead(status_reg, 1) & alarm2_status) << 1));
    Wire.write(0);                              // Clear alarm flag bits
    Wire.endTransmission();                     // Stop transmission and release the I2C bus
  }
  DS3231_read();                                // Read time and calendar parameters from DS3231 RTC
  alarms_read_display();                        // Read and display alarms parameters
  DS3231_display();                             // Display time & calendar
  delay(50);                                    // Wait 50ms
}


/*
 *
 *
 *


############################ The code below uses the timer function rather than a delay ###############################

AC Light Control

 Updated by Robert Twomey

 Changed zero-crossing detection to look for RISING edge rather
 than falling.  (originally it was only chopping the negative half
 of the AC wave form).

 Also changed the dim_check() to turn on the Triac, leaving it on
 until the zero_cross_detect() turn's it off.

 Adapted from sketch by Ryan McLaughlin
 http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1230333861/30



#include  <TimerOne.h>          // Avaiable from http://www.arduino.cc/playground/Code/Timer1
volatile int i=0;               // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross=0;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 11;                // Output to Opto Triac
int dim = 0;                    // Dimming level (0-128)  0 = on, 128 = 0ff
int inc=1;                      // counting up or down, 1=up, -1=down

int freqStep = 75;    // This is the delay-per-brightness step in microseconds.
                      // For 60 Hz it should be 65
// It is calculated based on the frequency of your voltage supply (50Hz or 60Hz)
// and the number of brightness steps you want.
//
// Realize that there are 2 zerocrossing per cycle. This means
// zero crossing happens at 120Hz for a 60Hz supply or 100Hz for a 50Hz supply.

// To calculate freqStep divide the length of one full half-wave of the power
// cycle (in microseconds) by the number of brightness steps.
//
// (120 Hz=8333uS) / 128 brightness steps = 65 uS / brightness step
// (100Hz=10000uS) / 128 steps = 75uS/step

void setup() {                                      // Begin setup
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  attachInterrupt(0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);
  // Use the TimerOne Library to attach an interrupt
  // to the function we use to check to see if it is
  // the right time to fire the triac.  This function
  // will now run every freqStep in microseconds.
}

void zero_cross_detect() {
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
}

// Turn on the TRIAC at the appropriate time
void dim_check() {
  if(zero_cross == true) {
    if(i>=dim) {
      digitalWrite(AC_pin, HIGH); // turn on light
      i=0;  // reset time step counter
      zero_cross = false; //reset zero cross detection
    }
    else {
      i++; // increment time step counter
    }
  }
}

void loop() {
  dim+=inc;
  if((dim>=128) || (dim<=0))
    inc*=-1;
  delay(18);
}



################################### AC Voltage dimmer with Zero cross detection #######################################


AC Voltage dimmer with Zero cross detection
Author: Charith Fernanado Adapted by DIY_bloke
License: Creative Commons Attribution Share-Alike 3.0 License.
Attach the Zero cross pin of the module to Arduino External Interrupt pin
Select the correct Interrupt # from the below table
(the Pin numbers are digital pins, NOT physical pins:
digital pin 2 [INT0]=physical pin 4 and digital pin 3 [INT1]= physical pin 5)
check: <a href="http://arduino.cc/en/Reference/attachInterrupt">interrupts</a>

Pin    |  Interrrupt # | Arduino Platform
---------------------------------------
2      |  0            |  All -But it is INT1 on the Leonardo
3      |  1            |  All -But it is INT0 on the Leonardo
18     |  5            |  Arduino Mega Only
19     |  4            |  Arduino Mega Only
20     |  3            |  Arduino Mega Only
21     |  2            |  Arduino Mega Only
0      |  0            |  Leonardo
1      |  3            |  Leonardo
7      |  4            |  Leonardo
The Arduino Due has no standard interrupt pins as an iterrupt can be attached to almosty any pin.

In the program pin 2 is chosen


int AC_LOAD = 3;    // Output to Opto Triac pin
int dimming = 128;  // Dimming level (0-128)  0 = ON, 128 = OFF

void setup()
{
  pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
  attachInterrupt(0, zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
}

//the interrupt function must take no parameters and return nothing
void zero_crosss_int()  //function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle)
  // For 60Hz => 8.33ms (10.000/120)
  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65

  int dimtime = (75*dimming);    // For 60Hz =>65
  delayMicroseconds(dimtime);    // Wait till firing the TRIAC
  digitalWrite(AC_LOAD, HIGH);   // Fire the TRIAC
  delayMicroseconds(10);         // triac On propogation delay
         // (for 60Hz use 8.33) Some Triacs need a longer period
  digitalWrite(AC_LOAD, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
}

void loop()  {
  for (int i=5; i <= 128; i++){
    dimming=i;
    delay(10);
   }
}





############################################## DHT11 ###############################################################

// Interfacing Arduino with DHT11 humidity and temperature sensor

// include LCD library code
#include <LiquidCrystal.h>
// include DHT library code
#include "DHT.h"

#define DHTPIN 13            // DHT11 data pin is connected to Arduino pin 13

// LCD module connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

#define DHTTYPE DHT11       // DHT11 sensor is used
DHT dht(DHTPIN, DHTTYPE);   // Initialize DHT library

char temperature[] = "Temp = 00.0 C  ";
char humidity[]    = "RH   = 00.0 %  ";
void setup() {
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  dht.begin();
}

void loop() {
  delay(1000);           // wait 1s between readings
  // Read humidity
  byte RH = dht.readHumidity();
  //Read temperature in degree Celsius
  byte Temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(RH) || isnan(Temp)) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Error");
    return;
  }

  temperature[7]     = Temp / 10 + 48;
  temperature[8]     = Temp % 10 + 48;
  temperature[11]    = 223;
  humidity[7]        = RH / 10 + 48;
  humidity[8]        = RH % 10 + 48;
  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print(humidity);
}

####################################################################################################################



 */
