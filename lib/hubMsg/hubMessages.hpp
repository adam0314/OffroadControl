#ifndef hubMessages_hpp
#define hubMessages_hpp
#include <arduino.h>

enum MessageType
{
    MOTOR_START_SPEED,
    MOTOR_START_SPEED_FOR_TIME,
    MOTOR_GOTO_ABS,
    MOTOR_STOP,
    MOTOR_START_DEG,
    HUB_TURN_OFF,
    PORT_SYNC
};

class HubMessage
{
    protected:
        MessageType _messageType;
        byte _command;
        byte _length;
        HubMessage();
        HubMessage(MessageType, byte, byte);
    public:
        virtual byte parseIntoBuf(byte*); //returns message length
        virtual ~HubMessage() = default;
};

class HubMessageMotorStartSpeed : public HubMessage
{
    private:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
        byte _speed;
        uint16_t _time;
        byte _maxPower;
        byte _useProfile;
    public:
        HubMessageMotorStartSpeed();
        HubMessageMotorStartSpeed(byte);
        HubMessageMotorStartSpeed(byte, uint16_t);
        HubMessageMotorStartSpeed(byte, byte, byte, byte, byte);
        byte parseIntoBuf(byte*);
};

class HubMessageMotorStop : public HubMessage
{
    private:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
    public:
        HubMessageMotorStop();
        HubMessageMotorStop(byte, byte);
        byte parseIntoBuf(byte*);
};

class HubMessageMotorGotoAbs : public HubMessage
{
    private:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
        int32_t _absPos;
        byte _speed;
        byte _maxPower;
        byte _endState;
        byte _useProfile;
    public:
        HubMessageMotorGotoAbs();
        byte parseIntoBuf(byte*);
};

class HubMessageMotorStartDeg : public HubMessage
{
    private:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
        int32_t _deg;
        byte _speed;
        byte _maxPower;
        byte _endState;
        byte _useProfile;
    public:
        HubMessageMotorStartDeg();
        HubMessageMotorStartDeg(int32_t, byte);
        byte parseIntoBuf(byte*);
};

#endif