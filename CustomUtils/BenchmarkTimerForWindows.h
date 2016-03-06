#ifndef TIMERCLASSWIN32
#define TIMERCLASSWIN32
#include <Windows.h>
#include <iostream>
class BenchmarkTimer
{
private:
	double frequency;
	__int64 startTime;
	__int64 endTime;
	bool isTimerStarted;
public:
	BenchmarkTimer()
	{
		LARGE_INTEGER temp;
		QueryPerformanceFrequency(&temp);
		this->frequency = temp.QuadPart;
		this->isTimerStarted = false;
	}
	void startTimer()
	{
		LARGE_INTEGER temp;
		this->isTimerStarted = true;
		QueryPerformanceCounter(&temp);
		this->startTime = temp.QuadPart;
	}
	void endTimer()
	{
		if (!this->isTimerStarted)
		{
			std::cout << "Timer not started yet" << std::endl;
			return;
		}
		LARGE_INTEGER temp;
		this->isTimerStarted = false;
		QueryPerformanceCounter(&temp);
		this->endTime = temp.QuadPart;
	}
	double getDeltaTimeInms()
	{
		return 1000 * ((this->endTime - this->startTime) / this->frequency);
	}
};
#endif