#pragma once

#include "AbilitySystemComponent.h"
#include "DungeonsAbilitySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct FHandleUIDataInfo {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle Handle;

	UPROPERTY(BlueprintReadOnly)
	UGameplayEffectUIData* UiData;

};

UCLASS()
class UDungeonsAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()
public:
	void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FHandleUIDataInfo> GetActiveEffectsWithUiData();
};
