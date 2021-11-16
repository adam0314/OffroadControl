#ifndef hubMessages_h
#define hubMessages_h

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
};

class HubMessageMotorSetSpeed : HubMessage
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
        HubMessageMotorSetSpeed();
        HubMessageMotorSetSpeed(byte);
        HubMessageMotorSetSpeed(byte, uint16_t);
        HubMessageMotorSetSpeed(byte, byte, byte, byte, byte);
        byte parseIntoBuf(byte*);
};

class HubMessageMotorStop : HubMessage
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

#endif