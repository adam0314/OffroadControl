#include "car.hpp"

Car::Car()
{
    _hm10.init();
    _msgForward100 = std::make_shared<HubMessageMotorStartSpeed>(100);
}

void Car::send()
{
    _hm10.msgOutEnqueue(_msgForward100);
}