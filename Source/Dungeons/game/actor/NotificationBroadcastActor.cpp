#include "NotificationBroadcastActor.h"
#include "game/item/ItemTypeDefs.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/ObjectiveInterestPoint.h"
#include "game/util/PlayerQuery.h"
#include "game/component/ChatComponent.h"
#include "Engine/LocalPlayer.h"


namespace notificationBroadcast {
	TOptional<FName> findSkinId(ABasePlayerState* playerState) {
		for (auto character : actorquery::getActors<APlayerCharacter>(playerState->GetWorld())) {
			if (playerState == character->GetDungeonsBasePlayerState()) {
				return character->GetSkinId();
			}
		}
		return {};
	}

	bool hasJoinInfo(ABasePlayerState* playerState)	{
		return playerState && playerState->IsDisplayNameAssigned() && findSkinId(playerState).IsSet();
	}
}

ANotificationBroadcastActor::ANotificationBroadcastActor() {
	bReplicates = true;
	bNetLoadOnClient = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 0.1f;
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void ANotificationBroadcastActor::BeginPlay() {
	Super::BeginPlay();

	for (auto player : actorquery::getActors<ABasePlayerController>(GetWorld())) {

		if (!player->GetLocalPlayer() || player->IsOwnedByInitialLocalPlayer())
		{
			if (auto chatComponent = player->FindComponentByClass<UChatComponent>()) {
				chatComponent->OnChatItem.AddUObject(this, &ANotificationBroadcastActor::OnItemPickup);
			}
		}
	}
}

void ANotificationBroadcastActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	PendingJoinNotificationPlayerStates.RemoveAll([](ABasePlayerState* playerState) {
		return playerState == nullptr;
	});

	for (auto playerState : PendingJoinNotificationPlayerStates) {
		if (notificationBroadcast::hasJoinInfo(playerState)) {
			BroadcastNotification(FPlayerNotification(notificationBroadcast::findSkinId(playerState).Get(NAME_None), playerState), true);
			PendingJoinNotificationPlayerStates.Remove(playerState);
			break;
		}
	}

	if (PendingJoinNotificationPlayerStates.Num() == 0) {
		SetActorTickEnabled(false);
	}
}

void ANotificationBroadcastActor::BroadcastNotification(const FEventNotification& notification) {
	OnEventNotificationBroadcast.Broadcast(notification);
}

void ANotificationBroadcastActor::BroadcastNotification(const FPlayerNotification& notification, bool joined) {
	if (joined) {
		OnPlayerJoinedNotificationBroadcast.Broadcast(notification);
	}
	else {
		OnPlayerLeftNotificationBroadcast.Broadcast(notification);
	}
}

void ANotificationBroadcastActor::BroadcastNotification(const FSelfGearPickupNotification& notification) {
	const auto& itemData = GetItemRegistry().Get(notification.ItemData.GetItemId());
	if (!itemData.isInstant() || itemData.hasTag(ItemTag::UIFriendly)) {
		OnSelfGearPickupNotificationBroadcast.Broadcast(notification);
	}
}

void ANotificationBroadcastActor::BroadcastNotification(const FOtherGearPickupNotification& notification) {
	const auto& itemData = GetItemRegistry().Get(notification.ItemData.GetItemId());
	if (!itemData.isInstant() || itemData.hasTag(ItemTag::UIFriendly)) {
		OnOtherGearPickupNotificationBroadcast.Broadcast(notification);
	}
}

void ANotificationBroadcastActor::TriggerObjectiveUpdatedEffect(FVector location) const {
	ensure(ObjectiveUpdatedEffect);
	GetWorld()->SpawnActor<AObjectiveInterestPoint>(ObjectiveUpdatedEffect, location, FRotator::ZeroRotator);
}

void ANotificationBroadcastActor::OnItemPickup(int32 playerId, const FString& sender, const FInventoryItemData& data) {
	TArray<ABasePlayerController*> localPlayerControllers;
	for (auto localPlayer : GetGameInstance()->GetLocalPlayers()) {
		localPlayerControllers.Add(Cast<ABasePlayerController>(localPlayer->GetPlayerController(GetWorld())));
	}

	if (auto localPlayerPtr = localPlayerControllers.FindByPredicate([playerId](const ABasePlayerController* controller) { return controller->GetPlayerId() == playerId; })) {
		if (auto localPlayer = *localPlayerPtr) {
			BroadcastNotification(FSelfGearPickupNotification(data, localPlayer));
		}
	}
	else {
		BroadcastNotification(FOtherGearPickupNotification(FText::FromString(sender), data));
	}
}

void ANotificationBroadcastActor::MulticastPlayerJoinedNotification_Implementation(ABasePlayerState* playerState) {
	if (!playerState) {
		UE_LOG(LogDungeons, Error, TEXT("Tried to show join notification for a null playerState"));
	}
	else if (auto pc = playerquery::getFirstLocalPlayerController(GetWorld())) {
		if (playerState != pc->PlayerState) {
			AddJoinedPlayerState(playerState);
		}
	}
}

void ANotificationBroadcastActor::MulticastPlayerLeftNotification_Implementation(ABasePlayerState* playerState) {
	if (playerState) {
		BroadcastNotification(FPlayerNotification(notificationBroadcast::findSkinId(playerState).Get(NAME_None), playerState), false);
	}
}

void ANotificationBroadcastActor::AddJoinedPlayerState(ABasePlayerState* playerState) {
	if (notificationBroadcast::hasJoinInfo(playerState)) {
		BroadcastNotification(FPlayerNotification(notificationBroadcast::findSkinId(playerState).Get(NAME_None), playerState), true);
	}
	else {
		PendingJoinNotificationPlayerStates.Emplace(playerState);
		SetActorTickEnabled(true);
	}
}

FNotification::FNotification() : Type(ENotificationType::Invalid) {
}

FNotification::FNotification(ENotificationType type) 
	: Type(type) {
}

FEventNotification::FEventNotification(EEventType eventType, FText title, FText message)
	: FNotification(ENotificationType::Event), 
	  EventType(eventType),
	  EventTitle(std::move(title)), 
	  EventMessage(std::move(message)) {
}

FEventNotification::FEventNotification()
	: FNotification(ENotificationType::Event)
	, EventType(EEventType::None) {
}

FPlayerNotification::FPlayerNotification(FName skinId, ABasePlayerState* playerState)
	: FNotification(ENotificationType::PlayerJoin), 
	  SkinId(skinId),
	  PlayerState(playerState) {
}

FPlayerNotification::FPlayerNotification()
	: FNotification(ENotificationType::PlayerJoin)
	, SkinId(NAME_None) {
}

FSelfGearPickupNotification::FSelfGearPickupNotification(FInventoryItemData itemData, ABasePlayerController* playerController) 
	: FNotification(ENotificationType::SelfGearPickup), 
	  ItemData(std::move(itemData)),
	  PlayerController(playerController){
}

FSelfGearPickupNotification::FSelfGearPickupNotification() 
	: FNotification(ENotificationType::SelfGearPickup) {
}

FOtherGearPickupNotification::FOtherGearPickupNotification(FText name, FInventoryItemData itemData)
	: FNotification(ENotificationType::OtherGearPickup), 
	PlayerName(std::move(name)),
	ItemData(std::move(itemData)) {
}

FOtherGearPickupNotification::FOtherGearPickupNotification() 
	: FNotification(ENotificationType::OtherGearPickup) {
}
