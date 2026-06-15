#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnFreelyComponent.generated.h"

class ABasePlayerState;
class ADungeonsGameState;
class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API URespawnFreelyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void BeginPlay() override;
private:

	void OnPlayerCharacterAdded(APlayerCharacter* player);

	//Respawn player freely
	void RespawnPlayerCharacter(APlayerCharacter* player);
	
	ADungeonsGameState* GetGameState() const;
};
