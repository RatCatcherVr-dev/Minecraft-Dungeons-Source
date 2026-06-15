#pragma once

#include "CoreMinimal.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include "online/seasons/EventView.h"
#include "NewsClaimContainerWidget.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FNewsRewardInfo {
	GENERATED_BODY()
public:
	FNewsRewardInfo() {}
	FNewsRewardInfo(FName id, UTexture2D* icon, bool unlocked, bool claimed, FText desc, FText progress);

	UPROPERTY(BlueprintReadOnly)
	FName RewardId;
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Icon;
	UPROPERTY(BlueprintReadOnly)
	bool bUnlocked;
	UPROPERTY(BlueprintReadOnly)
	bool bClaimed;
	UPROPERTY(BlueprintReadOnly)
	FText ChallengeDescription;
	UPROPERTY(BlueprintReadOnly)
	FText ChallengeProgress;
};

UCLASS(Abstract)
class DUNGEONS_API UNewsClaimContainerWidget : public UDungeonsUserWidget {
	GENERATED_BODY()
public:
	void NativeConstruct() override;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRewardInfo(const TArray<FNewsRewardInfo>& Info);
	UFUNCTION(BlueprintCallable)
	void ClaimReward(FName rewardId);
private:
	void UpdateProgress(const TOptional<online::liveops::EventView>& eventView);

	FText GetFormattedProgress(int progress, int requirement);

	TOptional<online::liveops::EventRewardView> GetRewardFromAP(const online::liveops::EventView&, int AP) const;

	UTexture2D* GetIcon(minecraft::api::RewardTypes, FName) const;

	UPROPERTY()
	UGameInstance* GameInstance;

	FDelegateHandle EventViewHandle;
};
