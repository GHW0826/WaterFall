#pragma once
#ifndef __MEMORYPOOL__
#define __MEMORYPOOL__


// 각 메모리 풀 담당
// [32 비트 전용] [64 비트 전용] [] [] 
// or
// 통합 풀

enum {
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size)
	{}

	static void* AttachHeader(MemoryHeader* header, int32 size) {
		new(header)MemoryHeader(size); // placement new
		// ++ 연산시 MemoryHeader만큼 이동
		return reinterpret_cast<void*>(++header);
	}
	static MemoryHeader* DetachHeader(void* ptr) {
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

// pool 형태이기 때문에 stomp allocator는 알맞지 않음
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();
private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;			//
	atomic<int32> _useCount = 0;	// 할당된 메모리 갯수
	atomic<int32> _reserveCount = 0;
	
	/*
	USE_LOCK;
	queue<MemoryHeader*> _queue;
	*/
};

#endif
