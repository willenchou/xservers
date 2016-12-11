#ifndef XCONNECTION_SOCKET_H_
#define XCONNECTION_SOCKET_H_

#include <string>
#include <xbase/os_common.h>
#include "event_dispatch.h"

namespace x {

#ifdef OS_WIN
	typedef int socklen_t;
#else
	typedef int SOCKET;
	const int SOCKET_ERROR = -1;
	const int INVALID_SOCKET = -1;
#endif

enum SOCKET_STATE {
	SOCKET_STATE_IDLE,
	SOCKET_STATE_LISTENING,
	SOCKET_STATE_CONNECTING,
	SOCKET_STATE_CONNECTED,
	SOCKET_STATE_CLOSING
};

const uint32_t MAX_SOCKET_BUF_SIZE = 128 * 1024;

class Socket : public EventDispatch::Callback{
public:
	class Callback {
	public:
		virtual void OnConnect(Socket* socket) {}
		virtual void OnConfirm() {}
		virtual void OnRead() {}
		virtual void OnWrite() {}
		virtual void OnClose() {}
	};

	Socket();
	~Socket();
	
	SOCKET GetSocket() const { return socket_; }
	void SetSocket(SOCKET fd) { socket_ = fd; }
	SOCKET_STATE GetState() const { return state_; }
	void SetState(SOCKET_STATE state) { state_ = state; }
	Callback* GetCallback() { return callback_;}
	void SetCallback(Callback* callback) { callback_ = callback; }
	const char* GetRemoteIP() { return remoteIp_.c_str();}
	void SetRemoteIP(char* ip) { remoteIp_ = ip; }
	uint16_t GetRemotePort() { return remotePort_; }
	void SetRemotePort(uint16_t port) { remotePort_ = port; }
	void SetRecvBufSize(uint32_t size);
	void SetSendBufSize(uint32_t size);


	bool Listen(const char* ip, uint16_t port);
	bool Connect(const char* ip, uint16_t port);
	int Send(void* buf, int len);
	int Recv(void* buf, int len);
	int Close();

	void OnRead();
	void OnWrite();
	void OnClose();

private:
	int GetErrorCode();
	bool IsBlock(int errorCode);

	void SetNonblock(SOCKET fd);
	void SetReuseAddr(SOCKET fd);
	void SetNoDelay(SOCKET fd);
	void SetAddr(const char* ip, const uint16_t port, sockaddr_in* addr);
	
	void AcceptNewSocket();

private:
	std::string					localIp_;
	uint16_t					localPort_;
	std::string					remoteIp_;
	uint16_t					remotePort_;

	Callback*					callback_;

	SOCKET_STATE				state_;
	SOCKET						socket_;
};


#ifdef OS_WIN
class SocketInitHelper {
public:
	SocketInitHelper() {
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		WSAStartup(wVersionRequested, &wsaData);
	}
	
	~SocketInitHelper() {
		WSACleanup();
	}
};
static SocketInitHelper _socketInitHelper;
#endif

}//namespace x

#endif //XCONNECTION_SOCKET_H_
