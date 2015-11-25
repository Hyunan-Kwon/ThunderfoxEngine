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

	double stop(){
		if (m_start == -1){
			return 0.0;
		}
		else{
			return glfwGetTime() - m_start;
		}
	}
};

class TFFrameRateTimer : public TFRef{
protected:
	std::deque<double> m_deltaTimes;
	double m_sum_deltaTimes;
	unsigned int m_maxRecordSize;

	TFFrameRateTimer(unsigned int maxRecordSize) : m_sum_deltaTimes(0.0), m_maxRecordSize(maxRecordSize) { }
public:
	static TFFrameRateTimer* create(unsigned int maxRecordSize = 30){
		return static_cast<TFFrameRateTimer *>((new TFFrameRateTimer(maxRecordSize))->autorelease());
	}

	virtual ~TFFrameRateTimer() { }

	double elapse() {
		static double lastTime = glfwGetTime();
		double deltaTime = glfwGetTime() - lastTime;
		m_sum_deltaTimes += deltaTime;
		m_deltaTimes.push_back(deltaTime);
		if (m_deltaTimes.size() > m_maxRecordSize){
			m_sum_deltaTimes -= m_deltaTimes.front();
			m_deltaTimes.pop_front();
		}
		lastTime = glfwGetTime();
		return deltaTime;
	}

	double getFPS() {
		if (m_deltaTimes.empty()){
			TFLOG("Zero division in FPS");
			return 0.0;
		}

		double average = m_sum_deltaTimes / static_cast<double>(m_deltaTimes.size());
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