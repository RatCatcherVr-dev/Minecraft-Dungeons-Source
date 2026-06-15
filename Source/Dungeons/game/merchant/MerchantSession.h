#pragma once
#include "game/difficulty/DifficultyStats.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/difficulty/DifficultyRecommendation.h"
#include "game/progress/ProgressStat.h"
#include "game/GameSettings.h"
#include "MerchantSaveData.h"
#include "currency/MerchantPrice.h"

class APlayerCharacter;
class AMerchantBase;
class UWalletComponent;
class UMissionProgressComponent;
class UMerchantCurrencyComponent;
struct FInventoryItemData;
class UMerchantPricingComponent;
class UItemStashComponent;
class UPickupItemComponent;
class UShopperComponent;
class UAdventureHubComponent;

namespace merchant {
	struct Session {
	private:
		APlayerCharacter& mShopper;
		AMerchantBase& mMerchant;

		FName GetMerchantSaveName() const;
	public:
		Session(AMerchantBase& merchant, APlayerCharacter& shopper);
		const game::DifficultyStats& GetGameDifficultyStats() const;
		const TOptional<game::DifficultyStats>& GetHighestCompletedDifficultyStats() const;
		const game::DifficultyRecommendation& GetDifficultyRecommendation() const;
		
		const game::Settings& GetGameSettings() const;

		const UWalletComponent* GetWalletComponent() const;
		UWalletComponent* GetWalletComponentMutable() const;
		const UMissionProgressComponent* GetMissionProgress() const;
		const UMerchantCurrencyComponent* GetCurrencyComponent() const;
		const UMerchantPricingComponent* GetPricingComponent() const;
		const UItemStashComponent* GetItemStashComponent() const;
		UItemStashComponent* GetItemStashComponentMutable() const;
		UPickupItemComponent* GetPickupItemComponentMutable() const;
		UAdventureHubComponent* GetAdventureHubComponentMutable() const;

		int GetBalance() const;
		void Deduct(const FMerchantPricing& pricing) const;
		bool CanAfford(const FMerchantPricing& pricing) const;

		TOptional<FMerchantPricing> GetPrice(const FInventoryItemData& item, float PriceMultiplier = 1.0f, float RebateFraction = 0.0f) const;
		TOptional<FMerchantPricing> GetGiftPrice(const FInventoryItemData& item, float PriceMultiplier = 1.0f) const;
		TOptional<FMerchantPricing> GetUpgradePrice(const FInventoryItemData& fromItem, const FInventoryItemData& toItem, float PriceMultiplier = 1.0f) const;
		TOptional<FMerchantPricing> GetRestockPrice() const;

		const AMerchantBase& GetMerchant() const;
		const APlayerCharacter& GetShopper() const;

		void RestockSlots() const;
	};
}