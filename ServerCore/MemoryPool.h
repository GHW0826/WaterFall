#pragma once
#ifndef __MEMORYPOOL__
#define __MEMORYPOOL__


// �� �޸� Ǯ ���
// [32 ��Ʈ ����] [64 ��Ʈ ����] [] [] 
// or
// ���� Ǯ

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
		// ++ ����� MemoryHeader��ŭ �̵�
		return reinterpret_cast<void*>(++header);
	}
	static MemoryHeader* DetachHeader(void* ptr) {
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

// pool �����̱� ������ stomp allocator�� �˸��� ����
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
	atomic<int32> _useCount = 0;	// �Ҵ�� �޸� ����
	atomic<int32> _reserveCount = 0;
	
	/*
	USE_LOCK;
	queue<MemoryHeader*> _queue;
	*/
};

#endif
