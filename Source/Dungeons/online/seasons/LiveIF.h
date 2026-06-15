#pragma once

#include <functional>
#include <memory>
#include <vector>

class LiveOpsClient;
class UGameInstance;

namespace online
{
namespace liveops
{

class MultiRequest;


class LiveIF {
public:
	LiveIF(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* InGameInstance)
		: mApiClient(apiClient)
		, GameInstance(InGameInstance) {}

	virtual ~LiveIF() {}

	virtual void Teardown() = 0;
	virtual void Init() = 0;
	virtual void Request() = 0;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) = 0;

protected:
	std::shared_ptr<LiveOpsClient> mApiClient;
	UGameInstance* GameInstance;
};

class MultiRequest {
public:
	using CompleteCallback = std::function<void(const MultiRequest*)>;
	using Step = std::vector<LiveIF*>;
	using Sequence = std::vector<Step>;

	MultiRequest(Sequence sequence, CompleteCallback onCompletion)
		: sequence(sequence)
		, step(this->sequence.begin())
		, requestsLeft(step->size())
		, onCompletion(std::move(onCompletion))
		, success(true) {
	}

	const Step& GetInitialStep() const {
		return *step;
	}
	
	template <typename Response>
	void CompleteSingleRequest(Response&& response) {
		success = success && response.successful();
		requestsLeft -= 1;

		if (requestsLeft == 0) {
			DoNextStep();
		}
	}

	bool WasSuccessful() const { return success; }

	static void Initiate(const Sequence& steps, const CompleteCallback& onCompletion) {
		auto multiRequest = std::make_shared<MultiRequest>(steps, onCompletion);

		for (LiveIF* handler : multiRequest->GetInitialStep()) {
			handler->MultiRequestPartial(multiRequest);
		}
	}

private:
	void DoNextStep() {
		++step;
		if (!success || step == sequence.end()) {
			onCompletion(this);
		} else {
			Initiate({ step, sequence.end() }, onCompletion);
		}
	}

	Sequence sequence;
	Sequence::iterator step;
	size_t requestsLeft;
	CompleteCallback onCompletion;
	bool success;
};

}
}
