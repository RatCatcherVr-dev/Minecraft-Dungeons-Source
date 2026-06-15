#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MobActivationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivationChanged, bool, bIsActive);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UMobActivationComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMobActivationComponent();

	UFUNCTION(BlueprintCallable)
	void ActivateMob();

	UFUNCTION(BlueprintCallable)
	void DeactivateMob();

	UFUNCTION(BlueprintCallable)
	bool IsMobActive();

	UPROPERTY(BlueprintAssignable)
	FOnActivationChanged OnActivationChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMobActive = false;
private:
	/*UPROPERTY(EditDefaultsOnly)
	bool bIsMobActive = false;*/
};
