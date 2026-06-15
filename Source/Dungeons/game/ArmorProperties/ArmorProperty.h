#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayPrediction.h"
#include "game/ArmorProperties/ArmorPropertyEnumTypes.h"
#include <GameplayEffect.h>
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "NoExportTypes.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/util/GearUtil.h"
#include "ArmorProperty.generated.h"


class ABaseProjectile;


USTRUCT(BlueprintType)
struct DUNGEONS_API FPropertyColorPrio {
	GENERATED_USTRUCT_BODY()
	const FLinearColor getMultipliedColor() const {
		return Color * ColorMultiplier;
	}

	const FLinearColor getColor() const {
		return Color;
	}

	UPROPERTY(EditDefaultsOnly)
	float Prio = 199.0f; 
	/* this number is for mystery armor gem color. If value is 0 it has highest chance to display
	It is 199 so it will never show up so you don't have to care about it when making new armor properties*/
protected:
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color = FColor::White;
	UPROPERTY(EditDefaultsOnly)
	float ColorMultiplier = 12.0f;
};

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API UArmorProperty : public UGearUtil
{
	GENERATED_BODY()

public:	
	UArmorProperty();

	UFUNCTION(Client, Reliable)
	void BroadcastArmorPropertyTriggeredEvent() const;

	virtual bool ShouldDeactivate() { return true; };

	void SetItemPower(float ItemPower);

	virtual FFormatOrderedArguments createFormattedDisplayValueString() const;

	EArmorPropertyID GetTypeID() const;

	const FPropertyColorPrio& GetPropertyColorPrio() const;

	class ABaseCharacter* GetOwnerCharacter() const;


	static TArray< UTexture2D* > s_IconTextures;
	static TArray< UMaterialInstance* > s_IconMaterials;
	static TArray< UClass* > s_ArmorPropertyClasses;

	static void PreloadIconTextures();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UTexture2D* GetIconTextureForArmorPropertyType(UObject* ref, EArmorPropertyID type);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UMaterialInstance* GetIconMaterialInstanceForArmorPropertyType(UObject* ref, EArmorPropertyID type);
	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static FText GetNameForArmorPropertyType(EArmorPropertyID type);
protected:
	UPROPERTY(EditDefaultsOnly)
	FPropertyColorPrio PropertyColorPrio;

	virtual void BeginPlay() override;

	virtual FString createFormattedValueString() const { return ""; };
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EArmorPropertyID TypeID;

	float SourceItemPower = 1.0f;
};
