#pragma once
#ifndef __JOBTIMER__
#define __JOBTIMER__

struct JobData {
	JobData(std::weak_ptr<JobQueue> owner, JobRef job)
		: owner(owner),
		job(job)
	{}

	// Ÿ�̸ӿ� ������ shared�� �ϸ� ������ �ȵǼ� weak�� ��
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
	// item�� priorityQueue�� �ִٰ� �ص� ��ġ�� �ٲ���� �ִµ�
	// �׋����� �����ϸ� ����Ʈ ������ ���� ����� �� ����
	JobData* jobData = nullptr;
};

class JobTimer {
public:
	void Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void Distribute(uint64 now);	// �ð��� �� item ���
	void Clear();
private:
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;

};

#endif

