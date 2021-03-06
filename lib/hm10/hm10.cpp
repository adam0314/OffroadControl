#include "hm10.hpp"

void Hm10::init(unsigned long baudRate)
{
    HM10.begin(baudRate);
}

void Hm10::checkAndRead()
{
    if (!HM10.available()) return;

    byte sizeByte = HM10.read();

    if (sizeByte == 0x4F) // AT command message starting with char 0x4F = 'O' (like OK-CONN)
    {
        HM10.readBytesUntil(0x0A, _buffer, 32); // Throw this away
    }
    else // Assuming no msgs with length 4F come from Lego Hub, and all AT msgs start with 'O'
    {
        //Serial.println("Data from Hub on HM10:");
        _buffer[0] = sizeByte;
        for (int i = 1; i < sizeByte; i++)
        {
            _buffer[i] = HM10.read();
            delay(10);
        }
        helpers::printHex8(_buffer, sizeByte);
        Serial.println();
        // TODO: Put the message in msgIn for further handling
        switch(_buffer[2]) //msg type
        {
        case 0x82: // Port Output Feedback
            msgInEnqueue(std::make_shared<HubMsgMotorFeedback>(_buffer));
            break;
        
        default:
            break;
        }
    }
    delay(50); // kill me again
}

void Hm10::checkAndSend()
{
    while (_msgOut.size() > 0)
    {
        msgOutSend();
    }
}

void Hm10::msgOutEnqueue(std::shared_ptr<HubMsg> msg)
{
    _msgOut.push(msg);
}

void Hm10::msgOutSend()
{
    if (_msgOut.empty()) return;
    byte size = _msgOut.front()->parseIntoBuf(_buffer);
    //helpers::printHex8(_buffer, size);
    HM10.write(_buffer, size * sizeof(_buffer[0]));
    delay(50); // kill me
    _msgOut.pop();
}
void Hm10::msgInEnqueue(std::shared_ptr<HubMsg> msg)
{
    if (_msgIn.size() > 63)
    {
        _msgIn.pop(); // YEET
    }
    
    Serial.println("pushed your momma");
    _msgIn.push(msg);
}

void Hm10::msgOutClear()
{
    if (_msgOut.size() == 0) return;
    _msgOut = std::queue<std::shared_ptr<HubMsg>>();
}