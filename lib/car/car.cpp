#include "car.hpp"

Car::Car()
{
    
}

void Car::init()
{
    initHm10();
    _msgSwitchOff = std::make_shared<HubMsgHubAction>(1);
    _msgStop = std::make_shared<HubMsgMotorStop>();
    _msgForward100 = std::make_shared<HubMsgMotorStartSpeed>(100);
    _msgBack100 = std::make_shared<HubMsgMotorStartSpeed>(-100);
    _msgTurnR = std::make_shared<HubMsgMotorStartDeg>(75, 30);
    _msgTurnL = std::make_shared<HubMsgMotorStartDeg>(75, -30);
    _msgTurn0 = std::make_shared<HubMsgMotorGotoAbs>();
    turn0();
}

void Car::initHm10(unsigned long baudRate)
{
    _hm10.init(baudRate);
}

void Car::loopHm10()
{
    _hm10.checkAndRead();
    _hm10.checkAndSend();
}

void Car::switchOff()
{
    _hm10.msgOutEnqueue(_msgSwitchOff);
}

void Car::setSpeed(byte speed)
{
    if (speed == 0) // Special case to stop car
    {
        stop();
    }
    else
    {
        _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartSpeed>(speed));
    }
}

void Car::setSpeed(byte speed, uint16_t timeInMs)
{
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartSpeed>(speed, timeInMs));
}

void Car::driveForward()
{
    _hm10.msgOutEnqueue(_msgForward100);
}

void Car::driveBack()
{
    _hm10.msgOutEnqueue(_msgBack100);
}

void Car::stop()
{
    _hm10.msgOutEnqueue(_msgStop);
}

void Car::turn0()
{
    Serial.println("turn0");
    _hm10.msgOutEnqueue(_msgTurn0); // TODO: This should be used with feedback and change turnState only when turn0 is finished
    _turnState = TURN0;
}

void Car::turnR()
{
    switch (_turnState)
    {
    case TURNL:
        turn0();
    case TURN0:
        Serial.println("turnR");
        _hm10.msgOutEnqueue(_msgTurnR);
        _turnState = TURNR;
        return;    
    default:
        return;
    }
}

void Car::turnL()
{
    switch (_turnState)
    {
    case TURNR:
        turn0();
    case TURN0:
        Serial.println("turnL");
        _hm10.msgOutEnqueue(_msgTurnL);
        _turnState = TURNL;
        return;    
    default:
        return;
    }
}