#pragma once
#ifndef __ALLOCATOR__
#define __ALLOCATOR__

// BaseAllocator
class BaseAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};


// StompAllocator
// 할당을 뒤에서부터 함해서 오버 플로우를 잡음
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Alloc(int32 size);
	static void	 Release(void* ptr);
};

#endif