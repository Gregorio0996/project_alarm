#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <Wire.h>
#include <Servo.h>

//DS3231 address bus:
#define DS3231_I2C_ADDRESS 0x68
#define DS3231_TEMPERATURE_ADDR 0x11

//Date:
#define YEAR 0
#define MONTH 1
#define DAYS 2
#define WEEKDAY 3

//weekday state:
#define MIN 0
#define SEN 1
#define SEL 2
#define RAB 3
#define KAM 4
#define JUM 5
#define SAB 6

//Time:
#define HOURS 0
#define MINUTES 1
#define SECONDS 2
#define ACTIVATED 3

//menu list:
#define IDLEMENU 0
#define SETDATE 1
#define SETTIME 2
#define MENU_KIPAS 3
#define MENU_JENDELA 4
#define SET_ALARM 5
#define SET_ALARM_ON 6
#define SHOW 7
#define DONE 8

#define KIPAS 26

// The LCD screen
LCDKeypad lcd;
Servo jendela;

// The time model
byte year = 0;
byte month = 1;
byte days = 1;
byte hours = 0;
byte minutes = 0;
byte seconds = 0;
byte weekday = 0;
byte datesetting = 0;
byte timesetting = 0;
byte menustate = 0;
byte al_hour = 0;
byte al_min = 0;
byte al_sec = 0;
byte al_hour_on = 0;
byte al_min_on = 0;
byte al_sec_on = 0;
byte pp = 3;
String stat_alarm = "OFF";
String stat_alarm2 = "OFF";
bool act_alarm = false;
bool act_alarm2 = false;
bool act_kipas = false;
bool act_jendela = false;

int buzzerPin = 22;
unsigned int LDR;

void setup() {
  //power up ZS-042
  Serial.begin(9600);
  jendela.attach(24);
  pinMode(KIPAS, OUTPUT);
  pinMode(53, OUTPUT);
  pinMode (buzzerPin, OUTPUT);
  digitalWrite(53, HIGH);
  //initialize I2C and lcd:
  lcd.begin(16, 2);
  Wire.begin();
  readDS3231time(&seconds, &minutes, &hours, &weekday, &days, &month, &year);
}

void loop() {
  // Increase the time model by one second
  incTime();
  //readDS3231time(&seconds, &minutes, &hours, &weekday, &days, &month, &year);
  // Print the time on the LCD
  printTime();
  // Listen for buttons for 1 second
  buttonListen();

}

void buttonListen() {
  // Read the buttons five times in a second
  for (int i = 0; i < 9; i++) {

    // Read the buttons value
    int button = lcd.button();
    switch (menustate) {
      case SETDATE:
        switch (button) {
          // Right button was pushed
          case KEYPAD_RIGHT:
            datesetting++;
            break;

          // Left button was pushed
          case KEYPAD_LEFT:
            datesetting--;
            if (datesetting == -1) datesetting = 3;
            break;

          // Up button was pushed
          case KEYPAD_UP:
            switch (datesetting) {
              case YEAR:
                year++;
                break;
              case MONTH:
                month++;
                break;
              case DAYS:
                days++;
                break;
              case WEEKDAY:
                weekday++;
            }
            break;

          // Down button was pushed
          case KEYPAD_DOWN:
            switch (datesetting) {
              case YEAR:
                year--;
                if (year == -1) year = 99;
                break;
              case MONTH:
                month--;
                if (month == 0) month = 12;
                break;
              case DAYS:
                days--;
                if (days == 0) days = 31;
                break;
              case WEEKDAY:
                weekday--;
                if (weekday == 0) days = 6;
            }
        }
        break;
      case SETTIME:
        switch (button) {
          // Right button was pushed
          case KEYPAD_RIGHT:
            timesetting++;
            break;

          // Left button was pushed
          case KEYPAD_LEFT:
            timesetting--;
            if (timesetting == -1) timesetting = 2;
            break;

          // Up button was pushed
          case KEYPAD_UP:
            switch (timesetting) {
              case HOURS:
                hours++;
                break;
              case MINUTES:
                minutes++;
                break;
              case SECONDS:
                seconds++;
            }
            break;

          // Down button was pushed
          case KEYPAD_DOWN:
            switch (timesetting) {
              case HOURS:
                hours--;
                if (hours == -1) hours = 23;
                break;
              case MINUTES:
                minutes--;
                if (minutes == -1) minutes = 59;
                break;
              case SECONDS:
                seconds--;
                if (seconds == -1) seconds = 59;
            }
        }
        break;

      case SET_ALARM:
        switch (button) {
          case KEYPAD_SELECT:
            setDS3231time(seconds, minutes, hours, weekday, days, month, year);
            break;

          // Right button was pushed
          case KEYPAD_RIGHT:
            timesetting++;
            break;

          // Left button was pushed
          case KEYPAD_LEFT:
            timesetting--;
            if (timesetting == -1) timesetting = 3;
            break;

          // Up button was pushed
          case KEYPAD_UP:
            switch (timesetting) {
              case HOURS:
                al_hour++;
                break;
              case MINUTES:
                al_min++;
                break;
              case SECONDS:
                break;
              case ACTIVATED:
                stat_alarm = "ON";
                act_alarm = true;

            }
            break;

          // Down button was pushed
          case KEYPAD_DOWN:
            switch (timesetting) {
              case HOURS:
                al_hour--;
                if (al_hour == -1) al_hour = 23;
                break;
              case MINUTES:
                al_min--;
                if (al_min == -1) al_min = 59;
              case SECONDS:
                break;
              case ACTIVATED:
                stat_alarm = "OFF";
                act_alarm = false;
            }
        }
        break;

      case SET_ALARM_ON:
        switch (button) {
          case KEYPAD_SELECT:
            setDS3231time(seconds, minutes, hours, weekday, days, month, year);
            break;

          // Right button was pushed
          case KEYPAD_RIGHT:
            timesetting++;
            break;

          // Left button was pushed
          case KEYPAD_LEFT:
            timesetting--;
            if (timesetting == -1) timesetting = 3;
            break;

          // Up button was pushed
          case KEYPAD_UP:
            switch (timesetting) {
              case HOURS:
                al_hour_on++;
                break;
              case MINUTES:
                al_min_on++;
                break;
              case SECONDS:
                break;
              case ACTIVATED:
                stat_alarm2 = "ON";
                act_alarm2 = true;

            }
            break;

          // Down button was pushed
          case KEYPAD_DOWN:
            switch (timesetting) {
              case HOURS:
                al_hour_on--;
                if (al_hour_on == -1) al_hour_on = 23;
                break;
              case MINUTES:
                al_min_on--;
                if (al_min_on == -1) al_min_on = 59;
              case SECONDS:
                break;
              case ACTIVATED:
                stat_alarm2 = "OFF";
                act_alarm2 = false;
            }
        }
        break;


      case MENU_KIPAS:
        lcd.clear();
        if (button == KEYPAD_UP && act_kipas == false) {
          kipasnyala();
          act_kipas = true;
        }
        else if (button == KEYPAD_UP && act_kipas == true) {
          kipasmati();
          act_kipas = false;
        }
        break;


      case MENU_JENDELA:
        lcd.clear();
        if (button == KEYPAD_DOWN && act_jendela == false) {
          jendelabuka();
          act_jendela = true;
        }
        else if (button == KEYPAD_DOWN && act_jendela == true) {
          jendelatutup();
          act_jendela = false;
        }
        break;

    }
    if (button == KEYPAD_SELECT) {
      menustate++;
    }
    datesetting %= 4;
    timesetting %= 4;
    menustate %= 8;
    printSetting();

    year %= 100;
    month %= 13;
    days %= 32;
    hours %= 24;
    al_hour %= 24;
    al_min %= 60;
    al_hour_on %= 24;
    al_min_on %= 60;
    minutes %= 60;
    seconds %= 60;
    weekday %= 7;
    printTime();

    // Wait one fifth of a second to complete
    while (millis() % 125 != 0);
  }
}


void printSetting() {
  int button = lcd.button();
  int tempC = DS3231_get_treg();
  char time[17];
  switch (menustate) {
    case SETDATE:
      lcd.setCursor(0, 0);
      lcd.print("SETTING:        ");
      lcd.setCursor(9, 0);
      switch (datesetting) {
        case YEAR:
          lcd.print("YEAR   ");
          break;
        case MONTH:
          lcd.print("MONTH  ");
          break;
        case DAYS:
          lcd.print("Days   ");
          break;
        case WEEKDAY:
          lcd.print("WeekDay");
      }
      break;
    case SETTIME:
      lcd.setCursor(0, 0);
      lcd.print("SETTING:        ");
      lcd.setCursor(9, 0);
      switch (timesetting) {
        case HOURS:
          lcd.print("Hours  ");
          break;
        case MINUTES:
          lcd.print("Minutes");
          break;
        case SECONDS:
          lcd.print("Seconds");
      }
      break;
    case SET_ALARM:
      lcd.setCursor(0, 0);
      lcd.print("SET OFF:        ");
      lcd.setCursor(9, 0);
      switch (timesetting) {
        case HOURS:
          lcd.print("Hours  ");
          break;
        case MINUTES:
          lcd.print("Minutes");
        case SECONDS:
          break;
        case ACTIVATED:
          lcd.print("Status");
          if (button == KEYPAD_UP) {
            lcd.setCursor(10 , 1);
            lcd.print("ON");
            delay(1000);
          }
          else if (button == KEYPAD_DOWN) {
            lcd.setCursor(10 , 1);
            lcd.print("OFF");
            delay(1000);
          }
      }
      break;

    case SET_ALARM_ON:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET ON:        ");
      lcd.setCursor(9, 0);
      switch (timesetting) {
        case HOURS:
          lcd.print("Hours  ");
          break;
        case MINUTES:
          lcd.print("Minutes");
        case SECONDS:
          break;
        case ACTIVATED:
          lcd.print("Status");
          if (button == KEYPAD_UP) {
            lcd.setCursor(10 , 1);
            lcd.print("ON");
            delay(1000);
          }
          else if (button == KEYPAD_DOWN) {
            lcd.setCursor(10 , 1);
            lcd.print("OFF");
            delay(1000);
          }
      }
      break;

    case MENU_KIPAS:
      lcd.setCursor(0, 0);
      lcd.print("ATUR KIPAS:        ");
      lcd.setCursor(0 , 1);
      if (button == KEYPAD_UP && act_kipas == true) {
        lcd.setCursor(0 , 1);
        lcd.print("ON");

      }
      else if (button == KEYPAD_UP && act_kipas == false) {
        lcd.setCursor(0 , 1);
        lcd.print("OFF");

      }
      break;


    case MENU_JENDELA:
      lcd.setCursor(0, 0);
      lcd.print("ATUR JENDELA:        ");
      lcd.setCursor(0 , 1);
      if (button == KEYPAD_DOWN && act_jendela == true) {
        lcd.setCursor(0 , 1);
        lcd.print("BUKA");

      }
      else if (button == KEYPAD_DOWN && act_jendela == false) {
        lcd.setCursor(0 , 1);
        lcd.print("TUTUP");

      }
      break;
    case IDLEMENU:
      LDR = analogRead(A8);
      if (hours == al_hour && minutes == al_min && act_alarm == true) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("ALARM OFF IS");
        lcd.setCursor(1 , 1);
        lcd.print(" ACTIVE");
        kipasmati();
        jendelatutup();
        for (int i = 0; i < 20; i++) {
          int button = lcd.button();
          if (button == KEYPAD_DOWN) {
            stat_alarm = "OFF";
            act_alarm = false;
            digitalWrite(buzzerPin, LOW);
            break;

          }
          bunyi();
        }
        if (act_alarm == true) {
          lcd.print("POSTPONE");
          act_alarm = false;
          delay (400);
          act_alarm = true;
          al_min = al_min + pp;
        }
      }


      if (hours == al_hour_on && minutes == al_min_on && act_alarm2 == true) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("ALARM ON IS");
        lcd.setCursor(1 , 1);
        lcd.print(" ACTIVE");
        kipasnyala();
        jendelabuka();
        for (int i = 0; i < 20; i++) {
          int button = lcd.button();
          if (button == KEYPAD_DOWN) {
            stat_alarm2 = "OFF";
            act_alarm2 = false;
            digitalWrite(buzzerPin, LOW);
            break;

          }
          bunyi();
        }
        if (act_alarm2 == true) {
          lcd.print("POSTPONE");
          act_alarm2 = false;
          delay (400);
          act_alarm2 = true;
          al_min_on = al_min_on + pp;
        }
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      sprintf(time, "%02i:%02i:%02i        ", hours, minutes, seconds);
      lcd.print(time);
      lcd.setCursor(12, 0);
      lcd.print(tempC);
      lcd.print((char)223);
      lcd.print("C ");
      lcd.setCursor(9, 1);
      lcd.print(stat_alarm);
      lcd.setCursor(13, 1);
      lcd.print(stat_alarm2);
      lcd.setCursor(0, 1);
      sprintf(time, "%02i/%02i/%02i", year, month, days);
      lcd.print(time);
      if (stat_alarm == "OFF" && act_alarm == false && stat_alarm2 == "OFF" && act_alarm2 == false ) {
        if (LDR > 200) {
          jendelatutup();
        }
        else if (LDR < 200) {
          jendelabuka();
        }
      }
      break;

  }
}


void incTime() {
  // Increase seconds
  seconds++;

  if (seconds == 60) {
    // Reset seconds
    seconds = 0;

    // Increase minutes
    minutes++;

    if (minutes == 60) {
      // Reset minutes
      minutes = 0;

      // Increase hours
      hours++;

      if (hours == 24) {
        // Reset hours
        hours = 0;

        // Increase days
        days++;
        weekday++;
      }
    }
  }
}

// Print the time on the LCD
void printTime() {
  char time[17];
  //check which state right now:
  switch (menustate) {
    case SETDATE:
      lcd.setCursor(0, 1);
      switch (weekday) {
        case MIN:
          sprintf(time, "%02i/%02i/%02i     MIN", year, month, days);
          break;
        case SEN:
          sprintf(time, "%02i/%02i/%02i     SEN", year, month, days);
          break;
        case SEL:
          sprintf(time, "%02i/%02i/%02i     SEL", year, month, days);
          break;
        case RAB:
          sprintf(time, "%02i/%02i/%02i     RAB", year, month, days);
          break;
        case KAM:
          sprintf(time, "%02i/%02i/%02i     KAM", year, month, days);
          break;
        case JUM:
          sprintf(time, "%02i/%02i/%02i     JUM", year, month, days);
          break;
        case SAB:
          sprintf(time, "%02i/%02i/%02i     SAB", year, month, days);
      }
      lcd.print(time);
      break;
    case SETTIME:
      lcd.setCursor(0, 1);
      sprintf(time, "%02i:%02i:%02i        ", hours, minutes, seconds);
      lcd.print(time);
      break;
    case SET_ALARM:
      lcd.setCursor(0, 1);
      sprintf(time, "%02i:%02i:%02i        ", al_hour, al_min, al_sec);
      lcd.print(time);
      break;
    case SET_ALARM_ON:
      lcd.setCursor(0, 1);
      sprintf(time, "%02i:%02i:%02i        ", al_hour_on, al_min_on, al_sec_on);
      lcd.print(time);
      break;

  }
  // Set the cursor at the begining of the second row

}
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (0=Sunday, 6=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

float DS3231_get_treg()
{
  int rv;  // Reads the temperature as an int, to save memory
  //  float rv;

  uint8_t temp_msb, temp_lsb;
  int8_t nint;

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(DS3231_TEMPERATURE_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
  temp_msb = Wire.read();
  temp_lsb = Wire.read() >> 6;

  if ((temp_msb & 0x80) != 0)
    nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
  else
    nint = temp_msb;

  rv = 0.25 * temp_lsb + nint;

  return rv;
}

void bunyi() {
  digitalWrite (buzzerPin, HIGH);
  delay (100);
  digitalWrite (buzzerPin, LOW);
  delay (1000);
}

void kipasnyala() {
  digitalWrite(KIPAS, 1);
}

void kipasmati() {
  digitalWrite(KIPAS, 0);
}

void jendelabuka() {
  jendela.write(20);
}

void jendelatutup() {
  jendela.write(140);
}
