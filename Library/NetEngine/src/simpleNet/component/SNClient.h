//
//  SNClient.h
//  NetEngine
//
//  Created by evanlee on 13/3/2021.
//

#ifndef SNClient_h
#define SNClient_h

#include <stdio.h>
#include "SNNetBase.h"

namespace simpleNet {

enum SNClientState {
    SNClientStateIdle,
    SNClientStatetConnected,                // after accepted
};


class SNSessionFactory;
class SNSession;
class SNSocket;
class SNSocketAddr;



class SNClient : public SNNetBase {
public:
    SNClient();
    //void setSessionFactory(SNSessionFactory *factory);
    bool connectServer(const SNSocketAddr &addr);
    void checkNetwork();           // evan: is it a good method name?
    
//private:
//    SNSessionFactory* _factory = NULL;
//    SNSession *_session = NULL;         // connected session
//    SNSocket *_clientSocket = NULL;           // connected socket
    SNClientState _state;
    
private:
//    void initClientSocket();
    void checkNetworkWhenConnect();
//    
//    bool checkIncomingData();         // false if connection broken

    
};

}


#endif /* SNClient_h */
