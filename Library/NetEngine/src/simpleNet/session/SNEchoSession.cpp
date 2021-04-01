//
//  SNEchoSession.c
//  NetEngine
//
//  Created by evanlee on 7/3/2021.
//

#include "SNEchoSession.h"
namespace simpleNet {


SNEchoSession::SNEchoSession(SNSocket &&sock)
    : SNSession(std::move(sock))
{
    
}


void SNEchoSession::onConnect()
{
    sendString("Welcome to Echo server\n");
}

void SNEchoSession::onRecvData(std::vector<u8> &buf, size_t &nRead)
{
    const char *msg = (const char *) buf.data();
    std::cout << " input: " << msg << "\n";

    SNString returnMsg = SNString("ECHO: ");
    returnMsg.append(msg);
    
    
    sendString(returnMsg);
}


} // end of namespace
