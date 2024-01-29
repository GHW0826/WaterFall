#pragma once
#ifndef __MEMORY__
#define __MEMORY__
#include "Allocator.h"

class MemoryPool;

class Memory {
	enum {
		// ~1024바이트까지는 32바이트 단위 풀,
		// ~2048바이트까지는 128바이트 단위 풀
		// ~4096바이트까지는 256바이트 단위 풀
		// 그 이상은 기본 할당기 사용
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};
public:
	Memory();
	~Memory();

	void*	Allocate(int32 size);
	void	Release(void* ptr);
private:
	vector<MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 빠르게 찾기 위한 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename... Args>
Type* xnew(Args&&... args) {
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new
	new(memory) Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj) {
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args) {
	return shared_ptr<Type> { xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
}

#endif