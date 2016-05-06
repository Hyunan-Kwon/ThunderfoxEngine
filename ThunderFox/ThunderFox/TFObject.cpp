#include "TFObject.h"
#include "TFAutoreleasePool.h"
#include "TFHandle.h"

//@ Tracker fo not released objects.
std::set<TFRef *> TFRef::m_tracker;

TFRef::TFRef()
: m_count(1u) {
#if TF_DEBUG_MODE
	TFRef::m_tracker.insert(this);
#endif
}

TFRef::~TFRef(){
#if TF_DEBUG_MODE
	TFRef::m_tracker.erase(this);
#endif
}

void TFRef::release() {
	TFASSERT(m_count > 0, "Reference counting error.");

	if (--m_count == 0){
#if TF_DEBUG_MODE
		TFAutoreleasePool *pool = TFAutoreleasePool::getInstance();
		if (!pool->isCleaning() && pool->isSwimming(this)){
			TFASSERT(false, "Trying to delete the reference swimming in the pool.");
		}
#endif
		delete this;
	};
}

TFRef* TFRef::autorelease() {
	TFAutoreleasePool::getInstance()->dive(this);
	return this;
}

void TFRef::detectLeaking() {
	for (std::set<TFRef *>::iterator it = TFRef::m_tracker.begin(); it != TFRef::m_tracker.end(); ++it){
		TFLOG("(%s) is still active with reference count %d.", typeid(*(*it)).name(), (*it)->getCount());
	}
}

void TFRef::release_all() {
	if (!TFRef::m_tracker.empty()){
		TFLOG("Releasing %d references in the pool.", m_tracker.size());
		std::set<TFRef *>::iterator it = TFRef::m_tracker.begin();
		while (it != m_tracker.end()){
			TFRef *ref = *it;
			++it;
			delete ref;
		}
	}
}