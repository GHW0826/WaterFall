#include "pch.h"
#include "LockFreeStack.h"

#ifdef first

// [  data  ][    ][ ]
// [ Header ][next]
void InitializeHead(SListHeader* header) {
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry) {
	entry->next = header->next;
	header->next = entry;

}

SListEntry* PopEntrySList(SListHeader* header) {

	SListEntry* first = header->next;
	if (first != nullptr) {
		header->next = first->next;
	}
	return first;
}

#endif

#ifdef second
// [  data  ][    ][ ]
// [ Header ][next]
void InitializeHead(SListHeader* header) {
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry) {

	entry->next = header->next;
	while (::InterlockedCompareExchange64(
		(int64*)&header->next, 
		(int64)entry, 
		(int64)entry->next) == 0) {
	}
	header->next = entry;

}

// [][]
// Header [ next ]
SListEntry* PopEntrySList(SListHeader* header) {

	SListEntry* expected = header->next;

	// ABA Problem
	// A 상태 -> B 상태 -> A 상태
	// 
	// 만약에 header가 5000이라면, header에다 6000을 넣어줘
	// [5000]->[6000]->[7000]
	// [Header]
	while (expected && ::InterlockedCompareExchange64(
		(int64*)&header->next,
		(int64)expected->next,
		(int64)expected) == 0
		) {
		// 
	}

	return expected;
}

#endif


void InitializeHead(SListHeader* header) {
	header->alignment = 0;
	header->region = 0;
}

// 16바이트 정렬시 최하위 4비트는 0000
void PushEntrySList(SListHeader* header, SListEntry* entry) {

	SListHeader expected = {};
	SListHeader desired = {};

	// 16바이트 정렬
	desired.HeaderX64.next = (((uint64)entry) >> 4);

	while (true) {
		expected = *header;

		// 이 사이에 변경될 수 있다.
		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128(
			(int64*)header,
			desired.region,
			desired.alignment,
			(int64*)&expected) == 1
			) {
			break;
		}
	}
}

SListEntry* PopEntrySList(SListHeader* header) {

	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true) {
		expected = *header;

		// 16바이트 정렬
		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) >> 4);
		if (entry == nullptr)
			break;

		// ABA문제를 해결하기 위해 카운터를 둔게 메인임
		// Use-After-Free문제 있음
		// pop 스레드가 하나라면 안전
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128(
			(int64*)header,
			desired.region,
			desired.alignment,
			(int64*)&expected) == 1
			) {
			break;
		}
	}

	return entry;
}