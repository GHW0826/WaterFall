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
// �Ҵ��� �ڿ������� ���ؼ� ���� �÷ο츦 ����
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Alloc(int32 size);
	static void	 Release(void* ptr);
};

#endif