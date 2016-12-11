#ifndef MULTIPLEXING_H_
#define MULTIPLEXING_H_

#include "socket.h"
#include <map>
#include <vector>

namespace x{

	struct IOEventCallback { 
		virtual void OnRead() = 0;
		virtual void OnWrite() =0;
		virtual void OnClose() =0;
		virtual void OnTimer(int id) =0;
	};


	enum IO_EVENT
	{
		io_event_none = 0,
		io_event_read,//recv io_event_accept,//newconnect
		io_event_write,//send ,connected
		io_event_close //error close
	};
	struct EventItem {
		SOCKET fd;
		IOEventCallback* callback;
		IO_EVENT io_event; //转换成统一的事件
	};

	class IMultiPlexing{

	/*public:
		typedef std::vector<EventItem*> EventList;*/

	public:
		/// Polls the I/O events.
		/// Must be called in the loop thread.
		virtual int poll(int timeoutMs) = 0;

		/// add the interested I/O events.
		/// Must be called in the loop thread.
		virtual void addChannel(SOCKET fd,IOEventCallback* channel) = 0;

		/// Remove the channel, when it destructs.
		/// Must be called in the loop thread.
		virtual void removeChannel(SOCKET fd) = 0;

		static IMultiPlexing* newMultiPlexing();
	

		//EventMap readevents_;
		//EventMap writeevents_;
		//EventMap erroevents_;

	};
}
#endif//MULTIPLEXING_H_


