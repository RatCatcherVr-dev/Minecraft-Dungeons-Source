// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "Dungeons.h"
#include "SpinWheelInstance.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/item/power/ItemPowerEffectDefs.h"

USpinWheelGameplayEffect::USpinWheelGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

FVectorForce::FVectorForce(FVector force, FVectorSpeedInfo speedInfo)
	: Vector(std::move(force))
	, SpeedInfo(std::move(speedInfo)) {
}

void FVectorForce::Update(float DeltaTime) {
	auto vectorSize = Vector.Size();
	auto acc = SpeedInfo.Acceleration * DeltaTime;
	vectorSize = FMath::Clamp(vectorSize + acc, 0.f, SpeedInfo.TargetSize);

	Vector.Normalize();
	Vector *= vectorSize;
}

void FVectorForce::SetDirectionKeepSize(const FVector& dir) {
	auto size = FMath::Max(1.f, Vector.Size());
	Vector = dir;
	Vector *= size;
}

WheelState::WheelState(std::function<void(WheelState&)> startConfiguration, std::function<bool(WheelState&)> completeCondition, std::function<void(const FVector&)> moveVectorSetter, ASpinWheelActor* wheel, TArray<ForceWithDirection> forces, EWheelStateType state, EWheelStateType nextState, TMap<FString, float> floatOptions, TMap<FString, FVector> vectorOptions)
	: StartConfiguration(std::move(startConfiguration))
	, CompleteCondition(std::move(completeCondition))
	, MoveVectorSetter(std::move(moveVectorSetter))
	, Wheel(wheel)
	, Forces(std::move(forces))
	, State(state)
	, NextState(nextState)
	, FloatOptions(std::move(floatOptions))
	, VectorOptions(std::move(vectorOptions)) {
}

EWheelStateType WheelState::Update(float deltaTime) {
	if (CompleteCondition(*this)) {
		return NextState;
	}

	for (auto& force : Forces) {
		force.DirectionSetter(force.Force);
		force.Force.Update(deltaTime);
	}

	if (auto movement = Wheel->FindComponentByClass<UMovementComponent>()) {
		for (auto& force : Forces) {
			MoveVectorSetter(force.Force.Vector * deltaTime);
		}

		if (Wheel->MoveVector.Size() > Wheel->GetMaxSpeed()) {
			Wheel->MoveVector.Normalize();
			Wheel->MoveVector *= Wheel->GetMaxSpeed();
		}

		movement->Velocity = Wheel->MoveVector;
	}

	return State;
}

ASpinWheelActor::ASpinWheelActor() {
	PrimaryActorTick.bCanEverTick = true;

	ImpactTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SpinWheel");

	MaxSpeed = 3000.f;
	LaunchDistance = 500.f;
	ArcDurationSeconds = 0.5f;

	LaunchDecelerationInfluence = { 40000.f, 40000.f };
	LaunchSideInfluence = { 10000.f, 10000.f };
	ArcDownwardInfluence = { 40000.f, 40000.f };
	ArcSideInfluence = { 20000.f, 10000.f };
	ReturnInfluence = { 100000.f, 10000.f };

	SuckInDistance = 500.f;
	AbsorbDistance = 200.f;
}

void ASpinWheelActor::BeginPlay() {
	Super::BeginPlay();

	if (auto sphere = FindComponentByClass<USphereComponent>()) {
		sphere->OnComponentBeginOverlap.AddDynamic(this, &ASpinWheelActor::OnOverlapBegin);
	}

	State = EWheelStateType::Launch;

	auto sideVector = -GetOwner()->GetActorRightVector();
	auto startVector = MoveVector;
	startVector.Normalize();

	States = {
		TPair<EWheelStateType, WheelState>(
			EWheelStateType::Launch, WheelState(
				[](WheelState& state) {
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Launch");
				},
				[](WheelState& state) {
					return (state.Wheel->GetActorLocation() - state.VectorOptions["StartPosition"]).Size() > state.FloatOptions["MaxDistance"];
				},
				[&](const FVector& force) { MoveVector += force; },
				this,
				{
					ForceWithDirection(FVectorForce(-startVector, LaunchDecelerationInfluence), [](FVectorForce& force) {}),
					ForceWithDirection(FVectorForce(sideVector, LaunchSideInfluence), [](FVectorForce& force) {})
				},
				EWheelStateType::Launch,
				EWheelStateType::Arc,
				{ TPair<FString, float>("MaxDistance", LaunchDistance) },
				{ TPair<FString, FVector>("StartPosition", GetActorLocation()) }
		)),
		TPair<EWheelStateType, WheelState>(
			EWheelStateType::Arc, WheelState(
				[&](WheelState& state) {
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "Arc");
					state.FloatOptions["StartTime"] = UGameplayStatics::GetRealTimeSeconds(GetWorld());
				},
				[&](WheelState& state) {
					return UGameplayStatics::GetRealTimeSeconds(GetWorld()) > state.FloatOptions["StartTime"] + ArcDurationSeconds;
				},
				[&](const FVector& force) { MoveVector += force; },
				this,
				{
					ForceWithDirection(FVectorForce(FVector::ZeroVector, ArcDownwardInfluence), [&](FVectorForce& force) {
						const auto ownerDiff = GetOwner()->GetActorLocation() - GetActorLocation();
						auto dirToOwner = ownerDiff; dirToOwner.Normalize();
						force.SetDirectionKeepSize(dirToOwner);
					}),
					ForceWithDirection(FVectorForce(sideVector, ArcSideInfluence), [](FVectorForce& force) {})
				},
				EWheelStateType::Arc,
				EWheelStateType::Return,
				{ TPair<FString, float>("StartTime", 0) },
				{  }
		)),
		TPair<EWheelStateType, WheelState>(
			EWheelStateType::Return, WheelState(
				[&](WheelState& state) {
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Return");
				},
				[&](WheelState& state) {
					const auto ownerDiff = GetOwner()->GetActorLocation() - GetActorLocation();
					auto dirToOwner = ownerDiff; dirToOwner.Normalize();
					if (ownerDiff.Size() < AbsorbDistance) {
						return true;
					}
					return false;
				},
				[&](const FVector& force) {
					const auto ownerDiff = GetOwner()->GetActorLocation() - GetActorLocation();
					if (ownerDiff.Size() < SuckInDistance) {
						MoveVector = force * MaxSpeed;
					}
					else {
						MoveVector += force;
					}
				},
				this,
				{
					ForceWithDirection(FVectorForce(FVector::ZeroVector, ReturnInfluence), [&](FVectorForce& force) {
						const auto ownerDiff = GetOwner()->GetActorLocation() - GetActorLocation();
						auto dirToOwner = ownerDiff; dirToOwner.Normalize();
						force.SetDirectionKeepSize(dirToOwner);
					})
				},
				EWheelStateType::Return,
				EWheelStateType::None,
				{ TPair<FString, float>("StartVectorSize", 0) },
				{ }
		))
	};

	auto& state = States[State];
	state.StartConfiguration(state);
}

void ASpinWheelActor::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
}

void ASpinWheelActor::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	auto& state = States[State];
	auto oldState = State;
	State = state.Update(deltaTime);
	if (State != oldState) {
		if (State == EWheelStateType::None) {
			OnReturnedToPlayer();
			OnReturn.ExecuteIfBound();
			Destroy();
			return;
		}
		auto& newState = States[State];
		newState.StartConfiguration(newState);
	}
}

float ASpinWheelActor::GetMaxSpeed() const {
	return MaxSpeed;
}

void ASpinWheelActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || !OtherActor->IsA(AMobCharacter::StaticClass())) {
		return;
	}

	auto owner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = owner->GetAbilitySystemComponent();

	auto otherCharacter = Cast<ABaseCharacter>(OtherActor);

	if (!owner->IsFriendlyTowards(otherCharacter) && otherCharacter->IsTargetable() && actorquery::isAlive(otherCharacter)) {
		FGameplayCueParameters parameters;
		parameters.NormalizedMagnitude = 1.f;
		parameters.Location = otherCharacter->GetActorLocation();
		abilitySystem->ExecuteGameplayCue(ImpactTag, parameters);

		if (HasAuthority()) {
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USpinWheelGameplayEffect>(abilitySystem);
			FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
			spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
			context->AddInstigator(GetOwner(), this);
			context->AddOrigin(GetOwner()->GetActorLocation());

			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, otherCharacter->GetAbilitySystemComponent());

			OnHit.ExecuteIfBound();
		}
	}
}

ASpinWheelInstance::ASpinWheelInstance() {
	LaunchVelocity = 2000.f;
	LaunchOffset = FVector(50.f, 0, 0);
	InitialDirectionOffsetRotation = FRotator(0, 10.f, 0);
	LaunchDelay = 1.f;
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
}

void ASpinWheelInstance::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker<ASpinWheelInstance>::AddInstance(GetWorld(), this);
}

void ASpinWheelInstance::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
	InstanceTracker<ASpinWheelInstance>::RemoveInstance(GetWorld(), this);
}

void ASpinWheelInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	if (LaunchDelay > 0.f) {
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ASpinWheelInstance::OnLaunch, predictionKey), LaunchDelay, false);
	}
	else {
		OnLaunch(predictionKey);
	}
}

bool ASpinWheelInstance::CanActivate() const {
	return !Wheel.IsValid() && Super::CanActivate();
}

bool ASpinWheelInstance::IsBusy() const {
	return Wheel.IsValid();
}

float ASpinWheelInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return Damage;
	}
	return -1;
}

void ASpinWheelInstance::ServerSpawnWheel_Implementation(const FPredictionKey& predictionKey, const FVector& direction) {
	MulticastSpawnWheel(predictionKey, direction);
}
bool ASpinWheelInstance::ServerSpawnWheel_Validate(const FPredictionKey& predictionKey, const FVector& direction) {
	return true;
}

void ASpinWheelInstance::SpawnWheel(const FVector& direction) {
	const auto ownerLocation = GetOwner()->GetActorLocation();
	const auto offsetDirection = GetOwner()->GetActorForwardVector().Rotation().RotateVector(LaunchOffset);
	const auto spawnPosition = ownerLocation + offsetDirection;
	FTransform trans(spawnPosition);

	Wheel = GetWorld()->SpawnActorDeferred<ASpinWheelActor>(SpinWheelActorClass, trans, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Wheel.IsValid()) {
		
		Wheel->MoveVector = direction * LaunchVelocity;
		Wheel->Damage = GetPowerEffect()->GetMultiplier(ItemPower) * Damage;
		Wheel->OnReturn.BindUObject(this, &ASpinWheelInstance::OnWheelReturn);
		Wheel->OnHit.BindUObject(this, &ASpinWheelInstance::OnWheelHit);
		UGameplayStatics::FinishSpawningActor(Wheel.Get(), trans);
	}
}

void ASpinWheelInstance::MulticastSpawnWheel_Implementation(const FPredictionKey& predictionKey, const FVector& direction) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		SpawnWheel(direction);
	}
}

void ASpinWheelInstance::OnLaunch(FPredictionKey predictionKey) {
	const auto direction = GetWheelDirection();
	if (HasAuthority() && GetOwner()->GetRemoteRole() != ENetRole::ROLE_AutonomousProxy) {
		MulticastSpawnWheel(predictionKey, direction);
	}
	else {
		SpawnWheel(direction);
		ServerSpawnWheel(predictionKey, direction);
	}
}

// D11.SSN
void ASpinWheelInstance::OnWheelHit() {
	hits++;
}

void ASpinWheelInstance::OnWheelReturn() {
	Cooldown().TriggerCooldown(CalculateCooldown());
	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->SpinBladeHitCount = hits;
		playerCharacter->OnRep_SpinBladeHitCount();
	}
	hits = 0;
}

FVector ASpinWheelInstance::GetWheelDirection() const {
	const auto ownerLocation = GetOwner()->GetActorLocation();
	const auto* controller = Cast<ABasePlayerController>(Cast<APlayerCharacter>(GetOwner())->GetController());
	if (controller && controller->IsLocalController() && GetGameInstance<UDungeonsGameInstance>()->GetControllerTypeManager()->GetControllerType(controller->GetLocalPlayer()->GetControllerId()) == EDungeonsControllerType::Controller_MouseAndKeyboard) {
		FHitResult hitResult;
		if (controller->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult)) {
			auto impactPoint = hitResult.ImpactPoint;
			impactPoint.Z = ownerLocation.Z;
			const auto dir = impactPoint - ownerLocation;
			return !dir.IsNearlyZero() ? dir.GetUnsafeNormal() : InitialDirectionOffsetRotation.RotateVector(GetOwner()->GetActorForwardVector());
		}
	}
	return InitialDirectionOffsetRotation.RotateVector(GetOwner()->GetActorForwardVector());
}
