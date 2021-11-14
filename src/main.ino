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

int ledPin = 22;
int joyBtnPin = 45;
byte joyBtnStatePrev;
bool joyControl = false;
enum CarState {TURNR, TURNL, STRAIGHT};
CarState carStatePrev;
byte buffer[128] = {0x00};

bool analogBtnRead[5] = {false, false, false, false, false};

void setup()
{
    Serial.begin(115200);
    hm10HubBt.begin(115200); // hm10
    hc05PhoneBt.begin(9600); // hc05
    pinMode(ledPin, OUTPUT);
    pinMode(joyBtnPin, INPUT_PULLUP);
    joyBtnStatePrev = digitalRead(joyBtnPin);
    carStatePrev = STRAIGHT;
    Serial.println("Setup done");
}

void loop()
{
    if (hc05PhoneBt.available())
    {
        byte size = hc05PhoneBt.read();
        delay(10);
        //Serial.println(size);
        if (size == 0x00)
        {
            //Serial.println("mom i made 00xd");
            byte carCommand = hc05PhoneBt.read();
            //Serial.println(carCommand);
            switch (carCommand)
            {
                case 0x01:
                    carGoForward();
                    break;
                case 0x02:
                    carGoBack();
                    break;
                case 0xFF:
                    carTurnOff();
                    break;
                case 0x00:
                    carStop();
                    break;                
                default:
                    //Serial.println("carCommand value: ");
                    //Serial.println(carCommand);
                    break;
            }
        }
        else if (size > 0)
        {
            buffer[0] = size;
            for (int i = 1; i < size; i++)
            {
                buffer[i] = hc05PhoneBt.read();
                delay(10);
            }            
            hm10HubBt.write(buffer, size * sizeof(buffer[0]));
        }
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

    // Toggling control: automatic or joystick

    byte joyBtnState = digitalRead(joyBtnPin);
    if (joyBtnState == LOW && joyBtnStatePrev == HIGH)
    {
        /Serial.println("joy button pressed bitch");
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

void carGoForward()
{
    Serial.println("car go forward fn");
    byte command1[] =
    {
        0x09, 0x00, 0x81, 0x00, 0x10, 0x07, 0xB0, 0x5A, 0x00
    }; // 90% forward motor A
    byte command2[] =
    {
        0x09, 0x00, 0x81, 0x01, 0x10, 0x07, 0xB0, 0x5A, 0x00
    }; // 90% forward motor B
    hm10HubBt.write(command1, sizeof(command1));
    delay(100);
    hm10HubBt.write(command2, sizeof(command2));
}

void carGoBack()
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
    delay(100);
    hm10HubBt.write(command2, sizeof(command2));
}

void carStop()
{
    byte command1[] =
    {
        0x09, 0x00, 0x81, 0x00, 0x10, 0x07, 0x00, 0x64, 0x00
    }; // stop A
    byte command2[] =
    {
        0x09, 0x00, 0x81, 0x01, 0x10, 0x07, 0x00, 0x64, 0x00
    }; // stop B
    hm10HubBt.write(command1, sizeof(command1));
    delay(100);
    hm10HubBt.write(command2, sizeof(command2));
}

void carTurnOff()
{
    byte command[] = {0x04, 0x00, 0x02, 0x01}; // turn off Technic Hub
    hm10HubBt.write(command, sizeof(command));
}