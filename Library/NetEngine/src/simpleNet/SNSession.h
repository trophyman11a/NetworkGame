//
//  SNSession.h
//  SimpleNet
//
//  Created by evanlee on 7/3/2021.
//

#ifndef SNSession_h
#define SNSession_h

#include <stdio.h>
#include <vector>
#include <simpleNet/CoreLib.h>

#include "SNSocket.h"


namespace simpleNet {

class SNSession : NonCopyable {
public:
    SNSession(SNSocket *socket);
    SNSession(SNSocket &&socket);
    virtual ~SNSession();           // evan: why need virtual??

    SNSocket _mySocket;
    // SNSocket *_socket = nullptr;      // evan: the socketing connecting the client or server
    bool isHost;           
    
    
    
    int sendData(std::vector<u8> &dataBuf);
    void sendString(SNString &str);     // evan: is sendString(SNString *str) better??
    void sendString(const char *str);
    
    
    void setNonblock(bool flag);
    void setConnected(bool isHost);
    void close();
    bool isAlive();
    bool hasData();
    bool isConncting(size_t &availableByte);
    void receiveData();
    void putBufferWithStr(SNString &str);
    void flushBuffer();
    
protected:  // implemented by the subclass
    virtual void onConnect() = 0;
    virtual void onDisconnect() {}
    virtual void onRecvData(std::vector<u8> &buf, size_t &nRead) = 0; // evan: use & reference to save copy
    
private:
    std::vector<u8> _outBuffer;
    std::vector<u8> _inBuffer;            // evan: use to store the incoming buffer
    bool _isAlive;
    
    
    size_t availableBytesToRead();
};


}

#endif /* SNSession_h */
