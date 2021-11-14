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

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed()
{
    _messageType = MOTOR_START_SPEED;
    _command = 0x81;
    _length = 9;
}

HubMessageMotorSetSpeed::HubMessageMotorSetSpeed(
    byte portId, byte startComplInfo, byte speed, bool goForward, byte maxPower, byte useProfile)
    : HubMessage(MOTOR_START_SPEED, 0x81, 9),
    _portId(portId), _startComplInfo(startComplInfo), _speed(speed), _goForward(goForward), _maxPower(maxPower), _useProfile(useProfile)
{
    _subCommand = 0x07;
}

byte HubMessageMotorSetSpeed::parseIntoBuf(byte* buffer)
{
    buffer[3] = _portId;
    buffer[4] = _startComplInfo;
    buffer[5] = _subCommand;
    if (_goForward)
    {
        buffer[6] = 0 - _speed;
    }
    else
    {
        buffer[6] = _speed;
    }
    buffer[7] = _maxPower;
    buffer[8] = _useProfile;
    
    return HubMessage::parseIntoBuf(buffer);
}

