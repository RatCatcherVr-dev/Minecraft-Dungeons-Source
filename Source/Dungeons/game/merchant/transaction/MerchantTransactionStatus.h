#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/merchant/MerchantContext.h"
#include "game/merchant/currency/MerchantPrice.h"
#include "game/item/ItemBulletPoint.h"
#include "ui/status/ProblemStatus.h"
#include "MerchantTransactionStatus.generated.h"


UENUM(BlueprintType)
enum class EMerchantTransactionStatusReason : uint8 {
	UNSET,
	//Problem
	CANT_AFFORD,
	INVENTORY_FULL,	
	RECIPIENT_INVENTORY_FULL,
	NO_SLOT_SELECTED,
	SLOT_LOCKED,
	SLOT_OCCUPIED,
	ITEM_LOCKED,
	NO_ITEM_SELECTED,
	NO_INVENTORY_SLOT_SELECTED,
	NO_INVENTORY_ITEM_SELECTED,
	NO_REGIFTING,
	NO_GIFTING_TO_SELF,
	CANT_GIFT_CLONES,
	NO_PLAYER_SELECTED,
	ALREADY_RESERVED,
	NOT_RESERVED,
	NOTHING_TO_RESTOCK,
	NOTHING_TO_WITHDRAW,
	OFFER_AT_LEAST_ONE_ITEM,
	MISSION_VOTE_IN_PROGRESS,
	MISSION_DISABLED,
	CANNOT_OFFER_GILDED_ITEMS,
	CANNOT_OFFER_CLONED_ITEMS,
	OFFERING_PROBLEM,
	NOT_A_HYPERMISSION,
	NO_ENCHANTMENT_INDEX_SELECTED,
	NO_NEW_ENCHANTMENTS_AVAILABLE,
	//Confirm
	GIFT_ITEM,
	BUY_ITEM,
	COLLECT_ITEM,
	UPGRADE_COLLECT_ITEM,
	UPGRADE_ITEM,
	RESERVE_ITEM,
	UNRESERVE_ITEM,
	OFFER_ITEM,
	WITHDRAW_ITEM,
	CONFIRM_OFFERINGS,
	RESTOCK,
	MODIFY_ITEM,
	CLAIM_HUB_ITEM
};

USTRUCT()
struct FMerchantExecuteStatus {
	GENERATED_BODY()	
	TOptional<EMerchantTransactionStatusReason> mStatusReason;
	TOptional<FProblemStatus> mProblem;	

	FMerchantExecuteStatus(EMerchantTransactionStatusReason, FProblemStatus);
	FMerchantExecuteStatus(EMerchantTransactionStatusReason);
	FMerchantExecuteStatus();
};

USTRUCT()
struct FMerchantTransactionStatus {
	GENERATED_BODY()
	typedef std::function<void()> ValidatedTransaction;

	TOptional<FMerchantExecuteStatus> mExecuteStatus;
	TOptional<FMerchantPricing> mPrice;
	TArray<FItemBulletPoint> mBulletPoints;
	TOptional<ValidatedTransaction> mValidatedTransaction;

	bool CanExecute() const;
	void Execute();	
	TOptional<FMerchantPricing> GetOptionalPrice() const;
	TOptional<EMerchantTransactionStatusReason> GetOptionalStatus() const;	
	TOptional<FProblemStatus> GetOptionalProblemStatus() const;
	TArray<FItemBulletPoint> GetBulletPoints() const;
};

UCLASS(BlueprintType)
class UMerchantTransactionUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetTransactionReasonText(EMerchantTransactionStatusReason reason);	
};
