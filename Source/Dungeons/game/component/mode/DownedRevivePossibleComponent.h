#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DownedRevivePossibleComponent.generated.h"

class ABasePlayerState;
class ADungeonsGameState;
class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDownedRevivePossibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
private:
	ADungeonsGameState* GetGameState() const;
};
