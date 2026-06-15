#pragma once

#include <Animation/AnimNotifies/AnimNotify.h>
#include "MaterialNotify.generated.h"

UCLASS()
class DUNGEONS_API UMaterialNotify : public UAnimNotify {
	GENERATED_BODY()

public:
	/*virtual?*/
	virtual bool Received_Notify(class USkeletalMeshComponent* meshComponent, class UAnimSequence* sequence) const;

private:
	UPROPERTY(EditDefaultsOnly)
	int materialIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	FName parameterName;

	UPROPERTY(EditDefaultsOnly)
	float value;
};
