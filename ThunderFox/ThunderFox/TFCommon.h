#ifndef _TFCOMMON_H_
#define _TFCOMMON_H_

#include "gl\glew.h"
#include "TFBuffer.h"

#define PER255F 0.003921568627f	//@ /255.0f

template <typename T1, typename T2>
struct TFWord{
	T1 key;
	T2 value;

	TFWord(T1 key, T2 value) : key(key), value(value) { }

	bool operator < (const TFWord &word) const{
		return key < word.key;
	}
};

template <typename T1, typename T2>
class TFDictionary{
private:
	std::set<TFWord<T1, T2>> m_data;
public:
	bool add(const T1 &key, const T2 &value){
		return m_data.insert(TFWord<T1, T2>(key, value)).second;
	}
	bool add(const TFWord<T1, T2> &word){
		return m_data.insert(word).second;
	}

	bool getValue(const T1 &key, T2 *value) const{
		std::set<TFWord<T1, T2>>::iterator found = m_data.find(TFWord<T1, T2>(key, 0));
		if (found != m_data.end()){
			*value = found->value;
			return true;
		}
		else{
			return false;
		}
	}

	void clear(){
		m_data.clear();
	}

	bool remove(const T1 &key){
		std::set<TFWord<T1, T2>>::iterator found = m_data.find(TFWord<T1, T2>(key, 0));
		if (found != m_data.end()){
			m_data.erase(found);
			return true;
		}
		else{
			return false;
		}
	}

	typedef typename std::set<TFWord<T1, T2>>::iterator iterator;
	iterator begin(){		return m_data.begin();	}
	iterator end(){		return m_data.end();	}
	iterator find(const T1 &key) { return m_data.find(TFWord<T1, T2>(key, 0)); }
	void remove(iterator position){ m_data.erase(position); }
};

namespace TFFramework {
	extern TFArrayBuffer *vertexBuffer_quad, *uvBuffer_quad;

	extern void init();
	extern void cleanup();
}

#endif