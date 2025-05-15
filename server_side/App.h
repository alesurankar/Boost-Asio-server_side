#pragma once
#include "MessageHandler.h"
#include "FrameTimer.h"
#include <string>
#include <mutex>
#include <thread>
#include <mutex>
#include <queue>


class App
{
public:
	App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in);
	~App();
	//std::pair<int, int> UpdatePos(const std::string& command);
	void UpdateLoop();
	void Go();
private:
	void TakeFromQueue();
	void PushToQueue();
	void GetMessage();
	void UpdateParameters(std::string command);
	void SetMessage();
private:
	FrameTimer ft;
	int x = 100;
	int y = 100;
	std::string message;
	std::thread UpdateThread;
	std::shared_ptr<MessageHandler> msgHandler;
	std::atomic<bool>& running;
	std::atomic<bool> nextFrame;
	std::mutex(mtx_in);
	std::mutex(mtx_out);
	std::queue<std::string> msg_toUpdate;
	std::queue<std::string> msg_isUpdated;
	std::atomic<bool> noUpdate;
};