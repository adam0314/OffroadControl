#include <HCSR04.h>
#include "hm10.hpp"
#include "hubMsg.hpp"
#include "car.hpp"

#define hm10HubBt Serial1
#define hc05PhoneBt Serial2

int ledPin = 29;
int joyBtnPin = 45;
int hm10ConnPin = 28;
int trigPin = 30;
int echoPin = 31;
int micAnalogPin = A15;
int micDigitalPin = 50;
bool sensorClose = false;
bool sensorClosePrev = false;
byte buffer[128] = {0x00};

Car car;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

// void carBackUpFromWallAndTurn()
// {
//     //carTurnLeft();
//     //delay(100);
//     //carBackUp();
//     // delay(100);
//     // carTurnRight();
//     // delay(500);
//     // carGoForward();
// }

float getDistanceFromSensor()
{
    float dist = distanceSensor.measureDistanceCm();
    //Serial.println(dist);
    return dist;
}

bool isHubConnected()
{
    return digitalRead(hm10ConnPin) == HIGH;
}

void handleHc05()
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
                    car.driveForward();
                    break;
                case 0x02:
                    car.driveBack();
                    break;
                case 0x03:
                    //carBackUpFromWallAndTurn();
                    break;
                case 0x04:
                    Serial.println(getDistanceFromSensor());
                    break;
                case 0x14:
                    car.turnL();
                    break;
                case 0x15:
                    car.turn0();
                    break;
                case 0x16:
                    car.turnR();
                    break;
                case 0xFF:
                    car.switchOff();
                    break;
                case 0x00:
                    car.stop();
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
}

void setup()
{
    Serial.begin(115200); // Serial for PC communication
    car.init(); // also inits hm10 at 115200 baud
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
    }
    else //TODO: Change this into commands and set some acknowledgment from LEGO
    {
        Serial.println("LEGO already connected");
    }

    car.syncPorts();
    car.calibrateTurning();
    digitalWrite(ledPin, HIGH);
    Serial.println("Setup done");
}

void loop()
{
    handleHc05();
    car.loopHm10();

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


    //int micOutput = analogRead(micAnalogPin);
    //int micOutput = digitalRead(micDigitalPin);
    //Serial.println(micOutput);

    digitalWrite(ledPin, digitalRead(micDigitalPin));
}