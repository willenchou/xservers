#ifndef XBASE_REF_COUNTED_H_
#define XBASE_REF_COUNTED_H_

#include "mutex.h"
#include "xbase_export.h"

namespace x {

struct IRefCounted {
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

template<class T, class R = IRefCounted>
class XBASE_EXPORT RefCounted {
public:
	RefCounted(){ ref_ = 1; }
	virtual ~RefCounted(){ }
	virtual void AddRef() { ref_++; };
	virtual void Release() { if (--ref_ == 0) delete static_cast<T*>(this);	}
private:
	mutable int ref_;
};

template<class T, class R = IRefCounted>
class XBASE_EXPORT RefCountedThreadSafe : public R {
public:
	RefCountedThreadSafe() { ref_ = 1; }
	~RefCountedThreadSafe() { }
	virtual void AddRef() { ScopedLock lock(mutex_); ref_++; };
	virtual void Release() { ScopedLock lock(mutex_); if (--ref_ == 0) delete static_cast<T*>(this);	}
private:
	int ref_;
	Mutex mutex_;
};


}//namespace x

#endif //XBASE_REF_COUNTED_H_
