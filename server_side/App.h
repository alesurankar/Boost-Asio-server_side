#pragma once
#include "MessageHandler.h"
#include <string>


class App
{
public:
	App();
	void Run();
	void UpdatePos(const std::string& command);
private:
	MessageHandler msg;
	int x = 200;
	int y = 200;
	int width = 20;
	int height = 20;
};