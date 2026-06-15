#pragma once
#include <map>
#include <queue>
#include <string>
#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"

#include "LiveIF.h"
#include "LiveOpsClient.h"
#include "core/HttpServiceResponse.h"
#include "Engine/EngineTypes.h"
#include "modules/LiveOps/ProgressRequest.h"
#include "modules/LiveOps/ProgressResponse.h"

namespace online
{
namespace liveops
{

struct Progress {
	std::string name;
	int64 value;
};

minecraft::api::ProgressRequest MakeProgressRequest(const std::string& name, const int64& score);

class ProgressHandler : public LiveIF {
public:
	typedef std::map<std::string, Progress> List;
	typedef std::deque<minecraft::api::ProgressRequest> Queue;

	ProgressHandler(std::shared_ptr<LiveOpsClient>, UGameInstance*);

	virtual void Init() override;
	virtual void Teardown() override;
	virtual void Request() override;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;

	void AddToQueue(const minecraft::api::ProgressRequest&);
	TOptional<int64> GetStatValue(const std::string& name);
	const List& GetAllStats() const;

#if !UE_BUILD_SHIPPING
	void ResetAllProgression(const LiveOpsClient::Callback<minecraft::api::ResetAllProgressResponse>& callback);
#endif

private:
	LiveOpsClient::Callback<minecraft::api::PostProgressResponse> CreatePostProgressCallback(const minecraft::api::ProgressPost& stat);
	void EngageQueue(bool fireAndForget = false);

	void UpdateProgressions(const std::vector<minecraft::api::Progress>&);

	List CurrentProgressions;
	Queue UnhandledProgressions;
	FTimerHandle QueueTickTimer;
};
}
}
