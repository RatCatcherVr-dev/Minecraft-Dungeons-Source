#include "Dungeons.h"
#include "LeapComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "DungeonsGameInstance.h"

UGhostLeapGameplayEffect::UGhostLeapGameplayEffect() {
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.GhostLeap"));
	
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}

ULeapComponent::ULeapComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULeapComponent::HandleLanded() {
	if (!isLeaping) {
		return;
	}

	SetComponentTickEnabled(true);

	landTime = GetWorld()->GetTimeSeconds();
	isLeaping = false;

	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (auto montage = Cast<UAnimMontage>(landSequence)) {
		ownerCharacter->MulticastPlayMontage(montage);
	}
	else {
		ownerCharacter->MulticastPlayAnimationAsDynamicMontage(landSequence, FName(TEXT("FullBody")), 0.1f, 0.1f, 1.f, 1);
	}

	if (OnLanded.IsBound()) {
		OnLanded.Broadcast();
	}
}

void ULeapComponent::BeginPlay() {
	Super::BeginPlay();
	
	landTime = GetWorld()->GetTimeSeconds();
	isLeaping = false;
	Cast<ABaseCharacter>(GetOwner())->OnCharacterWalking.AddUObject(this, &ULeapComponent::HandleLanded);
}

void ULeapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetWorld()->GetTimeSeconds() - landTime > ghostDisableDelay) {
		SetComponentTickEnabled(false);

		if (ghostLeapHandle.IsValid()) {
			const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
			auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

			abilitySystem->RemoveActiveGameplayEffect(ghostLeapHandle);
		}
	}
}

bool ULeapComponent::CanLeap() {
	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	const auto movementComponent = ownerCharacter->FindComponentByClass<UCharacterMovementComponent>();

	return !movementComponent->IsFalling() && 
		GetWorld()->GetTimeSeconds() - landTime > ghostDisableDelay;
}

namespace {
	template <typename T>
	struct BeforeAndAfter { T before; T after; };

	struct YawAndAmount { float yaw; float amount; };
}

YawAndAmount computeLeap(const TArray<FLeapXyConfig>& entries, float distanceXy, float offsetZ) {
	const auto& entriesXy = [] (const auto& entries, auto value) -> BeforeAndAfter<FLeapXyConfig> {
		if (entries.Num() == 1 || value <= entries[0].distanceXy) {
			return { entries[0], entries[0] };
		}		

		const auto indexLast = entries.Num() - 1;
		if (value >= entries[indexLast].distanceXy) {
			return { entries[indexLast], entries[indexLast] };
		}

		auto i = 0;
		while (entries[i].distanceXy < value) {
			i++;
		}

		return { entries[i - 1], entries[i] };
	}(entries, distanceXy);
	
	const auto& getEntriesZ = [] (const auto& entries, float value) -> BeforeAndAfter<FLeapZConfig> {
		if (entries.Num() == 1 || value <= entries[0].offsetZ) {
			return { entries[0], entries[0] };
		}		

		const auto indexLast = entries.Num() - 1;
		if (value >= entries[indexLast].offsetZ) {
			return { entries[indexLast], entries[indexLast] };
		}

		auto i = 0;
		while (entries[i].offsetZ < value) {
			i++;
		}

		return { entries[i - 1], entries[i] };
	};

	const auto mix = [] (float inA, float inB, float value, float outA, float outB) {
		return inA == inB ? outA : FMath::Lerp(outA, outB, (inA - value) / (inA - inB));
	};

	const auto mixEntriesZ = [mix] (const auto& pair, float offsetZ) -> YawAndAmount {	
		const auto yaw = mix(
			pair.before.offsetZ, pair.after.offsetZ,
			offsetZ,
			pair.before.leapYaw, pair.after.leapYaw
		);

		const auto amount = mix(
			pair.before.offsetZ, pair.after.offsetZ,
			offsetZ,
			pair.before.leapAmount, pair.after.leapAmount
		);		

		return { yaw, amount };
	};

	if (entriesXy.before.distanceXy == entriesXy.after.distanceXy) {
		return mixEntriesZ(getEntriesZ(entriesXy.before.entries, offsetZ), offsetZ);
	}
	else {
		const auto mixedZBefore = mixEntriesZ(getEntriesZ(entriesXy.before.entries, offsetZ), offsetZ);
		const auto mixedZAfter = mixEntriesZ(getEntriesZ(entriesXy.after.entries, offsetZ), offsetZ);

		const auto yaw = mix(
			entriesXy.before.distanceXy, entriesXy.after.distanceXy,
			distanceXy,
			mixedZBefore.yaw, mixedZAfter.yaw
		);

		const auto amount = mix(
			entriesXy.before.distanceXy, entriesXy.after.distanceXy,
			distanceXy,
			mixedZBefore.amount, mixedZBefore.amount
		);

		return { yaw, amount };
	}	
}

void ULeapComponent::Leap(FVector target) {
	if (!CanLeap()) {
		return;
	}

	if (entries.Num() <= 0) {
		return;
	}

	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	
	auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
	ghostLeapHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UGhostLeapGameplayEffect>(abilitySystem));
	
	const auto ownerLocation = ownerCharacter->GetActorLocation();
	const auto ownerBottomZ = ownerLocation.Z - ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const auto leapParameters = computeLeap(
		entries,
		FVector::Dist2D(ownerLocation, target),
		target.Z - ownerBottomZ
	);

	FVector launchVector = [&] {
		FVector directionZ = (target - ownerLocation).GetSafeNormal2D();		
		directionZ.Z = 1.f;

		const FVector directionXy {
			FMath::Cos(FMath::DegreesToRadians(leapParameters.yaw)),
			FMath::Cos(FMath::DegreesToRadians(leapParameters.yaw)),
			FMath::Sin(FMath::DegreesToRadians(leapParameters.yaw))
		};

		return directionZ * directionXy * leapParameters.amount;
	}();

	// DrawDebugLine(GetWorld(), ownerLocation, ownerLocation + launchVector, FColor::White, false, 1.f);
	
	if (auto movementComponent = ownerCharacter->FindComponentByClass<UCharacterMovementComponent>()) {
		movementComponent->StopActiveMovement();
		movementComponent->Launch(launchVector);
		movementComponent->HandlePendingLaunch();
	}

	if (auto montage = Cast<UAnimMontage>(leapStartSequence)) {
		ownerCharacter->MulticastPlayMontage(montage);
	}
	else {
		ownerCharacter->MulticastPlayAnimationAsDynamicMontage(leapStartSequence, FName(TEXT("FullBody")), 0.1f, 0.1f, 1.f, 1);
	}

	isLeaping = true;
}

void ULeapComponent::Leap(ABaseCharacter* target, float forwardMultiplier, float offsetRadiusMax) {
	const auto targetLocation = target->GetActorLocation();	
	const auto targetVelocity = target->GetVelocity() * forwardMultiplier;

	// non-uniform is ok
	const auto radius = FMath::RandRange(0.f, offsetRadiusMax);
	const auto angle = FMath::RandRange(0.f, PI * 2.f);	

	const auto targetBottomZ = targetLocation.Z - target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	Leap(FVector {
		targetLocation.X + targetVelocity.X + FMath::Cos(angle) * radius,
		targetLocation.Y + targetVelocity.Y + FMath::Sin(angle) * radius,
		targetBottomZ
	});
}