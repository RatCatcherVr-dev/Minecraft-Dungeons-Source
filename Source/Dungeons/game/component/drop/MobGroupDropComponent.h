#pragma once

#include "PredefinedItemDropComponent.h"
#include "SharedPointer.h"
#include "MobGroupDropComponent.generated.h"

USTRUCT()
struct DUNGEONS_API FMobDropGroup {
	GENERATED_USTRUCT_BODY()

	FMobDropGroup() = default;

	FMobDropGroup(const TArray<AMobCharacter*>& mobs);

	TArray<TWeakObjectPtr<AMobCharacter>> Mobs;

	bool IsMobGroupDead() const;
};

/**
 * Generates drops based on its predefined settings, but only once all the mobs in the group are dead.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UMobGroupDropComponent : public UPredefinedItemDropComponent {
	GENERATED_BODY()

	UMobGroupDropComponent();
public:
	static UMobGroupDropComponent* Create(UObject* outer, EItemRarityChanceCategory, EDropCategory, float);

	void SetMobGroup(TSharedPtr<FMobDropGroup> group);

protected:	
	void OnMobKilled(const FOnAttributeChangeData& data) override;
	TSharedPtr<FMobDropGroup> MobGroup;
};