#include <HCSR04.h>
#include <hm10HubBt.hpp>
#include <hubMessages.hpp>
#include <car.hpp>

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

int ledPin = 29;
int joyBtnPin = 45;
int hm10ConnPin = 28;
int trigPin = 30;
int echoPin = 31;
int micAnalogPin = A15;
int micDigitalPin = 50;
bool sensorClose = false;
bool sensorClosePrev = false;
enum CarState {TURNR, TURNL, STRAIGHT};
byte buffer[128] = {0x00};
Hm10HubBt hm10(hm10HubBt);
Car car;

HubMessageMotorStartDeg commandTurnR = HubMessageMotorStartDeg(75, 30);
HubMessageMotorStartDeg commandTurnL = HubMessageMotorStartDeg(75, -30);
HubMessageMotorGotoAbs commandGoto0 = HubMessageMotorGotoAbs();

CarState carState;

UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

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

void writeMsgToHm10(HubMessage & message); // bruh don't delete this declaration
void writeMsgToHm10(HubMessage & message)
{
    byte size = message.parseIntoBuf(buffer);
    //printHex8(buffer, size);
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}

// Car moves

void carGoForward()
{
    // auto msg = HubMessageMotorStartSpeed(100);
    // hm10.msgOutEnqueue(msg);
    //hm10.msgOutSend();

    car.send();
}

void carGoBack()
{
    byte size = HubMessageMotorStartSpeed(-100).parseIntoBuf(buffer);
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

void carBackUp()
{
    byte size = HubMessageMotorStartSpeed(-100, 2000).parseIntoBuf(buffer); // Drive backwards at 100% speed for 2 s (2000 ms)
    hm10HubBt.write(buffer, size * sizeof(buffer[0]));
}

void carTurnRight()
{
    switch (carState)
    {
        case TURNL:
            writeMsgToHm10(commandGoto0);
            carState = STRAIGHT;
            break;
        case STRAIGHT:
            writeMsgToHm10(commandTurnR);
            carState = TURNR;
            break;
        default:
            break;
    }
}

void carTurnLeft()
{
    switch (carState)
    {
        case TURNR:
            writeMsgToHm10(commandGoto0);
            carState = STRAIGHT;
            break;
        case STRAIGHT:
            writeMsgToHm10(commandTurnL);
            carState = TURNL;
            break;
        default:
            break;
    }
}

void carBackUpFromWallAndTurn()
{
    carTurnLeft();
    delay(100);
    carBackUp();
    // delay(100);
    // carTurnRight();
    // delay(500);
    // carGoForward();
}

float getDistanceFromSensor()
{
    float dist = distanceSensor.measureDistanceCm();
    //Serial.println(dist);
    return dist;
}

void setup()
{
    Serial.begin(115200);
    // hm10HubBt.begin(115200); // hm10
    hc05PhoneBt.begin(9600); // hc05
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    pinMode(hm10ConnPin, INPUT);
    pinMode(micDigitalPin, INPUT);

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
    carState = STRAIGHT;
    digitalWrite(ledPin, HIGH);
    Serial.println("Setup done");
}

void loop()
{
    if (hc05PhoneBt.available())
    {
        byte size = hc05PhoneBt.read();
        delay(10);
        if (size == 0x00) // First byte is 0
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
                    carBackUpFromWallAndTurn();
                    break;
                case 0x04:
                    Serial.println(getDistanceFromSensor());
                    break;
                case 0x10:
                    carTurnLeft();
                    break;
                case 0x11:
                    carTurnRight();
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

    // Ultrasonic sensor

    float dist = getDistanceFromSensor();
    //Serial.println(dist);
    sensorClose = dist < 10.0f && dist > 0.0f ;
    if (sensorClose == true && sensorClosePrev == false)
    {
        //Serial.println("sensor close!");
        //carBackUpFromWallAndTurn();
    }
    sensorClosePrev = sensorClose;


    int micOutput = analogRead(micAnalogPin);
    //int micOutput = digitalRead(micDigitalPin);
    //Serial.println(micOutput);

    digitalWrite(ledPin, digitalRead(micDigitalPin));
}

// End of loop

// HM10 communication inbound