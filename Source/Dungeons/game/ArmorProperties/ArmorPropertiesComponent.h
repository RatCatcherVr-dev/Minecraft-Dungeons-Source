#pragma once

#include "CoreMinimal.h"
#include "GameplayPrediction.h"
#include "Components/ActorComponent.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include <GameplayEffect.h>
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/component/GearUtilComponent.h"
#include "ArmorPropertiesComponent.generated.h"

struct FGameplayEffectModCallbackData;
class ABaseProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UArmorPropertiesComponent : public UGearUtilComponent
{
	GENERATED_BODY()
public:	
	UArmorPropertiesComponent();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddProperties(const TArray<FArmorPropertyData>& propertyData, float itemPower);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveAllProperties();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerActivateProperties(const TArray<FArmorPropertyData>& propertyData, float itemPower);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeactivateProperties();
	
	FLinearColor DetermineArmorColor();	
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	void RemoveAllProperties();
	void AddProperties(const TArray<FArmorPropertyData>& propertyData, float itemPower);

	UAbilitySystemComponent* GetAbilitySystem() const;

	UPROPERTY(Transient, Replicated)
	TArray<class UArmorProperty*> Properties;

	FRandomStream DefaultRandom;
	FDelegateHandle OnOwnerRevived;

protected:
	TArray<UGearUtil*> GetValidGearUtils() const override;

};