#include "Dungeons.h"
#include "CommonTypes.h"
#include "MutableEmoteDef.h"
#include "util/CollectionUtils.h"
#include "util/Algo.h"
#include "EmoteDefs.h"
#include <SoftObjectPath.h>

namespace emotes {

	auto emoteDefs = Util::createDefaultedTArrayOfSize<Unique<MutableEmoteDef>>(enum_cast(EEmote::COUNT));

	MutableEmoteDef& create(EEmote emote) {
		const int index = enum_cast(emote);
		check(!emoteDefs[index]);
		emoteDefs[index] = make_unique<MutableEmoteDef>(emote);
		return *emoteDefs[index];
	}

#define LOCTEXT_NAMESPACE ""

	//
	// Emote definitions
	//

	const EmoteDef& Invalid = create(EEmote::INVALID);

	const EmoteDef& ComeHere = create(EEmote::ComeHere)
		.text(LOCTEXT("HUD_ComeHere", "Come Here!"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconComehere.T_IconComehere"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_ComeHereChatMapPin.UMG_ComeHereChatMapPin_C"))
		.detachable()
		;

	const EmoteDef& Thanks = create(EEmote::Thanks)
		.text(LOCTEXT("HUD_Thanks", "Thanks"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconThankYou.T_IconThankYou"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_ThanksChatMapPin.UMG_ThanksChatMapPin_C"))
		;

	const EmoteDef& Ok = create(EEmote::Ok)
		.text(LOCTEXT("HUD_OK", "OK!"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconOK.T_IconOK"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_OKChatMapPin.UMG_OKChatMapPin_C"))
		;

	const EmoteDef& NeedArrows = create(EEmote::NeedArrows)
		.text(LOCTEXT("HUD_Hurry", "Need arrows"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_AmmoHere.T_AmmoHere"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_NeedArrowsChatMapPin.UMG_NeedArrowsChatMapPin_C"))
		;

	const EmoteDef& Wait = create(EEmote::Wait)
		.text(LOCTEXT("HUD_Wait", "Wait"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconWait.T_IconWait"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_WaitChatMapPin.UMG_WaitChatMapPin_C"))
		;

	const EmoteDef& NeedHealth = create(EEmote::NeedHealth)
		.text(LOCTEXT("HUD_+NeedHealthPotion", "Need Health"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconHealth.T_IconHealth"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_NeedHealthChatMapPin.UMG_NeedHealthChatMapPin_C"))
		;

	const EmoteDef& No = create(EEmote::No)
		.text(LOCTEXT("HUD_Nope", "No"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconNo.T_IconNo"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_NoChatMapPin.UMG_NoChatMapPin_C"))
		;

	const EmoteDef& SuppliesHere = create(EEmote::SuppliesHere)
		.text(LOCTEXT("HUD_NeedSupplies", "Supplies here!"))
		.icon(FSoftObjectPath("/Game/UI/Materials/ChatWheel/New/T_IconChest.T_IconChest"))
		.mapPin(FSoftClassPath("/Game/UI/Chat/MapPins/UMG_SuppliesHereChatMapPin.UMG_SuppliesHereChatMapPin_C"))
		.detachable()
		;

#undef LOCTEXT_NAMESPACE


	const EmoteDef& get(EEmote emote) {
		if (auto dlc = getChecked(emote)) {
			return *dlc;
		}
		return Invalid;
	}

	const EmoteDef* getChecked(EEmote emote) {
		const int index = static_cast<int>(emote);
		if (index > static_cast<int>(EEmote::INVALID) && index < static_cast<int>(EEmote::COUNT)) {
			return emoteDefs[index].get();
		}
		return nullptr;
	}
}


//
// Blueprint interface
//

const FText& UEmoteDefs::GetEmoteText(EEmote emote) {
	if (auto emoteDef = emotes::getChecked(emote)) {
		return emoteDef->GetText();
	}
	return FText::GetEmpty();
}

TSoftObjectPtr<UTexture2D> UEmoteDefs::GetEmoteIconSoftTexture(EEmote emote) {
	if (auto emoteDef = emotes::getChecked(emote)) {		
		return TSoftObjectPtr<UTexture2D>(emoteDef->GetIconTexturePath());		
	}
	return {};
}

UTexture2D* UEmoteDefs::GetEmoteIconTexture(EEmote emote) {
	if (auto emoteDef = emotes::getChecked(emote)) {
		return emoteDef->GetIconTexture();
	}
	return nullptr;
}



