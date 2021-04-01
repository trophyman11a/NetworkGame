//
//  SNSession.c
//  SimpleNet
//
//  Created by evanlee on 7/3/2021.
//

#include "SNSession.h"
#include <fcntl.h> /* Added for the nonblocking socket */

namespace simpleNet {


//SNSession::SNSession(SNSocket *socket)
//{
//    _socket = socket;
//
//    _isAlive = _socket == nullptr ? false : true;
//}

SNSession::SNSession(SNSocket &&socket)
:_mySocket(std::move(socket))
{
    _isAlive = true; // _mySocket.;
    std::cout << "SNSession: move constructor\n";
    _mySocket.printInfo();
}


SNSession::~SNSession()
{
//    if(_socket != nullptr)
//    {
//        _socket->close();
//        _socket = nullptr;
//    }
    _mySocket.close();
}
    
void SNSession::onRecvData(std::vector<u8> &buf, size_t &nRead)
{
    // Need implemented by child class
    std::cout << "onRecvData: " << nRead << " bytes received\n";
}

int SNSession::sendData(std::vector<u8> &dataBuf)
{
    int result = _mySocket.send(dataBuf.data(), dataBuf.size());
    //LOG("sendData: result=%d", result);
    
    return result;
}

size_t SNSession::availableBytesToRead()
{
    return _mySocket.availableBytesToRead();
}

void SNSession::setConnected(bool isHostFlag)
{
    isHost = isHostFlag;
    onConnect();
}

void SNSession::receiveData() {
    size_t nRead = availableBytesToRead();
    // log("SNSession.receiveData: nRead=%d", nRead);
    if(nRead == 0) {    // evan: nRead = 0 if not data retrieved at the moment
        //sleep(1);       // 1 second     //
        return;
    }

    _mySocket.recv(_inBuffer, nRead);
    _inBuffer.push_back(0);   // add the character '\0' to make it a string

    std::string strValue = std::string(_inBuffer.begin(), _inBuffer.end());
    // std::cout << "receiveData: " << strValue << "\n";
    
    onRecvData(_inBuffer, nRead);
}


void SNSession::close() {
    _isAlive = false;
    
    _mySocket.close();
}

bool SNSession::isConncting(size_t &availableByte)
{
    size_t result = availableBytesToRead();
    //log("SNSession.isConncting: nRead=%d", result);
    
    // evan: todo: Check for disconnection
    
    
//    if(nRead == 0) {    // evan: nRead = 0 if not data retrieved at the moment
//        return false;
//    }
    availableByte = result;
    return true;
}

bool SNSession::isAlive() {
    //log("SNSession: isAlive: %d", _isAlive);
    return _isAlive;
}

void SNSession::sendString(const char *str)
{
    SNString s = SNString(str); // evan: FIXME: how to write better?
    sendString(s);
}

// http://www.kegel.com/dkftpbench/nonblocking.html
void SNSession::flushBuffer()
{
    int sendCount = sendData(_outBuffer);
    if(sendCount < 0) {
        ERROR_LOG("flushBuffer. sendData error");
        return;
    }
    
    
    if(sendCount > 0) {
        std::string strValue = std::string(_outBuffer.begin(), _outBuffer.end());
        LOG("%s count:%d", strValue.c_str(), sendCount);
    }
    
    _outBuffer.erase(_outBuffer.begin(), _outBuffer.begin() + sendCount);
    //_outBuffer.clear();
}

void SNSession::putBufferWithStr(SNString &str)
{
    //_outBuffer.ins
    //str.
    str.appendTo(_outBuffer);
    // str.app
}

void SNSession::sendString(SNString &str)
{
    str.copyTo(_outBuffer);     // the outBuffer is clear here...!!!
    
    // Reference here!!!
    sendData(_outBuffer);
}

} // end of namespace
