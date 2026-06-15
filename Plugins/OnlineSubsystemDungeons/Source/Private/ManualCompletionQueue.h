#pragma once

#include "xsapiServicesInclude.h"

class ManualCompletionQueue {
public:
	ManualCompletionQueue();
	~ManualCompletionQueue();
	
	ManualCompletionQueue(const ManualCompletionQueue&) = delete;
	ManualCompletionQueue& operator=(const ManualCompletionQueue&) = delete;
	
	XTaskQueueHandle GetQueue() const;
	void Completion() const;
	
private:
	XTaskQueueHandle QueueHandle = nullptr;
};
