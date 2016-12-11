
#include "channel.h"
namespace x {

Channel::Channel(SOCKET fd,EventDispatch* eventdispatch)
{
	callback_ = NULL;
	socket_ = fd;
	ownerEventDispatch_ = eventdispatch;
	state_ = Channel_STATE_CONNECTING;
}


Channel::~Channel(void)
{
}

void Channel::SetCallBack(ChannelCallback* callback){
	callback_ = callback;
}
void Channel::SetState(Channel_STATE state){
	state_ = state;
}
 Channel* Channel::NewChannel(){
	 return NULL;
 }

void Channel::OnRead() {
	//printf("Channel::OnRead--state_:[%d]",state_);
	if (state_ == Channel_STATE_LISTENING) {
		if (callback_) callback_->OnAccept();
	} else {
#ifdef OS_WIN
		u_long avail = 0;
		int iresult = ioctlsocket(socket_, FIONREAD, &avail);
		if (( iresult== SOCKET_ERROR) /*|| (avail == 0)*/) {	
			OnClose();
		} else {
			if (avail == 0)
			{
				return;
			}
			if (callback_) callback_->OnRead();
		}
#else
		if (callback_) callback_->OnRead();
	
#endif // WIN_32
	
			
		
	}
	
}

void Channel::OnWrite() {
	if (state_ == Channel_STATE_CONNECTING) {
		int error = 0;
		socklen_t len = sizeof(error);
		getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
		if (error) {
			OnClose();
		} else {
			state_ = Channel_STATE_CONNECTED;
			if (callback_) callback_->OnConfirm();
		}
	} else {
		if (callback_) callback_->OnWrite();
	}
}

void Channel::OnClose() {
	ownerEventDispatch_->RemoveEvent(socket_);
	state_ = Channel_STATE_CLOSING;
	if (callback_) callback_->OnClose();
}
void Channel::OnTimer(int id){

}
}
