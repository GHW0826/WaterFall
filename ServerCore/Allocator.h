#pragma once
#ifndef __ALLOCATOR__
#define __ALLOCATOR__

// BaseAllocator
class BaseAllocator {
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

// StompAllocator
// 할당을 뒤에서부터 함해서 오버 플로우를 잡음
class StompAllocator {
	enum { 
		PAGE_SIZE = 0x1000  // 4096
	};
public:
	static void* Alloc(int32 size);
	static void	 Release(void* ptr);
};


// PoolAllocator
class PoolAllocator {
public:
	static void* Alloc(int32 size);
	static void	 Release(void* ptr);
};



// STLAllocator
template<typename T>
class StlAllocator {
public:
	using value_type = T;

	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t count) {
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::Alloc(size));//xxalloc(size));
	}

	void deallocate(T* ptr, size_t count) {
		PoolAllocator::Release(ptr); // xxrelease(ptr);
	}


	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};

#endif