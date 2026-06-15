#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Synergy.h"
#include "BeastBoss.generated.h"


UCLASS()
class DUNGEONS_API UBeastBossDamageBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBeastBossDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UBeastBoss : public UEnchantment {
    GENERATED_BODY()
	
public:
    UBeastBoss();

	//FText CreateDescription() const override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBeastBossDamageBoostGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
    float BaseDamageBoost = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PerLevelDamageBoost = 0.1f;

protected:
	void OnStart() override;
	void OnEnd() override;
	
private:
	void ApplyBuffToMinion(ABaseCharacter* minion);
	void RemoveBuffFromMinion(ABaseCharacter* minion);
};
