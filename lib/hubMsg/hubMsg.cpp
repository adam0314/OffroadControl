#include "hubMsg.hpp"

HubMsg::HubMsg(MsgType messageType, byte command, byte length)
    : _messageType(messageType), _command(command), _length(length)
{

}

byte HubMsg::parseIntoBuf(byte* buffer)
{
    buffer[0] = _length;
    buffer[1] = 0;
    buffer[2] = _command;
    return _length;
}

HubMsgMotor::HubMsgMotor(MsgType messageType, byte command, byte length, byte portId, byte startComplInfo, byte subCommand)
:  HubMsg(messageType, command, length), _portId(portId), _startComplInfo(startComplInfo), _subCommand(subCommand)
{

}

void HubMsgMotor::withFeedback()
{
    _startComplInfo = _startComplInfo || 0x01;
}

byte HubMsgMotor::parseIntoBuf(byte* buffer)
{
    buffer[3] = _portId;
    buffer[4] = _startComplInfo;
    buffer[5] = _subCommand;

    return HubMsg::parseIntoBuf(buffer);
}

HubMsgMotorStartSpeed::HubMsgMotorStartSpeed() : HubMsgMotor(MOTOR_START_SPEED, 0x81, 9, 0x00, 0x00, 0x07)
{
    
}

HubMsgMotorStartSpeed::HubMsgMotorStartSpeed(byte speed) : HubMsgMotor(MOTOR_START_SPEED, 0x81, 9, 0x10, 0x00, 0x07)
{
    _speed = speed * -1;
    _time = 0;
    _maxPower = abs(speed); // same power as speed by default
    _useProfile = 0x00;
    _subCommand = 0x07;
}

HubMsgMotorStartSpeed::HubMsgMotorStartSpeed(byte speed, uint16_t time)
    : HubMsgMotor(MOTOR_START_SPEED_FOR_TIME, 0x81, 12, 0x10, 0x10, 0x07), _time(time)
{
    _speed = speed * -1;
    _maxPower = abs(speed); // same power as speed by default
    _useProfile = 0x00;
    _subCommand = 0x09;
}

HubMsgMotorStartSpeed::HubMsgMotorStartSpeed(
    byte portId, byte startComplInfo, byte speed, byte maxPower, byte useProfile)
    : HubMsgMotor(MOTOR_START_SPEED, 0x81, 9, portId, startComplInfo, 0x07),
    _speed(speed * -1), _maxPower(maxPower), _useProfile(useProfile)
{
    
}

byte HubMsgMotorStartSpeed::parseIntoBuf(byte* buffer)
{
    if (_messageType == MOTOR_START_SPEED)
    {
        buffer[6] = _speed;
        buffer[7] = _maxPower;
        buffer[8] = _useProfile;
    }
    else // use time
    {
        buffer[6] = _time & 0xFF;
        buffer[7] = (_time >> 8) & 0xFF;
        buffer[8] = _speed;
        buffer[9] = _maxPower;
        buffer[10] = 0x00;
        buffer[11] = _useProfile;
    }
    
    return HubMsgMotor::parseIntoBuf(buffer);
}

HubMsgMotorStop::HubMsgMotorStop() : HubMsgMotor(MOTOR_STOP, 0x81, 8, 0x10, 0x10, 0x51)
{

}

HubMsgMotorStop::HubMsgMotorStop(
    byte portId, byte startComplInfo)
    : HubMsgMotor(MOTOR_STOP, 0x81, 8, portId, startComplInfo, 0x51)
{
    _subCommand = 0x51;
}

byte HubMsgMotorStop::parseIntoBuf(byte* buffer)
{
    buffer[6] = 0x00;
    buffer[7] = 0x00;
    return HubMsgMotor::parseIntoBuf(buffer);
}

HubMsgMotorGotoAbs::HubMsgMotorGotoAbs() : HubMsgMotor(MOTOR_GOTO_ABS, 0x81, 14, 0x02, 0x10, 0x0D)
{
    _absPos = 0x00000000;
    _speed = 0x20;
    _maxPower = 0x64;
    _endState = 0x7E;
    _useProfile = 0x00;
}

byte HubMsgMotorGotoAbs::parseIntoBuf(byte* buffer)
{
    buffer[6] = _absPos & 0xFF;
    buffer[7] = (_absPos >> 8) & 0xFF;
    buffer[8] = (_absPos >> 16) & 0xFF;
    buffer[9] = (_absPos >> 24) & 0xFF;
    buffer[10] = _speed;
    buffer[11] = _maxPower;
    buffer[12] = _endState;
    buffer[13] = _useProfile;

    return HubMsgMotor::parseIntoBuf(buffer);
}

HubMsgMotorStartDeg::HubMsgMotorStartDeg() : HubMsgMotor(MOTOR_START_DEG, 0x81, 14, 0x02, 0x00, 0x0B)
{
    _deg = 0;
    _speed = 0;
    _maxPower = 0x50;
    _endState = 0x00; // 0x00 FLOAT (no power) 0x7E HOLD (Supplying power) 0x7F BRAKE (short motor)
    _useProfile = 0;
}

HubMsgMotorStartDeg::HubMsgMotorStartDeg(int32_t deg, byte speed) : HubMsgMotorStartDeg()
{
    _deg = deg;
    _speed = speed;
}

byte HubMsgMotorStartDeg::parseIntoBuf(byte* buffer)
{
    buffer[6] = _deg & 0xFF;
    buffer[7] = (_deg >> 8) & 0xFF;
    buffer[8] = (_deg >> 16) & 0xFF;
    buffer[9] = (_deg >> 24) & 0xFF;
    buffer[10] = _speed;
    buffer[11] = _maxPower;
    buffer[12] = _endState;
    buffer[13] = _useProfile;

    return HubMsgMotor::parseIntoBuf(buffer);
}

HubMsgHubAction::HubMsgHubAction() : HubMsg(HUB_ACTION, 0x02, 4)
{
    
}

HubMsgHubAction::HubMsgHubAction(byte actionType) : HubMsgHubAction()
{
    _actionType = actionType;
}

byte HubMsgHubAction::parseIntoBuf(byte* buffer)
{
    buffer[3] = _actionType;

    return HubMsg::parseIntoBuf(buffer);
}