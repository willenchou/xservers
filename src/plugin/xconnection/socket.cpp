#include "socket.h"
#include "errors.h"

namespace x {

Socket::Socket()
:state_(SOCKET_STATE_IDLE)
,socket_(INVALID_SOCKET)
,callback_(NULL){

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

	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_ == INVALID_SOCKET) {
		return false;
	}

	SetReuseAddr(socket_);
	SetNonblock(socket_);

	sockaddr_in svrAddr;
	SetAddr(ip, port, &svrAddr);
	int ret = bind(socket_, (sockaddr*)&svrAddr, sizeof(svrAddr));
	if (ret == SOCKET_ERROR) {
		return false;
	}

	state_ = SOCKET_STATE_LISTENING;
	
	EventDispatch::Instance()->AddEvent(socket_, this);

	ret = listen(socket_, 64);
	if (ret == SOCKET_ERROR) {
		return false;
	}

	return true;
}

bool Socket::Connect(const char* ip, uint16_t port) {
	remoteIp_ = ip;
	remotePort_ = port;

	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_ == INVALID_SOCKET) {
		return false;
	}
	
	SetNonblock(socket_);
	SetNoDelay(socket_);
	
	state_ = SOCKET_STATE_CONNECTING;
	
	EventDispatch::Instance()->AddEvent(socket_, this);
	
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
	if (state_ != SOCKET_STATE_CONNECTED) {
		return -1;
	}

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
	EventDispatch::Instance()->RemoveEvent(socket_);
	closesocket(socket_);
	return 0;
}

void Socket::OnRead() {
	if (state_ == SOCKET_STATE_LISTENING) {
		AcceptNewSocket();
	} else {
		u_long avail = 0;
		if ((ioctlsocket(socket_, FIONREAD, &avail) == SOCKET_ERROR) || (avail == 0)) {
			OnClose();
		} else {
			if (callback_) callback_->OnRead();
		}
	}
}

void Socket::OnWrite() {
	if (state_ == SOCKET_STATE_CONNECTING) {
		int error = 0;
		int len = sizeof(error);
		getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
		if (error) {
			OnClose();
		} else {
			state_ = SOCKET_STATE_CONNECTED;
			if (callback_) callback_->OnConfirm();
		}
	} else {
		if (callback_) callback_->OnWrite();
	}
}

void Socket::OnClose() {
	EventDispatch::Instance()->RemoveEvent(socket_);
	state_ = SOCKET_STATE_CLOSING;
	if (callback_) callback_->OnClose();
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

void Socket::SetNonblock(SOCKET fd) {
#ifdef OS_WIN
	u_long nonblock = 1;
	int ret = ioctlsocket(fd, FIONBIO, &nonblock);
#else
	int ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL);
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

void Socket::AcceptNewSocket() {
	SOCKET fd = 0;
	sockaddr_in peerAddr;
	socklen_t addrLen = sizeof(sockaddr_in);
	char ipStr[64];

	while ((fd = accept(socket_, (sockaddr*)&peerAddr, &addrLen)) != INVALID_SOCKET ) {
		Socket* socket = new Socket();

		uint32_t ip = ntohl(peerAddr.sin_addr.s_addr);
		uint16_t port = ntohs(peerAddr.sin_port);
		
		snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

		socket->SetSocket(fd);
		socket->SetState(SOCKET_STATE_CONNECTED);
		socket->SetRemoteIP(ipStr);
		socket->SetRemotePort(port);

		SetNoDelay(fd);
		SetNonblock(fd);
		
		EventDispatch::Instance()->AddEvent(fd, socket);
		
		if (callback_) callback_->OnConnect(socket);
	}
}

}//namespace x