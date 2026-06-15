#pragma once

#include "Components/ActorComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "WorldspaceHUDComponent.generated.h"


UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UWorldspaceHUDComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void AddClicky(UInteractableComponent* clickyComponent);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveClicky(UInteractableComponent* clickyComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateContextPrompt(UInteractableComponent* interactalbleComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDrownCountdown(APlayerCharacter* character, int countdownValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	bool IsPointHiddenByHotbar(FVector2D point);
};
