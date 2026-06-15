#pragma once

#include "IDungeonsAPIClient.h"
#include "LiveIF.h"
#include "Challenges.h"

#include <memory>

namespace online
{
namespace liveops
{

class ChallengesHandler : public LiveIF {
public:
	ChallengesHandler(std::shared_ptr<LiveOpsClient>, UGameInstance*);

	virtual void Teardown() override;
	virtual void Init() override;
	virtual void Request() override;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;

	void UseMockedChallenges();

	const Challenges& GetChallenges() const;
	Challenge* GetChallenge(const std::string& name) const;

private:
	void InternalUpdateChallenges(const std::vector<minecraft::api::ChallengeResponse>&);
	bool ShouldUseMockedChallenges() const;

	Challenges challenges;
	bool useMockedChallenges;
};

}
}
