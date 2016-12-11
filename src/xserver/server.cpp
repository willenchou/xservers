#include "server.h"
#include <iostream>
#include <xbase/util.h>
#include <xbase/thread.h>
#include "plugin.h"
#include "args.h"

namespace x {

Server::Server(){
	container_ = new Container();
}

Server::~Server(){

}

bool Server::Run(){
	if(!Init()){
		return false;
	}

	if(!container_->Start()){
		return false;
	}

	MainLoop();

	return true;
}

bool Server::Init(){
	if(!LoadConfig()){
		return false;
	}

	if(!container_->Init(&xmlDoc_)){
		return false;
	}

	return true;
}

bool Server::LoadConfig(){
	Args* args = Args::Instance();

	const char* file = args->GetArg(ARGS_APP_CFG);
	if(!file){
		return false;	
	}

	if(xmlDoc_.LoadFile(file) != XML_SUCCESS){
		std::cerr << "load " << file <<" fail" << std::endl;
		return false;
	}
	
	XMLElement* appNode = xmlDoc_.FirstChildElement("app");
	if(!appNode){
		return false;
	}

	const char* appName = appNode->Attribute("name");
	if(!appName){
		return false;
	}
	args->SetArg(ARGS_APP_NAME, appName);

	return true;
}

void Server::MainLoop(){
	while(1){
		x::SleepX(10);
	}
}

}//namespace x