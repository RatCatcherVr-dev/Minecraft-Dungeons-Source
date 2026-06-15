#include "ImpactActionHandler.h"
#include "item/BaseProjectile.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/ProjectileFunctionLibrary.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawner.h"
#include "util/CharacterQuery.h"
#include "game/Enchantments/ChainLightning.h"
#include "game/team/TeamQuery.h"
#include "game/abilities/effects/ExplosionImpactEffect.h"
#include "game/Conversion.h"


namespace impactaction {
	void ExplodeProjectile(UAbilitySystemComponent* instigator, FGameplayEffectSpec& spec, float radius, FGameplayTag explosionCue, AActor* originActor, const FPushback& push, int32* OUT_MobKillCount/* = nullptr*/) {
		if (!instigator->IsOwnerActorAuthoritative()) {
			return;
		}
			

		FGameplayCueParameters params;
		params.AbilityLevel = 1.f;
		params.NormalizedMagnitude = 1.f;
		params.Location = originActor->GetActorLocation();

		instigator->ExecuteGameplayCue(explosionCue, params);
		FGameplayTagContainer tags;
		spec.GetAllGrantedTags(tags);

		bool damageFriends = tags.HasTag(damageTag::damageFriends());
		const FDungeonsGameplayEffectContext* effects = effects::GetDungeonsContextFromSpec(spec);

		const auto canDamage = [&](const ABaseCharacter* v) {
			return damageFriends || teamquery::can::damage(v->GetCurrentTeam(), effects->InstigatorTeam);
		};

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);

		if (OUT_MobKillCount)
		{
			int32 iMobKillCount = 0;

			for (auto targetCharacter : actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(originActor, radius, canDamage)) {
				auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent();
				pushback::pushback(push, *originActor, *targetCharacter);
				spec.SetContext(spec.GetEffectContext().Duplicate());
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, *originActor, *targetCharacter, 1.5f, 1.0f));
				targetAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);

				//count num mobs killed
				if (const auto mob = Cast<AMobCharacter>(targetCharacter)) {
					if (mob->IsNotAlive()) {
						++iMobKillCount;
					}
				}
				
			}

			*OUT_MobKillCount = iMobKillCount;
		}
		else
		{
			for (auto targetCharacter : actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(originActor, radius, canDamage)) {
				auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent();
				pushback::pushback(push, *originActor, *targetCharacter);
				spec.SetContext(spec.GetEffectContext().Duplicate());
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, *originActor, *targetCharacter, 1.5f, 1.0f));
				targetAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}
		}
	}
	float GetItemPower(const ABaseProjectile * projectile, EEffectLevelSource source) {
		switch (source)
		{
		case EEffectLevelSource::SourceItem:
			return projectile->SourceItemPower.Get(projectile->WeaponItemPower);
		case EEffectLevelSource::RangedWeapon:
		default:
			return projectile->WeaponItemPower;
		}
	}
}

void UGameplayEffectImpactAction::OnLaunch(ABaseProjectile * projectile) {
	auto owner = projectile->GetInstigator();

	if (owner && owner->HasAuthority()) {
		UAbilitySystemComponent* ownerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owner);

		const float power = UsePowerSource ? impactaction::GetItemPower(projectile, PowerSource) : PowerLevel;

		Spec = effects::CreateGameplayEffectSpecFromSubClass(ownerAbilitySystem, EffectToApply, power);
		Spec->GetContext().AddInstigator(owner, projectile);
		OnGameplayEffectSpecCreated(Spec.GetValue(), projectile);
	}
}

void UGameplayEffectImpactAction::Reset() {
	Super::Reset();
	Spec.Reset();
}


void UGameplayEffectImpactAction::PostInit() {
	//Need to make sure these class types are rooted or the GC kicks off
	if (EffectToApply != nullptr) {
		if (!EffectToApply->IsRooted()) {
			EffectToApply->AddToRoot();
		}
	}
}

UExplosionAction::UExplosionAction() {
	Pushback.pushbackStrength = 4.0f;
	Pushback.pushbackZFactor = 1.0f;
	Pushback.enablePushback = true;
	EffectToApply = UExplosionImpactGameplayEffect::StaticClass();
}

void UExplosionAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impactInfo) {
	Super::OnImpact(projectile, impactInfo);

	auto owner = projectile->GetInstigator();
	UAbilitySystemComponent* ownerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owner);

	if (owner && owner->HasAuthority()) {
		impactaction::ExplodeProjectile(ownerAbilitySystem, Spec.GetValue(), ExplosionRadius, ExplosionCue, projectile, Pushback);
	}
}

void UExplosionAction::OnGameplayEffectSpecCreated(FGameplayEffectSpec& spec, ABaseProjectile* projectile) {
	spec.SetSetByCallerMagnitude(effects::HealthName, -ExplosionDamage * (bRespectProjectileDamageMultiplier ? projectile->GetDamageFactor() : 1.f));
	effects::SetStunMultiplier(spec, effects::PROJECTILE_EXPLOSION_STUN_MULTIPLIER);

	if (!projectile->IgnoreFriendlyFire) {
		spec.DynamicAssetTags.AddTag(damageTag::damageFriends());
	}
}

UDelayedExplosionAction::UDelayedExplosionAction() {
	Pushback.pushbackStrength = 4.0f;
	Pushback.pushbackZFactor = 1.0f;
	Pushback.enablePushback = true;
	EffectToApply = UExplosionImpactGameplayEffect::StaticClass();
	PowerSource = EEffectLevelSource::RangedWeapon;
}

void UDelayedExplosionAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impactInfo) {
	Super::OnImpact(projectile, impactInfo);

	if (auto instigator = projectile->GetInstigator()) {

		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(instigator);
		ABaseProjectileProp* pArrowProp = projectile->ProjectilePropClass ?
			(impactInfo.ImpactedActor ? UProjectileFunctionLibrary::SpawnAttachedArrowProp(projectile, impactInfo.ImpactedActor, projectile->ProjectilePropClass) : UProjectileFunctionLibrary::SpawnProjectileProp(projectile, projectile->ProjectilePropClass, projectile->GetTransform()))
			: nullptr;

		if (pArrowProp) {
			pArrowProp->InitialiseProp(false, ExplosionDelaySeconds);
			pArrowProp->DelayedExplosion(ExplosionDelaySeconds, ExplosionRadius, abilitySystem, Spec, Pushback, ExplosionCue);
		}
	}
}

void UDelayedExplosionAction::OnGameplayEffectSpecCreated(FGameplayEffectSpec& spec, ABaseProjectile* projectile) {
	spec.SetSetByCallerMagnitude(effects::HealthName, -ExplosionDamage);
	effects::SetStunMultiplier(spec, effects::PROJECTILE_EXPLOSION_STUN_MULTIPLIER);
}


void UMobSpawnAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impactInfo) {
	Super::OnImpact(projectile, impactInfo);

	if (projectile->GetInstigatorHadAuthority() && impactInfo.bFinalImpact) {
		const FTransform transform{ projectile->GetActorLocation() - projectile->GetActorForwardVector() * 50.f - FVector { 0.f, 0.f, 100.f } };

		game::mobspawn::spawnNow(
			*projectile->GetWorld(),
			EntityTypeFromString(std::string(TCHAR_TO_UTF8(*MobType))),
			RETLAMBDA0(transform),
			game::mobspawn::configs::DefaultNoVariants(true)
		);
	}
}

UChainLightningAction::UChainLightningAction() {
	TriggerType = EImpactTriggerType::Always;
	EffectToApply = URangedChainLightningDamageGameplayEffect::StaticClass();
}

void UChainLightningAction::OnGameplayEffectSpecCreated(FGameplayEffectSpec& spec, ABaseProjectile* projectile) {
	spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
	auto owner = projectile->GetInstigator();
	effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
}


void UChainLightningAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impact) {
	Super::OnImpact(projectile, impact);

	if (projectile->GetInstigatorHadAuthority()) {
		auto owner = projectile->GetInstigator();
		bool foundOpenTarget = impact.ImpactedActor ? true : false;
		const auto canTargetPredicate = [&](const ABaseCharacter* v) { 
			if (!foundOpenTarget) {
				FHitResult res;
				foundOpenTarget = !owner->GetWorld()->LineTraceSingleByChannel(res, impact.ImpactLocation, v->GetActorLocation(), (ECollisionChannel)ECustomTraceChannels::TerrainOnly);
			}
			return foundOpenTarget && characterquery::is::targetable(v) && actorquery::is::alive(v) && (!impact.bFinalImpact || actorquery::getActorDistance(v, projectile) > FinalImpactExclusionRadius); 
		};

		TArray<ABaseCharacter*> targets = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(projectile->GetWorld(), projectile->GetActorLocation(), ChainRadius, RETLAMBDA(it != impact.ImpactedActor && canTargetPredicate(it)));

		if (!impact.bFinalImpact) {
			if (auto* target = Cast<ABaseCharacter>(impact.ImpactedActor)) {
				if (canTargetPredicate(target)) {
					targets.Insert(target, 0);
				}
			}
		}

		TArray<TWeakObjectPtr<ABaseCharacter>> allTargets = algo::map_tarray(targets, [](ABaseCharacter* c) { return TWeakObjectPtr<ABaseCharacter>(c); });
		chainlightning::ApplyChainLightning(projectile, allTargets, Delay, ChainRadius, Spec.GetValue());
	}
}

void USpawnActorAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impact) {
	Super::OnImpact(projectile, impact);

	const bool instigatorHadAuthority = projectile->GetInstigatorHadAuthority();
	bool shouldSpawn = instigatorHadAuthority || bSpawnLocally;

	if(!bSpawnLocally && instigatorHadAuthority && SpawnChance < 1.f) {
		shouldSpawn = FMath::FRand() < SpawnChance;
	}
	
	if (shouldSpawn) {
		auto world = projectile->GetWorld();
		FTransform transform = Snap(TransformGenerator.IsBound() ? TransformGenerator.Execute(projectile, impact) : GetDefaultTransform(projectile, impact));

		if (auto actor = world->SpawnActorDeferred<AActor>(ActorClass, transform, projectile->GetInstigator(), projectile->GetInstigator())) {
			OnActorSpawned.Broadcast(actor, impact);
			actor->FinishSpawning(transform);
		}
	}
}

FTransform USpawnActorAction::GetDefaultTransform(ABaseProjectile* projectile, const FImpactInfo& info) const {
	switch (DefaultTransformMode) {
	//Intentional fallthrough here
	case ESpawnActorDefaultTransform::BelowCharacter:
	{
		if(const auto* character = Cast<ABaseCharacter>(info.ImpactedActor)) {
			FVector location = character->GetActorLocation();
			location.Z -= character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			return FTransform(location);
		}
	}
	case ESpawnActorDefaultTransform::ImpactLocation:
		return FTransform(info.ImpactLocation);
	default:
		return projectile->GetActorTransform();
	}
}

FTransform USpawnActorAction::Snap(const FTransform& transform) const {
	switch(SnappingMode) {
	case ESpawnActorSnappingMode::SnapXY:
	{
		FVector snapped = conversion::blockCenterXYZToUe(conversion::ueToBlock(transform.GetTranslation()));
		snapped.Z = transform.GetTranslation().Z;
		return FTransform(transform.GetRotation(), snapped, transform.GetScale3D());
	}
	case ESpawnActorSnappingMode::Snap:
	{
		FVector snapped = conversion::blockCenterXYZToUe(conversion::ueToBlock(transform.GetTranslation()));
		return FTransform(transform.GetRotation(), snapped, transform.GetScale3D());
	}
	default:
		return transform;
	}
}

UEffectToRandomTargetsAction::UEffectToRandomTargetsAction() {
	TriggerType = EImpactTriggerType::Always;
}

void UEffectToRandomTargetsAction::OnImpact(ABaseProjectile* projectile, const FImpactInfo& impactInfo) {
	Super::OnImpact(projectile, impactInfo);

	if (projectile->GetInstigatorHadAuthority()) {
		if (auto instigatorCharacter = Cast<ABaseCharacter>(projectile->GetInstigator())) {
			ApplyEffectToRandomTargets(projectile, instigatorCharacter, impactInfo);
		}
	}
}


void UEffectToRandomTargetsAction::OnGameplayEffectSpecCreated(FGameplayEffectSpec& spec, ABaseProjectile* projectile)
{
	if (Period > 0) {
		Spec.GetValue().Period = Period;
	}

	for (auto It = SetByCallerTagMagnitudes.CreateConstIterator(); It; ++It)
	{
		Spec.GetValue().SetSetByCallerMagnitude(It.Key(), It.Value());
	}
}

void UEffectToRandomTargetsAction::ApplyEffectToRandomTargets(ABaseProjectile* source, ABaseCharacter* instigator, const FImpactInfo& impactInfo) const {
	TArray<ABaseCharacter*> Targets;

	if (bAlwaysApplyToImpactedTarget && impactInfo.ImpactedActor->IsA<ABaseCharacter>()) {
		Targets.Add(Cast<ABaseCharacter>(impactInfo.ImpactedActor));
	}

	auto targetsToApply = FMath::RandRange(MinTargets, MaxTargets);
	if (targetsToApply > 0) {
		const auto shouldApply = [&](const ABaseCharacter* c) {
			return teamquery::can::damage(c->GetCurrentTeam(), effects::GetDungeonsContextFromSpec(Spec.GetValue())->InstigatorTeam) && actorquery::is::alive(c) && characterquery::is::targetable(c);
		};

		auto additionalTargets = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(Targets.Num() > 0 ? Cast<AActor>(Targets.Last()) : source, SearchRadius, shouldApply);
		for (; targetsToApply > 0 && additionalTargets.Num() > 0; --targetsToApply) {
			if (additionalTargets.Num() > 1) {
				additionalTargets.Swap(FMath::RandRange(0, additionalTargets.Num() - 2), additionalTargets.Num() - 1);
			}

			Targets.Add(additionalTargets.Pop());
		}
	}
	Spec->GetContext().AddOrigin(source->GetActorLocation());

	for (auto target : Targets) {
		target->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(Spec.GetValue());
	}
}

UImpactActionHandler::UImpactActionHandler() {
}

void UImpactActionHandler::OnLaunch(ABaseProjectile* projectile) {
	QueuedActions.Empty();
	QueuedActions.Append(DefaultImpactActions);
	QueuedActions.Append(RuntimeImpactActions);

	for (auto* action : QueuedActions) {
		action->OnLaunch(projectile);
	}

	bHasLaunched = true;
}

void UImpactActionHandler::OnImpact(ABaseProjectile* projectile, FImpactInfo info) {
	for (auto* action : QueuedActions) {
		if (info.bFinalImpact || action->TriggerType == EImpactTriggerType::Always) {
			action->OnImpact(projectile, info);
		}
	}
}

void UImpactActionHandler::Reset()
{
	ImpactInfo.Reset();
	QueuedActions.Reset();
	RuntimeImpactActions.Reset();

	for (auto ImpactHandler : DefaultImpactActions)
	{
		ImpactHandler->Reset();
	}

	bHasLaunched = true;
}

void UImpactActionHandler::PostInit() {
	for (auto ImpactHandler : DefaultImpactActions) {
		if(ImpactHandler) ImpactHandler->PostInit();
	}

	for (auto ImpactHandler : RuntimeImpactActions) {
		if (ImpactHandler) ImpactHandler->PostInit();
	}
}

bool UImpactActionHandler::HasActions() const {
	return DefaultImpactActions.Num() > 0 || RuntimeImpactActions.Num() > 0;
}

TArray<UImpactAction*> UImpactActionHandler::GetActions() const
{
	TArray<UImpactAction*> actions;

	actions.Append(DefaultImpactActions);
	actions.Append(RuntimeImpactActions);

	return actions;
}

void UImpactActionHandler::AddRuntimeAction(UImpactAction* action) {
	RuntimeImpactActions.Add(action);

	if(bHasLaunched) {
		action->OnLaunch(Cast<ABaseProjectile>(GetOuter()));
		QueuedActions.Add(action);
	}
}

void UImpactActionHandler::RemoveRuntimeAction(UImpactAction* action) {
	RuntimeImpactActions.Remove(action);
}

TArray<UImpactAction*>  UImpactActionHandler::GetRuntimeActions(TSubclassOf<UImpactAction> cls) const {
	return algo::copy_if(RuntimeImpactActions, RETLAMBDA(it->IsA(cls)));
}

bool UImpactActionHandler::HasAction(TSubclassOf< UImpactAction> cls) const {
	auto pred = RETLAMBDA(it->IsA(cls));
	return DefaultImpactActions.ContainsByPredicate(pred) || RuntimeImpactActions.ContainsByPredicate(pred);
}

TArray<const UImpactAction*> UImpactActionHandler::GetActions(TSubclassOf<UImpactAction> cls) const {
	return algo::copy_if_map_tarray(GetActions(), RETLAMBDA(it->IsA(cls)), RETLAMBDA(Cast<const UImpactAction>(it)));
}

