#include "Dungeons.h"
#include "MissionInterest.h"

#define LOCTEXT_NAMESPACE "MissionInterest"

FText UMissionInterestUtil::GetMissionInterestText(EMissionInterest interestType, int count) {
	if(count==1){
		switch (interestType) {
		case EMissionInterest::NEW_MISSION:
			return LOCTEXT("Interest_New", "New");
		case EMissionInterest::NEW_REWARD:
			return LOCTEXT("Interest_NewReward", "New Reward");
		case EMissionInterest::NEW_MERCHANT:
			return LOCTEXT("Interest_CapturedMerchant", "Captured Merchant");
		case EMissionInterest::NEW_LOCATION:
			return LOCTEXT("Interest_SecretLocation", "Secret Location");
		case EMissionInterest::PLAYABLE:
			return LOCTEXT("Interest_Playable", "Playable");
		case EMissionInterest::NEW_BOSS:
			return LOCTEXT("Interest_Boss", "Boss");
		case EMissionInterest::NEW_ENDEREYE:
			return LOCTEXT("Interest_EnderEye", "Eye of Ender");
		case EMissionInterest::UNSET:
		default:
			return FText::GetEmpty();
		}
	} else {
		switch (interestType) {
		case EMissionInterest::NEW_MISSION:
			return LOCTEXT("Interest_New", "New");
		case EMissionInterest::NEW_REWARD:
			return LOCTEXT("Interest_NewRewards", "New Rewards");
		case EMissionInterest::NEW_MERCHANT:
			return LOCTEXT("Interest_CapturedMerchants", "Captured Merchants");
		case EMissionInterest::NEW_LOCATION:
			return LOCTEXT("Interest_SecretLocations", "Secret Locations");
		case EMissionInterest::PLAYABLE:
			return LOCTEXT("Interest_Playables", "Playables");
		case EMissionInterest::NEW_BOSS:
			return LOCTEXT("Interest_Bosses", "Bosses");
		case EMissionInterest::NEW_ENDEREYE:
			return LOCTEXT("Interest_EnderEyes", "Eyes of Ender");
		case EMissionInterest::UNSET:
		default:
			return FText::GetEmpty();
		}
	}
}

FText UMissionInterestUtil::GetMissionInterestCarouselText(const FMissionInterestCarousel& interestCarousel) {
	return GetMissionInterestText(interestCarousel.interestType, interestCarousel.missions.Num());
}

bool UMissionInterestUtil::GetMissionInterestIsNew(EMissionInterest interestType) {
	switch (interestType) {
	case EMissionInterest::NEW_MISSION:
	case EMissionInterest::NEW_REWARD:
	case EMissionInterest::NEW_MERCHANT:
	case EMissionInterest::NEW_LOCATION:
	case EMissionInterest::NEW_BOSS:
	case EMissionInterest::NEW_ENDEREYE:
		return true;
	case EMissionInterest::UNSET:
	case EMissionInterest::PLAYABLE:
	default:
		return false;		
	}
}


#undef LOCTEXT_NAMESPACE


