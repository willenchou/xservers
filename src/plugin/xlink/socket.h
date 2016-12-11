#ifndef XLINK_SOCKET_H_
#define XLINK_SOCKET_H_
#include <string>
#include <xbase/os_common.h>


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

//ISOCKET ICHANNEL
class Socket{
public:


	Socket();
	~Socket();

	SOCKET GetSocket() const { return socket_; }
	void SetSocket(SOCKET fd) { socket_ = fd; }
	//SOCKET_STATE GetState() const { return state_; }
	//void SetState(SOCKET_STATE state) { state_ = state; }
	//Callback* GetCallback() { return callback_;}
	//void SetCallback(Callback* callback) { callback_ = callback; }
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

	static SOCKET NewSOCKET();
	static void SetNonblock(SOCKET fd);
	static void SetReuseAddr(SOCKET fd);
	static void SetNoDelay(SOCKET fd);

private:
	int GetErrorCode();
	bool IsBlock(int errorCode);

	
	void SetAddr(const char* ip, const uint16_t port,sockaddr_in* addr);
	

private:
	std::string					localIp_;
	uint16_t					localPort_;
	std::string					remoteIp_;
	uint16_t					remotePort_;

	SOCKET						socket_;
	//IChannel
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

#endif //XLINK_SOCKET_H_
