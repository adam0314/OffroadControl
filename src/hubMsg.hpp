#ifndef hubMsg_h
#define hubMsg_h

namespace hubMsg
{
    extern int test;
    int testFn(int);

    struct HubMsg
    {
        byte* bytes;
        unsigned long long sizeOfBytes;

        HubMsg();
        ~HubMsg();
    };

    HubMsg getMessageForBytes(byte[], int);
}

#endif