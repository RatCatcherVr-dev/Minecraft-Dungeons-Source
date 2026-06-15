// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "game/item/InventoryItemData.h"
#include "game/objective/EventTypes.h"
#include "character/player/BasePlayerState.h"
#include "character/player/BasePlayerController.h"
#include "NotificationBroadcastActor.generated.h"

UENUM(BlueprintType)
enum class ENotificationType : uint8 {
	Invalid,
	Event,
	PlayerJoin,
	SelfGearPickup,
	OtherGearPickup
};
ENUM_NAME(ENotificationType)

USTRUCT(BlueprintType)
struct DUNGEONS_API FNotification {
	GENERATED_BODY()

	FNotification();
	FNotification(ENotificationType type);
	
	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	ENotificationType Type;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEventNotification : public FNotification {
	GENERATED_BODY()

	FEventNotification();
	FEventNotification(EEventType eventType, FText title, FText message);

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	EEventType EventType;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FText EventTitle;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FText EventMessage;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FPlayerNotification : public FNotification {
	GENERATED_BODY()

	FPlayerNotification();
	FPlayerNotification(FName skinId, ABasePlayerState* playerState);

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FName SkinId;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	ABasePlayerState* PlayerState;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FSelfGearPickupNotification : public FNotification {
	GENERATED_BODY()

	FSelfGearPickupNotification();
	FSelfGearPickupNotification(FInventoryItemData itemData, ABasePlayerController* playerController);

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FInventoryItemData ItemData;

	//D11.KS - Extended functionality for tracking on local coop.
	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	ABasePlayerController* PlayerController;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FOtherGearPickupNotification : public FNotification {
	GENERATED_BODY()

	FOtherGearPickupNotification();
	FOtherGearPickupNotification(FText name, FInventoryItemData itemData);

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FText PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	FInventoryItemData ItemData;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventNotificationBroadcast, const FEventNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedNotificationBroadcast, const FPlayerNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeftNotificationBroadcast, const FPlayerNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelfGearPickupNotificationBroadcast, const FSelfGearPickupNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOtherGearPickupNotificationBroadcast, const FOtherGearPickupNotification&, Notification);

UCLASS(Blueprintable)
class DUNGEONS_API ANotificationBroadcastActor : public AInfo {
	GENERATED_BODY()
public:	
	ANotificationBroadcastActor();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;
	void BroadcastNotification(const FEventNotification& notification);
	void BroadcastNotification(const FPlayerNotification& notification, bool joined);
	void BroadcastNotification(const FSelfGearPickupNotification& notification);
	void BroadcastNotification(const FOtherGearPickupNotification& notification);

	void TriggerObjectiveUpdatedEffect(FVector location) const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerJoinedNotification(ABasePlayerState* playerState);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerLeftNotification(ABasePlayerState* playerState);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnEventNotificationBroadcast OnEventNotificationBroadcast;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerJoinedNotificationBroadcast OnPlayerJoinedNotificationBroadcast;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerLeftNotificationBroadcast OnPlayerLeftNotificationBroadcast;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSelfGearPickupNotificationBroadcast OnSelfGearPickupNotificationBroadcast;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnOtherGearPickupNotificationBroadcast OnOtherGearPickupNotificationBroadcast;

	UPROPERTY(EditDefaultsOnly, /*Blueprint*/ Category = "Dungeons")
	TSubclassOf<AActor> ObjectiveUpdatedEffect;

private:
	UFUNCTION()
	void OnItemPickup(int32 playerId, const FString& sender, const FInventoryItemData& data);

	void AddJoinedPlayerState(ABasePlayerState*);

	UPROPERTY()
	TArray<ABasePlayerState*> PendingJoinNotificationPlayerStates;
};

