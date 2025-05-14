#pragma once

#include <chrono>

class FrameTimer
{
public:
	FrameTimer();
	float Mark();
public:
	std::chrono::steady_clock::time_point last;
};