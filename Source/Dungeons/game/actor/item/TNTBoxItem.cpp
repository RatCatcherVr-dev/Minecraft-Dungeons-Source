#include "Dungeons.h"
#include "TNTBoxItem.h"
#include "AIController.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "game/util/Pushback.h"
#include "game/component/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "StatTracker.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ArrowItemSlot.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

UTNTBoxDamageGameplayEffect::UTNTBoxDamageGameplayEffect() {
	InheritableGameplayEffectTags.AddTag(damageTag::strongExplosion());
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Explosion.Strong"), 0, 1);
}

ATNTBoxItem::ATNTBoxItem(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bCanEverTick = true;
	EffectDamage = 200;
	EffectTime = 3.0f;
	EffectRadius = 900;
	SetReplicates(true);
	EffectTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Explosion.TNT"));
	SlotType = ESlotType::TNT;
	ThrowType = EThrowableType::TNT;
	mCanMove = true;
}

void ATNTBoxItem::Tick(float DeltaSeconds) {
	if (PendingActivatedUpdate) {
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OnActivated();
		PendingActivatedUpdate = false;
	}
}

void ATNTBoxItem::ThrowInMouseDir() {
	//D11.KS - use owning player controller instead.
	if (APlayerCharacter* owningPlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if (APlayerController* owningPlayerController = owningPlayerCharacter->GetPlayerController())
		{
			DisableInput(nullptr);

			const auto controller = Cast<ABasePlayerController>(owningPlayerController);
			const auto mouseWorldPos = GetMouseClickedWorldPosition(controller);
			auto dir = mouseWorldPos - GetActorLocation();
			dir.Z = 0;

			Throw(dir.SizeSquared() > 1e-6 ? dir.GetUnsafeNormal() : GetOwner()->GetActorForwardVector());
		}
	}
}

void ATNTBoxItem::Activate(const FVector& dir)
{
	Super::Activate(dir);

	auto impulse = dir;
	if (!IsFirstInStack()) {
		impulse = impulse.RotateAngleAxis(FMath::FRandRange(-30, 30), FVector(0, 0, 1)) * FMath::FRandRange(0.7, 1.3);
	}
	if (HasAuthority()) {
		auto collisionMesh = Cast<UBoxComponent>(GetRootComponent());
		collisionMesh->SetSimulatePhysics(true);
		collisionMesh->AddImpulse(impulse, NAME_None, true);
		GetWorld()->GetTimerManager().SetTimer(EffectTimerHandler, this, &ATNTBoxItem::Explode, EffectTime);
	}
}

uint16 ATNTBoxItem::DealDamageWithinRadius(const int radius, AActor* owner) {
	uint16 count = Super::DealDamageWithinRadius(radius, owner);

	// D11.SSN
	if (auto* playerCharacter = Cast<APlayerCharacter>(owner)) {
		playerCharacter->TNTKillCount = count;
		playerCharacter->OnRep_TNTKillCount();
	}

	return count;
}

FGameplayEffectSpec ATNTBoxItem::GetGameplayEffectSpec(UAbilitySystemComponent* AbilitySystemComponent, AActor* owner, const float scaledDamage)
{
	return effects::CreateGameplayEffectSpec<UTNTBoxDamageGameplayEffect>(AbilitySystemComponent, effects::HealthName, -scaledDamage, owner, this, owner->GetActorLocation(), 1.f);
}

void ATNTBoxItem::ThrowServer_Implementation(const FVector& dir)
{
	Activate(3500 * dir);
	if (Slot) Slot->Consume();
}
