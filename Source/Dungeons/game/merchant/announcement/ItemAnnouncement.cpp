#include "Dungeons.h"
#include "ItemAnnouncement.h"

#define LOCTEXT_NAMESPACE "MerchantAnnouncement"
FText UMerchantAnnouncementUtil::GetAnnouncementText(EItemAnnouncementType type) {
	switch (type) {
	case EItemAnnouncementType::PURCHASED:
		return LOCTEXT("PURCHASED", "Purchased");
	case EItemAnnouncementType::GIFT_SENT:
		return LOCTEXT("GIFT_SENT", "Gift Sent");
	case EItemAnnouncementType::GIFT_RECEIVED:
		return LOCTEXT("GIFT_RECEIVED", "Gift Received");
	case EItemAnnouncementType::UPGRADED:
		return LOCTEXT("UPGRADED", "Upgraded");
	case EItemAnnouncementType::COLLECTED:
		return LOCTEXT("COLLECTED", "Collected");
	case EItemAnnouncementType::RECEIVED:
		return LOCTEXT("RECEIVED", "Received");
	default:
		return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE