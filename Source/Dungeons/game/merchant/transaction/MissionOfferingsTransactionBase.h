#pragma once
#include "MerchantTransactionBase.h"
#include "game/levels.h"
#include "game/inventory/InventoryItem.h"
#include "game/mission/offerings/MissionOfferings.h"
#include "MissionOfferingsTransactionBase.generated.h"

class UInventoryItem;

UCLASS()
class DUNGEONS_API UMissionOfferingsTransactionBase : public UMerchantTransactionBase {
	GENERATED_BODY()

private:
	ELevelNames mMission = ELevelNames::Invalid;

public:
	template <typename T>
	static T* CreateSubobject(UObject* object, const FName& name, ELevelNames mission) {	
		auto offerings = object->CreateDefaultSubobject<T>(name);
		offerings->mMission = mission;
		return offerings;	
	}

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMissionOfferings GetMissionOfferings() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ELevelNames GetMission() const { return mMission; };

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetIsItemBeingOffered(UInventoryItem* item) const;

protected:

	virtual FMissionOfferings CreateMissionOfferings() const { return {}; };
	virtual bool IsItemBeingOffered(UInventoryItem*) const { return false; };

};
