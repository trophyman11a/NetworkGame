//
//  SimpleNetAppSession.h
//  test_gui
//
//  Created by evanlee on 14/3/2021.
//

#ifndef SimpleNetAppSession_h
#define SimpleNetAppSession_h

#include <stdio.h>

#include <stdio.h>
#include <simpleNet/NetEngine.h>

using namespace simpleNet;

class SimpleNetApp;

class SimpleHostSession : public SNSession
{
public:
    SimpleNetApp &mainApp;
    std::shared_ptr<SimpleNetApp> app;
    bool isHost;
    
    SimpleHostSession(SNSocket && sock, SimpleNetApp &_app, bool isHost);
    ~SimpleHostSession();
    
    virtual void onRecvData(std::vector<u8> &buf, size_t &nRead) override;
    virtual void onConnect() override;
    virtual void onDisconnect() override;
    
    void extractCommands(
                std::vector<u8> &buf, std::vector<SNString> &commands);
    
private:
    std::vector<char> _remainCommandBuf;    // evan: store the incomplete command
    std::vector<SNString> _cmdList;
};


class SimpleSessionFactory : public SNSessionFactory {
public:
    
    SimpleSessionFactory(SimpleNetApp& appRef, bool isHost)
        : SNSessionFactory()
        , _appRef(appRef)
        , _isHost(isHost)
    {
    }
    
    virtual std::unique_ptr<SNSession> newSession(SNSocket &&socket) override {        
        auto ptr = std::make_unique<SimpleHostSession>(
                            std::move(socket), _appRef, _isHost);
        return ptr;
    }
   
private:
    bool _isHost;
    SimpleNetApp& _appRef;
};

#endif /* SimpleNetAppSession_h */
