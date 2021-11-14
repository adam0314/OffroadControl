#ifndef hubMessages_h
#define hubMessages_h

enum MessageType
{
    MOTOR_START_SPEED,
    MOTOR_GOTO_ABS,
    MOTOR_BRAKE,
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
        bool _goForward;
        byte _maxPower;
        byte _useProfile;
    public:
        HubMessageMotorSetSpeed();
        HubMessageMotorSetSpeed(byte, byte, byte, bool, byte, byte);
        byte parseIntoBuf(byte*);
};

#endif