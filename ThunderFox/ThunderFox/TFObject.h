#pragma once

#include "gl/glew.h"
#include <set>

class TFRef{
private:
	unsigned int m_count;
	static std::set<TFRef *> m_tracker;

	TFRef(TFRef const&);
	void operator = (TFRef const&);
protected:
	TFRef();

	TFRef* autorelease();
public:
	virtual ~TFRef();

	void retain() { ++m_count; }
	void release();

	unsigned int getCount() const { return m_count; }

	static void detectLeaking();
	static void release_all();
};

class TFGLObject{
protected:
	GLuint m_id;
public:
	TFGLObject() : m_id(0u) { }
	GLuint getID() const { return m_id; }
};