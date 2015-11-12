#ifndef _TFAUTORELEASEPOOL_H_
#define _TFAUTORELEASEPOOL_H_

#include "TFObject.h"
#include <list>
#include <algorithm>

class TFAutoreleasePool {
private:
	std::list<TFRef *> m_pool;
	bool m_isCleaning;

	TFAutoreleasePool() : m_isCleaning(false) { }
	TFAutoreleasePool(TFAutoreleasePool const&);
	void operator = (TFAutoreleasePool const&);
public:
	static TFAutoreleasePool* getInstance() {
		static TFAutoreleasePool m_instance;
		return &m_instance;
	}

	void dive(TFRef *ref) {
		m_pool.push_back(ref);
	}
	void comeout(TFRef *ref) {
		m_pool.remove(ref);
	}
	void clean() {
		m_isCleaning = true;
		for (auto &ref : m_pool){
			ref->release();
		}
		m_pool.clear();
		m_isCleaning = false;
	}

	bool isSwimming(TFRef *ref){
		std::list<TFRef *>::const_iterator found = std::find(m_pool.begin(), m_pool.end(), ref);
		return found != m_pool.end();
	}

	bool isCleaning() const{
		return m_isCleaning;
	}
};

#endif