#pragma once
#ifndef __GLOBALQUEUE__
#define __GLOBALQUEUE__

class GlobalQueue {
public:
	GlobalQueue();
	~GlobalQueue();

	void Push(JobQueueRef jobQueue);
	JobQueueRef Pop();
private:
	LockQueue<JobQueueRef> _jobQueue;
};

#endif
