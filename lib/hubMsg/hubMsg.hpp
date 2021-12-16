#ifndef hubMessages_hpp
#define hubMessages_hpp
#include <arduino.h>

enum MsgType
{
    MOTOR_START_SPEED,
    MOTOR_START_SPEED_FOR_TIME,
    MOTOR_GOTO_ABS,
    MOTOR_STOP,
    MOTOR_START_DEG,
    HUB_ACTION,
    PORT_SYNC
};

class HubMsg
{
    protected:
        MsgType _messageType;
        byte _command;
        byte _length;
        HubMsg();
        HubMsg(MsgType, byte, byte);
    public:
        virtual byte parseIntoBuf(byte*); //returns message length
        virtual ~HubMsg() = default;
};

class HubMsgMotor : public HubMsg
{
    protected:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
    public:
        HubMsgMotor(MsgType, byte, byte, byte, byte, byte);
        virtual byte parseIntoBuf(byte*);
        void withFeedback();
};

class HubMsgMotorStartSpeed : public HubMsgMotor
{
    private:
        byte _speed;
        uint16_t _time;
        byte _maxPower;
        byte _useProfile;
    public:
        HubMsgMotorStartSpeed();
        HubMsgMotorStartSpeed(byte);
        HubMsgMotorStartSpeed(byte, uint16_t);
        HubMsgMotorStartSpeed(byte, byte, byte, byte, byte);
        byte parseIntoBuf(byte*);
};

class HubMsgMotorStop : public HubMsgMotor
{
    public:
        HubMsgMotorStop();
        HubMsgMotorStop(byte, byte);
        byte parseIntoBuf(byte*);
};

class HubMsgMotorGotoAbs : public HubMsgMotor
{
    private:
        int32_t _absPos;
        byte _speed;
        byte _maxPower;
        byte _endState;
        byte _useProfile;
    public:
        HubMsgMotorGotoAbs();
        byte parseIntoBuf(byte*);
};

class HubMsgMotorStartDeg : public HubMsgMotor
{
    private:
        int32_t _deg;
        byte _speed;
        byte _maxPower;
        byte _endState;
        byte _useProfile;
    public:
        HubMsgMotorStartDeg();
        HubMsgMotorStartDeg(int32_t, byte);
        byte parseIntoBuf(byte*);
};

class HubMsgHubAction : public HubMsg
{
    private:
        byte _actionType;
    public:
        HubMsgHubAction();
        HubMsgHubAction(byte);
        byte parseIntoBuf(byte*);
};

#endif