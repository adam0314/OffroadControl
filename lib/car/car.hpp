#ifndef car_hpp
#define car_hpp
#include "hubMsg.hpp"
#include "hm10.hpp"

enum TurnState {TURNR, TURNL, TURN0};

class Car
{
    private:
        // Messages and buffer
        std::shared_ptr<HubMsgHubAction> _msgSwitchOff;
        std::shared_ptr<HubMsgMotorStartSpeed> _msgForward100;
        std::shared_ptr<HubMsgMotorStartSpeed> _msgBack100;
        std::shared_ptr<HubMsgMotorStop> _msgStop;
        std::shared_ptr<HubMsgMotorStartDeg> _msgTurnL;
        std::shared_ptr<HubMsgMotorStartDeg> _msgTurnR;
        std::shared_ptr<HubMsgMotorGotoAbs> _msgTurn0;
        byte _buffer[128];

        //Other
        Hm10 _hm10;
        TurnState _turnState;
    public:
        Car();
        void init();
        void initHm10(unsigned long = 115200);
        void loopHm10();
        void switchOff();

        // Drive or turning commands
        void setSpeed(byte);
        void setSpeed(byte, uint16_t);
        void driveForward();
        void driveBack();
        void stop();
        void turn0();
        void turnR();
        void turnL();
};

#endif