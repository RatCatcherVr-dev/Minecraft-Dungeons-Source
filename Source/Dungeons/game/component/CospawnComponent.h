#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "CospawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UCospawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCospawnComponent();

	virtual void BeginPlay() override;

	void SpawnAll();
	
	void Spawn(FVector offset);

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	EntityType entityType;

	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	bool assignMaster = true;

	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	bool scaleToOwner = false;

	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	int count = 5;

	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	float radius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons|CospawnComponent")
	TArray<FEnchantmentData> enchantments;
};
