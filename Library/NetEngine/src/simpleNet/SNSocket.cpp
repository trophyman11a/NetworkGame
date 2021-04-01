//
//  SNSocket.cpp
//  SimpleNet
//
//  Created by evanlee on 6/3/2021.
//


#include "SNSocket.h"
#include <iostream>
#include <unistd.h>
#include <exception>
#include <simpleNet/CoreLib.h>
#include <fcntl.h> /* Added for the nonblocking socket */

#define BACKLOG 10

namespace simpleNet {

/**
 SNSocketAddr
 */
SNSocketAddr::SNSocketAddr()
{
	memset(&_addr, 0, sizeof(_addr));
    
}

void SNSocketAddr::setPort(uint16_t port)
{
    *reinterpret_cast<uint16_t*>(_addr.sa_data) = htons(port);
}

uint16_t SNSocketAddr::port() const
{
    return ntohs(*reinterpret_cast<const uint16_t*>(_addr.sa_data));
}

void SNSocketAddr::setIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    _addr.sa_family = AF_INET;
    _addr.sa_data[2] = a;
    _addr.sa_data[3] = b;
    _addr.sa_data[4] = c;
    _addr.sa_data[5] = d;
}


/**
 Socket
 */


class SNSocketInit {
public:
#ifdef _WIN32
    SNSocketInit() {
        WSADATA data;
        if (0 != WSAStartup(MAKEWORD(2,2), &data)) {
            std::cout << "fail to start up Winsocket\n";
            return;
            //throw std::exception("WSAStartup");
        }
    }

    ~SNSocketInit() {
        WSACleanup();
    }

    static void init() {
        static SNSocketInit s;
    }
#else
    static void init() {
    }
#endif
};

SNSocket::SNSocket()
{
    _enableReuseAddress = false;
}

SNSocket::SNSocket(SNSocket &&other)
: _sock(other._sock)
{
    // Copy
    std::cout << "Move Constructor is called\n";
    other._sock = INVALID_SOCKET;
}

int SNSocket::getSockFd()
{
    return _sock;
}

void SNSocket::setReuseAddress(bool enableReuse)
{
    _enableReuseAddress = enableReuse;
}

void SNSocket::setNonBlock(bool nonBlock)
{
    int flags;
    flags = ::fcntl(_sock ,F_GETFL,0);   // F_GETFL =  Get the file access mode
                                        //           and the file status flags;
    //assert(flags != -1);
    if(nonBlock) {
        ::fcntl(_sock, F_SETFL, flags | O_NONBLOCK);     // Set the mode to nonBlock
        //::fcntl(_sock, F_SETFL, flags |);     // Set the mode to nonBlock
    } else {
        fcntl(_sock, F_SETFL, flags & ~O_NONBLOCK);    // unset nonBlocking flag
        
    }
}

/**
 SNSocket
 */
void SNSocket::close()
{    
    if (_sock != INVALID_SOCKET)
    {
        #ifdef _WIN32
            closesocket(_sock);
        #else
            ::close(_sock);
        #endif
        _sock = INVALID_SOCKET;
        std::cout << "Socket is closed\n";
    }
}

void SNSocket::createTCP()
{
    close();

    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_sock == INVALID_SOCKET) {
        // throw std::exception("createTCP");
        std::cout << "fail to connect socket\n";
        return;
    }
    // std::cout << "socket okay. sock=" << _sock << "\n";
}

void SNSocket::enableReuseAddress(const SNSocketAddr& addr)
{
    if(_sock <= INVALID_SOCKET) {
        std::cout << "enableReuseAddress: socket not ready\n";
        return;
    }
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&addr._addr),
               static_cast<int>(sizeof(addr._addr)));
    std::cout << "DEBUG: enableReuseAddress: option is set\n";
}

bool SNSocket::bind(const SNSocketAddr& addr)
{
    if(_enableReuseAddress) {
        enableReuseAddress(addr);
    }
    
    int ret = ::bind(_sock, &addr._addr, sizeof(addr._addr));
    if (ret != 0) {
        //throw MyError("bind");
        std::cout << "fail to bind socket\n";
        return false;
    }

    std::cout << "Bind to port: " <<  addr.port() << "\n";
    return true;
}



bool SNSocket::listen(int backLog)
{
    int ret = ::listen(_sock, BACKLOG);
    if (ret != 0) {
        //throw MyError("bind");
        
        std::cout << "fail to bind socket\n";
        return false;
    }

    LOG("Waiting for incoming data");
    
    return true;
}


void SNSocket::connect(const SNSocketAddr& addr)
{
    int ret = ::connect(_sock, &addr._addr, sizeof(addr._addr));
    if (ret < 0) {
        ERROR_LOG("connect: fail to connect: %d; error=%d", ret, errno);
        throw SNError("connect");
    }
    ERROR_LOG("connect: Socket connected");
}

SNSocketAcceptStatus SNSocket::attempAccept(SNSocket &acceptedSocket)
{
    acceptedSocket.close();

    // https://jameshfisher.com/2017/04/05/set_socket_nonblocking/
    int value = ::accept(_sock, nullptr, nullptr);
    if(value == -1) {
        if (errno == EWOULDBLOCK) {
            return SNSocketAcceptStatus::Pending;
        }
        return SNSocketAcceptStatus::Fail;
    }
    
    acceptedSocket._sock = value;
    
    return SNSocketAcceptStatus::Success;
//    if (c == INVALID_SOCKET) {
//        return false;
//    }
//
//    // printf("accept\n");
//    acceptedSocket._sock = c;
//    return true;
}


bool SNSocket::accept(SNSocket &acceptedSocket)
{
    acceptedSocket.close();

    auto c = ::accept(_sock, nullptr, nullptr);
    if (c == INVALID_SOCKET) {
        return false;
    }

    // printf("accept\n");
    acceptedSocket._sock = c;
    return true;
}


int SNSocket::send(const char* data, size_t dataSize)
{
    return send(reinterpret_cast<const u8*>(data), dataSize);
}


int SNSocket::send(const u8* data, size_t dataSize)
{
    if (dataSize > INT_MAX) {
        std::cout << "Send dataSize too big\n";
        return -1;
    }

    int ret = ::send(_sock, data, (int)dataSize, 0);
    if (ret < 0) {
        std::cout << "Fail to send " << ret << "\n";
        return -1;
    }
    //std::cout << "DataSize: " << dataSize << "\n";
    
    return ret; 
}


size_t SNSocket::availableBytesToRead()
{
#ifdef _WIN32
    u_long n = 0;
    if (0 != ::ioctlsocket(_sock, FIONREAD, &n)) {
        throw SNError("availableBytesToRead");
    }
    
    return static_cast<size_t>(n);
#else
    int n = 0;
    
    // io control: FIONREAD,
    // returns the number of data bytesin the location pointed
    int result = ::ioctl(_sock, FIONREAD, &n);
    if (0 != result) {
        ERROR_LOG("ERROR: availableBytesToRead: result is %d errno=%d", result, errno);
        throw SNError("availableBytesToRead");
    }

    return static_cast<size_t>(n);
#endif
    return 0;
}



void SNSocket::recv(std::vector<u8> & buf, size_t bytesToRecv)
{
    buf.clear();
    
    if (bytesToRecv > INT_MAX) {
        throw SNError("recv: bytesToRecv is too big");
    }
    
    buf.resize(bytesToRecv);

    int ret = ::recv(_sock, buf.data(), (int)bytesToRecv, 0);
    if (ret < 0) {
        throw SNError("recv: fail to recv");
    }

}

void SNSocket::mockConnect()
{
    _sock = 100;
}

void SNSocket::printInfo()
{
    std::cout << "info: " << _sock << "\n";
}
}
