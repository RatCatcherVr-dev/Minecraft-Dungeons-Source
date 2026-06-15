#include "PerfectFormComponent.h"

#include "Engine/Engine.h"
#include "Engine/Classes/GameFramework/Character.h"
#include "game/component/HealthComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "Net/UnrealNetwork.h"
#include "BehaviorOptionsComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/VoidTouched.h"

UPerfectFormComponent::UPerfectFormComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UPerfectFormComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	GetAnimInstance();

	// Set Health Thresholds. We do it here so design only need to change when the phase starts on the behavior options
	if (UBehaviorOptionsComponent* OwnerBehaviorOptions = GetOwner()->FindComponentByClass<UBehaviorOptionsComponent>()) {
		if (UHealthComponent* OwnerHealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			const float optPhase2HealthThreshold = OwnerBehaviorOptions->Get("phase2-health-threshold", 0.7f);
			const float optPhase3HealthThreshold = OwnerBehaviorOptions->Get("phase3-health-threshold", 0.3f);

			OwnerHealthComponent->SetHealthThresholds({ optPhase2HealthThreshold, optPhase3HealthThreshold });
		}
	}

	if (UAbilitySystemComponent* OwnerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>()) {
		auto spec = effects::CreateGameplayEffectSpec<UVoidStrikeImmunityGameplayEffect>(OwnerAbilitySystem, 1.f);
		OwnerAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UPerfectFormComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TickOnScreenDebugging(DeltaTime);
}

void UPerfectFormComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPerfectFormComponent, PerfectFormLocomotionBlend);
	DOREPLIFETIME(UPerfectFormComponent, laserBodyRotationRate);
}

void UPerfectFormComponent::SetAnimLocomotionState(EPerfectFormAnimLocomotion animLocomotion)
{
	if (!GetOwner()->HasAuthority())
		return;

	PerfectFormLocomotionBlend = animLocomotion;
	OnRep_AnimLocomotionState();
}

void UPerfectFormComponent::SetLaserBodyRotationRate(float inRate)
{
	if (!GetOwner()->HasAuthority())
		return;

	laserBodyRotationRate = inRate;
	OnRep_LaserBodyRotationRate();
}

void UPerfectFormComponent::SetEndermiteFeast(bool isActive)
{
	OnEndermiteFeastDelegate.Broadcast(isActive);
}

void UPerfectFormComponent::SetVoidPoolActive(bool isActive)
{
	OnVoidPoolActiveDelegate.Broadcast(isActive);
}

void UPerfectFormComponent::SetMirrorsActive(bool isActive, float activationChance)
{
	OnSetMirrorsActiveDelegate.Broadcast(isActive, activationChance);
}

bool UPerfectFormComponent::GetDebugState() const
{
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	return bDebuggingEnabled;
#endif
	return false;
}

void UPerfectFormComponent::OnRep_AnimLocomotionState()
{
	GetAnimInstance()->PerfectFormLocomotionBlend = PerfectFormLocomotionBlend;
}

void UPerfectFormComponent::OnRep_LaserBodyRotationRate()
{
	GetAnimInstance()->laserBodyRotationRate = laserBodyRotationRate;
}

void UPerfectFormComponent::TickOnScreenDebugging(float DeltaTime)
{
	if (!GetDebugState())
		return;

	int onScreenMessageCounter{ 0 };

	const auto cPhaseAsString = [&]() -> FString {
		switch (cPhase) {
		case CombatPhase::PhaseInit:				return "Phase Init";
		case CombatPhase::PhaseOne:					return "Phase One";
		case CombatPhase::PhaseTwo:					return "Phase Two";
		case CombatPhase::PhaseFinal:				return "Phase Final";
		default: checkNoEntry();					return "ERROR";
		}
	};

	const auto aTypeAsString = [&]() -> FString {
		switch (aType) {
		case AttackType::Charge:					return "Charge";
		case AttackType::BombSpew:					return "Bomb Spew";
		case AttackType::EndermiteScatter:			return "Endermite Scatter";
		case AttackType::EndermiteFeast:			return "Endermite Feast";
		case AttackType::LaserPhaser:				return "Laser Phaser";
		case AttackType::VoidPull:					return "Void Pull";
		case AttackType::NONE:						return "NONE";
		default: checkNoEntry();					return "ERROR";
		}
	};

	const auto tPhaseAsString = [&]() -> FString {
		switch (tPhase) {
		case TransitionPhase::Begin:				return "Begin";
		case TransitionPhase::Active:				return "Active";
		case TransitionPhase::Ending:				return "Ending";
		case TransitionPhase::NONE:					return "NONE";
		default: checkNoEntry();					return "ERROR";
		}
	};

	const auto lPhaseAsString = [&]() -> FString {
		switch (lPhase) {
		case LaserPhase::Begin:						return "Begin";
		case LaserPhase::Active:					return "Active";
		case LaserPhase::Ending:					return "Ending";
		case LaserPhase::NONE:						return "NONE";
		default: checkNoEntry();					return "ERROR";
		}
	};

	const auto debugMessage = [&](const FString& Message) {
		GEngine->AddOnScreenDebugMessage(static_cast<int32>(GetUniqueID() + onScreenMessageCounter), DeltaTime, FColor::White, Message);
		onScreenMessageCounter++;
	};
	
	const auto formatFString = [](const FString& Subject, const FString& Prefix = "", const FString& Suffix = "") -> FString {
		return FString::Printf(TEXT("%s [ %s ] %s"), *Prefix, *Subject, *Suffix);
	};
	
	debugMessage(formatFString(lPhaseAsString(), "Laser Phase"));
	debugMessage(formatFString(tPhaseAsString(), "Transition Phase"));
	debugMessage(formatFString(aTypeAsString(), "Attack Type"));
	debugMessage(formatFString(cPhaseAsString(), "Combat Phase"));
	debugMessage(formatFString(FString::SanitizeFloat(HealthComponent->GetCurrentHealthPercentage() * 100.f), "Health", "%"));
}

UPerfectFormAnimInstance* UPerfectFormComponent::GetAnimInstance()
{
	if (!Animinstance) {
		Animinstance = Cast<UPerfectFormAnimInstance>(Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance());
	}
	return Animinstance;
}

void UPerfectFormComponent::PrintOnce(const FString& Message, float Duration)
{
	if (GetDebugState()) {
		GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, Message);
	}
}

