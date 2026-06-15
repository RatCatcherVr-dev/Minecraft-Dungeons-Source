#include "Dungeons.h"
#include "TotemBaseItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "RuntimeMeshComponent.h"
#include "Engine/EngineTypes.h"

namespace {
	bool isLocal(APlayerCharacter* playerCharacter) {
		if (auto playerController = Cast<ABasePlayerController>(playerCharacter->GetController())) {
			if (playerController->IsLocalController()) {
				return true;
			}
		}
		return false;
	}
}

FVitalityThreshold::FVitalityThreshold()
	: Threshold(0) {
}

FVitalityThreshold::FVitalityThreshold(const float threshold, FString name)
	: Threshold(threshold)
	, Name(std::move(name)) {	
}

ATotemBaseActor::ATotemBaseActor() {
	bReplicates = true;

	WeakenedVitalityThresholds = {
		{ 0.5f, "First" },
		{ 0.3f, "Second"}, 
		{ 0.1f, "Third"}
	};
}

void ATotemBaseActor::TryStartDestroyCountdown() {
	if (!IsDestroying()) {
		OnDestroyCountdownStarted();
		OnDestroyCountdownStarted_Internal();

		GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &ATotemBaseActor::DestroyTotem, DestroyTime);
	}
}

void ATotemBaseActor::BeginPlay() {
	Super::BeginPlay();

	if (auto sphereMesh = FindComponentByClass<USphereComponent>()) {
		sphereMesh->OnComponentBeginOverlap.AddDynamic(this, &ATotemBaseActor::OnBeginSphereOverlap);
		sphereMesh->OnComponentEndOverlap.AddDynamic(this, &ATotemBaseActor::OnEndSphereOverlap);
	}

	CurrentVitalityThreshold = WeakenedVitalityThresholds[0];
}

void ATotemBaseActor::TryNotifyWeakenedVitality() {
	if (CurrentVitalityThreshold.IsSet()) {
		if (GetHealthFraction() <= CurrentVitalityThreshold->Threshold) {
			OnWeakenedVitalityReached(CurrentVitalityThreshold.GetValue());
			SwitchNextVitalityThreshold();
		}
	}
}

void ATotemBaseActor::OnDestroyCountdownStarted_Internal() {
	if (auto sphereMesh = FindComponentByClass<USphereComponent>()) {
		sphereMesh->OnComponentBeginOverlap.RemoveDynamic(this, &ATotemBaseActor::OnBeginSphereOverlap);
		sphereMesh->OnComponentEndOverlap.RemoveDynamic(this, &ATotemBaseActor::OnEndSphereOverlap);
		sphereMesh->SetGenerateOverlapEvents(false);
	}
}

bool ATotemBaseActor::IsDestroying() const {
	return GetWorld()->GetTimerManager().GetTimerRemaining(DestructionTimerHandle) > 0.0f;
}

void ATotemBaseActor::SwitchNextVitalityThreshold() {
	FString name = CurrentVitalityThreshold->Name;
	auto currentIndex = WeakenedVitalityThresholds.IndexOfByPredicate([name](const FVitalityThreshold& threshold) { return threshold.Name == name; });
	const int nextIndex = currentIndex + 1;
	if (nextIndex < WeakenedVitalityThresholds.Num()) {
		CurrentVitalityThreshold = WeakenedVitalityThresholds[nextIndex];
	}
}

void ATotemBaseActor::DestroyTotem() {
	if (HasAuthority()) {
		Destroy();
	}
}

void ATotemBaseActor::OnBeginSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (auto playerCharacter = Cast<APlayerCharacter>(OtherActor)) {
		if (!IsDestroying()) {
			OnPlayerEnter(playerCharacter, ::isLocal(playerCharacter));
		}
	}
}

void ATotemBaseActor::OnEndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (auto playerCharacter = Cast<APlayerCharacter>(OtherActor)) {
		if (!IsDestroying()) {
			OnPlayerExit(playerCharacter, ::isLocal(playerCharacter));
		}
	}
}

void ATotemBaseItemInstance::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (Totem.IsValid()) {
		Totem->TryStartDestroyCountdown();
	}
}

void ATotemBaseItemInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	if (Totem.IsValid()) {
		Totem->TryStartDestroyCountdown();
	}
	if (HasAuthority()) {
		SpawnTotem();
	}
}

void ATotemBaseItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATotemBaseItemInstance, Totem);
}

void ATotemBaseItemInstance::OnSetupWithValidOwner() {
	Super::OnSetupWithValidOwner();
	if(auto playerOwner = Cast<APlayerCharacter>(GetOwner())){
		playerOwner->OnPlayerDeath.AddUObject(this, &ATotemBaseItemInstance::OnPlayerDeath);
	}
}

void ATotemBaseItemInstance::SpawnTotem() {
	const auto& spawnLocation = GetOwner()->GetActorLocation();
	FTransform transform;
	transform.SetLocation(spawnLocation);
	Totem = GetWorld()->SpawnActorDeferred<ATotemBaseActor>(TotemClass, transform, GetOwner(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (Totem.IsValid()) {
		PreTotemBeginPlay(Totem.Get());
		UGameplayStatics::FinishSpawningActor(Totem.Get(), transform);
		
		FHitResult Hit;
		FVector start = spawnLocation;
		FVector end = spawnLocation;
		start.Z += 100;
		end.Z -= 10000;
		TArray<AActor*> ActorsToIgnore;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), ActorsToIgnore);
		ActorsToIgnore.Add(GetOwner());
		UKismetSystemLibrary::LineTraceSingle(GetOwner(), start, end, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
		if (Hit.Component.IsValid() || Hit.Actor.IsValid())
		{
			FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
			if (Hit.Component.IsValid())
			{
				if (Hit.Component.Get()->IsA(URuntimeMeshComponent::StaticClass()))
				{
					//D11.SC - Hit a runtime generated mesh of the world, network/xp[lay has issues replicating these instances across, so dont bother attaching or they will end up lost if UE cant work out its equivilent on the client (its only required for moving UE objects anyway)
					return;
				}
				
				Totem.Get()->AttachToComponent(Hit.Component.Get(), Rules);
			}
			else if(Hit.Actor.IsValid())
			{
				Totem.Get()->AttachToActor(Hit.Actor.Get(), Rules);
			}

			FVector newTotemLocation = Hit.ImpactPoint;
			// Needed because the ImpactoPoint is underneath the floor. Would be nice to know why
			newTotemLocation.Z += 100;
			Totem.Get()->SetActorLocation(newTotemLocation);
		}
	}
}

void ATotemBaseItemInstance::OnPlayerDeath() {
	if (HasAuthority()) {
		if (Totem.IsValid()) {
			Totem->TryStartDestroyCountdown();
		}
	}
}
