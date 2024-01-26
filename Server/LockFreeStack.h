#pragma once


#ifdef first
template<typename T>
struct Node {
	T data;
	Node* node;
};

struct SListEntry {
	SListEntry* next;
};

class Data { // : public SListEntry {
public:
	SListEntry _entry;

	int32 _hp;
	int32 _mp;
};

struct SListHeader {
	SListEntry* next = nullptr;
};

// [  data  ][    ][ ]
// [ Header ][next]
void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);

/* ex

int main() {

	SListHeader header;
	InitializeHead(&header);

	Data* data = new Data();
	data->_hp = 10;
	data->_mp = 20;
	PushEntrySList(&header, (SListEntry*)data);

	Data* popData = (Data*)PopEntrySList(&header);

	return 0;
}

*/

#endif


#ifdef second
struct SListEntry {
	SListEntry* next;
};

struct SListHeader {
	SListEntry* next = nullptr;
};


void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
#endif

/*
DECLSPEC_ALIGN(16)
class Data  {
public:
	SListEntry _entry;
	int64 _rand = rand() % 1000;
};

SListHeader* GHeader;

int main() {
	GHeader = new SListHeader();
	InitialzeHead(GHeader);

	for (int32 i = 0; i < 3; ++i) {
		GThreadManager->Launch([]() {
			while (true) {
				Data* data = new Data();
				ASSERT_CRASH(((uint64)data % 16) == 0);

				PushEntrySList(GHeader, (SListEntry*)data);
				this_thread::sleep_for(10ms);
			}
		});
	}

	for (int32 i = 0; i < 2; ++i) {
		GThreadManager->Launch([]() {
			while (true) {
				Data* pop = nullptr;
				pop = (Data*)PopEntrySList(GHeader);
				if (pop) {
					cout << pop->_rand << endl;
					delete pop;
				}
				else {
					cout << "NONE" << endl;
				}
			}
		});
	}
	GThreadManager->Join();
}
*/

// 16비트 정렬
DECLSPEC_ALIGN(16)
struct SListEntry {
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader {

	SListHeader() {
		alignment = 0;
		region = 0;
	}

	union {
		struct {
			uint64 alignment;
			uint64 region;
		} DUMMYSTRUCTNAME;
		struct {
			uint64 depth	: 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next		: 60;
		} HeaderX64;
	};
	SListEntry* next = nullptr;
};


void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);