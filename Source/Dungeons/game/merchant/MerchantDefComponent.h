#pragma once
#include "type/MerchantBase.h"
#include "MerchantDef.h"
#include "MerchantDefComponent.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API UMerchantDefComponent : public UActorComponent {
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	TMap<APlayerCharacter*, AMerchantBase*> mPlayerMerchantMap;

	UFUNCTION()
	void OnPlayerCharacterAdding(APlayerCharacter* player);

	UFUNCTION()
	void OnPlayerCharacterRemoving(APlayerCharacter* player);

	void CreateMerchantFor(APlayerCharacter* player);
	void DestroyMerchantFor(APlayerCharacter* player);

	bool HasMerchantFor(APlayerCharacter* player) const;

public:
	void BeginPlay() override;	
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	AMerchantBase* GetOrCreateMerchantFor(APlayerCharacter* player);
	AMerchantBase* GetMerchantFor(APlayerCharacter* player) const;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMerchantDef> MerchantDefinition;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetMerchantDisplayName() const;

	bool ShouldShowObjectiveMarkerFor(APlayerCharacter* player) const;
};