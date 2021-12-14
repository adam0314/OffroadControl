#ifndef hm10HubBt_hpp
#define hm10HubBt_hpp
#include <ArduinoSTL.h>
#include "hubMessages.hpp"
#include "helpers.hpp"
#include <ArxSmartPtr.h>
#include <queue>

#define HM10 Serial1

class Hm10
{
    // TODO: change into smart pointers? idk
    protected:
        std::queue<std::shared_ptr<HubMessage>> _msgOut;
        std::queue<HubMessage*> _msgIn; // uhh kinda useless now
        byte _buffer[128] = {0x00};
    public:
        void init(unsigned long = 115200);
        void checkAndRead();
        void checkAndSend();
        void msgOutEnqueue(std::shared_ptr<HubMessage>);
        void msgOutSend();
};

#endif