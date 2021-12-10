#ifndef hm10HubBt_hpp
#define hm10HubBt_hpp
#include <ArduinoSTL.h>
#include "hubMessages.hpp"
#include <queue>

class Hm10HubBt
{
    // TODO: change into smart pointers? idk
    protected:
        HardwareSerial _hm10;
        std::queue<HubMessage> _msgOut;
        std::queue<HubMessage> _msgIn;
        byte _buffer[128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    public:
        Hm10HubBt(HardwareSerial, unsigned long = 115200);
        void msgOutEnqueue(HubMessage&);
        void msgOutSend();
};

#endif