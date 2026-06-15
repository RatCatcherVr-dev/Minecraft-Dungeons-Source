#include "Dungeons.h"
#include "MerchantDef.h"

const TSubclassOf<AMerchantBase>& UMerchantDef::GetMerchantClass() const {
	return MerchantClass;
}

const FString& UMerchantDef::GetUnlockProgressKey() const {
	return UnlockProgressKey;
}

TOptional<FString> UMerchantDef::GetMerchantLevelProgressKey(const APlayerControllerBase* controller) const {
	if (GenerationLevelProgressKeys.Num() > 0) {
		auto level = GetMerchantLevelIndexFor(controller);
		return GenerationLevelProgressKeys[FMath::Clamp(level, 0, GenerationLevelProgressKeys.Num()-1)];
	}
	return {};
}

FName UMerchantDef::CreateSaveName() const {
	return GetFName();
}

TOptional<ELevelNames> UMerchantDef::GetLegacyUnlockMission() const {
	return LegacyUnlockMission;
}

const FText& UMerchantDef::GetDisplayName() const {
	return DisplayName;
}

int UMerchantDef::GetMerchantLevelIndexFor(const APlayerControllerBase* controller) const {
	if (auto defaultMerchant = MerchantClass->GetDefaultObject<AMerchantBase>()) {
		defaultMerchant->AssignPlayerController(controller, CreateSaveName(), GetDisplayName());
		auto completedQuests = defaultMerchant->GetCompletedSlotQuests();
		defaultMerchant->ClearPlayerController();
		return completedQuests;
	}
	return 0;
}
