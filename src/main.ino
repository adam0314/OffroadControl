#include <LiquidCrystal.h>
#include "hubMessages.hpp"
#include <ArduinoSTL.h>

#define hm10HubBt Serial1
#define hc05PhoneBt Serial2

void printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
      char tmp[16];
        for (int i=0; i<length; i++) { 
          sprintf(tmp, "%.2X", data[i]); 
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
int hm10ConnPin = 28;
byte joyBtnStatePrev;
bool joyControl = false;
enum CarState {TURNR, TURNL, STRAIGHT};
CarState carStatePrev;
byte buffer[128] = {0x00};

byte commandTurnR[14] = 
{
    0x0E, 0x00, 0x81, 0x02, 0x10, 0x0B, 0x4B, 0x00, 0x00, 0x00, 0x20, 0x40, 0x7E, 0x00
};
byte commandTurnL[14] = 
{
    0x0E, 0x00, 0x81, 0x02, 0x10, 0x0B, 0x4B, 0x00, 0x00, 0x00, 0xD0, 0x40, 0x7E, 0x00
};

bool turnDirLeft;

CarState carState;
int vrx = 512;

void setup()
{
    Serial.begin(115200);
    hm10HubBt.begin(115200); // hm10
    hc05PhoneBt.begin(9600); // hc05
    pinMode(ledPin, OUTPUT);
    pinMode(joyBtnPin, INPUT_PULLUP);
    pinMode(hm10ConnPin, INPUT);
    joyBtnStatePrev = digitalRead(joyBtnPin);
    carStatePrev = STRAIGHT;

    if (!isHubConnected())
    {
        while (!isHubConnected())
        {
            Serial.println("No LEGO connection to HM-10");
            Serial.println("Retrying in 3 seconds");
            delay(3000);
        }
        Serial.println("LEGO connected");

        // Assuming virtual port is not assigned

        byte command[6] = {0x06, 0x00, 0x61, 0x01, 0x00, 0x01};
        hm10HubBt.write(command, sizeof(command));
        delay(500);
    }
    else //TODO: Change this into commands and set some acknowledgment from LEGO
    {
        Serial.println("LEGO already connected");
        byte command[6] = {0x06, 0x00, 0x61, 0x01, 0x00, 0x01};
        hm10HubBt.write(command, sizeof(command));
        // TODO: Add checking if virtual port is already defined
        delay(500);
    }
    Serial.println("Setup done");
}

void loop()
{
    if (hc05PhoneBt.available())
    {
        byte size = hc05PhoneBt.read();
        delay(10);
        if (size == 0x00)
        {
            byte carCommand = hc05PhoneBt.read();
            switch (carCommand)
            {
                case 0x01:
                    carGoForward();
                    break;
                case 0x02:
                    carGoBack();
                    break;
                case 0x03:
                    test1();
                    break;
                case 0xFF:
                    carTurnOff();
                    break;
                case 0x00:
                    carStop();
                    break;                
                default:
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

    checkAndReadHm10();

    // Joystick

    // TODO: in program set the def. value of C motor degs and then control it here to turn the motor
    // This will be painful

    vrx = analogRead(A15);

    if (joyControl)
    {
        // Control car with joystick x axis
        if (vrx < 318)
        {
            carState = TURNL;
        }
        else if (vrx > 718)
        {
            carState = TURNR;
        }
        else
        {
            carState = STRAIGHT;
        }

        if (carState != carStatePrev)
        {
            // Something has changed
            switch (carStatePrev)
            {
                case STRAIGHT:
                    turnDirLeft = (carState == TURNL);
                    carStatePrev = carState;
                    break;
                case TURNL:
                    turnDirLeft = false; // always turning right from there
                    carStatePrev = STRAIGHT;
                    break;
                case TURNR:
                    turnDirLeft = true; // always turning right from there
                    carStatePrev = STRAIGHT;                  
                    break;
                default:
                    break;
            }

            if (turnDirLeft)
            {
                hm10HubBt.write(commandTurnL, sizeof(commandTurnL));
            }
            else
            {
                hm10HubBt.write(commandTurnR, sizeof(commandTurnR));
            }

            byte size = HubMessageMotorSetSpeed(100).parseIntoBuf(buffer);
            hm10HubBt.write(buffer, size * sizeof(buffer[0]));
        }
    }

    // Toggling control: automatic or joystick

    byte joyBtnState = digitalRead(joyBtnPin);
    if (joyBtnState == LOW && joyBtnStatePrev == HIGH)
    {
        //Serial.println("joy button pressed bitch");
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

// End of loop

// HM10 communication inbound

bool isHubConnected()
{
    return digitalRead(hm10ConnPin) == HIGH;
}

void checkAndReadHm10()
{
    if (hm10HubBt.available())
    {
        byte firstByte = hm10HubBt.read();

        if (firstByte == 0x4F) // AT command message starting with char 'O' (like OK-CONN)
        {
            hm10HubBt.readBytesUntil(0x0A, buffer, 32); // Throw this away
        }
        else // Assuming no msgs with length 4F come from Lego Hub, and all AT msgs start with 'O'
        {
            Serial.println("Data from Hub on HM10:");
            buffer[0] = firstByte;
            for (int i = 1; i < firstByte; i++)
            {
                buffer[i] = hm10HubBt.read();
                delay(10);
            }
            printHex8(buffer, firstByte);
            Serial.println();
        }
    }
}

// Car moves

void carGoForward()
{
    byte size = HubMessageMotorSetSpeed(100).parseIntoBuf(buffer);
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}

void carGoBack()
{
    byte size = HubMessageMotorSetSpeed(-100).parseIntoBuf(buffer);
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}

void carStop()
{
    byte size = HubMessageMotorStop().parseIntoBuf(buffer);
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}

void carTurnOff()
{
    byte command[] = {0x04, 0x00, 0x02, 0x01}; // turn off Technic Hub
    hm10HubBt.write(command, sizeof(command));
}

void test1()
{
    byte size = HubMessageMotorSetSpeed(100, 5000).parseIntoBuf(buffer); // Drive forward at 100% speed for 5 s (5000 ms)
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}