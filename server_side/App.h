#pragma once
#include "MessageHandler.h"
#include "FrameTimer.h"
#include <string>
#include <mutex>
//#include <thread>


class App
{
public:
	App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in);
	//std::pair<int, int> UpdatePos(const std::string& command);
	void UpdateLoop();
private:
	void GetMessage();
	void UpdateParameters();
	void SetMessage();
	void Unpack(const std::string& command);
	void PackToString();
private:
	FrameTimer ft;
	int x = 100;
	int y = 100;
	//std::thread UpdateThread;
	std::shared_ptr<MessageHandler> msgHandler;
	std::atomic<bool>& running;
	std::string msg;
};