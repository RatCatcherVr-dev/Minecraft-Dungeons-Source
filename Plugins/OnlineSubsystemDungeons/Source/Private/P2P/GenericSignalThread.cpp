#include "PCHOnlineDungeonsSubsystem.h"
#include "GenericSignalThread.h"

void GenericSignalThread::Initialize()
{
	mpSignalThread = rtc::Thread::CreateWithSocketServer();
	mpSignalThread->SetName("rtc_signal_thread", nullptr);
	mpSignalThread->Start();
}

void GenericSignalThread::Teardown()
{
	mpSignalThread->Quit();
	mpSignalThread = nullptr;
}
