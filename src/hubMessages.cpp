#include "hubMessages.hpp"

HubMessage::HubMessage(MessageType messageType, byte command, byte length)
    : _messageType(messageType), _command(command), _length(length)
{

}

byte HubMessage::parseIntoBuf(byte* buffer)
{
    buffer[0] = _length;
    buffer[1] = 0;
    buffer[2] = _command;
    return _length;
}

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed() : HubMessage(MOTOR_START_SPEED, 0x81, 9)
{
    _subCommand = 0x07;
}

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed(byte speed) : HubMessage(MOTOR_START_SPEED, 0x81, 9)
{
    _portId = 0x10;
    _startComplInfo = 0x00;
    _speed = speed * -1;
    _time = 0;
    _maxPower = abs(speed); // same power as speed by default
    _useProfile = 0x00;
    _subCommand = 0x07;
}

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed(byte speed, uint16_t time) : HubMessage(MOTOR_START_SPEED_FOR_TIME, 0x81, 12), _time(time)
{
    _portId = 0x10;
    _startComplInfo = 0x00;
    _speed = speed * -1;
    _maxPower = abs(speed); // same power as speed by default
    _useProfile = 0x00;
    _subCommand = 0x09;
}

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed(
    byte portId, byte startComplInfo, byte speed, byte maxPower, byte useProfile)
    : HubMessage(MOTOR_START_SPEED, 0x81, 9),
    _portId(portId), _startComplInfo(startComplInfo), _speed(speed * -1), _maxPower(maxPower), _useProfile(useProfile)
{
    _subCommand = 0x07;
}

byte HubMessageMotorSetSpeed::parseIntoBuf(byte* buffer)
{
    buffer[3] = _portId;
    buffer[4] = _startComplInfo;
    buffer[5] = _subCommand;
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
    
    return HubMessage::parseIntoBuf(buffer);
}

HubMessageMotorStop::HubMessageMotorStop() : HubMessage(MOTOR_STOP, 0x81, 8)
{
    _portId = 0x10;
    _startComplInfo = 0x10;
    _subCommand = 0x51;
}

HubMessageMotorStop::HubMessageMotorStop(
    byte portId, byte startComplInfo)
    : HubMessage(MOTOR_STOP, 0x81, 8),
    _portId(portId), _startComplInfo(startComplInfo)
{
    _subCommand = 0x51;
}

byte HubMessageMotorStop::parseIntoBuf(byte* buffer)
{
    buffer[3] = _portId;
    buffer[4] = _startComplInfo;
    buffer[5] = _subCommand;
    buffer[6] = 0x00;
    buffer[7] = 0x00;
    return HubMessage::parseIntoBuf(buffer);
}

