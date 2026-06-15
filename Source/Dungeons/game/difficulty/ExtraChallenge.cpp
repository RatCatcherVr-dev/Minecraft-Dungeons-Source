#include "Dungeons.h"
#include "ExtraChallenge.h"
#include "util/FloatRange.h"

namespace extrachallengequery {
	//First extra challenge should return 0.0f, last should return 1.0f;
	float toExtraFraction(EExtraChallenge extraChallenge) {
		if (extraChallenge == EExtraChallenge::Invalid) {
			return 0.0f;
		}

		const int extraChallengeIndex = enum_cast(extraChallenge);
		const float extraChallengeFraction = ((float)extraChallengeIndex - (float)EExtraChallengeFirstIndex) / (float)(NumberOfExtraChallenges);
		return FMath::Clamp(extraChallengeFraction, 0.0f, 1.0f);
	}

	//1 should correspond to challenge_1
	//2 challenge_2, and so on.
	EExtraChallenge getExtraChallengeFromAmount(int amount) {
		check((amount >= 0 && amount <= NumberOfExtraChallenges) && "trying to get an extra challenge out of range");
		if(amount >= 0 && amount <= NumberOfExtraChallenges){
			return static_cast<EExtraChallenge>(EExtraChallengeFirstIndex + amount);
		}
		return EExtraChallenge::NoExtraChallenge;
	}

	TOptional<EExtraChallenge> getNextExtraChallenge(EExtraChallenge extraChallenge) {
		if (extraChallenge < extrachallengequery::Last) {
			return static_cast<EExtraChallenge>(enum_cast(extraChallenge) + 1);
		}
		return {};
	}

#define LOCTEXT_NAMESPACE "Difficulty"	
	FText getExtraChallengeDisplayName(EExtraChallenge challenge)
	{
		switch (challenge) {
		case EExtraChallenge::Challenge_1:
			return LOCTEXT("ExtraChallenge_1", "Hard Challenge");
		case EExtraChallenge::Challenge_2:
			return LOCTEXT("ExtraChallenge_2", "Harder Challenge");
		case EExtraChallenge::Challenge_3:
			return LOCTEXT("ExtraChallenge_3", "Epic Challenge");
		default:
			return FText::GetEmpty();
		}
	}	

#undef LOCTEXT_NAMESPACE
}
