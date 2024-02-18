#pragma once
#ifndef __JOBTIMER__
#define __JOBTIMER__

struct JobData {
	JobData(std::weak_ptr<JobQueue> owner, JobRef job)
		: owner(owner),
		job(job)
	{}

	// 타이머에 있을떄 shared로 하면 해제가 안되서 weak로 함
	std::weak_ptr<JobQueue> owner;
	JobRef job;
};

struct TimerItem {

	TimerItem(uint64 executeTick, JobData* jobData)
		: executeTick(executeTick),
		jobData(jobData)
	{}

	bool operator<(const TimerItem& offer) const {
		return executeTick > offer.executeTick;
	}

	uint64 executeTick = 0;
	// item이 priorityQueue에 있다고 해도 위치가 바뀔수도 있는데
	// 그떄마다 복사하면 스마트 포인터 관리 어려울 수 있음
	JobData* jobData = nullptr;
};

class JobTimer {
public:
	void Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void Distribute(uint64 now);	// 시간이 된 item 배분
	void Clear();
private:
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;

};

#endif

