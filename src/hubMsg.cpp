#include "hubMsg.hpp"

hubMsg::HubMsg hubMsg::getMessageForBytes(byte inputBytes[], int length)
{
    HubMsg msg;
    msg.bytes = new byte[length + 2];
    msg.bytes[0] = length;
    msg.bytes[1] = 0x00;
    for (int i = 0; i < length; i++)
    {
        msg.bytes[i+2] = inputBytes[i];
    }
    msg.sizeOfBytes = sizeof(msg.bytes);
    return msg;
};

int hubMsg::testFn(int argg)
{
    return 6 + argg;
};

hubMsg::HubMsg::HubMsg() {};
hubMsg::HubMsg::~HubMsg()
{
    delete[] bytes;
};