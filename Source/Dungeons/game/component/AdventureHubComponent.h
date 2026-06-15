// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/AdventureHubUtil.h"
#include "online/seasons/LiveOps.h"
#include "AdventureHubComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateAdventureHubData, FAdventureHubInfo, Info);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAdventureHubComponent : public UActorComponent {
	GENERATED_BODY()
public:
	UAdventureHubComponent();
protected:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type reason) override;

	void OnDataUpdate(online::liveops::UpdateRequestStatus status);

	UPROPERTY(BlueprintAssignable)
		FOnUpdateAdventureHubData OnUpdateAdventureHubData;

	FDelegateHandle DataUpdateHandle;
};
