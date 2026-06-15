#pragma once

#include <Animation/AnimNotifies/AnimNotifyState.h>
#include "MaterialLerpNotify.generated.h"

UCLASS()
class DUNGEONS_API UMaterialLerpNotify : public UAnimNotifyState {
	GENERATED_BODY()

public:
	/*virtual?*/
	virtual bool Received_NotifyBegin(class USkeletalMeshComponent* meshComponent, class UAnimSequence* sequence, float totalDuration) const;

private:
	UPROPERTY(EditDefaultsOnly)
	int materialIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	FName parameterValueStartName;

	UPROPERTY(EditDefaultsOnly)
	FName parameterTimeStartName;

	UPROPERTY(EditDefaultsOnly)
	FName parameterValueEndName;

	UPROPERTY(EditDefaultsOnly)
	FName parameterTimeEndName;

	UPROPERTY(EditDefaultsOnly)
	float valueStart = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float valueEnd = 1.f;
};
