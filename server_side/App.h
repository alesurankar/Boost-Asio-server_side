#pragma once
#include "MessageHandler.h"
#include "FrameTimer.h"
#include <string>
#include <mutex>
#include <thread>
#include <queue>


class App
{
public:
	App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in);
	~App();
	void UpdateLoop();
	void Go();
private:
	void TakeFromQueue();
	void PushToQueue();
	void GetMessage();
	void UpdateParameters(std::string command);
	void UpdateGame();
	void SetMessage();
private:
	FrameTimer ft;
	int x;
	int y;
	int xEnemy = 200;
	int yEnemy = 200;
	bool moveUP = true;
	bool moveLeft = true;
	std::string message;
	std::thread UpdateThread;
	std::shared_ptr<MessageHandler> msgHandler;
	std::atomic<bool>& running;
	std::atomic<bool> nextFrame;
	std::mutex(mtx_in);
	std::mutex(mtx_out);
	std::queue<std::string> msg_toUpdate;
	std::queue<std::string> msg_isUpdated;
};