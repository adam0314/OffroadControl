#ifndef hubMessages_hpp
#define hubMessages_hpp
#include <arduino.h>

enum MsgType
{
    // OUT
    MOTOR_START_SPEED,
    MOTOR_START_SPEED_FOR_TIME,
    MOTOR_GOTO_ABS,
    MOTOR_STOP,
    MOTOR_START_DEG,
    HUB_ACTION,
    PORT_SYNC,
    PORT_INPUT_FORMAT,
    // IN
    MOTOR_FEEDBACK
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

// OUTBOUND

class HubMsgMotor : public HubMsg
{
    protected:
        byte _portId;
        byte _startComplInfo;
        byte _subCommand;
    public:
        HubMsgMotor(MsgType, byte, byte, byte, byte, byte);
        virtual byte parseIntoBuf(byte*);
        void useFeedback();
        void useExecuteImmediately();
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

class HubMsgVirtualPortSetup : public HubMsg
{
    private:
        bool _connect;
        byte _portIdA;
        byte _portIdB;
    public:
        HubMsgVirtualPortSetup(bool = true);
        HubMsgVirtualPortSetup(bool, byte, byte = 0);
        byte parseIntoBuf(byte*);
};

class HubMsgPortInputFormat : public HubMsg
{
    private:
        byte _portId;
        byte _mode; // 0x03: APOS (abs. position)
        uint32_t _delta;
        bool _enableNotification;
    public:
        HubMsgPortInputFormat();
        HubMsgPortInputFormat(byte, byte, uint32_t, bool);
        byte parseIntoBuf(byte*);
};

// INBOUND

class HubMsgMotorFeedback : public HubMsgMotorStartDeg
{
    private:
        byte _portId;
        byte _feedbackBitfields;
    public:
        HubMsgMotorFeedback(byte*);
        void parseFromBuf(byte*);
        byte getPortId();
        bool isIdle();
        bool isEmptyAndCompleted();
};

#endif