#include <car.hpp>

Car::Car()
{
    _msgForward100 = HubMessageMotorStartSpeed(100);
}

void Car::send()
{
    byte size = _msgForward100.parseIntoBuf(_buffer);
    Serial2.write(_buffer, size * sizeof(_buffer[0]));
}