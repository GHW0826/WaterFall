#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*
  로그를 찍거나 카운팅해 할당한 객체 갯수 확인
  메모리릭이 일어나는지 확인하기 위해 할당, 해제 짝을 맞춰 테스트도 가능
*/
void* BaseAllocator::Alloc(int32 size) {
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr) {
	::free(ptr);
}

/// 
void* StompAllocator::Alloc(int32 size) {

	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(nullptr, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr) {
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}


/// 
void* PoolAllocator::Alloc(int32 size) {
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr) {
	GMemory->Release(ptr);
}
