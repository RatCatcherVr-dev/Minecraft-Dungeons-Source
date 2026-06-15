#include "PCHOnlineDungeonsSubsystem.h"
#include "ManualCompletionQueue.h"
#include "HrLog.h"

ManualCompletionQueue::ManualCompletionQueue() {
	HrCheck(XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::Manual, &QueueHandle), "[ManualCompletionQueue] Creating manual completion queue handle");
}

ManualCompletionQueue::~ManualCompletionQueue() {
	XTaskQueueCloseHandle(QueueHandle);
}

XTaskQueueHandle ManualCompletionQueue::GetQueue() const {
	return QueueHandle;
}

void ManualCompletionQueue::Completion() const {
	XTaskQueueDispatch(QueueHandle, XTaskQueuePort::Completion, 0);
}
