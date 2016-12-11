#include "socket.h"


namespace x {

Socket::Socket()
//:state_(SOCKET_STATE_IDLE)
:socket_(INVALID_SOCKET){

}

Socket::~Socket() {
	
}

void Socket::SetRecvBufSize(uint32_t size) {
	int ret = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
	if (ret == SOCKET_ERROR) {
	}

	int realSize = 0;
	socklen_t len = sizeof(realSize);
	getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&realSize, &len);
}

void Socket::SetSendBufSize(uint32_t size) {
	int ret = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
	if (ret == SOCKET_ERROR) {
	}

	int realSize = 0;
	socklen_t len = sizeof(realSize);
	getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&realSize, &len);
}

bool Socket::Listen(const char* ip, uint16_t port) {
	localIp_ = ip;
	localPort_ = port;


	SetReuseAddr(socket_);
	SetNonblock(socket_);

	sockaddr_in svrAddr;
	SetAddr(ip, port, &svrAddr);
	int ret = bind(socket_, (sockaddr*)&svrAddr, sizeof(svrAddr));
	if (ret == SOCKET_ERROR) {
		return false;
	}

	//state_ = SOCKET_STATE_LISTENING;	

	ret = listen(socket_, 64);
	if (ret == SOCKET_ERROR) {
		return false;
	}

	return true;
}

bool Socket::Connect(const char* ip, uint16_t port) {
	remoteIp_ = ip;
	remotePort_ = port;
	
	SetNonblock(socket_);
	SetNoDelay(socket_);
	
	//state_ = SOCKET_STATE_CONNECTING;
		
	sockaddr_in svrAddr;
	SetAddr(ip, port, &svrAddr);
	int ret = connect(socket_, (sockaddr*)&svrAddr, sizeof(svrAddr));
	if (ret == SOCKET_ERROR) {
		//closesocket(socket_);
		return false;
	}
	
	return true;
}

int Socket::Send(void* buf, int len) {

	int ret = send(socket_, (char*)buf, len, 0);
	if (ret == SOCKET_ERROR) {
		int errCode = GetErrorCode();
		if (IsBlock(errCode)) {
			ret = 0;
		} else {
			//log
		}
	}

	return ret;
}

int Socket::Recv(void* buf, int len) {
	return recv(socket_, (char*)buf, len, 0);
}

int Socket::Close() {
#ifdef OS_WIN
	closesocket(socket_);
#else
	close(socket_);
#endif

	return 0;
}

int Socket::GetErrorCode() {
#ifdef OS_WIN
	return WSAGetLastError();
#else
	return errno;
#endif
}

bool Socket::IsBlock(int errorCode) {
#ifdef OS_WIN
	return ( (errorCode == WSAEINPROGRESS) || (errorCode == WSAEWOULDBLOCK) );
#else
	return ( (errorCode == EINPROGRESS) || (errorCode == EWOULDBLOCK) );
#endif
}
 SOCKET Socket::NewSOCKET(){
	return  socket(AF_INET, SOCK_STREAM, 0);
 }
void Socket::SetNonblock(SOCKET fd) {
#ifdef OS_WIN
	u_long nonblock = 1;
	int ret = ioctlsocket(fd, FIONBIO, &nonblock);
#else
	int ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
#endif
	if (ret == SOCKET_ERROR) {
		//log something
	}
}

void Socket::SetReuseAddr(SOCKET fd) {
#ifdef OS_WIN
    char reuse = 1;
#else
    int reuse = 1;
#endif
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if (ret == SOCKET_ERROR) {
		//log something
	}
}

void Socket::SetNoDelay(SOCKET fd) {
#ifdef OS_WIN
    char nodelay = 1;
#else
    int nodelay = 1;
#endif
	int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
	if (ret == SOCKET_ERROR) {
		//log something
	}
}

void Socket::SetAddr(const char* ip, const uint16_t port, sockaddr_in* addr) {
	memset(addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip);
	if (addr->sin_addr.s_addr == INADDR_NONE) {
		hostent* host = gethostbyname(ip);
		if (host == NULL) {
			return;
		}

		addr->sin_addr.s_addr = *(uint32_t*)host->h_addr;
	}
}



}//namespace x