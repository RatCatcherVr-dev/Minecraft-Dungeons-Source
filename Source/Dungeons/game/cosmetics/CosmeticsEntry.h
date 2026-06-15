#pragma once

#include "Engine/DataTable.h"
#include "CosmeticType.h"
#include <SoftObjectPtr.h>
#include "game/actor/cosmetics/CosmeticItemInfo.h"
#include <Engine/Texture2D.h>
#include <Sound/SoundCue.h>
#include <Animation/AnimSequenceBase.h>
#include "CosmeticsEntryBase.h"
#include "CosmeticsEntry.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FCosmeticsEntry : public FCosmeticsEntryBase {
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FText DisplayDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftObjectPtr<USoundCue> EquippedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftObjectPtr<UAnimSequenceBase> EquippedAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftClassPtr<ACosmeticItemInfo> BlueprintClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FString Entitlement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	ECosmeticType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PreferredInventoryCharacterRotation;

	TArray<FSoftObjectPath> LinkedAssets() const override {
		return {BlueprintClass.ToSoftObjectPath(), IconTexture.ToSoftObjectPath(), EquippedSound.ToSoftObjectPath(), EquippedAnimation.ToSoftObjectPath()};
	}
};