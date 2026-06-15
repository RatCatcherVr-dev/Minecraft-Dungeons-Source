#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "PerfectFormMinionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UPerfectFormMinionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerfectFormMinionComponent();

	TArray<FEnchantmentData> GetPossibleEnchantments() const { return PossibleEnachants; }

	bool ShouldRandomiseEnchants() const { return bRandomisedEnchants; }

	void BindPerfectFormDelegate(class AMobCharacter*);

	void UnbindPerfectFormDelegate();

	bool GetFeastState() const { return bCanFeast; }

private:
	void OnEndermiteFeast(bool isActive);

private:
	UPROPERTY(EditDefaultsOnly)
	bool bRandomisedEnchants = false;

	UPROPERTY(EditDefaultsOnly)
	TArray<FEnchantmentData> PossibleEnachants;

	bool bCanFeast;
	TWeakObjectPtr<class UPerfectFormComponent> PerfectFormComponent;
	FDelegateHandle OnEndermiteFeastDelegateHangle;
};
