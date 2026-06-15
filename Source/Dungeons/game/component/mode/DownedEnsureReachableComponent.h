#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "lovika/LevelCommon.h"
#include "game/Game.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "DownedEnsureReachableComponent.generated.h"


class ABasePlayerState;
class ADungeonsGameState;
class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDownedEnsureReachableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void BeginPlay() override;
private:

	void OnPlayerCharacterAdded(APlayerCharacter* player);	
	void OnPlayerCharacterDown(APlayerCharacter* player) const;	
	void OnPlayerCharacterLanded(APlayerCharacter* player) const;
	void OnPlayerCharacterRevive(APlayerCharacter* player) const;	
	void OnPlayerCharacterMaterial(EMaterialTypeEnum material, APlayerCharacter* player) const;
	void OnPlayerCharacterDamageType(float damageAmount, const FGameplayTagContainer& damageTag, APlayerCharacter* player) const;

	static bool IsPlayerInDangerousMaterial(APlayerCharacter* player);
	static bool IsDamageTypeUnreachable(const FGameplayTagContainer& damage);
	bool IsAnyPlayer(EAliveState aliveState) const;

	bool IsAnyPlayerAlive() const;	
	static bool IsDown(APlayerCharacter*& otherPlayer);

	static const TArray<EMaterialTypeEnum> DangerousMaterials;

	ADungeonsGameState* GetGameState() const;
};
