#ifndef car_hpp
#define car_hpp
#include <hubMessages.hpp>

class Car
{
    private:
        HubMessageMotorStartSpeed _msgForward100;
        byte _buffer[128];
    public:
        Car();
        void send();
};

#endif