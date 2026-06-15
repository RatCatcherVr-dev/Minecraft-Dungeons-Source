#include "Dungeons.h"
#include "MerchantSession.h"
#include "type/MerchantBase.h"
#include "pricing/MerchantPricingComponent.h"
#include "currency/MerchantCurrencyComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/GameSettings.h"
#include "game/Game.h"
#include "game/component/AdventureHubComponent.h"

namespace merchant {

	Session::Session(AMerchantBase& merchant, APlayerCharacter& shopper)
		: mMerchant(merchant)
		, mShopper(shopper)
	{}

	const game::DifficultyStats& Session::GetGameDifficultyStats() const {
		return GetGameSettings().difficultyStats;
	}

	const TOptional<game::DifficultyStats>& Session::GetHighestCompletedDifficultyStats() const {
		return mShopper.GetMissionProgressComponent()->GetHighestCompletedDifficultyStats();
	}

	const game::DifficultyRecommendation& Session::GetDifficultyRecommendation() const {
		return mShopper.GetEquipmentComponent()->GetDifficultyRecommendation();
	}
	
	const game::Settings& Session::GetGameSettings() const {
		const auto* game = actorquery::getGame(mShopper.GetWorld());
		check(game && "merchants are not supported outside game levels");
		return game->settings();
	}

	const UWalletComponent* Session::GetWalletComponent() const {
		return mShopper.GetWalletComponent();
	}

	UWalletComponent* Session::GetWalletComponentMutable() const {
		return mShopper.GetWalletComponent();
	}

	const UMissionProgressComponent* Session::GetMissionProgress() const {
		return mShopper.GetMissionProgressComponent();
	}

	const UMerchantCurrencyComponent* Session::GetCurrencyComponent() const {
		return mMerchant.GetCurrencyComponent();
	}

	const UMerchantPricingComponent* Session::GetPricingComponent() const {
		return mMerchant.GetPricingComponent();
	}

	const UItemStashComponent* Session::GetItemStashComponent() const {
		return mShopper.GetItemStashComponent();
	}

	UItemStashComponent* Session::GetItemStashComponentMutable() const {
		return mShopper.GetItemStashComponent();
	}

	UPickupItemComponent* Session::GetPickupItemComponentMutable() const {
		return mShopper.GetPickupItemComponent();
	}

	UAdventureHubComponent* Session::GetAdventureHubComponentMutable() const {
		return mShopper.GetAdventureHubComponent();
	}

	int Session::GetBalance() const {
		if (auto currencyComponent = GetCurrencyComponent()) {
			return currencyComponent->GetBalance();
		}
		return 0;
	}

	void Session::Deduct(const FMerchantPricing& pricing) const {
		if (auto currencyComponent = GetCurrencyComponent()) {
			currencyComponent->Deduct(pricing.Price);
		}
	}

	bool Session::CanAfford(const FMerchantPricing& pricing) const {
		return GetBalance() >= pricing.Price;
	}

	TOptional<FMerchantPricing> Session::GetPrice(const FInventoryItemData& item, float PriceMultiplier /*= 1.0f*/, float RebateFraction /*= 0.0f*/) const {
		if (auto pricingComponent = GetPricingComponent()) {
			return pricingComponent->GetPrice(item, PriceMultiplier, RebateFraction);
		}
		return {};
	}

	TOptional<FMerchantPricing> Session::GetGiftPrice(const FInventoryItemData& item, float PriceMultiplier /*= 1.0f*/) const {
		if (auto pricingComponent = GetPricingComponent()) {
			return pricingComponent->GetGiftPrice(item, PriceMultiplier);
		}
		return {};
	}

	TOptional<FMerchantPricing> Session::GetUpgradePrice(const FInventoryItemData& fromItem, const FInventoryItemData& toItem, float PriceMultiplier /*= 1.0f*/) const {
		if (auto pricingComponent = GetPricingComponent()) {
			return pricingComponent->GetUpgradePrice(fromItem, toItem, PriceMultiplier);
		}
		return {};
	}

	TOptional<FMerchantPricing> Session::GetRestockPrice() const {
		if (auto pricingComponent = GetPricingComponent()) {
			return pricingComponent->GetRestockPrice();
		}
		return {};
	}

	const AMerchantBase& Session::GetMerchant() const {
		return mMerchant;
	}

	const APlayerCharacter& Session::GetShopper() const {
		return mShopper;
	}

	void Session::RestockSlots() const {
		mMerchant.RestockSlots();
	}
}