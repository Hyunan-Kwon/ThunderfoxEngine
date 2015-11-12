#pragma once

#include "TFObject.h"
#include "gl\glfw3.h"
#include "TFHandle.h"
#include <deque>

class TFTimer : public TFRef{
protected:
	double m_start;

	TFTimer() : m_start(-1) { }

public:
	static TFTimer* create(){
		return static_cast<TFTimer *>((new TFTimer())->autorelease());
	}

	virtual ~TFTimer() { }

	void start(){
		m_start = glfwGetTime();
	}

	virtual double stop(){
		if (m_start == -1){
			return 0.0;
		}
		else{
			return glfwGetTime() - m_start;
		}
	}
};

class TFFrameRateTimer : public TFTimer{
protected:
	std::deque<double> m_deltaTimes;
	unsigned int m_maxRecordSize;

	TFFrameRateTimer(unsigned int maxRecordSize) : m_maxRecordSize(maxRecordSize) { }
public:
	static TFFrameRateTimer* create(unsigned int maxRecordSize = 30){
		return static_cast<TFFrameRateTimer *>((new TFFrameRateTimer(maxRecordSize))->autorelease());
	}

	virtual ~TFFrameRateTimer() { }

	virtual double stop() {
		m_deltaTimes.push_back(TFTimer::stop());
		if (m_deltaTimes.size() > m_maxRecordSize){
			m_deltaTimes.pop_front();
		}
		return m_deltaTimes.back();
	}

	double getFPS() {
		if (m_deltaTimes.empty()){
			TFLOG("Zero division in FPS");
			return 0.0;
		}

		double average = 0.0;
		for (auto &deltaTime : m_deltaTimes){
			average += deltaTime;
		}
		average /= static_cast<double>(m_deltaTimes.size());
		return 1.0 / average;
	}

	double getDeltaTime(){
		if (m_deltaTimes.empty()){
			TFLOG("Zero division in dt");
			return 0.0;
		}

		return m_deltaTimes.back();
	}
};