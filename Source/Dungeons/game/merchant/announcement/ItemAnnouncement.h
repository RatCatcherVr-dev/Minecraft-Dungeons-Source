#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/item/InventoryItemData.h"
#include "ItemAnnouncement.generated.h"


UENUM(BlueprintType)
enum class EItemAnnouncementType : uint8 {
	UNSET,
	//Annoucement
	PURCHASED,
	GIFT_SENT,
	GIFT_RECEIVED,
	UPGRADED,
	COLLECTED,
	RECEIVED,	
};

USTRUCT(BlueprintType)
struct FItemAnnouncement {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EItemAnnouncementType type;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FInventoryItemData itemData;
};

UCLASS(BlueprintType)
class UMerchantAnnouncementUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetAnnouncementText(EItemAnnouncementType type);
};
