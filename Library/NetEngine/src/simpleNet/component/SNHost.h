//
//  SNHostComponent.h
//  SimpleNet
//
//  Created by evanlee on 13/3/2021.
//

#ifndef SNHostComponent_h
#define SNHostComponent_h

#include <stdio.h>
#include "SNNetBase.h"

namespace simpleNet {

class SNSessionFactory;
class SNSession;
class SNSocket;
class SNString;

enum SNHostState {
    SNHostStateIdle,
    SNHostStatetWaitForClient,            // after bindPort and before accept
    SNHostStatetConnected,                // after accepted
};


class SNHost : public SNNetBase {
public:
    SNHost();
    //void setSessionFactory(SNSessionFactory *factory);
    bool bindPort(int port);
    void checkNetwork();           // evan: is it a good method name?
//    void sendDataOut();
//    void queueToOutBuffer(SNString &str);
    
    // SNSession *getSession();        // evan: ???: should use const SNSession?
private:
//    SNSessionFactory* _factory = NULL;
//    SNSession *_session = NULL;         // connected session
//    SNSocket *_clientSocket = NULL;           // connected socket
    SNSocket *_serverSocket = NULL;
    int _port = 0;
    SNHostState _state;
    
private:
    void initServerSocket();
    void initClientSocket();
    
    bool attemptAccept();       // true if successfully accepted
    //bool checkIncomingData();         // false if connection broken

    void checkNetworkWhenWaitClient();
    void checkNetworkWhenConnect();
};

}

#endif /* SNHostComponent_h */
