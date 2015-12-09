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
	double m_sumDeltaTimes;

	double m_MPF;
	int m_numFrames;

	unsigned int m_maxRecordSize;

	TFFrameRateTimer(unsigned int maxRecordSize) : m_sumDeltaTimes(0.0), m_MPF(0.0), m_numFrames(0), m_maxRecordSize(maxRecordSize) { }
public:
	static TFFrameRateTimer* create(unsigned int maxRecordSize = 30){
		return static_cast<TFFrameRateTimer *>((new TFFrameRateTimer(maxRecordSize))->autorelease());
	}

	virtual ~TFFrameRateTimer() { }

	double elapse() {
		double currentTime = glfwGetTime();

		static double MPF_lastTime = currentTime;
		++m_numFrames;
		if (currentTime - MPF_lastTime >= 1.0){
			m_MPF = 1000.0 / static_cast<double>(m_numFrames);
			m_numFrames = 0;
			MPF_lastTime += 1.0;
		}

		static double dt_lastTime = currentTime;
		double deltaTime = currentTime - dt_lastTime;
		m_sumDeltaTimes += deltaTime;
		m_deltaTimes.push_back(deltaTime);
		if (m_deltaTimes.size() > m_maxRecordSize){
			m_sumDeltaTimes -= m_deltaTimes.front();
			m_deltaTimes.pop_front();
		}
		dt_lastTime = currentTime;

		return deltaTime;
	}

	//@ Averaged FPS.
	double getFPS() const{
		if (m_deltaTimes.empty()){
			return 0.0;
		}

		double average = m_sumDeltaTimes / static_cast<double>(m_deltaTimes.size());
		return 1.0 / average;
	}

	//@ Milliseconds per Frame.
	//@ Good MPF : 16.6666ms for 60fps, 33.3333ms for 30fps.
	double getMPF() const{
		return m_MPF;
	}

	double getDeltaTime() const{
		if (m_deltaTimes.empty()){
			return 0.0;
		}

		return m_deltaTimes.back();
	}
};