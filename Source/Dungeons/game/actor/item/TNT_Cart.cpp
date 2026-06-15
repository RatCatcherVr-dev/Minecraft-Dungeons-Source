#include "UnrealNetwork.h"
#include "Dungeons.h"
#include "TNT_Cart.h"
#include "TNTBoxItem.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/util/Pushback.h"


extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

UTNT_Cart::UTNT_Cart()
{
	PrimaryComponentTick.bCanEverTick = true;
	ExplosionRadius = 900;
	ExplosionDamage = 200;
}


void UTNT_Cart::BeginPlay()
{
	Super::BeginPlay();
}

void UTNT_Cart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTNT_Cart, ExplosionDamage)
	DOREPLIFETIME(UTNT_Cart, ExplosionRadius)
}

void UTNT_Cart::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTNT_Cart::Explode(const ABaseCharacter* HitCharacter) {
	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), ExplosionRadius, 50, FColor::Green, false, 5.0f);
	}

	const auto owner = GetOwner();
	ExecuteExplosionGameplayCue(owner, HitCharacter);
	DealDamageWithinRadius(ExplosionRadius, owner);
}

void UTNT_Cart::ExecuteExplosionGameplayCue(const AActor* owner, const ABaseCharacter* HitCharacter) {
	if (auto abilitySystem = HitCharacter->FindComponentByClass<UAbilitySystemComponent>())
	{
		FGameplayCueParameters params;
		params.AbilityLevel = 1.f;
		params.NormalizedMagnitude = 1.f;
		params.Instigator = owner;
		params.EffectCauser = owner;
		params.Location = GetOwner()->GetActorLocation();

		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Explosion.TNT")), params);
	}
}

void UTNT_Cart::DealDamageWithinRadius(const int radius, AActor* owner) {
	const auto& tntLocation = GetOwner()->GetActorLocation();

	uint16 count = 0;
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
	for (auto actor : actorquery::getNearbyActors<ABaseCharacter>(GetOwner(), radius)) {
		if (const auto mob = Cast<AMobCharacter>(actor)) {
			if (mob->IsAlive()) {
				DealDamageToMob(mob, tntLocation, owner, ExplosionDamage);
				if (mob->IsNotAlive()) {
					count++;
				}
			}
		}
		else if (const auto player = Cast<APlayerCharacter>(actor)) {
			DealDamageToPlayer(player, owner, ExplosionDamage);
		}
	}
}

void UTNT_Cart::DealDamageToMob(const AMobCharacter* mob, const FVector& tntLocation, AActor* owner, const float scaledDamage) {
	if (mob->IsAlive()) {
		const auto push = CreatePushback(mob, tntLocation);
		pushback::pushback(push, *owner, *mob);

		if (const auto mobAbilitySystem = mob->GetAbilitySystemComponent())
		{
			auto mobDamageSpec = effects::CreateGameplayEffectSpec<UTNTBoxDamageGameplayEffect>(mobAbilitySystem, effects::HealthName, -scaledDamage, owner, owner, owner->GetActorLocation(), 1.f);
			//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
			effects::StorePushbackInNormal(mobDamageSpec, pushback::getLaunchVector(push, *GetOwner(), *mob, 1.5f, 1.0f));
			mobAbilitySystem->ApplyGameplayEffectSpecToSelf(mobDamageSpec);
		}
	}
}

FPushback UTNT_Cart::CreatePushback(const AActor* entity, const FVector& tntLocation) const {
	const auto launchMagnitude = CalculateLaunchMagnitude(entity, tntLocation);

	FPushback push;
	push.pushbackStrength = launchMagnitude * 15.0f;
	push.pushbackStrength = FMath::Max(push.pushbackStrength, 2.0f);
	push.pushbackZFactor = 2.0f;
	push.enablePushback = true;

	return push;
}

float UTNT_Cart::CalculateLaunchMagnitude(const AActor* entity, const FVector& tntLocation) {
	const auto distToExplosionCenter = entity->GetActorLocation() - tntLocation;
	auto launchMagnitude = distToExplosionCenter.Size() / 900.0f;
	launchMagnitude = 1.0f - launchMagnitude;
	return launchMagnitude;
}

void UTNT_Cart::DealDamageToPlayer(const APlayerCharacter* player, AActor* owner, const float scaledDamage) {
	if (player->IsAlive()) {
		if (const auto playerAbilitySystem = player->GetAbilitySystemComponent())
		{
			auto playerDamageSpec = effects::CreateGameplayEffectSpec<UTNTBoxDamageGameplayEffect>(playerAbilitySystem, effects::HealthName, -scaledDamage, owner, owner, owner->GetActorLocation(), 1.f);
			effects::StorePushbackInNormal(playerDamageSpec, pushback::getLaunchVector(CreatePushback(player, owner->GetActorLocation()), *owner, *player, 1.5f, 1.0f));
			playerAbilitySystem->ApplyGameplayEffectSpecToSelf(playerDamageSpec);
		}
	}
}