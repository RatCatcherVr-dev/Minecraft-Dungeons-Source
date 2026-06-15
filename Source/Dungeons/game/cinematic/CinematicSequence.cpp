 #include "Dungeons.h"
#include "CinematicSequence.h"
#include "game/Conversion.h"
#include "game/util/ComponentUtils.h"
#include "game/util/ActorQuery.h"
#include <DefaultLevelSequenceInstanceData.h>


//RAII workaround for UObjects now having any constructor args
UCinematicSequence* UCinematicSequence::Construct(UWorld& world, UClass* sequenceClass, const Placement& placement, const BlockPos& originalPos){
	auto cinematic = NewObject<UCinematicSequence>();
	cinematic->mWorld = &world;
	cinematic->mSequenceClass = sequenceClass;
	cinematic->mPlacement = placement;
	cinematic->mOriginalPos = originalPos;
	return cinematic;
}

UCinematicSequence::UCinematicSequence() {
	InstanceData = CreateDefaultSubobject<UDefaultLevelSequenceInstanceData>(FName("InstanceData"));
}

void UCinematicSequence::stop() {

	RemoveDelayedPlayTimer();

	if (player())
		player()->Stop();

}

bool UCinematicSequence::load()
{
	if (!mSequenceClass) //we dont have a sequence class, just count us as loaded
		return true;

	if (mLevelSequencePtr == nullptr) {
		mLevelSequencePtr = mWorld->SpawnActorDeferred<ALevelSequenceActor>(mSequenceClass, FTransform());
		mLevelSequencePtr->PlaybackSettings.bAutoPlay = false;

		if (auto instanceData = Cast<UDefaultLevelSequenceInstanceData>(mLevelSequencePtr->DefaultInstanceData)) {
			mLevelSequencePtr->bOverrideInstanceData = true;
			instanceData->TransformOrigin = FTransform(FRotator::ZeroRotator, conversion::blockToUe(mPlacement.position - mOriginalPos));
		}
		UGameplayStatics::FinishSpawningActor(mLevelSequencePtr.Get(), FTransform());
	}

	return (mLevelSequencePtr->GetSequence() != nullptr); //sequence loaded/streamed
}

void UCinematicSequence::play() {
	
	
	RemoveDelayedPlayTimer();
	
	if (player() != nullptr) {
		player()->OnStop.AddDynamic(this, &UCinematicSequence::OnStopped);

		if (!mLevelSequencePtr->GetSequence())
		{
			//we havent managed to load a sequence, its decided to do it async internally, start a delayed play
			mWorld->GetTimerManager().SetTimer(mDelayedPlayTimerHandle, this, &UCinematicSequence::DelayedPlayCallback, 0.001f, true);
		}
		else
		{
			player()->Play();
			OnCinematicPlaying.Broadcast();
		}
	}
}

void UCinematicSequence::DelayedPlayCallback()
{
	if (mLevelSequencePtr != nullptr)
	{
		//play once we have loaded the sequence
		if (mLevelSequencePtr->GetSequence())
		{
			player()->Play();
			OnCinematicPlaying.Broadcast();
			RemoveDelayedPlayTimer();
		}
	}
	else
	{
		RemoveDelayedPlayTimer();
	}
}

void UCinematicSequence::RemoveDelayedPlayTimer()
{
	//clear out delayed timer handle
	if (mDelayedPlayTimerHandle.IsValid())
	{
		mWorld->GetTimerManager().ClearTimer(mDelayedPlayTimerHandle);
	}
}

void UCinematicSequence::OnStopped() {
	RemoveDelayedPlayTimer();

	if (player() != nullptr) {
		player()->OnStop.RemoveDynamic(this, &UCinematicSequence::OnStopped);
	}
	OnCinematicStopped.Broadcast();
}

bool UCinematicSequence::isPlaying() const {
	if (!player())
		return false;

	//currently loading the sequence
	if (mDelayedPlayTimerHandle.IsValid() && mWorld->GetTimerManager().IsTimerActive(mDelayedPlayTimerHandle))
	{
		return true;
	}

	return player()->IsPlaying();
}

ULevelSequencePlayer* UCinematicSequence::player() const {
	return mLevelSequencePtr.IsValid() ? mLevelSequencePtr->SequencePlayer : nullptr;
}

UWorld* UCinematicSequence::GetWorld() const {
	return mWorld;
}
