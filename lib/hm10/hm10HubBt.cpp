#include "hm10HubBt.hpp"

Hm10HubBt::Hm10HubBt(HardwareSerial hm10, unsigned long baudRate = 115200) : _hm10(hm10)
{
    _hm10.begin(baudRate);
}

void Hm10HubBt::msgOutEnqueue(HubMessage & msg)
{
    Serial.println("amogus");
    byte size = msg.parseIntoBuf(_buffer);
    //printHex8(buffer, size);
    Serial.println(_buffer[0]);
    Serial.println(_buffer[1]);
    Serial.println(_buffer[2]);
    Serial.println(_buffer[3]);
    Serial.println(_buffer[4]);
    Serial.println(_buffer[5]);
    Serial.println(_buffer[6]);
    Serial.println(_buffer[7]);
    Serial.println(_buffer[8]);
    _hm10.write(_buffer, size * sizeof(_buffer[0]));
}

void Hm10HubBt::msgOutSend()
{
    if (_msgOut.empty()) return;    

    byte size = _msgOut.front().parseIntoBuf(_buffer);
    Serial.println(_buffer[0]);
    Serial.println(_buffer[1]);
    Serial.println(_buffer[2]);
    Serial.println(_buffer[3]);
    Serial.println(_buffer[4]);
    Serial.println(_buffer[5]);
    Serial.println(_buffer[6]);
    Serial.println(_buffer[7]);
    Serial.println(_buffer[8]);
    _hm10.write(_buffer, size * sizeof(_buffer[0]));
    delay(100); // kill me
    _msgOut.pop();
}