#pragma once
#include <functional>

#include "Engine/EngineTypes.h"
#include "SeasonsCommon.h"

class UGameInstance;

namespace online {
namespace liveops {

class PollBase {
public:
	PollBase(UGameInstance*);
	PollBase(UGameInstance*, float frequency);
	virtual ~PollBase();
	void Start();

protected:
	virtual void DoRequest() = 0;

	UGameInstance* gameInstance;
	float rate;
	FTimerHandle handle;
};

class Heartbeat : public PollBase {
public:
	using Response = std::function<void(UpdateRequestStatus)>;
	using MultiRequest = std::function<void(Response)>;

	Heartbeat(UGameInstance* gameInstance, MultiRequest multiRequest, float rate)
	: PollBase(gameInstance, rate)
	, multiRequest(multiRequest) {
	}

protected:
	void DoRequest() override {
		multiRequest([&] (UpdateRequestStatus) {
			UE_LOG(LogLiveOps, Warning, TEXT("Updated connection status"));
		});
	}
	bool awaitingResponse;
	MultiRequest multiRequest;
};

}
}

