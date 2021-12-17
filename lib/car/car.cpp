#include "car.hpp"

Car::Car()
{
    
}

void Car::init()
{
    initHm10();
    _msgSwitchOff = HubMsgHubAction(1);
    _msgPortSync = HubMsgVirtualPortSetup(true, 0x00, 0x01);
    _msgStop = HubMsgMotorStop();
    _msgForward100 = HubMsgMotorStartSpeed(100);
    _msgBack100 = HubMsgMotorStartSpeed(-100);
    _msgTurnR = HubMsgMotorStartDeg(75, 30);
    _msgTurnR.useFeedback();
    _msgTurnL = HubMsgMotorStartDeg(75, -30);
    _msgTurnL.useFeedback();
    _msgTurn0 = HubMsgMotorGotoAbs();
    _msgTurn0.useFeedback();
    _turnState = TURN0;
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
    _hm10.msgOutEnqueue(std::make_shared<HubMsgHubAction>(_msgSwitchOff));
}

void Car::syncPorts()
{
    // TODO: Add checking if ports aren't synced already
    _hm10.msgOutEnqueue(std::make_shared<HubMsgVirtualPortSetup>(_msgPortSync));
}

void Car::calibrateTurning()
{
    _hm10.msgOutClear(); //piss off

    // Change port 2 (turning) value only in jumps of 4 of APOS, subscribe to notifications and pewdiepie
    auto msgPort2EnableNotify = std::make_shared<HubMsgPortInputFormat>(0x02, 0x03, 4, true);
    _hm10.msgOutEnqueue(msgPort2EnableNotify);
    _hm10.msgOutSend();
    //TODO: Wait for handshake notification 0x47

    // Turn max to the left
    auto msgTurnMaxLeft = HubMsgMotorStartDeg(-150, 100);
    msgTurnMaxLeft.useFeedback();
    msgTurnMaxLeft.useExecuteImmediately();
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartDeg>(msgTurnMaxLeft));
    _hm10.msgOutSend();
    //TODO: Wait for feedback, assert ack-ed messages from notify, get minimum absolute position (TurnL limit)

    // Turn max to the right
    auto msgTurnMaxRight = HubMsgMotorStartDeg(150, 100);
    msgTurnMaxRight.useFeedback();
    msgTurnMaxRight.useExecuteImmediately();
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartDeg>(msgTurnMaxRight));
    _hm10.msgOutSend();
    //TODO: Wait for feedback, assert ack-ed messages from notify, get maximum absolute position (TurnR limit)

    // Turn0 = (TurnR - TurnL) / 2

    //turn0(true);
    _hm10.checkAndSend();
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
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartSpeed>(_msgForward100));
}

void Car::driveBack()
{
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartSpeed>(_msgBack100));
}

void Car::stop()
{
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStop>(_msgStop));
}

void Car::turn0(bool force)
{
    if (_turnState == TURN0)
    {
        if (!force)
            return;        
    }    
    
    Serial.println("turn0");
    _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorGotoAbs>(_msgTurn0)); // TODO: This should be used with feedback and change turnState only when turn0 is finished
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
        _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartDeg>(_msgTurnR));
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
        _hm10.msgOutEnqueue(std::make_shared<HubMsgMotorStartDeg>(_msgTurnL));
        _turnState = TURNL;
        return;    
    default:
        return;
    }
}