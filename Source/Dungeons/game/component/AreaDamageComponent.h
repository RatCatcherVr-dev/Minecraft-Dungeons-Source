#pragma once

#include "game/actor/character/BaseCharacter.h"
#include "game/util/Pushback.h"
#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <GameplayTagContainer.h>
#include "AreaDamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UAreaDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAreaDamageComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float radius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float damage = 20.f;
private:
	bool Affects(const ABaseCharacter* target, const ABaseCharacter* source);
	
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float tickInterval = .5f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int tickSkip = 0;
	
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int tickMax = -1;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool blockBased = false;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	FGameplayTag damageType;

	//Only valid if source is not a character
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool affectsPlayers = true;

	//Only valid if source is not a character
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool affectsMobs = true;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool affectsPets = true;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool affectsSelf = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FPushback pushback;
};
