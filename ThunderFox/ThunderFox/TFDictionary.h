#pragma once

#include <set>

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
	std::set<TFWord<T1, T2> > m_data;
public:
	typedef typename std::set<TFWord<T1, T2> >::iterator iterator;

	bool add(const T1 &key, const T2 &value){
		return m_data.insert(TFWord<T1, T2>(key, value)).second;
	}
	bool add(const TFWord<T1, T2> &word){
		return m_data.insert(word).second;
	}

	bool getValue(const T1 &key, T2 *value) const{
		iterator found = m_data.find(TFWord<T1, T2>(key, 0));
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
		iterator found = m_data.find(TFWord<T1, T2>(key, 0));
		if (found != m_data.end()){
			m_data.erase(found);
			return true;
		}
		else{
			return false;
		}
	}

	iterator begin(){ return m_data.begin(); }
	iterator end(){ return m_data.end(); }
	iterator find(const T1 &key) { return m_data.find(TFWord<T1, T2>(key, 0)); }
	void remove(iterator position){ m_data.erase(position); }
};