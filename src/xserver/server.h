#ifndef XSERVER_SERVER_H_
#define XSERVER_SERVER_H_

#include <string>
#include <xbase/xml.h>
#include <include/xservice_intf.h>
#include "container.h"

namespace x {

class Server {
public:
	Server();
	~Server();

	bool Run();

private:
	bool Init();
	bool LoadConfig();
	void MainLoop();

	x::XMLDocument xmlDoc_;
	Container* container_;
};

}//namespace x

extern x::Server* _app;

#endif //XSERVER_SERVER_H_
