#pragma once
#include "PlayerCommonTypes.generated.h"

UENUM(BlueprintType)
enum class ETeleportState : uint8 {
	Idle = 0,
	Charging,
	Flying,
	Landing,
};

// move collection to PlayerCommonTypes .. access from PlayerCharacter
USTRUCT(BlueprintType)
struct DUNGEONS_API FAvatarData
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|UI")
	UMaterialInstance* PortraitMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|UI")
	FColor PlayerColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|UI")
	FColor FrameBackgroundColor;
};

