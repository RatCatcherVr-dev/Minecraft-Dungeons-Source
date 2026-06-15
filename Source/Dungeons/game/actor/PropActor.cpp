#include "Dungeons.h"
#include "PropActor.h"
#include "DungeonsGameState.h"

DataTracker<APropActor::InstanceData, false> APropActor::Instances;

APropActor::APropActor() {
 	PrimaryActorTick.bCanEverTick = false;
	NetUpdateFrequency = 10.0f;
	MinNetUpdateFrequency = 1.0f;
}

APropActor::APropActor(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bCanEverTick = false;
	NetUpdateFrequency = 10.0f;
	MinNetUpdateFrequency = 1.0f;
}

void APropActor::BeginPlay() {
	Super::BeginPlay();	
	AddDataInstanceTracking();
}

void APropActor::AddDataInstanceTracking()
{
	Instances.AddInstance(GetWorld(), InstanceData(this));
}

void APropActor::EndPlay(EEndPlayReason::Type EndPlayReason) {
	RemoveDataInstanceTracking();	
	Super::EndPlay(EndPlayReason);
}

void APropActor::RemoveDataInstanceTracking()
{
	Instances.RemoveInstance(GetWorld(), InstanceData(this));
}

bool APropActor::IsNetRelevantFor(const AActor * RealViewer, const AActor * ViewTarget, const FVector & SrcLocation) const {
	if (IsCinematicCurrentlyPlaying()) {
		return AActor::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}
	
	return mPlayerVisible && AActor::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

void APropActor::ResetInstanceData()
{
	if (APropActor::InstanceData* pFoundInstanceData = Instances.GetList(GetWorld()).FindByPredicate([&](APropActor::InstanceData& InstanceDataVal) { return InstanceDataVal.Actor == this; }))
	{
		pFoundInstanceData->ResetToActorValues();
	}
}

void APropActor::SetPlayerInVisibleRange(bool Visible)
{
	mPlayerVisible = Visible;

	OnPlayerInVisibleRangeChange(Visible);

	//Call Unique Scripted Events
	if (Visible)
	{
		PlayerEnteredVisibleRange();
	}
	else
	{
		PlayerLeftVisibleRange();
	}

	PlayerInVisibleRangeChanged(Visible); //Call Generic Scripted Event
}


bool APropActor::IsCinematicCurrentlyPlaying() const {
	const auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());
	return gameState && gameState->IsCinematicPlaying();
}

void APropActor::PlayerEnteredVisibleRange_Implementation() {
}

void APropActor::PlayerLeftVisibleRange_Implementation() {	
}

bool APropActor::GetInPlayerVisibleRange() const {
	return mPlayerVisible;
}

APropActor::InstanceData::InstanceData(APropActor* actor)
	: Actor(actor)
	, DoPlayerDistanceCheck(actor->mDoPlayerDistanceCheck)
	, CheckDistance(actor->mCheckDistance)
	, ActorLocation(actor->GetActorLocation())
	, PlayerVisible(actor->mPlayerVisible)
	, CanMove(actor->mCanMove)
{
}

FVector APropActor::InstanceData::GetActorLocation() const {
	if (CanMove) {
		return Actor->GetActorLocation();
	}
	else {
		return ActorLocation;
	}
}

void APropActor::InstanceData::SetPlayerVisible(bool isVisible) {
	if (isVisible != PlayerVisible) {
		PlayerVisible = isVisible;
		Actor->SetPlayerInVisibleRange(isVisible);
		VerifyCachedValues();
	}
}

void APropActor::InstanceData::VerifyCachedValues() {
	ensureMsgf(CheckDistance == Actor->mCheckDistance, TEXT("CheckDistance have been changed in actor %s during runtime. Don't do it after the construction script."), *Actor->GetFName().ToString());
	ensureMsgf(DoPlayerDistanceCheck == Actor->mDoPlayerDistanceCheck, TEXT("DoPlayerDistanceCheck have been changed in actor %s during runtime. Don't do it after the construction script."), *Actor->GetFName().ToString());
	if (!CanMove) {
			ensureMsgf(ActorLocation == Actor->GetActorLocation(), TEXT("The actor %s have moved after creation, while the CanMove property is false. CanMove must be true if the actor can move."), *Actor->GetFName().ToString());
		if (ActorLocation != Actor->GetActorLocation()) {
			CanMove = true; // Fallback in case some prop is forgotten
		}
	}
}

void APropActor::InstanceData::ResetToActorValues()
{
	if (Actor)
	{
		ActorLocation = Actor->GetActorLocation();
		PlayerVisible = Actor->mPlayerVisible;
		CanMove = Actor->mCanMove;
	}
}

