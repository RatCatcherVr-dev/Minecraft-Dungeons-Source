// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Dynamo.generated.h"

UCLASS()
class DUNGEONS_API UDynamoGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDynamoGameplayEffect();

	static const FName DamageIncreaseKey;
};


UCLASS()
class DUNGEONS_API UDynamoMeleeGameplayEffect : public UDynamoGameplayEffect {
	GENERATED_BODY()
public:
	UDynamoMeleeGameplayEffect();
};

UCLASS()
class DUNGEONS_API UDynamoRangedGameplayEffect : public UDynamoGameplayEffect {
	GENERATED_BODY()
public:
	UDynamoRangedGameplayEffect();
};

/**
 * 
 */
UCLASS(Abstract)
class DUNGEONS_API UDynamo : public UEnchantment
{
	GENERATED_BODY()
public:
	UDynamo();

	void Consume();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDynamoGameplayEffect> Effect;

	void OnDodgeRollEnd(FPredictionKey);
	void EndPlay(const EEndPlayReason::Type) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamage = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageIncreasePerLevel = 15.0f;

	int StackCount() const { return CurrentStackCount; }
private:

	int CurrentStackCount;
};

UCLASS()
class DUNGEONS_API UDynamoMelee : public UDynamo {
	GENERATED_BODY()
public:
	UDynamoMelee();
	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window);
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window);
};

UCLASS()
class DUNGEONS_API UDynamoRanged : public UDynamo {
	GENERATED_BODY()
public:
	UDynamoRanged();
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	//Used in BaseProjectile
	static const FName DynamoRangedStackCountKey;
private:
};