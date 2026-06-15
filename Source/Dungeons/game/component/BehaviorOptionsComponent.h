#pragma once

#include "CoreMinimal.h"
#include "game/ai/bt/BtTime.h"
#include "world/entity/EntityTypes.h"
#include "Components/ActorComponent.h"
#include "GameplayEffect.h"
#include "BehaviorOptionsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UBehaviorOptionsComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UBehaviorOptionsComponent();

	float Get(const FString&, float) const;

	bt::Duration Get(const FString&, bt::Duration) const;

	UAnimSequenceBase* Get(const FString&) const;
	
	TSubclassOf<AActor> GetClass(const FString&) const;

	EntityType Get(const FString&, EntityType defaultValue) const;

	TSubclassOf<UGameplayEffect> GetEffect(const FString&) const;

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TMap<FString, float> Options;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TMap<FString, UAnimSequenceBase*> Sequences;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TMap<FString, TSubclassOf<AActor>> Classes;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TMap<FString, EntityType> Entities;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TMap<FString, TSubclassOf<UGameplayEffect>> GameplayEffects;
};
