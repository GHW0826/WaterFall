#pragma once
#ifndef _REFCOUNTABLE_
#define _REFCOUNTABLE_

class RefCountable
{
public:
	RefCountable()
		: _refCount(1)
	{}
	virtual ~RefCountable() 
	{}

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef() {
		int32 refCount = --_refCount;
		if (refCount == 0)
			delete this;
		return refCount;
	}
protected:
	atomic<int32> _refCount;
};


// 1) 이미 만들어진  class 대상으로 사용 불가
// 2) 순환 (Cycle) 발생
//	std::shared_ptr은 [T*][CountBlocking*]으로 카운팅 클래스 보유
//	make_shared로 만들면 CountBlock 관련 해서 자동 상속된 버전으로 만들어짐
//	shared_ptr은 Uses, Weaks 2가지 카운트 관리. uses는 shaerd_ptr로 관리되는 참조 카운팅, Weaks는 weak_ptr로 관리되는 참조 카운팅
//  uses가 0이 되면 객체는 유효하지 않음. RefCountingBlock은 weak 카운팅이 살아있으면 아직 유효함.
//  그렇기 때문에 다른 weak_ptr 객체가 목표 포인터가 유효하지 않아도 질의할 수 있음.
//	weak 포인터는 shared_ptr마냥 포인터로 사용할 수 없고, wpr.expired() API로 가리키는 객체가 존재하는지 체크하고 사용.
//  아니면 wpr.lock() API를 사용해 shared_ptr로 변환시키고 사용해야 함.  lock 호출시 객체 자체가 존재하지 않을 수 있음.
//	CountBlocking를 참조해 포인터 관리에 적용되지 않음. weak 카운팅도 0이되면 CountBlocking 메모리도 날림.
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr()
	{}
	TSharedPtr(T* ptr) { 
		Set(ptr);
	}

	// 복사
	TSharedPtr(const TSharedPtr& rhs) {
		Set(rhs._ptr);
	}
	// 이동
	TSharedPtr(TSharedPtr&& rhs) {
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
	}

	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) {
		Set(static_cast<T*>(rhs._ptr));
	}
	~TSharedPtr() {
		Release();
	}
public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs) {
		if (_ptr != rhs._ptr) {
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}
	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs) {
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}
	bool operator==(const TSharedPtr& rhs) const	{ return _ptr == rhs._ptr; }
	bool operator=(T* ptr) const					{ return _ptr == ptr; }
	bool operator!=(const TSharedPtr& rhs) const	{ return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const					{ return _ptr != ptr; }
	bool operator<(const TSharedPtr& rhs) const		{ return _ptr < rhs._ptr; }

	T*			operator*()			{ return _ptr; }
	const T*	operator*() const	{ return _ptr; }
	operator T* () const			{ return _ptr; }
	T*			operator->()		{ return _ptr; }
	const T*	operator->() const	{ return _ptr; }

	bool IsNull() { return _ptr == nullptr; }
public:
	inline void Set(T* ptr) {
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}
	inline void Release() {
		if (_ptr != nullptr) {
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}
private:
	T* _ptr = nullptr;
};


#endif