#ifndef car_hpp
#define car_hpp
#include <hubMessages.hpp>
#include <hm10HubBt.hpp>

class Car
{
    private:
        
        std::shared_ptr<HubMessageMotorStartSpeed> _msgForward100;
        byte _buffer[128];
    public:
        Hm10 _hm10;
        Car();
        void send();
};

#endif