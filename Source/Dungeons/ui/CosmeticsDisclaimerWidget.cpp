// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "CosmeticsDisclaimerWidget.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "DungeonsGameInstance.h"
#include "TextBlock.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "LiveOpsClient.h"
#include "game/component/cosmetics/CosmeticsComponent.h"

namespace cosmetics {
DisplayPredicate CheckCoopVisibility() {
	return [](const MessageContext& context) -> bool {
		return context.gameInstance->IsLocalCoop() && context.player->GetPlayerNumber() != 0;
	};
}

DisplayPredicate CheckConnectedWithCacheVisibility() {
	return [](const MessageContext& context) -> bool {
		auto liveOps = online::getLiveOps(context.gameInstance);
		return context.player->GetPlayerNumber() == 0 && liveOps->GetConnectionStatus() != EMinecraftAPIConnectionStatus::Connected;
	};
}

DisplayPredicate CheckConnectedNoCacheVisibility() {
	return [](const MessageContext& context) -> bool {
		auto liveOps = online::getLiveOps(context.gameInstance);
		auto cosmeticsComp = context.player->GetCosmeticsComponent();
		return !cosmeticsComp->HasAnyCosmetics() && context.player->GetPlayerNumber() == 0 && liveOps->GetConnectionStatus() != EMinecraftAPIConnectionStatus::Connected;
	};
}

TextGen CoopDisclaimerText() {
	return []() {
		const auto text = LocTableFromFile::Get("InventoryHUDLabels.csv", "cosmetics_disclaimer_local_coop");
		const auto playerText = LocTableFromFile::Get("/Game/Decor/Text/NetworkUILabels.NetworkUILabels", "player_1");
		return FText::Format(text, playerText);
	};
}

TextGen DisconnectedWithCacheDisclaimerText() {
	return []() {
		return LocTableFromFile::Get("InventoryHUDLabels.csv", "cosmetics_disclaimer_disconnected_cache");
	};
}

TextGen DisconnectedNoCacheDisclaimerText() {
	return []() {
		return LocTableFromFile::Get("InventoryHUDLabels.csv", "cosmetics_disclaimer_disconnected_nocache");
	};
}

}

void UCosmeticsDisclaimerWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	Messages = { 
		{ cosmetics::CheckConnectedNoCacheVisibility(), cosmetics::DisconnectedNoCacheDisclaimerText() },
		{ cosmetics::CheckConnectedWithCacheVisibility(), cosmetics::DisconnectedWithCacheDisclaimerText() },
		{ cosmetics::CheckCoopVisibility(), cosmetics::CoopDisclaimerText() } 
	};
}

void UCosmeticsDisclaimerWidget::OnCosmeticFilterOpened() {
	auto player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	auto gi = Cast<UDungeonsGameInstance>(GetGameInstance());

	if (auto message = Messages.FindByPredicate(RETLAMBDA(it.Pred({ player, gi })))) {
		SetVisibility(ESlateVisibility::HitTestInvisible);
		DisclaimerText->SetText(message->DisplayText());
	}
	else {
		SetVisibility(ESlateVisibility::Collapsed);
	}
}




