#include <LiquidCrystal.h>
#include "hubMsg.hpp"
#include <ArduinoSTL.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define hm10HubBt Serial1
#define hc05PhoneBt Serial2

int readLcdButtons() {
    int adcKeyIn = analogRead(0);
    if (adcKeyIn > 1000) return btnNONE;
    if (adcKeyIn < 60)   return btnRIGHT;
    if (adcKeyIn < 200)  return btnUP;
    if (adcKeyIn < 400)  return btnDOWN;
    if (adcKeyIn < 600)  return btnLEFT;
    if (adcKeyIn < 800)  return btnSELECT;
    return btnNONE;
}

void printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
      char tmp[16];
        for (int i=0; i<length; i++) { 
          sprintf(tmp, "0x%.2X", data[i]); 
          Serial.print(tmp); Serial.print(" ");
        }
}

void printHex8(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
    char tmp[16];
    sprintf(tmp, "%.2X", data); 
    Serial.print(tmp); Serial.print(" ");
}

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int ledPin = 22;
int joyBtnPin = 45;
byte joyBtnStatePrev;
bool joyControl = false;
enum CarState {TURNR, TURNL, STRAIGHT};
CarState carStatePrev;

bool analogBtnRead[5] = {false, false, false, false, false};

void setup()
{
    setupLcd();
    Serial.begin(115200);
    hm10HubBt.begin(115200); // hm10
    hc05PhoneBt.begin(9600); // hc05
    pinMode(ledPin, OUTPUT);
    pinMode(joyBtnPin, INPUT_PULLUP);
    joyBtnStatePrev = digitalRead(joyBtnPin);
    carStatePrev = STRAIGHT;
    Serial.println("Setup done");
}

void setupLcd()
{
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("amogus");
}

void loop()
{
    if (hc05PhoneBt.available())
    {
        std::vector<byte> v;
        while (hc05PhoneBt.available())
        {
            byte readHex = hc05PhoneBt.read();
            v.push_back(readHex);
            //printHex8(readHex);
        }
        
        hm10HubBt.write(&v[0], v.size() * sizeof(v[0]));
    }

    if (readLcdButtons() == btnSELECT)
    {
        //Serial.println("Select button read");
        byte command[] = {0x04, 0x00, 0x02, 0x01}; // turn off Technic Hub
        hm10HubBt.write(command, sizeof(command));
    }

    if (readLcdButtons() == btnUP)
    {
        byte command1[] =
        {
            0x09, 0x00, 0x81, 0x00, 0x10, 0x07, 0xB0, 0x5A, 0x00
        }; // 90% forward motor A
        byte command2[] =
        {
            0x09, 0x00, 0x81, 0x01, 0x10, 0x07, 0xB0, 0x5A, 0x00
        }; // 90% forward motor B
        hm10HubBt.write(command1, sizeof(command1));
        delay(10);
        hm10HubBt.write(command2, sizeof(command2));
    }

    if (readLcdButtons() == btnDOWN)
    {
        byte command1[] =
        {
            0x09, 0x00, 0x81, 0x00, 0x10, 0x07, 0x50, 0x5A, 0x00
        }; // 90% backwards motor A
        byte command2[] =
        {
            0x09, 0x00, 0x81, 0x01, 0x10, 0x07, 0x50, 0x5A, 0x00
        }; // 90% backwards motor B
        hm10HubBt.write(command1, sizeof(command1));
        delay(10);
        hm10HubBt.write(command2, sizeof(command2));
    }

    if (readLcdButtons() == btnRIGHT)
    {
        //Serial.println("Right button read");
        byte command[] = {0x05, 0x00, 0x01, 0x01, 0x05}; // ask for name
        hm10HubBt.write(command, sizeof(command));
    }

    if (readLcdButtons() == btnLEFT)
    {
        //Serial.println("Left button read, stop");
        byte command1[] =
        {
            0x09, 0x00, 0x81, 0x00, 0x10, 0x07, 0x00, 0x64, 0x00
        }; // stop A
        byte command2[] =
        {
            0x09, 0x00, 0x81, 0x01, 0x10, 0x07, 0x00, 0x64, 0x00
        }; // stop B
        hm10HubBt.write(command1, sizeof(command1));
        delay(10);
        hm10HubBt.write(command2, sizeof(command2));
    }

    if (hm10HubBt.available())
    {
        // TODO: Use vector, get first hex and put this many bytes into vector instead of while

        Serial.println("Data from HM10:");
        while (hm10HubBt.available())
        {
            printHex8(hm10HubBt.read());
        }
        Serial.println();
    }

    // Joystick

    int vrx = analogRead(A15);
    lcd.setCursor(0, 1);
    char s[5] = "0000";
    sprintf(s, "%04d", vrx);
    lcd.print(s);

    // TODO: in program set the def. value of C motor degs and then control it here to turn the motor
    // This will be painful

    if (joyControl)
    {
        byte command[14] = 
        {
            0x0E, 0x00, 0x81, 0x02, 0x10, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00, 0x00
        };
        CarState carState;
        // Control car with joystick x axis
        if (vrx < 318)
        {
            command[6] = 0xFF;
            command[7] = 0xFF;
            command[8] = 0xFF;
            command[9] = 0xFE;
            carState = TURNL;
        }
        else if (vrx > 718)
        {
            command[9] = 0x02;
            carState = TURNR;
        }
        else
        {
            carState = STRAIGHT;
        }

        if (carState != carStatePrev)
        {
            carStatePrev = carState;
            hm10HubBt.write(command, sizeof(command));
            delay(10);
        }        

        // TODO: Add a state machine to only turn the car when needed. Maybe 3 states? (TurningL, TurningR, Straight)
        // Right now, there is a flood of turn signals (and straight signal as well)
    }



    /////

    byte joyBtnState = digitalRead(joyBtnPin);
    if (joyBtnState == LOW && joyBtnStatePrev == HIGH)
    {
        Serial.println("joy button pressed bitch");
        joyControl = !joyControl;
        if (joyControl)
        {
            digitalWrite(ledPin, HIGH);
        }
        else
        {
            digitalWrite(ledPin, LOW);
        }
    }
    joyBtnStatePrev = digitalRead(joyBtnPin);
}

void lcdClear(unsigned int lineNo)
{
    if (lineNo > 1) return;
    lcd.setCursor(0, lineNo);
    lcd.print("                ");
}