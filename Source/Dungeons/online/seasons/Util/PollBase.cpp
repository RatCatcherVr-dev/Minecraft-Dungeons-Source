#include "PollBase.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"

namespace online {
namespace liveops {

PollBase::PollBase(UGameInstance* gameInstance)
	: PollBase(gameInstance, 60.0f) {
}

PollBase::PollBase(UGameInstance* gameInstance, float rate)
	: gameInstance(gameInstance)
	, rate(rate) {
}

PollBase::~PollBase() {
	gameInstance->GetTimerManager().ClearTimer(handle);
}

void PollBase::Start() {
	gameInstance->GetTimerManager().SetTimer(handle, [&]() {
		DoRequest();
	}, rate, true);
}

}
}
