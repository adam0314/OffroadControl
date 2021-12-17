#ifndef car_hpp
#define car_hpp
#include "hubMsg.hpp"
#include "hm10.hpp"

enum TurnState {TURNR, TURNL, TURN0};

class Car
{
    private:
        // Messages and buffer
        HubMsgHubAction _msgSwitchOff;
        HubMsgVirtualPortSetup _msgPortSync;
        HubMsgMotorStartSpeed _msgForward100;
        HubMsgMotorStartSpeed _msgBack100;
        HubMsgMotorStop _msgStop;
        HubMsgMotorStartDeg _msgTurnL;
        HubMsgMotorStartDeg _msgTurnR;
        HubMsgMotorGotoAbs _msgTurn0;
        byte _buffer[128];

        //Turn values
        int16_t turnRPos;
        int16_t turnLPos;
        int16_t turn0Pos;

        //Other
        Hm10 _hm10;
        TurnState _turnState;
    public:
        Car();
        void init();
        void initHm10(unsigned long = 115200);
        void loopHm10();
        void switchOff();

        // Misc commands
        void syncPorts();
        void calibrateTurning();

        // Drive or turning commands
        void setSpeed(byte);
        void setSpeed(byte, uint16_t);
        void driveForward();
        void driveBack();
        void stop();
        void turn0(bool = false);
        void turnR();
        void turnL();
};

#endif