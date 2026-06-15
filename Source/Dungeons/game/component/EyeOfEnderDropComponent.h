#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/drop/EyeOfEnderDefines.h"
#include "save/CharacterSaveData.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "EyeOfEnderDropComponent.generated.h"

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UEyeOfEnderDropComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEyeOfEnderDropComponent();

	void BeginPlay() override;

	void HandleDeath();

	UPROPERTY(EditDefaultsOnly)
	EEyeOfEnderType eyeOfEnderType = EEyeOfEnderType::NOT_SET;
};
