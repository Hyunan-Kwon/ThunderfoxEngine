#ifndef _TFOBJECT_H_
#define _TFOBJECT_H_

#include "gl\glew.h"
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

//class TFObject{
//private:
//	unsigned int m_refCount;
//	TFObject *m_parent;
//	std::list<TFObject *> m_childs;
//
//	TFObject(TFObject const&);
//	void operator = (TFObject const&);
//protected:
//	TFObject() : m_refCount(1u), m_parent(nullptr) { }
//public:
//	virtual ~TFObject();
//
//	void retain() { ++m_refCount; }
//	void release();
//	void autorelease();
//
//	void addChild(TFObject *child);
//	void removeChild(TFObject *child);
//	void removeFromParent();
//	TFObject* getParent() const { return m_parent; }
//	const std::list<TFObject *>& getChilds() const{ return m_childs; }
//
//	// FIXME. delete this.
//	static TFObject* create() { return new TFObject(); }
//};

class TFGLObject{
protected:
	GLuint m_id;
public:
	TFGLObject() : m_id(0u) { }
	inline GLuint getID() const { return m_id; }
};

#endif
