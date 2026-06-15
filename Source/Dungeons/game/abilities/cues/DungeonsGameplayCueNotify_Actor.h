// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "DungeonsGameplayCueNotify_Actor.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API ADungeonsGameplayCueNotify_Actor : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	ADungeonsGameplayCueNotify_Actor();

	/** The socket to attach to */
	UPROPERTY(EditDefaultsOnly, Category = GameplayCue)
	FName Socket;

	UPROPERTY(EditDefaultsOnly, Category = GameplayCue)
	EAttachmentRule LocationRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditDefaultsOnly, Category = GameplayCue)
	EAttachmentRule RotationnRule = EAttachmentRule::SnapToTarget;
	UPROPERTY(EditDefaultsOnly, Category = GameplayCue)
	EAttachmentRule ScaleRule = EAttachmentRule::SnapToTarget;

	void SetOwner(AActor* NewOwner) override;
	void BeginPlay() override;

	void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
	
private:

	bool DeferredAttachment = false;

	void AttachToOwnerIfNecessaryWithSockets();
};
