#include "Dungeons.h"
#include "Array.h"
#include "AmbienceActor.h"
#include "Engine/PostProcessVolume.h"

#include "AmbienceUtil.h"

#if WITH_EDITOR
	#include "Editor.h"
	#include "LevelEditorViewport.h"
#endif
#include "AmbienceTriggerActor.h"
#include "game/actor/character/player/PlayerCharacter.h"


void AAmbienceVisualActor::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker< AAmbienceVisualActor >::AddInstance(GetWorld(), this);
}

void AAmbienceAudioActor::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker< AAmbienceAudioActor >::AddInstance(GetWorld(), this);
}

void AAmbienceVisualActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	InstanceTracker< AAmbienceVisualActor >::RemoveInstance(GetWorld(), this);
	Super::EndPlay(EndPlayReason);
}

bool AAmbienceVisualActor::Matches(const FAmbienceIDGroup& ambience) const {
	return AmbienceID == ambience.Id && GetGroupName() == ambience.GroupName;
}

void AAmbienceAudioActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	InstanceTracker< AAmbienceAudioActor >::RemoveInstance(GetWorld(), this);
	Super::EndPlay(EndPlayReason);
}

void AAmbienceActor::SetActive(const bool active) {
	if (bIsActive != active) {
		bIsActive = active;
		active ? OnActivated() : OnDeactivated();
		SetAttachedActorsVisible(ShouldBeVisible());
	}
}

void AAmbienceActor::SetHidden(const bool hidden) {
	if (bIsHidden != hidden) {
		bIsHidden = hidden;
		SetAttachedActorsVisible(ShouldBeVisible());
	}
}

bool AAmbienceActor::ShouldBeVisible() const {
	return bIsActive && !bIsHidden;
}

void AAmbienceActor::SetAttachedActorsVisible(bool visible){
	if (bAreAttachedActorsVisible != visible) {
		bAreAttachedActorsVisible = visible;
		SetAllAttachedActorVisibility(visible);		
	}
}

EAmbienceID AAmbienceVisualActor::GetAmbienceID() const {
	return AmbienceID;
}

const TArray<FString>& AAmbienceActor::GetAttachedNames() const {
	return attachedNames;
}

void AAmbienceActor::forceClearActiveAndHideAll() {
	bIsActive = false;
	bAreAttachedActorsVisible = false;
	SetAllAttachedActorVisibility(false);
}

void AAmbienceActor::SetAttachedActorNames(TArray<AActor*> childActors) {
	attachedNames.Reset();
	for (auto child : childActors) {
		attachedNames.Add(child->GetName());
	}
}

void AAmbienceActor::SetGroupName(FString groupName) {
	mGroupName = std::move(groupName);
}

float AAmbienceActor::GetRevealRadiusScale() const {
	return revealRadiusScale;
}

const FString& AAmbienceActor::GetGroupName() const {
	return mGroupName;
}

void AAmbienceActor::SetAllAttachedActorVisibility(const bool visibility) const {
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (auto Actor : ChildActors) {
		if (const auto post = Cast<APostProcessVolume>(Actor)) {
			post->bEnabled = visibility;
		}
		else {
			Actor->GetRootComponent()->SetVisibility(visibility);
		}
		if (auto triggerActor = Cast<AAmbienceTriggerActor>(Actor)) {
			visibility ? triggerActor->OnAmbienceShow() : triggerActor->OnAmbienceHide();
		}
	}
}

void AAmbienceActor::OnActivated() const {
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (auto Actor : ChildActors) {
		if (auto triggerActor = Cast<AAmbienceTriggerActor>(Actor)) {
			triggerActor->OnAmbienceActivated(ChildActors);
		}
	}
}

void AAmbienceActor::OnDeactivated() const {
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (auto Actor : ChildActors) {
		if (auto triggerActor = Cast<AAmbienceTriggerActor>(Actor)) {
			triggerActor->OnAmbienceDeactivated(ChildActors);
		}
	}
}


void AAmbienceActor::PlayerExited(APlayerCharacter* player)
{
	player->SetRevealRadiusScale(1.0f);

	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (auto Actor : ChildActors) {
		if (auto triggerActor = Cast<AAmbienceTriggerActor>(Actor)) {
			triggerActor->OnPlayerExitedAmbience(player);
		}
	}
}

void AAmbienceActor::PlayerEntered(APlayerCharacter* player)
{
	player->SetRevealRadiusScale(GetRevealRadiusScale());

	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (auto Actor : ChildActors) {
		if (auto triggerActor = Cast<AAmbienceTriggerActor>(Actor)) {
			triggerActor->OnPlayerEnteredAmbience(player);
		}
	}
}




void AAmbienceVisualActor::SetAsActiveAmbience() const {
#if WITH_EDITOR
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, StaticClass(), FoundActors);

	for (auto Actor : FoundActors) {
		const auto ambience = Cast<AAmbienceVisualActor>(Actor);
		ambience->SetAllAttachedActorVisibility(ambience->Matches({ GetAmbienceID(), GetGroupName() }));
	}

	if (auto client = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient())) {
		client->Invalidate(true, false);
	}
#endif
}

void AAmbienceVisualActor::PreSave(const ITargetPlatform* TargetPlatform) {
	Super::PreSave(TargetPlatform);
	Id = *GetEnumValueToString(AmbienceID);
}


EAmbienceAudioID AAmbienceAudioActor::GetAmbienceID() const {
	return AmbienceAudioID;

}

void AAmbienceAudioActor::PreSave(const ITargetPlatform* TargetPlatform) {
	Super::PreSave(TargetPlatform);
	Id = *GetEnumValueToString(AmbienceAudioID);
}

bool AAmbienceAudioActor::Matches(const FAmbienceAudioIDGroup& ambience) const {
	return AmbienceAudioID == ambience.Id && GetGroupName() == ambience.GroupName;
}
