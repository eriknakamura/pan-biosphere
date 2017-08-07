
/*

██████╗  █████╗ ███╗   ██╗    ██████╗ ██╗ ██████╗ ███████╗██████╗ ██╗  ██╗███████╗██████╗ ███████╗
██╔══██╗██╔══██╗████╗  ██║    ██╔══██╗██║██╔═══██╗██╔════╝██╔══██╗██║  ██║██╔════╝██╔══██╗██╔════╝
██████╔╝███████║██╔██╗ ██║    ██████╔╝██║██║   ██║███████╗██████╔╝███████║█████╗  ██████╔╝█████╗  
██╔═══╝ ██╔══██║██║╚██╗██║    ██╔══██╗██║██║   ██║╚════██║██╔═══╝ ██╔══██║██╔══╝  ██╔══██╗██╔══╝  
██║     ██║  ██║██║ ╚████║    ██████╔╝██║╚██████╔╝███████║██║     ██║  ██║███████╗██║  ██║███████╗
╚═╝     ╚═╝  ╚═╝╚═╝  ╚═══╝    ╚═════╝ ╚═╝ ╚═════╝ ╚══════╝╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝

  ____          ______      _ _      _   _       _                                   
 |  _ \        |  ____|    (_) |    | \ | |     | |                                  
 | |_) |_   _  | |__   _ __ _| | __ |  \| | __ _| | ____ _ _ __ ___  _   _ _ __ __ _ 
 |  _ <| | | | |  __| | '__| | |/ / | . ` |/ _` | |/ / _` | '_ ` _ \| | | | '__/ _` |
 | |_) | |_| | | |____| |  | |   <  | |\  | (_| |   < (_| | | | | | | |_| | | | (_| |
 |____/ \__, | |______|_|  |_|_|\_\ |_| \_|\__,_|_|\_\__,_|_| |_| |_|\__,_|_|  \__,_|
         __/ |                                                                       
        |___/   
        
        2017
*/

//TODO clean up code!


//DS18B20 dependencies
#include "OneWire.h"


//RGB control dependencies
#include <rgb-controls.h>
using namespace RGBControls;


#include <Adafruit_DHT.h>
#include <blynk.h>

// LCD library dependencies
#include "application.h"
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C   *lcd;


//BLYNK auth token
char auth[] = "xxxxxxxxxxxxx";

// Define ALL pins here!!!
#define HUM_FRONT A0
//#define TEMP_SUMP 4
#define LIGHT_SUMP D7 
#define LIGHT_MAIN D6
#define BLYNK_PRINT Serial
#define RE_RGB_BLUE A5
#define RE_RGB_RED A4
#define RE_RGB_GREEN A7
#define RE_RGB_SWITCH D3
#define RE_RGB_A D4
#define RE_RGB_B D5
//#define ONE_WIRE_BUS 
//LCD SDA D0 non changeable except in lib
//LCD SCL D1 non chaneable except in lib

// Define ALL i2C buses here!!!
#define BUS_LCD 0x3F

// Other definitions go here!!!
#define DHT_TYPE DHT11

/*
ALERTS!
*/
/*
FEED - twice a week, every monday and thursday at 19
DOSE_FLOURISH - once per week, saturday nights at 19
PLANTS_WATER - twice a week, every sunday and wednesday at 19
*/
//Define alert names, urgencies, and what day ([1,7] range) and time ((1,24] hour range) they should be called on.
/*
String alertName[6] = {FEED, DOSE_FLOURISH, WATER_LEVEL_LOW, WATER_TEMP_HIGH, WATER_TEMP_LOW, PLANTS_WATER};
int alertUrgency[6] = {2,1,5,5,5,3};
int FEEDdays[2] = {2,5};
int DOSE_FLOURISHdays[1] = {7};
int PLANTS_WATERdays[2] = {1,4};
int FEEDhour = 19;
int DOSE_FLOURISHhour = 19;
int PLANTS_WATERhour = 19;
int WATER_TEMP_LOWthreshold = 65; //in degreen farenhight
int WATER_TEMP_HIGHthreshold = 85; 
bool WATER_LEVEL_LOWswitch = false; //from the float switch
*/

//Function Declarations
void doEncoderA();
void doEncoderB();

//Rotary Encoder values
volatile bool A_set = false;
volatile bool B_set = false;
volatile int encoderPos = 0;
int prevPos = 0;
int value = 0;

//RGB Setup
Led led(RE_RGB_RED, RE_RGB_GREEN, RE_RGB_BLUE, false); //Common annode so false as last peram.
bool buttonPress = false;
//Preset Colors!
Color red(255, 0, 0);
Color blue(0, 0, 255);
Color green(0,255,0);
Color purple(167, 66, 244);
Color pink(244, 65, 184);
Color yellow(241, 244, 66);
Color healthyStatus[2] {blue, green};
Color alertStatus[2] {yellow, purple};
Color cloudErrorStatus[2] {red, purple};

//Some GUI Values
int upDateDay;
int upDateMonth;
int upDateYear;
bool btnRead = false; // holds the value of the button
bool oldBtnRead = false; // holds the old value of the button
bool myClick = false; 
int encoderCounter = 0;
int encoderCounterExtra = 0;
int prevEncoderCounterExtra;
int knobMin = 10;
int knobMax = 13;
bool clearScreen = false;
bool onHomeScreen = true;
int timeOffHomeScreen = 0;
int homeScreenReturnTime = 60*5; //five minutes. This is the time in which the GUI will return to the homescreen if left.
bool settingsNextMenu = false;

//System Values
String mode = "Dflt";
float dhtHumid = 0;
float dhtTemp = 0;
bool sumpLight = false;
bool mainLight = false;
bool sumpLightSwitch = false;
bool mainLightSwitch = false;
float sumpTempF;
float sumpTempC;
String lightExpirationSettingLight = "sump";

//System Timing Values
int sumpOnHr = 6;
int sumpOnMin = 0;
int sumpOffHr = 22;
int sumpOffMin = 0;
int mainOnHr = 8;
int mainOnMin = 0;
int mainOffHr = 22;
int mainOffMin = 0;
int sumpSetTime = 0;
int mainSetTime = 0;
int sumpExpirationTimeMin = 60;
int sumpExpirationTimeSec = 0;
int mainExpirationTimeMin = 60;
int mainExpirationTimeSec = 0;
int sumpExpirationTime = 60*60; //one hour in seconds as default
int mainExpirationTime = 60*60;
bool sumpSwitchValidation = false;
bool mainSwitchValidation = false;

//DS18B20 stuff



//DHT stuff
DHT dht(HUM_FRONT, DHT_TYPE);

void setup() {
    float DSTOffset = Time.getDSTOffset();
    if(Time.isDST()){
        Time.zone(-4 + DSTOffset);
    }
    else{
        Time.zone(-4);
    }
    
   
    
    Blynk.begin(auth);
    dht.begin();
    pinMode(LIGHT_SUMP, OUTPUT);
    pinMode(LIGHT_MAIN, OUTPUT);
    //pinMode(ONE_WIRE_BUS, INPUT);
    pinMode(RE_RGB_SWITCH, INPUT);      // active HIGH logic (not pullup)
    pinMode(RE_RGB_A, INPUT_PULLUP);
    pinMode(RE_RGB_B, INPUT_PULLUP);
    attachInterrupt(RE_RGB_A, doEncoderA, CHANGE);
    attachInterrupt(RE_RGB_B, doEncoderB, CHANGE);
    upDateDay = Time.day();
    upDateMonth = Time.month();
    upDateYear = Time.year();
    
    lcd = new LiquidCrystal_I2C(BUS_LCD, 20, 4);
    lcd->init();                      // initialize the lcd
    lcd->backlight();
    lcd->clear();
    
}

int prevEncoderCounter = 0;
int clickCounter = 0;
bool encoderLock = false;


void loop() {
    
int currentHour = Time.hour();
int currentMinute = Time.minute();
    
Blynk.run();

//Enter healthy status at startup
if(buttonPress == false) led.fade(healthyStatus, 2, 1300);

//If the photon loses connectinon, enter cloud error status on encoder
if(!Particle.connected() && !buttonPress){
    led.flash(cloudErrorStatus, 2, 200, 300);
    
}

//Declare local variables
//Wifi stuff
int wifiStrength = map(WiFi.RSSI(), -100, 0, 0, 100);
const char* networkSSID = WiFi.SSID();
String network(networkSSID);
//DHT stuff
dhtHumid = dht.getHumidity();
dhtTemp = dht.getTempFarenheit();
//DS18B20 stuff

//Set expiration times early
sumpExpirationTime = (sumpExpirationTimeMin * 60) + sumpExpirationTimeSec;
mainExpirationTime = (mainExpirationTimeMin * 60) + mainExpirationTimeSec;

//Return to home screen if away for too long
if(Time.now() - homeScreenReturnTime < timeOffHomeScreen){
    encoderCounter = 0;
    onHomeScreen = true;
}
/*
Light Control
*/
//first update switch validation
if(Time.now() - sumpSetTime < sumpExpirationTime){
    sumpSwitchValidation = true;
}
else{
    sumpSwitchValidation = false;
    //set set time to 0 to prevent memory issues?
}
if(Time.now() - mainSetTime < mainExpirationTime){
    mainSwitchValidation = true;
}
else{
    mainSwitchValidation = false;
    //set set time to 0 to prevent memory issues?
}

//First check if light is within on time. Then within that, check if there are any valid switches.
//for the sump light
if(sumpOnHr*60 + sumpOnMin < currentHour*60 + currentMinute && sumpOffHr*60 + sumpOffMin > currentHour*60 + currentMinute){
    if(!sumpSwitchValidation){
        sumpLight = true;
    }
    else if(sumpSwitchValidation){
        sumpLight = false;
    }
}
else if(sumpSwitchValidation){
    sumpLight = true;
}
else{
    sumpLight = false;
}
//for the main light
if(mainOnHr*60 + mainOnMin < currentHour*60 + currentMinute && mainOffHr*60 + mainOffMin > currentHour*60 + currentMinute){
    if(!mainSwitchValidation){
        mainLight = true;
    }
    else if(mainSwitchValidation && !mainLightSwitch){
        mainLight = false;
    }
}
else if(mainSwitchValidation && mainLightSwitch){
    mainLight = true;    
}
else{
    mainLight = false;
}

//now toggle pin states due to logic above
if(sumpLight){
    digitalWrite(LIGHT_SUMP, LOW);
}
else{
    digitalWrite(LIGHT_SUMP, HIGH);
}
if(mainLight){
    digitalWrite(LIGHT_MAIN, LOW);
}
else{
    digitalWrite(LIGHT_MAIN, HIGH);
}


//Smooth out encoder value with some simple stuff.
prevEncoderCounterExtra = encoderCounterExtra;
if (prevPos != encoderPos) {
      if(prevPos > encoderPos && encoderCounter >=10){
           //The encoder value went down
           if(!encoderLock){
               encoderCounter --;
           }
           encoderCounterExtra --;
           if(encoderCounter < knobMin) encoderCounter = knobMax;
           if(encoderCounter > knobMax) encoderCounter = knobMin;
      }
      else if(prevPos < encoderPos && encoderCounter >=10){
          //The encoder value went up
          if(!encoderLock){
                encoderCounter ++;
          }
          encoderCounterExtra ++;
          if(encoderCounter < knobMin) encoderCounter = knobMax;
          if(encoderCounter > knobMax) encoderCounter = knobMin;
      }
        prevPos = encoderPos;
    }

//Read the button on the RGB encoder with some software debouncing
if(digitalRead(RE_RGB_SWITCH) == HIGH){
      buttonPress = true;
      led.off();
      led.setColor(yellow);
      btnRead = true;
  }
  else{
      buttonPress = false;
      btnRead = false;
  }
  
if ( btnRead != oldBtnRead){
    if (btnRead == true){
        myClick = true;
    }
}
oldBtnRead = btnRead;

if(clearScreen){
    clearScreen = false;
    lcd->clear();
}


/*
All GUI switch cases
*/
switch (encoderCounter){
    case 0: //Home template
        homeTemplate(mode,sumpTempC,sumpTempF,dhtTemp,dhtHumid);
        if(myClick){
            myClick = false;
            clearScreen = true;
            encoderCounter = 10;
            knobMin = 10;
            knobMax = 13;
            timeOffHomeScreen = Time.now();
            onHomeScreen = false;
        }
        break;
    case 10: //main menu arrow one
        mainMenuTemplate();
        arrowOne();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 20;
            knobMin = 20;
            knobMax = 23;
        }
        break;
    case 11: //main menu arrow two
        mainMenuTemplate();
        arrowTwo();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 30;
            //clear lcd
            knobMin = 30;
            knobMax = 30;
        }
        break;
    case 12: //main menu arrow three
        mainMenuTemplate();
        arrowThree();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 40;
            //clear lcd
            knobMin = 40;
            knobMax = 47;
        }
        break;
    case 13: //main menu back arrow
        mainMenuTemplate();
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 0;
            knobMin = 10;
            knobMax = 13;
            onHomeScreen = true;
            timeOffHomeScreen = 0;
        }
        break;
    case 20:
        controlTemplate();
        arrowOne();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 50;
            knobMin = 50;
            knobMax = 52;
        }
        break;
    case 21:
        controlTemplate();
        arrowTwo();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 60;
            knobMin = 60;
            knobMax = 62;
        }
        break;
    case 22:
        controlTemplate();
        arrowThree();
        break;
    case 23:
        controlTemplate();
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 10;
            knobMin = 10;
            knobMax = 13;
        }
        break;
    case 30:
        statusesTemplate(wifiStrength, network, upDateMonth, upDateDay, upDateYear);
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 10;
            knobMin = 10;
            knobMax = 13;
        }
        break;
    case 40:
        settingsOneTemplate();
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 10;
            knobMin = 10;
            knobMax = 13;
        }
        break;
    case 41:
        settingsOneTemplate();
        arrowOne();
        break;
    case 42:
        settingsOneTemplate();
        arrowTwo();
        break;
    case 43: //Light validation time setting
        settingsOneTemplate();
        if(settingsNextMenu){
            settingsNextMenu = false;
            clearScreen = true;
        }
        break;
    case 44:
        if(!settingsNextMenu){
        settingsNextMenu = true;
        clearScreen = true;
        }
        settingsTwoTemplate();
        arrowZero();
        if(myClick){
            myClick = false;
            clearScreen = true;
            encoderCounter = 90;
            knobMin = 90;
            knobMax = 92;
            
        }
        arrowZero();
        break;
    case 45:
        settingsTwoTemplate();
        arrowOne();
        break;
    case 46:
        settingsTwoTemplate();
        arrowTwo();
        break;
    case 47:
        settingsTwoTemplate();
        arrowThree();
        break;
    case 50:
    //Sump light control
        lightOnOffTemplate();
        arrowOne();
        if(myClick){
            myClick = false;
            sumpLightSwitch = !sumpLightSwitch;
            sumpSetTime = Time.now();
            //    Blynk.virtualWrite(V0, sumpLight);
        }
        break;
    case 51:
    //Main light control
        lightOnOffTemplate();
        arrowTwo();
        if(myClick){
            myClick = false;
            mainLightSwitch = !mainLightSwitch;
            mainSetTime = Time.now();
            //    Blynk.virtualWrite(V1, mainLight);
        }
        break;
    case 52:
        arrowBack();
        lightOnOffTemplate();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 10;
            knobMin = 10;
            knobMax = 13;
        }
        break;
    case 60:
        //Sump light cycle set
        lightCyclesTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin, mainOnHr,  mainOnMin, mainOffHr, mainOffMin);
        arrowOne();
        if(myClick){
            myClick = false;
            clearScreen = true;
            encoderCounter = 72;
            knobMin = 70;
            knobMax = 72;
        }
        break;
        
    case 61:
        //Main light cycles set
        lightCyclesTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin, mainOnHr,  mainOnMin, mainOffHr, mainOffMin);
        arrowTwo();
        if(myClick){
            myClick = false;
            clearScreen = true;
            encoderCounter = 82;
            knobMin = 80;
            knobMax = 82;
            
        }
        break;
        
    case 62:
        lightCyclesTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin, mainOnHr,  mainOnMin, mainOffHr, mainOffMin);
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 23; //quick backups
            knobMin = 21;
            knobMax = 23;
        }
        break;
    case 70: //Set sump light cycles on time
        setLightCyclesSumpTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin);
        arrowOne();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here for making sure sump on hr is bounded between 1 and 24
                if(sumpOnHr < 24){
                    sumpOnHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here for making sure sump on hr is bounded between 1 and 24
                if(sumpOnHr > 1){
                    sumpOnHr--;
                }
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here to make sure sump on min is bound between 0 and 59, and if floods over 59, increment sump on hr up and set minutes to 0.
                if(sumpOnMin < 59){
                 sumpOnMin++;   
                }
                else if(sumpOnMin == 59){
                    sumpOnMin = 0;
                    sumpOnHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here to make sure sump on min is bound between 0 and 59, and if floods below 0, decrement sump on hr down and set sump on min to 59
                if(sumpOnMin > 0){
                    sumpOnMin--;
                }
                else if(sumpOnMin == 0){
                    sumpOnMin = 59;
                    sumpOnHr--;
                }
            }
        }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 71;
            clearScreen = true;
        }
        
        break;
    case 71: //Set sump light cycles off time
        setLightCyclesSumpTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin);
        arrowTwo();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here for making sure sump on hr is bounded between 1 and 24
                if(sumpOffHr < 24){
                    sumpOffHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here for making sure sump on hr is bounded between 1 and 24
                if(sumpOffHr > 1){
                    sumpOffHr--;
                }
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here to make sure sump on min is bound between 0 and 59, and if floods over 59, increment sump on hr up and set minutes to 0.
                if(sumpOffMin < 59){
                 sumpOffMin++;   
                }
                else if(sumpOffMin == 59){
                    sumpOffMin = 0;
                    sumpOffHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here to make sure sump on min is bound between 0 and 59, and if floods below 0, decrement sump on hr down and set sump on min to 59
                if(sumpOffMin > 0){
                    sumpOffMin--;
                }
                else if(sumpOffMin == 0){
                    sumpOffMin = 59;
                    sumpOffHr--;
                }
            }
        }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 71;
            clearScreen = true;
        }
        break;
    case 72:
        setLightCyclesSumpTemplate(sumpOnHr, sumpOnMin, sumpOffHr, sumpOffMin);
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 62; //quick backups
            knobMin = 60;
            knobMax = 62;
        }
        break;
    case 80:
        //set main cycle on time
        setLightCyclesMainTemplate(mainOnHr, mainOnMin, mainOffHr, mainOffMin);
        arrowOne();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here for making sure main on hr is bounded between 1 and 24
                if(mainOnHr < 24){
                    mainOnHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here for making sure main on hr is bounded between 1 and 24
                if(mainOnHr > 1){
                    mainOnHr--;
                }
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here to make sure main on min is bound between 0 and 59, and if floods over 59, increment main on hr up and set minutes to 0.
                if(mainOnMin < 59){
                 mainOnMin++;   
                }
                else if(mainOnMin == 59){
                    mainOnMin = 0;
                    mainOnHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here to make sure main on min is bound between 0 and 59, and if floods below 0, decrement main on hr down and set main on min to 59
                if(mainOnMin > 0){
                    mainOnMin--;
                }
                else if(mainOnMin == 0){
                    mainOnMin = 59;
                    mainOnHr--;
                }
            }
        }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 71;
            clearScreen = true;
        }
        break;
    case 81:
        setLightCyclesMainTemplate(mainOnHr, mainOnMin, mainOffHr, mainOffMin);
        arrowTwo();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here for making sure main on hr is bounded between 1 and 24
                if(mainOffHr < 24){
                    mainOffHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here for making sure main on hr is bounded between 1 and 24
                if(mainOffHr > 1){
                    mainOffHr--;
                }
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                //add conditions here to make sure main on min is bound between 0 and 59, and if floods over 59, increment main on hr up and set minutes to 0.
                if(mainOffMin < 59){
                 mainOffMin++;   
                }
                else if(mainOffMin == 59){
                    mainOffMin = 0;
                    mainOffHr++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                //add conditions here to make sure main on min is bound between 0 and 59, and if floods below 0, decrement main on hr down and set main on min to 59
                if(mainOffMin > 0){
                    mainOffMin--;
                }
                else if(mainOffMin == 0){
                    mainOffMin = 59;
                    mainOffHr--;
                }
            }
        }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 71;
            clearScreen = true;
        }
        break;
    case 82:
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 62; //quick backups
            knobMin = 60;
            knobMax = 62;
        }
        break;
    case 90:
        setLightValidationTemplate(sumpExpirationTimeMin, mainExpirationTimeMin, sumpExpirationTimeSec, mainExpirationTimeSec, lightExpirationSettingLight);
        arrowOne();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra %2 == 0){
                lightExpirationSettingLight = "main";
                lcd->setCursor(5,1);
                lcd->print("MAIN");
            }
            else{
                lightExpirationSettingLight = "sump";
                lcd->setCursor(5,1);
                lcd->print("SUMP");
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                if(lightExpirationSettingLight.equals("main")){
                    lcd->setCursor(5,1);
                    lcd->print("MAIN");
                    mainExpirationTimeMin++;   
                }
                else if(lightExpirationSettingLight.equals("sump")){
                    lcd->setCursor(5,1);
                    lcd->print("SUMP");
                    sumpExpirationTimeMin++;
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                if(lightExpirationSettingLight.equals("main") && mainExpirationTimeMin > 0){
                    lcd->setCursor(5,1);
                    lcd->print("MAIN");
                    mainExpirationTimeMin--;   
                }
                else if(lightExpirationSettingLight.equals("sump") && sumpExpirationTimeMin > 0){
                    lcd->setCursor(5,1);
                    lcd->print("SUMP");
                    sumpExpirationTimeMin--;
                }
            }
        }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 91;
            clearScreen = true;
        }
        break;
        case 91:
        setLightValidationTemplate(sumpExpirationTimeMin, mainExpirationTimeMin, sumpExpirationTimeSec, mainExpirationTimeSec, lightExpirationSettingLight);
        arrowTwo();
        if(myClick){
            myClick = false;
            clickCounter++;
        }
        if(clickCounter == 1){
            encoderLock = true;
            if(encoderCounterExtra %2 == 0){
                lightExpirationSettingLight = "main";
                lcd->setCursor(5,2);
                lcd->print("MAIN");
            }
            else{
                lightExpirationSettingLight = "sump";
                lcd->setCursor(5,2);
                lcd->print("SUMP");
            }
        }
        else if(clickCounter == 2){
            encoderLock = true;
            if(encoderCounterExtra > prevEncoderCounterExtra){
                if(lightExpirationSettingLight.equals("main")){
                    lcd->setCursor(5,2);
                    lcd->print("MAIN");
                    mainExpirationTimeSec++;   
                }
                else if(lightExpirationSettingLight.equals("sump")){
                    lcd->setCursor(5,2);
                    lcd->print("SUMP");
                    sumpExpirationTimeSec++;
                }
                }
            }
            else if(encoderCounterExtra < prevEncoderCounterExtra){
                if(lightExpirationSettingLight.equals("main") && mainExpirationTimeSec > 0){
                    lcd->setCursor(5,2);
                    lcd->print("MAIN");
                    mainExpirationTimeSec--;   
                }
                else if(lightExpirationSettingLight.equals("sump") && sumpExpirationTimeSec > 0){
                    lcd->setCursor(5,2);
                    lcd->print("SUMP");
                    sumpExpirationTimeSec--;
                }
            }
        else if(clickCounter == 3){
            encoderLock = false;
            encoderCounterExtra = 0;
            clickCounter = 0;
            encoderCounter = 91;
            clearScreen = true;
        }
        break;
        case 92:
        arrowBack();
        if(myClick){
            clearScreen = true;
            myClick = false;
            encoderCounter = 44;
            knobMin = 40;
            knobMax = 47;
        }
        break;
            
        
} 

  Particle.variable("mSec", mainExpirationTimeSec);
  Particle.variable("mMin", mainExpirationTimeMin);
  Particle.variable("sSec", sumpExpirationTimeSec);
  Particle.variable("sMin", sumpExpirationTimeMin);
  
  
  
  
}

/* 
ROTARY ENCODER FUNCTIONS
*/
void doEncoderA(){
  if( digitalRead(RE_RGB_A) != A_set ) {  // debounce once more
    A_set = !A_set;
    // adjust counter + if A leads B
    if ( A_set && !B_set ) 
      encoderPos += 1;
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB(){
   if( digitalRead(RE_RGB_B) != B_set ) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if( B_set && !A_set ) 
      encoderPos -= 1;
  }
}


 
 /*
 BLYNK FUNCTIONS
 */
 BLYNK_READ(V0)
{
  Blynk.virtualWrite(V0, sumpLight);
}

BLYNK_READ(V1)
{
  Blynk.virtualWrite(V1, mainLight);
}
 // Enable/disable relay using virt pin 1
BLYNK_WRITE(V0) {
  if (param[0].asInt()) {
    sumpLightSwitch = !sumpLightSwitch;
    sumpSetTime = Time.now();
    //digitalWrite(LIGHT_SUMP, HIGH);
        BLYNK_LOG("LIGHT_SUMP: on");

  } 
}

// Enable/disable relay using virt pin 2
BLYNK_WRITE(V1) {
  if (param[0].asInt()) {
    mainLightSwitch = !mainLightSwitch;
    mainSetTime = Time.now();
    //digitalWrite(LIGHT_MAIN, HIGH);
        BLYNK_LOG("LIGHT_MAIN: on");

  }
}

/*
DS18B20 Functions
*/

/*
Alert Functions
*/
/*
int alertSystem(){
    //cycle through all of the alerts
    for(int i=0;i<6,i++){
        //now go through all of the alerts and take the apropriet action
        if(alertName[i].equals("FEED")){
            for(int j=0;j<2;j++){
                if(Time.weekday() == FEEDday[j] && currentHour == FEEDhour){
                    if(onHomeScreen){
                        lcd->setCursor(7,3);
                        lcd->print("FEED!");
                        Blynk.notify("Feed the fish!");
                     return alertUrgency[i];   
                    }
                }
            }
        }
        else if(alertName[i].equals("DOSE_FLOURISH")){
            for(int k=0;k<1;k++){
                if(Time.weekday() == DOSE_FLOURISHday[k] && currenoutHour == DOSE_FLOURISHhour){
                    if(onHomeScreen){
                        lcd->setCursor(7,3);
                        lcd->print("DOSE FLOURISH!");
                        Blynk.notify("Dose 5mL flourish!");
                     return alertUrgency[i]; 
                    }
                }
            }
            
        }
        else if(alertName[i].equals("WATER_LEVEL_LOW")){
            
        }
        else if(alertName[i].equals("WATER_TEMP_HIGH")){
            
        }
        else if(alertName[i].equals("WATER_TEMP_LOW")){
            
        }
        else if(alertName[i].equals("PLANTS_WATER")){
            
        }
        
    }
    
}
*/

/*
GUI FUNCTIONS
*/

void centerPrint(String text, int leftBound[2], int rightBound[2]){
    int boundWidth = leftBound[0] - rightBound[0];
    int textLength = text.length();
    int dif = boundWidth - textLength;
    if(dif > 0){
        int align = dif/2;
        lcd->setCursor(align,leftBound[1]);
        lcd->print(text);
    }
    else{
        lcd->setCursor(leftBound[0],leftBound[1]);
        lcd->print(text);
    }
}

//All menu screens made up here

void homeTemplate(String mode, float sumpTemp, float mainTemp, float dhtTemp, float dhtHumid){
    lcd->setCursor(0,0);
    lcd->print("Mode:");
    lcd->setCursor(5,0);
    lcd->print(mode);
    lcd->setCursor(10,0);
    lcd->print(Time.hour());
    int hrLength = 0;
    int minLength = 0;
    int secLength = 0;
    if(Time.hour() < 10){
        hrLength = 1;
    }
    else{
        hrLength = 2;
    }
    if(Time.minute() < 10){
        minLength = 1;
    }
    else{
        minLength = 2;
    }
    if(Time.second() < 9){
        secLength = 1;
    }
    else{
        secLength = 2;
    }
    lcd->setCursor((10 + hrLength),0);
    lcd->print(":");
    if(minLength == 1){
        lcd->setCursor((10 + hrLength + 1),0);
        lcd->print("0");
        lcd->setCursor((10 + hrLength + 2),0);
        lcd->print(Time.minute());
    }
    else{
        lcd->setCursor((10 + hrLength + 1),0);
        lcd->print(Time.minute());
    }
    lcd->setCursor((13 + hrLength),0);
    lcd->print(":");
    if(secLength == 1){
        lcd->setCursor((14 + hrLength),0);
        lcd->print("0");
        lcd->setCursor((15 + hrLength),0);
        lcd->print(Time.second());
    }
    else{
        lcd->setCursor((14 + hrLength),0);
        lcd->print(Time.second());
    }
    lcd->setCursor(0,1);
    lcd->print("Water:");
    lcd->setCursor(6,1);
    lcd->print(sumpTempF);
    lcd->setCursor(10,1);
    lcd->print("f,");
    lcd->setCursor(12,1);
    lcd->print(sumpTempC);
    lcd->setCursor(16,1);
    lcd->print("c");
    lcd->setCursor(0,2);
    lcd->print("Air:");
    lcd->setCursor(4,2);
    lcd->print(dhtTemp);
    lcd->setCursor(8,2);
    lcd->print("f");
    lcd->setCursor(10,2);
    lcd->print("Hum:");
    lcd->setCursor(14,2);
    lcd->print(dhtHumid,1);
    if(dhtHumid < 100){
        lcd->setCursor(18,2);
        lcd->print("% ");
    }
    else{
       lcd->setCursor(19,2);
       lcd->print("%"); 
    }
    lcd->setCursor(0,3);
    lcd->print("Alerts:");
}
void mainMenuTemplate(){
    lcd->setCursor(1,0);
    lcd->print("^Biosphere 0.1 EN");
    lcd->setCursor(1,1);
    lcd->print("System Control");
    lcd->setCursor(1,2);
    lcd->print("System Statuses");
    lcd->setCursor(1,3);
    lcd->print("Settings");
}
void controlTemplate(){
    lcd->setCursor(1,0);
    lcd->print("^ System Control");
    lcd->setCursor(1,1);
    lcd->print("Lights ON/OFF");
    lcd->setCursor(1,2);
    lcd->print("Light Cycles");
    lcd->setCursor(1,3);
    lcd->print("Pump ON/OFF");
}
//Settings templates
void settingsOneTemplate(){
    lcd->setCursor(1,0);
    lcd->print("^    Settings");
    lcd->setCursor(1,1);
    lcd->print("Modes");
    lcd->setCursor(1,2);
    lcd->print("Alerts");
    lcd->setCursor(1,3);
    lcd->print("Lock-Outs");
}
void settingsTwoTemplate(){
    lcd->setCursor(1,0);
    lcd->print("Btn Validation Time");
    lcd->setCursor(1,1);
    lcd->print("Setting");
    lcd->setCursor(1,2);
    lcd->print("Setting");
    lcd->setCursor(1,3);
    lcd->print("Setting");
}
void statusesTemplate(int wifi, String networkIn, int month, int day, int year){
    lcd->setCursor(1,0);
    lcd->print("^ System Statuses");
    lcd->setCursor(0,1);
    lcd->print("WiFi:");
    int wifiLength;
    if(wifi < 10){
        wifiLength = 1;
    }
    else if (wifi >= 10){
        wifiLength = 2;
    }
    else {
        wifiLength = 3;
    }
    lcd->setCursor(5,1);
    lcd->print(wifi);
    lcd->setCursor((5+wifiLength),1);
    lcd->print("%");
    lcd->setCursor(6+wifiLength,1);
    lcd->print("NW:");
    lcd->setCursor(9+wifiLength,1);
    lcd->print(networkIn);
    lcd->setCursor(0,2);
    lcd->print("Up Since:");
    lcd->setCursor(10,2);
    lcd->print(month);
    int monthLength;
    if(month < 10){
        monthLength = 1;
    }
    else{
        monthLength = 2;
    }
    lcd->setCursor((10 + monthLength),2);
    lcd->print("/");
    lcd->setCursor((10 + monthLength + 1),2);
    lcd->print(day);
    int dayLength;
    if(day < 10){
        dayLength = 1;
    }
    else{
        dayLength = 2;
    }
    lcd->setCursor((11 + monthLength + dayLength),2);
    lcd->print("/");
    lcd->setCursor((12 + monthLength + dayLength),2);
    lcd->print(year);
    lcd->setCursor(2,3);
    lcd->print("By:Erik Nakamura");
}
void lightOnOffTemplate(){
    lcd->setCursor(1,0);
    lcd->print("^");
    lcd->setCursor(7,0);
    lcd->print("Lights");
    lcd->setCursor(1,1);
    lcd->print("Sump");
    lcd->setCursor(6,1);
    lcd->print("--");
    lcd->setCursor(8,1);
    String sumpLightStat;
    if(sumpLight){
        sumpLightStat = " ON ";
    }
    else{
        sumpLightStat = " OFF";
    }
    lcd->print(sumpLightStat);
    lcd->setCursor(1,2);
    lcd->print("Main");
    lcd->setCursor(6,2);
    lcd->print("--");
    lcd->setCursor(8,2);
    String mainLightStat;
    if(mainLight){
        mainLightStat = "ON ";
    }
    else{
        mainLightStat = "OFF";
    }
    lcd->print(mainLightStat);
}
void lightCyclesTemplate(int sumpOnTimeHr, int sumpOnTimeMin, int sumpOffTimeHr, int sumpOffTimeMin, int mainOnTimeHr,  int mainOnTimeMin, int mainOffTimeHr, int mainOffTimeMin){
    int sumpOnHrLength;
    int sumpOnMinLength;
    int mainOnHrLength;
    int mainOnMinLength;
    int sumpOffHrLength;
    int sumpOffMinLength;
    int mainOffHrLength;
    int mainOffMinLength;
    if(sumpOnTimeHr < 10){
        sumpOnHrLength = 1;
    }
    else{
        sumpOnHrLength = 2;
    }
    if(sumpOnTimeMin < 10){
        sumpOnMinLength = 1;
    }
    else{
        sumpOnMinLength = 2;
    }
    if(mainOnTimeHr < 10){
        mainOnHrLength = 1;
    }
    else{
        mainOnHrLength = 2;
    }
     if(mainOnTimeMin < 10){
        mainOnMinLength = 1;
    }
    else{
        mainOnMinLength = 2;
    }
    // Off
    if(sumpOffTimeHr < 10){
        sumpOffHrLength = 1;
    }
    else{
        sumpOffHrLength = 2;
    }
    if(sumpOffTimeMin < 10){
        sumpOffMinLength = 1;
    }
    else{
        sumpOffMinLength = 2;
    }
    if(mainOffTimeHr < 10){
        mainOffHrLength = 1;
    }
    else{
        mainOffHrLength = 2;
    }
     if(mainOffTimeMin < 10){
        mainOffMinLength = 1;
    }
    else{
        mainOffMinLength = 2;
    }
    lcd->setCursor(1,0);
    lcd->print("^");
    lcd->setCursor(4,0);
    lcd->print("Light Cycles");
    lcd->setCursor(1,1);
    lcd->print("Sump");
    lcd->setCursor((9 - sumpOnHrLength),1);
    lcd->print(sumpOnTimeHr);
    lcd->setCursor(9,1);
    lcd->print(":");
    
    if(sumpOnMinLength == 1){
        lcd->setCursor(10,1);
        lcd->print("0");
        lcd->setCursor(11,1);
        lcd->print(sumpOnTimeMin);
    }
    else{
        lcd->setCursor(10,1);
        lcd->print(sumpOnTimeMin);
    }
    lcd->setCursor(12,1);
    lcd->print("-");
    lcd->setCursor(13,1);
    lcd->print(sumpOffTimeHr);
    lcd->setCursor((13 + sumpOffHrLength),1);
    lcd->print(":");
    if(sumpOffMinLength == 1){
        lcd->setCursor((14 + sumpOffHrLength),1);
        lcd->print("0");
        lcd->setCursor((15 + sumpOffHrLength),1);
        lcd->print(sumpOffTimeMin);
    }
    else{
        lcd->setCursor((14 + sumpOffHrLength),1);
        lcd->print(sumpOffTimeMin);
    }
    //main
    lcd->setCursor(1,2);
    lcd->print("Main");
    lcd->setCursor((9 - mainOnHrLength),2);
    lcd->print(mainOnTimeHr);
    lcd->setCursor(9,2);
    lcd->print(":");
    
    if(mainOnMinLength == 1){
        lcd->setCursor(10,2);
        lcd->print("0");
        lcd->setCursor(11,2);
        lcd->print(mainOnTimeMin);
    }
    else{
        lcd->setCursor(10,2);
        lcd->print(mainOnTimeMin);
    }

    lcd->setCursor(12,2);
    lcd->print("-");
    lcd->setCursor(13,2);
    lcd->print(mainOffTimeHr);
    lcd->setCursor((13 + mainOffHrLength),2);
    lcd->print(":");
    if(mainOffMinLength == 1){
        lcd->setCursor((14 + mainOffHrLength),2);
        lcd->print("0");
        lcd->setCursor((15 + mainOffHrLength),2);
        lcd->print(mainOffTimeMin);
    }
    else{
        lcd->setCursor((14 + mainOffHrLength),2);
        lcd->print(mainOffTimeMin);
    }
    
    lcd->setCursor(4,3);
    lcd->print("Click to set");
}

void setLightCyclesSumpTemplate(int sumpOnTimeHr, int sumpOnTimeMin, int sumpOffTimeHr, int sumpOffTimeMin){
    int sumpOnHrLength;
    int sumpOnMinLength;
    int sumpOffHrLength;
    int sumpOffMinLength;
    if(sumpOnTimeHr < 10){
        sumpOnHrLength = 1;
    }
    else{
        sumpOnHrLength = 2;
    }
    if(sumpOnTimeMin < 10){
        sumpOnMinLength = 1;
    }
    else{
        sumpOnMinLength = 2;
    }
    // Off
    if(sumpOffTimeHr < 10){
        sumpOffHrLength = 1;
    }
    else{
        sumpOffHrLength = 2;
    }
    if(sumpOffTimeMin < 10){
        sumpOffMinLength = 1;
    }
    else{
        sumpOffMinLength = 2;
    }
    lcd->setCursor(1,0);
    lcd->print("^");
    lcd->setCursor(4,0);
    lcd->print("Set Sump Cycle");
    //on
    lcd->setCursor(1,1);
    lcd->print("ON TIME");
    lcd->setCursor((13 - sumpOnHrLength),1);
    lcd->print(sumpOnTimeHr);
    if(sumpOnHrLength == 1){
        lcd->setCursor(11,1);
        lcd->print(" ");
    }
    lcd->setCursor(13,1);
    lcd->print(":");
    if(sumpOnMinLength == 1){
        lcd->setCursor(14,1);
        lcd->print("0");
        lcd->setCursor(15,1);
        lcd->print(sumpOnTimeMin);
    }
    else{
        lcd->setCursor(14,1);
        lcd->print(sumpOnTimeMin);
    }
    
    //off
    lcd->setCursor(1,2);
    lcd->print("OFF TIME");
    lcd->setCursor((13 - sumpOffHrLength),2);
    lcd->print(sumpOffTimeHr);
    if(sumpOffHrLength == 1){
        lcd->setCursor(11,2);
        lcd->print(" ");
    }
    lcd->setCursor(13,2);
    lcd->print(":");
    if(sumpOffMinLength == 1){
        lcd->setCursor(14,2);
        lcd->print("0");
        lcd->setCursor(15,2);
        lcd->print(sumpOffTimeMin);
    }
    else{
        lcd->setCursor(14,2);
        lcd->print(sumpOffTimeMin);
    }
    
    lcd->setCursor(1,3);
    lcd->print("Click Hr>Min>Exit");
    

}
void setLightCyclesMainTemplate(int mainOnTimeHr, int mainOnTimeMin, int mainOffTimeHr, int mainOffTimeMin){
    int mainOnHrLength;
    int mainOnMinLength;
    int mainOffHrLength;
    int mainOffMinLength;
    if(mainOnTimeHr < 10){
        mainOnHrLength = 1;
    }
    else{
        mainOnHrLength = 2;
    }
    if(mainOnTimeMin < 10){
        mainOnMinLength = 1;
    }
    else{
        mainOnMinLength = 2;
    }
    if(mainOffTimeHr < 10){
        mainOffHrLength = 1;
    }
    else{
        mainOffHrLength = 2;
    }
    if(mainOffTimeMin < 10){
        mainOffMinLength = 1;
    }
    else{
        mainOffMinLength = 2;
    }
    lcd->setCursor(1,0);
    lcd->print("^");
    lcd->setCursor(4,0);
    lcd->print("Set Main Cycle");
    
    //on
    lcd->setCursor(1,1);
    lcd->print("ON TIME");
    lcd->setCursor((13 - mainOnHrLength),1);
    lcd->print(mainOnTimeHr);
    if(mainOnHrLength == 1){
        lcd->setCursor(11,1);
        lcd->print(" ");
    }
    lcd->setCursor(13,1);
    lcd->print(":");
    if(mainOnMinLength == 1){
        lcd->setCursor(14,1);
        lcd->print("0");
        lcd->setCursor(15,1);
        lcd->print(mainOnTimeMin);
    }
    else{
        lcd->setCursor(14,1);
        lcd->print(mainOnTimeMin);
    }
    
    //off
    lcd->setCursor(1,2);
    lcd->print("OFF TIME");
    lcd->setCursor((13 - mainOffHrLength),2);
    lcd->print(mainOffTimeHr);
    lcd->setCursor(13,2);
    if(mainOffHrLength == 1){
        lcd->setCursor(11,2);
        lcd->print(" ");
    }
    lcd->print(":");
    if(mainOffMinLength == 1){
        lcd->setCursor(14,2);
        lcd->print("0");
        lcd->setCursor(15,2);
        lcd->print(mainOffTimeMin);
    }
    else{
        lcd->setCursor(14,2);
        lcd->print(mainOffTimeMin);
    }
    
    lcd->setCursor(1,3);
    lcd->print("Click Hr>Min>Exit");
    
}

void setLightValidationTemplate(int minuteTimeSump, int minuteTimeMain, int secondTimeSump, int secondTimeMain, String lightType){
    lcd->setCursor(1,0);
    lcd->print("^ Btn Validation");
    lcd->setCursor(1,1);
    lcd->print("Min");
    lcd->setCursor(5,1);
    if(lightType.equals("main")){
        lcd->print("MAIN");
        lcd->setCursor(12,1);
        lcd->print(minuteTimeMain);
        if(minuteTimeMain < 10){
            lcd->setCursor(14,1);
            lcd->print(" ");
        }
    }
    else{
        lcd->print("SUMP");
        lcd->setCursor(12,1);
        lcd->print(minuteTimeSump);
        if(minuteTimeSump < 10){
            lcd->setCursor(14,1);
            lcd->print(" ");
        }
    }
    lcd->setCursor(1,2);
    lcd->print("Sec");
    lcd->setCursor(5,2);
    if(lightType.equals("main")){
        lcd->print("MAIN");
        lcd->setCursor(12,2);
        lcd->print(secondTimeMain);
        if(secondTimeMain < 10){
            lcd->setCursor(14,1);
            lcd->print(" ");
        }
    }
    else{
        lcd->print("SUMP");
        lcd->setCursor(12,2);
        lcd->print(secondTimeSump);
        if(secondTimeSump < 10){
            lcd->setCursor(14,1);
            lcd->print(" ");
        }
    }
    lcd->setCursor(0,3);
    lcd->print("Clk Light>Time>Exit");
    
}
void arrowZero(){
    lcd->setCursor(0,1);
    lcd->print(" ");
    lcd->setCursor(0,2);
    lcd->print(" ");
    lcd->setCursor(0,3);
    lcd->print(" ");
    lcd->setCursor(0,0);
    lcd->print(">");
}
void arrowOne(){
    lcd->setCursor(0,0);
    lcd->print(" ");
    lcd->setCursor(0,2);
    lcd->print(" ");
    lcd->setCursor(0,3);
    lcd->print(" ");
    lcd->setCursor(0,1);
    lcd->print(">");
}
void arrowTwo(){
    lcd->setCursor(0,0);
    lcd->print(" ");
    lcd->setCursor(0,1);
    lcd->print(" ");
    lcd->setCursor(0,3);
    lcd->print(" ");
    lcd->setCursor(0,2);
    lcd->print(">");
}
void arrowThree(){
    lcd->setCursor(0,0);
    lcd->print(" ");
    lcd->setCursor(0,1);
    lcd->print(" ");
    lcd->setCursor(0,2);
    lcd->print(" ");
    lcd->setCursor(0,3);
    lcd->print(">");
}
void arrowBack(){
    lcd->setCursor(0,1);
    lcd->print(" ");
    lcd->setCursor(0,2);
    lcd->print(" ");
    lcd->setCursor(0,3);
    lcd->print(" ");
    lcd->setCursor(0,0);
    lcd->print(">");
}
