#include "Dungeons.h"
#include "SuddenDeath.h"
#include "Dimmer.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "Object.h"


ASuddenDeath::ASuddenDeath(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; 
	PrimaryActorTick.TickInterval = .25f;
}

void ASuddenDeath::BeginPlay() {
	Super::BeginPlay();			
	mDimmer = actorquery::getFirstActor<ADimmer>(GetWorld());
}

FActiveGameplayEffectHandle ASuddenDeath::ApplyEffect(APlayerCharacter* player, float damage) {
	UAbilitySystemComponent* abilitySystem = player->GetAbilitySystemComponent();

	AGameBP* Game = actorquery::getFirstActor<AGameBP>(GetWorld());
	ENightModeType NightMode = ENightModeType::Normal;
	bool bDoAOEAttack = false;

	if (Game)
	{
		auto mission = missions::get(Game->GetGame()->settings().levelName);
		NightMode = mission.GetNightModeType();
	}

	FGameplayEffectSpec spec;
	switch (NightMode)
	{
	case ENightModeType::Normal:
		spec = effects::CreateGameplayEffectSpec<USuddenDeathDamageGameplayEffect>(abilitySystem, 1);
		break;
	case ENightModeType::Oceans:
		spec = effects::CreateGameplayEffectSpec<USuddenDeathOceansDamageGameplayEffect>(abilitySystem, 1);
		bDoAOEAttack = true;
		break;

	}
	
	spec.SetSetByCallerMagnitude(effects::HealthName, -damage);

	if (bDoAOEAttack) {
		ApplyEffectWithinRadius(player, 350.f, spec);
	}

	return abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void ASuddenDeath::SetMode(ESuddenDeathMode mode) {
	Mode = mode;
}

void ASuddenDeath::ExitSuddenDeath() {
	auto world = GetWorld();
	suddenDeathModeActive = false;
	if (HasAuthority()) {
		suddenDeathDamageApplyDelay = 0;

		for (auto player : InstanceTracker<APlayerCharacter>::GetList(world)) {
			player->GetAbilitySystemComponent()->RemoveActiveGameplayEffectBySourceEffect(
				USuddenDeathDamageGameplayEffect::StaticClass(),
				player->GetAbilitySystemComponent()
			);
		}
	}
	OnSuddenDeathEnded();
}

void ASuddenDeath::ExitNight() {
	nightModeActive = false;
	OnSuddenDeathCountStopped();
}

bool ASuddenDeath::ShouldEnterNightMode() const {
	switch (Mode)
	{
	case ESuddenDeathMode::Always:
		return true;
	case ESuddenDeathMode::OnAnyPlayerDowned:
		return algo::any_of(InstanceTracker<APlayerCharacter>::GetList(GetWorld()), [](const auto& p) { return !p->IsAlive(); });
	default:
		break;
	}

	return false;
}

void ASuddenDeath::ApplyEffectWithinRadius(AActor* center, float radius, const FGameplayEffectSpec spec)
{
	const auto& centerLocation = center->GetActorLocation();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
	for (auto actor : actorquery::getNearbyActors<ABaseCharacter>(center, radius, [center](const AActor* baseCharacter) { return baseCharacter != center; })) {
		if (const auto player = Cast<APlayerCharacter>(actor)) {
			UAbilitySystemComponent* abilitySystem = player->GetAbilitySystemComponent();
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

void ASuddenDeath::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	const auto world = GetWorld();

	if (!mDimmer.IsValid())
	{
		mDimmer = actorquery::getFirstActor<ADimmer>(world);
	}

	//Exit suddent death if the dimmer is not Night.
	if (mDimmer == nullptr || !mDimmer->IsNight() || mDimmer->IsBecomingDay()) {
		if (nightModeActive) {
			ExitNight();
		}

		if(suddenDeathModeActive){
			ExitSuddenDeath();
		}

		return;
	}
	
	//Consider entering Sudden Death night mode (i.e countdown towards sudden death
	bool shouldActivateNightMode = ShouldEnterNightMode();

	//We should enter night mode and start the countdown towards sudden death
	if (shouldActivateNightMode && !nightModeActive) {
		nightModeActive = true;
		nightModeStartStamp = world->GetTimeSeconds();
		OnSuddenDeathCountStarted();
	}
	//We are counting down but should no longer be doing so, exit countdown
	else if (nightModeActive && !shouldActivateNightMode) {
		ExitNight();
		//If we were actively hurting players in sudden death mode, we should leave that also.
		if (suddenDeathModeActive) {
			ExitSuddenDeath();
		}
	}

	//Countdown has completed, start hurting people!
	if (nightModeActive && GetTimeSecondsUntilSuddenDeath() <= 0.0f && !suddenDeathInterrupted) {
		if (!suddenDeathModeActive) {
			suddenDeathModeActive = true;
			OnSuddenDeathStarted();
		}

		if (HasAuthority()) {
			if (suddenDeathDamageApplyDelay <= 0.0f) {

				const float damageTotalDuration = getTimeSpentInSuddenDeathMode();
				const float startEndFraction = FMath::Clamp(damageTotalDuration / startToEndDuration, 0.0f, 1.0f);

				const auto playersAlive = actorquery::getActors<APlayerCharacter>(world)
					.FilterByPredicate([](APlayerCharacter* player) {
					return player->IsAlive();
				});

				const float damageFraction = FMath::Lerp(damageFractionStart, damageFractionEnd, startEndFraction);
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				for (auto player : playersAlive) {
					if (auto targetHealthComponent = player->FindComponentByClass<UHealthComponent>()) {
						const float damage = targetHealthComponent->GetMaximumHealth() * damageFraction;
						ApplyEffect(player, damage);
					}
				}

				suddenDeathDamageApplyDelay = FMath::Lerp(damagePeriodStart, damagePeriodEnd, startEndFraction);
			} else {
				suddenDeathDamageApplyDelay -= deltaTime;
			}
		}
	}
}

void ASuddenDeath::OnSuddenDeathCountStarted()
{
	if (SuddenDeathCountStarted.IsBound())
	{
		SuddenDeathCountStarted.Broadcast();
	}
}

void ASuddenDeath::OnSuddenDeathCountStopped()
{
	if (SuddenDeathCountStopped.IsBound())
	{
		SuddenDeathCountStopped.Broadcast();
	}
}

void ASuddenDeath::OnSuddenDeathEnded()
{
	if (SuddenDeathEnded.IsBound())
	{
		SuddenDeathEnded.Broadcast();
	}
}

void ASuddenDeath::OnSuddenDeathStarted()
{
	if (SuddenDeathStarted.IsBound())
	{
		SuddenDeathStarted.Broadcast();
	}
}

bool ASuddenDeath::IsSuddenDeathCountingDown() const {
	return nightModeActive;
}

float ASuddenDeath::GetTimeSecondsUntilSuddenDeath() const {
	if(IsSuddenDeathCountingDown()){
		if (suddenDeathInterrupted)
		{
			return timeLimit;
		}
		return FMath::Max(0.0f, timeLimit - (GetWorld()->GetTimeSeconds() - nightModeStartStamp));
	} else {
		return 0.0f;
	}
}


bool ASuddenDeath::IsSuddenDeathActive() const {
	return suddenDeathModeActive;
}

void ASuddenDeath::ToggleSuddenDeathInterruption()
{
	suddenDeathInterrupted = !suddenDeathInterrupted;
	nightModeStartStamp = GetWorld()->GetTimeSeconds();
}

float ASuddenDeath::getTimeSpentInSuddenDeathMode() const {
	if(IsSuddenDeathActive()){
		const float nightTimeTotal = GetWorld()->GetTimeSeconds() - nightModeStartStamp;
		return nightTimeTotal - timeLimit;
	} else {
		return 0.0f;
	}
}

USuddenDeathDamageGameplayEffect::USuddenDeathDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	DurationMagnitude = FScalableFloat(1.95f);
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = false;
	Period = 1.f;

	FCustomCalculationBasedFloat damageMagnitude;
	damageMagnitude.CalculationClassMagnitude = UDamageModCalculation::StaticClass();
	damageMagnitude.Coefficient = Period.GetValueAtLevel(1);

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	damageInfo.ModifierMagnitude = damageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.NightDamage"), 0, 1);	

	const auto burningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.NightDamage");
	InheritableOwnedTagsContainer.AddTag(burningEffectTag);
	InheritableGameplayEffectTags.AddTag(burningEffectTag);
}

USuddenDeathOceansDamageGameplayEffect::USuddenDeathOceansDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	DurationMagnitude = FScalableFloat(0.21f);
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = false;
	Period = 0.2f;

	FCustomCalculationBasedFloat damageMagnitude;
	damageMagnitude.CalculationClassMagnitude = UDamageModCalculation::StaticClass();

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	damageInfo.ModifierMagnitude = damageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.NightDamage.Oceans"), 0, 1);

	const auto burningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.NightDamage.Oceans");
	InheritableOwnedTagsContainer.AddTag(burningEffectTag);
	InheritableGameplayEffectTags.AddTag(burningEffectTag);
}
