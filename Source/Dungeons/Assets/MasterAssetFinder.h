#pragma once

#include "CoreMinimal.h"
#include "MasterAssetFinder.Generated.h"

class UItemAssetFinder;
class UEnchantmentAssetFinder;
class UArmorPropertyAssetFinder;
class UAmbienceFinder;

UCLASS()
class DUNGEONS_API UMasterAssetFinder : public UObject{
	GENERATED_BODY()
public:
	UMasterAssetFinder();

	FORCEINLINE UItemAssetFinder* GetItemAssetFinder() const { return ItemAssetFinder; }
	FORCEINLINE UEnchantmentAssetFinder* GetEnchantmentAssetFinder() const { return EnchantmentAssetFinder; }
	FORCEINLINE UArmorPropertyAssetFinder* GetArmorPropertyAssetFinder() const { return ArmorPropertyAssetFinder; }
	FORCEINLINE UAmbienceFinder* GetAmbienceFinder() const { return AmbienceFinder; }

	void Initialize(bool force);

private:
	UPROPERTY()
	UItemAssetFinder* ItemAssetFinder;

	UPROPERTY()
	UEnchantmentAssetFinder* EnchantmentAssetFinder;

	UPROPERTY()
	UArmorPropertyAssetFinder* ArmorPropertyAssetFinder;

	UPROPERTY()
	UAmbienceFinder* AmbienceFinder;
};
