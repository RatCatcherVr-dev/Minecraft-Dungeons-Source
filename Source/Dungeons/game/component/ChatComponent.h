// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/ItemStashComponent.h"
#include "game/emotes/Emotes.h"
#include "ChatComponent.generated.h"


UENUM(BlueprintType)
enum class EChatMessageType : uint8 {
	UNSET,
	Text,
	Icon,
	Item,
	Status,
};
ENUM_NAME(EChatMessageType);


DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatItem, int32, const FString&, const FInventoryItemData&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChatEmote, int32, EEmote);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UChatComponent();

	void BeginPlay() override;

	void OnPawnPossessed();

	UFUNCTION()
	void ItemPickedUp(const FInventoryItemData& item);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Dungeons")
	void ServerSendPickupItem(class ABasePlayerController* player, const FInventoryItemData& item);

	FOnChatEmote OnChatEmote;	
	FOnChatItem OnChatItem;

	//Blueprint interface
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void RecieveChatMessage(int32 playerId, const FString& sender, const FString& message, EChatMessageType messageType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void RecieveChatEmote(int32 playerId, EEmote emote);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Dungeons")
	void ServerSendChatMessage(class ABasePlayerController* player, const FText& message);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Dungeons")
	void ServerSendEmote(class ABasePlayerController* player, EEmote emote);
	
	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientSendPlayerStatus(int32 playerId, const FString& sender, EPlayerStatusType playerStatus);

private:

	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientSendChatMessage(int32 playerId, const FString& senderPrimaryName, const FString& senderSecondaryName, const FText& message);

	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientSendEmote(int32 playerId, EEmote emote);

	UFUNCTION(Client, Reliable, Category = "Dungeons")
	void ClientSendPickupItem(int32 playerId, const FString& senderPrimaryName, const FString& senderSecondaryName, const FInventoryItemData& item);

	TArray<UChatComponent*> GetChatComponents() const;
	
	int32 GetPlayerId() const;
};
