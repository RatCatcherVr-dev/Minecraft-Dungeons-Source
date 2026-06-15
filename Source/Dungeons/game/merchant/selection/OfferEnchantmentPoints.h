#pragma once
#include "MerchantSelectionBase.h"
#include "OfferEnchantmentPoints.generated.h"

UCLASS(BlueprintType)
class DUNGEONS_API UOfferEnchantmentPoints : public UMerchantSelectionBase {
	GENERATED_BODY()

private:
	using MaxProvider = std::function<int(int)>;
	MaxProvider mMaxProvider;

protected:	
	int mOfferedEnchantmentPoints = 0;
	int mLastMax = 0;

	void AvailableEnchantmentPointsChanged() const;

	void OnSetupSession() override;
	void OnCleanupSession() override;


public:	
	void Refresh() override;

	static UOfferEnchantmentPoints* CreateSubobject(UObject* object, const FName& name, const MaxProvider&);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetOfferedEnchantmentPoints(int points);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetMaxOfferableEnchantmentPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetOfferedEnchantmentPoints() const;

	int GetAvailableOfferedEnchantmentPoints() const;

	bool ShouldShowEnchantmentPoints() const override;
	void EnsureSelection() override;
	void ClearSelection() override;

	bool CanSelectAny() const override;
	bool HasSelectedAny() const override;
};

