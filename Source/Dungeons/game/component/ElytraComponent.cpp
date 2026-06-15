#include "ElytraComponent.h"
#include "game/GameTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerCharacterMovementComponent.h"
#include "Animation/AnimBlueprint.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UElytraComponent::UElytraComponent() {
	PrimaryComponentTick.bStartWithTickEnabled = true;
	ElytraCameraRotation = FRotator(-70, 45, 0);
	bAutoActivate = false;
	bReplicates = true;
	SetVisibility(false);
	SetRelativeRotation(FRotator(0, 0, 300));
	SetRelativeLocation(FVector(0, 10, -10));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> elytraSkeletalMesh(*(game::PrefabPath("Actors/Items/Elytra/SK_Elytra")));
	SetSkeletalMesh(elytraSkeletalMesh.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> elytraLandMarkerMesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> elytraLandMarkerMaterial(*(game::PrefabPath("Components/ArmorProperties/AllyDamageBoost/MI_AllyDamageBoostAuraOwnerSprite")));
	elytraLandComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElytraLandComponent"));
	elytraLandComponent->SetupAttachment(this);
	elytraLandComponent->SetStaticMesh(elytraLandMarkerMesh.Object);
	elytraLandComponent->SetAbsolute(true, true, true);
	elytraLandComponent->SetMaterial(0, elytraLandMarkerMaterial.Object);
	elytraLandComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	elytraLandComponent->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> playerBlastOffAnimation(*(game::PrefabPath("Actors/Items/Elytra/Animations/PlayerAnimations/Player_Blastoff_Montage")));
	launchMontage = playerBlastOffAnimation.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> elytraWingDashAnim(*(game::PrefabPath("Actors/Items/Elytra/Animations/Elytra_Roll")));
	elytraWingDashAnimation = elytraWingDashAnim.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> blastOffParticles(*(game::PrefabPath("Actors/Items/Elytra/PS_ElytraLaunch")));
	launchParticleSystem = blastOffParticles.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> landParticles(*(game::PrefabPath("Actors/Items/Elytra/PS_ElytraLand")));
	landParticleSystem = landParticles.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticles(*(game::PrefabPath("Effects/ParticleSystems/Items/PS_RollingBlast")));
	diveImpactParticleSystem = impactParticles.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> streamParticles(*(game::PrefabPath("Actors/Items/Elytra/P_ElytraStream")));
	leftStreamParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ElytraLeftStreamParticleSystemComponent"));
	RightStreamParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ElytraRightStreamParticleSystemComponent"));
	leftStreamParticleSystemComponent->SetupAttachment(this, "J_L_WingStream");
	leftStreamParticleSystemComponent->SetTemplate(streamParticles.Object);
	RightStreamParticleSystemComponent->SetupAttachment(this, "J_R_WingStream");
	RightStreamParticleSystemComponent->SetTemplate(streamParticles.Object);
	leftStreamParticleSystemComponent->bAutoActivate = false;
	RightStreamParticleSystemComponent->bAutoActivate = false;
}

void UElytraComponent::BeginPlay(){
	Super::BeginPlay();
	traceDelegate.BindUObject(this, &UElytraComponent::OnLandMarkerTrace);
}

void UElytraComponent::TriggerAudioStateChange(EElytraAudioState newState)
{
	if (auto owner = Cast<APlayerCharacter>(GetOwner()))
	{
		const bool isLocallyControlled = owner->IsLocallyControlled();

		if (auto gameInstance = owner->GetGameInstance<UDungeonsGameInstance>())
		{
			if (currentState != newState)
			{
				for (auto& soundCuePair : SoundCues)
				{
					if ((newState == EElytraAudioState::None || (1 << (uint8)newState) & soundCuePair.Value.stopState) && soundCuePair.Value.audioComponent.IsValid())
					{
						soundCuePair.Value.audioComponent.Get()->FadeOut(soundCuePair.Value.fadeOutDuration, soundCuePair.Value.fadeOutVolumeLevel);
						soundCuePair.Value.audioComponent.Reset();
					}

					if (isLocallyControlled && (newState == EElytraAudioState::None || (1 << (uint8)newState) & soundCuePair.Value.soundMixPopState) && soundCuePair.Value.soundMix)
					{
						gameInstance->GetSoundMixManager()->PopSoundMix(soundCuePair.Value.soundMix);
					}
				}

				auto soundCue = SoundCues.Find(newState);
				if (soundCue && !soundCue->audioComponent.IsValid() && (!soundCue->onlyPlayLocally || isLocallyControlled))
				{
					if (auto currentSound = UGameplayStatics::SpawnSoundAttached(soundCue->sound, GetAttachParent()))
					{
						currentSound->bAllowSpatialization = soundCue->allowSpatialization;
						currentSound->FadeIn(soundCue->fadeInDuration, soundCue->fadeInVolumeLevel, soundCue->fadeInStartTime);

						if (isLocallyControlled) {
							gameInstance->GetSoundMixManager()->PushSoundMix(soundCue->soundMix);
						}

						soundCue->audioComponent = currentSound;
					}
				}

				currentState = newState;

				if (newState == EElytraAudioState::DiveImpact || newState == EElytraAudioState::Land)
				{
					TriggerAudioStateChange(EElytraAudioState::None);
				}
			}
		}
	}
}

void UElytraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsFlying) {
		UpdateLandMarkerLocation();
	}
}

void UElytraComponent::Equip()
{
	bIsEquipped = true;
	SetVisibility(true);
	Activate();
	OnRep_IsEquipped();
}

void UElytraComponent::UpdateLandMarkerLocation() {

	if (!traceHandle.IsSet()) {
		const FVector startLocation = GetOwner()->GetActorLocation();
		const FVector endLocation = startLocation - FVector(0, 0, 5000.f);

		traceHandle = traceHandle = GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, startLocation, endLocation, (ECollisionChannel)ECustomTraceChannels::TerrainOnly, FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam, &traceDelegate);
	}
}

void UElytraComponent::OnLandMarkerTrace(const FTraceHandle& handle, FTraceDatum& data) {
	traceHandle.Reset();

	const FVector startLocation = GetOwner()->GetActorLocation();
	FVector endLocation = startLocation - FVector(0, 0, 5000.f);

	if (data.OutHits.Num() > 0) {
		endLocation = data.OutHits[0].Location;
	}

	elytraLandComponent->SetWorldLocation(endLocation + FVector(0, 0, 10.0f));
}

void UElytraComponent::SetIsFlying(bool flying) {
	if (GetOwner()->HasAuthority() && flying != bIsFlying) {
		bIsFlying = flying;
		OnRep_IsFlying();
	}
}

void UElytraComponent::DoDiveImpact()
{
	const auto& location = elytraLandComponent->GetComponentLocation();
	const auto effectScale = GetOwner()->GetVelocity().Z * DiveImpactScaleMultiplier;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), diveImpactParticleSystem, location, FRotator::ZeroRotator, FVector(effectScale));

	TriggerAudioStateChange(EElytraAudioState::DiveImpact);
	bPerformedDiveImpact = true;

	if (GetOwner()->HasAuthority())
	{
		int count = 0;
		for (auto mob : actorquery::getNearbyActors<AMobCharacter>(GetWorld(), location, DiveImpactRadius * effectScale)) {
			if (mob->IsAlive() && mob->IsTargetable() && !mob->IsFriendlyTowardsPlayers()) {
				count++;
				DealDamageToMob(mob, location, DiveImpactDamage);
			}
		}
		if(APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner()))
		{
			playerCharacter->ElytraAttackHitCount = count;
			playerCharacter->OnRep_ElytraAttackHitCount();
		}
	}
}

void UElytraComponent::OnRep_IsEquipped() {
	OnVisibilityChanged();
}

void UElytraComponent::OnRep_IsFlying() {
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (playerCharacter->IsLocallyControlled()) {
		const UDungeonsGameInstance* gameInstance = playerCharacter->GetGameInstance<UDungeonsGameInstance>();
		const auto initialCharacter = Cast<APlayerCharacter>(gameInstance->GetFirstLocalPlayerController()->GetPawn());

		if (bIsFlying) {
			if (USpringArmComponent* springArm = playerCharacter->GetCameraSpringArm()) {
				cachedCameraRotation = cachedLMPCameraRotation = springArm->GetComponentRotation();
			}

			if (gameInstance->IsLocalCoop())
			{
				const USpringArmComponent* lmpSpringArm = playerCharacter->GetLocalCoopCameraSpringArm();
				if (initialCharacter == playerCharacter && lmpSpringArm && !lmpSpringArm->GetComponentRotation().Equals(ElytraCameraRotation, 0.1f))
				{
					for (auto localPlayerController : gameInstance->GetUserManager()->GetAllLocalPlayerControllers())
					{
						if (const auto localCharacter = Cast<APlayerCharacter>(localPlayerController->GetPawn()))
						{
							localCharacter->GetElytraComponent()->cachedLMPCameraRotation = lmpSpringArm->GetComponentRotation();
						}
					}
				}
			}
		}

		const float rotationSpeed = bIsFlying ? ElytraEnableCameraRotationLagSpeed : ElytraDisableCameraRotationLagSpeed;

		if (USpringArmComponent* springArm = playerCharacter->GetCameraSpringArm()) {
			springArm->CameraRotationLagSpeed = rotationSpeed;
			springArm->bEnableCameraLag = !bIsFlying;
			springArm->SetWorldRotation(bIsFlying ? ElytraCameraRotation : cachedCameraRotation);
		}

		if (gameInstance->IsLocalCoop())
		{
			bool isAnyoneElseFlying = false;
			for (auto localPlayerController : gameInstance->GetUserManager()->GetAllLocalPlayerControllers())
			{
				const auto localCharacter = Cast<APlayerCharacter>(localPlayerController->GetPawn());
				if(localCharacter && localCharacter != playerCharacter)
				{
					if(localCharacter->GetElytraComponent()->bIsFlying)
					{
						isAnyoneElseFlying = true;
						break;
					}
				}
			}

			USpringArmComponent* lmpSpringArm = initialCharacter->GetLocalCoopCameraSpringArm();
			if (!isAnyoneElseFlying && lmpSpringArm)
			{
				lmpSpringArm->CameraRotationLagSpeed = rotationSpeed;
				lmpSpringArm->bEnableCameraLag = !bIsFlying;
				lmpSpringArm->SetWorldRotation(bIsFlying ? ElytraCameraRotation : cachedLMPCameraRotation);
			}
		}

		elytraLandComponent->SetVisibility(bIsFlying);

	}

	elytraLandComponent->SetWorldLocation(playerCharacter->GetActorLocation() - FVector(0, 0, playerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	if (bIsFlying) {
		playerCharacter->PlayMontage(launchMontage);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), launchParticleSystem, elytraLandComponent->GetComponentLocation());
		TriggerAudioStateChange(EElytraAudioState::Launch);
	}
	else if(!bPerformedDiveImpact){
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), landParticleSystem, elytraLandComponent->GetComponentLocation());
		TriggerAudioStateChange(EElytraAudioState::Land);
	}

	bPerformedDiveImpact = false;

	leftStreamParticleSystemComponent->SetActive(bIsFlying);
	RightStreamParticleSystemComponent->SetActive(bIsFlying);
	playerCharacter->SetCapsuleCollisionResponse( bIsFlying ? ESynchornizedCollisionResponse::WorldOnly : ESynchornizedCollisionResponse::Default);
}

void UElytraComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UElytraComponent, bIsFlying);
	DOREPLIFETIME(UElytraComponent, bIsEquipped);
}

void UElytraComponent::PerformRollAnimation() {
	if (IsEquipped())
	{
		TriggerAudioStateChange(EElytraAudioState::Dash);

		UAnimInstance* animInstance = GetAnimInstance();
		if (animInstance)
		{
			animInstance->PlaySlotAnimationAsDynamicMontage(elytraWingDashAnimation, TEXT("DefaultSlot"));

			if (auto world = GetWorld())
			{
				const auto disableWingVFX = FTimerDelegate::CreateLambda([this]()
				{
					leftStreamParticleSystemComponent->SetActive(false);
					RightStreamParticleSystemComponent->SetActive(false);
				});

				FTimerHandle handle;
				world->GetTimerManager().SetTimer(handle, disableWingVFX, elytraWingDashAnimation->GetPlayLength(), false);
				leftStreamParticleSystemComponent->SetActive(true);
				RightStreamParticleSystemComponent->SetActive(true);
			}
		}
	}
}

void UElytraComponent::RefreshVisibility(bool IsCharacterVisible) { 
	SetVisibility(IsCharacterVisible && bIsEquipped, true);
	elytraLandComponent->SetVisibility(bIsFlying, false);
}

float UElytraComponent::GetDistanceToLandMarker() const {
	return FMath::Abs(GetOwner()->GetActorLocation().Z - elytraLandComponent->GetComponentLocation().Z);
}

bool AElytraInstance::CanActivate() const {
	return Super::CanActivate() && Cast<APlayerCharacter>(GetOwner());
}

void AElytraInstance::Activate(const FPredictionKey& predictionKey) {
	if (HasAuthority())
	{
		auto owner = Cast<APlayerCharacter>(GetOwner());
		if (owner->IsLocallyControlled()) {
			if (auto gi = GetGameInstance<UDungeonsGameInstance>()) {
				const auto localPlayerControllers = gi->GetUserManager()->GetAllLocalPlayerControllers();
				for (auto lpc : localPlayerControllers) {
					Cast<APlayerCharacter>(lpc->GetCharacter())->GetElytraComponent()->Equip();
				}
			}
		}
		else {
			owner->GetElytraComponent()->Equip();
		}
	}

	Super::Activate(predictionKey);
}

void UElytraComponent::DealDamageToMob(const AMobCharacter* mob, const FVector& impactLocation, const float scaledDamage)
{
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);

	const auto owner = GetOwner();
	const auto push = CreatePushback(mob, impactLocation);
	pushback::pushback(push, *owner, *mob);

	const auto mobAbilitySystem = mob->GetAbilitySystemComponent();

	auto mobDamageSpec = effects::CreateGameplayEffectSpec<UElytaDiveImpactDamageGameplayEffect>(mobAbilitySystem, effects::HealthName, -scaledDamage, owner, owner, impactLocation, 1.f);
	effects::StorePushbackInNormal(mobDamageSpec, pushback::getLaunchVector(push, *owner, *mob, DiveImpactRagdollPushbackMultiplier, DiveImpactMinimumRagdollPushbackStrength));
	mobAbilitySystem->ApplyGameplayEffectSpecToSelf(mobDamageSpec);
}

FPushback UElytraComponent::CreatePushback(const AActor* entity, const FVector& impactLocation) const
{
	const auto launchMagnitude = CalculateLaunchMagnitude(entity, impactLocation);

	FPushback push;
	push.pushbackStrength = FMath::Max(launchMagnitude * DiveImpactPushbackMultiplier, DiveImpactMinimumPushbackStrength);
	push.pushbackZFactor = DiveImpactUpwardsPushback;
	push.enablePushback = true;

	return push;
}

float UElytraComponent::CalculateLaunchMagnitude(const AActor* entity, const FVector& impactLocation) const
{
	const auto distToEffectCenter = entity->GetActorLocation() - impactLocation;
	const auto launchMagnitude = 1.0f - (distToEffectCenter.Size() / (DiveImpactRadius * 2));
	return launchMagnitude;
}

UElytaDiveImpactDamageGameplayEffect::UElytaDiveImpactDamageGameplayEffect() {
	InheritableGameplayEffectTags.CombinedTags.Reset();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Explosion.Strong"), 0, 1);

}