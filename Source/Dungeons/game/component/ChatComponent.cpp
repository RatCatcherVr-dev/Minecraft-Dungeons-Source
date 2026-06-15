// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ChatComponent.h"
#include <GameFramework/Controller.h>
#include "DungeonsGameState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/power/ItemPowerUtil.h"
#include <Runtime/Engine/Classes/GameFramework/PlayerState.h>
#include "game/inventory/InventoryItemUtil.h"
#include "game/emotes/EmoteDefs.h"
#include "online/sessions/OnlineUtil.h"

UChatComponent::UChatComponent() {	
	PrimaryComponentTick.bCanEverTick = false;	
}

void UChatComponent::BeginPlay() {
	Super::BeginPlay();
}

void UChatComponent::OnPawnPossessed() {
	auto character = Cast<ABasePlayerController>(GetOwner())->GetPawn();
	if (auto ItemStash = character->FindComponentByClass<UItemStashComponent>()) {
		ItemStash->OnItemPickupInternal.AddUObject(this, &UChatComponent::ItemPickedUp);
	}
}




void UChatComponent::ItemPickedUp(const FInventoryItemData& item) {
	if (auto controller = Cast<ABasePlayerController>(GetOwner())) {
		ServerSendPickupItem(controller, item);
	}
}






void UChatComponent::ServerSendChatMessage_Implementation(ABasePlayerController* player, const FText& message) {
	for (auto chat : GetChatComponents()) {
		chat->ClientSendChatMessage(player->GetPlayerId(), player->GetPlayerPrimaryDisplayName(), player->GetPlayerSecondaryDisplayName(), message);
	}
}

bool UChatComponent::ServerSendChatMessage_Validate(ABasePlayerController* player, const FText& message) {
	return true;
}



void UChatComponent::ServerSendEmote_Implementation(ABasePlayerController* player, EEmote emote) {
	for (auto chat : GetChatComponents()) {
		chat->ClientSendEmote(player->GetPlayerId(), emote);
	}
}

bool UChatComponent::ServerSendEmote_Validate(ABasePlayerController* player, EEmote emote) {
	return true;
}


void UChatComponent::ServerSendPickupItem_Implementation(ABasePlayerController* player, const FInventoryItemData& item) {
	for (auto chat : GetChatComponents()) {
		chat->ClientSendPickupItem(player->GetPlayerId(), player->GetPlayerPrimaryDisplayName(), player->GetPlayerSecondaryDisplayName(), item);
	}
}

bool UChatComponent::ServerSendPickupItem_Validate(ABasePlayerController* player, const FInventoryItemData& item) {
	return true;
}









#define LOCTEXT_NAMESPACE "Chat"
void UChatComponent::ClientSendPickupItem_Implementation(int32 playerId, const FString& senderPrimaryName, const FString& senderSecondaryName, const FInventoryItemData& item) {
	auto&& ItemNameAndPowerText = UInventoryItemUtil::GetDisplayItemNameAndPowerText(item);
	auto sender = online::shouldShowSecondaryName(senderSecondaryName) ? senderSecondaryName : senderPrimaryName;
	auto message = FText::Format(LOCTEXT("chat_pickupitem", "Picked up: {1}"), FText::FromString(sender), ItemNameAndPowerText);	
	OnChatItem.Broadcast(playerId, sender, item); 
}
#undef LOCTEXT_NAMESPACE

void UChatComponent::ClientSendChatMessage_Implementation(int32 playerId, const FString& senderPrimaryName, const FString& senderSecondaryName, const FText& message) {
	auto sender = online::shouldShowSecondaryName(senderSecondaryName) ? senderSecondaryName : senderPrimaryName;
	RecieveChatMessage(playerId, sender, message.ToString(), EChatMessageType::Text);
}

void UChatComponent::ClientSendEmote_Implementation(int32 playerId, EEmote emote) {
	if(auto emoteDef = emotes::getChecked(emote)){		
		RecieveChatEmote(playerId, emote);
		OnChatEmote.Broadcast(playerId, emote);
	}
}

#define LOCTEXT_NAMESPACE "PlayerStatus"
void UChatComponent::ClientSendPlayerStatus_Implementation(int32 playerId, const FString& sender, EPlayerStatusType playerStatus) {
	const FText message = [&](){
		switch (playerStatus) {
			case EPlayerStatusType::Joined :
				return LOCTEXT("playerstatus_joined", "Has joined the game");
			break;
			case EPlayerStatusType::Left :
				return LOCTEXT("playerstatus_left", "Has left the game");
			break;
			default:
				return LOCTEXT("playerstatus_unknown", "unknown");
			break;
		}
	}();

	RecieveChatMessage(playerId, sender, message.ToString(), EChatMessageType::Status);
}
#undef LOCTEXT_NAMESPACE









TArray<UChatComponent*> UChatComponent::GetChatComponents() const {
	TArray<UChatComponent*> out;
	for (auto&& player : TActorRange<ABasePlayerController>(GetWorld())) {
		if (auto comp = player->FindComponentByClass<UChatComponent>()) {
			out.Add(comp);
		}
	}
	return out;
}


int32 UChatComponent::GetPlayerId() const {
	if (const auto PlayerController = Cast<ABasePlayerController>(GetOwner())) {
		return PlayerController->GetPlayerId();
	}
	return -1;
}
