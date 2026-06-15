#pragma once

#include "AItemInstance.h"
#include "save/CharacterSaveData.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "game/component/EyeOfEnderDropComponent.h"
#include "EyeOfEnderInstance.generated.h"

UCLASS()
class DUNGEONS_API AEyeOfEnderInstance : public AItemInstance {
	GENERATED_BODY()
public:
	void Activate(const FPredictionKey& predictionKey) override;

	UFUNCTION(NetMulticast, Reliable)
	void PickUpEye();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void OnRep_EyeType();

private:
	UPROPERTY(ReplicatedUsing = OnRep_EyeType)
	EEyeOfEnderType eyeOfEnderType = EEyeOfEnderType::NOT_SET;
};
