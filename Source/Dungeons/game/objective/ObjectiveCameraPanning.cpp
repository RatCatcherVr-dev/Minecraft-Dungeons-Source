#include "Dungeons.h"
#include "LoadingScreen/ScreenFader.h"
#include "game/actor/DungeonsPlayerCameraManager.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "ObjectiveCameraPanningDefs.h"
#include "game/component/LovikaSpringArmComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/ComponentUtils.h"
#include "game/Game.h"
#include "game/ai/BehaviorSystem.h"
#include "util/Algo.h"
#include "ObjectiveCameraPanning.h"
#include "game/actor/NotificationBroadcastActor.h"
#include "game/mission/EventLocTexts.h"
#include "util/ClassUtil.h"
#include "game/actor/ObjectiveInterestPoint.h"
#include "game/objective/EventTypes.h"

namespace game { namespace objective {

FVector GetCameraOffset(APlayerCharacter* player) {
	if (auto springArm = componentutils::GetComponentByTag<ULovikaSpringArmComponent>(player, FName{ "CameraSpringArm" })) {
		if (const auto camera = player->FindComponentByClass<UCameraComponent>()) {
			return -(desiredCameraRotation.Vector() * springArm->TargetArmLength) + springArmOffset;
		}
	}
	return FVector::ZeroVector;
}

PanningSequence createSequence(const FObjectiveEventLocation& eventLocation, float duration, UWorld& world, APlayerCharacter* player) {
	const auto& playerPos = player->GetActorLocation();

	// Get the halfway point between location and player pos
	FObjectiveEventLocation halfPointEvent = { FMath::Lerp(eventLocation.Location, playerPos, 0.5f), eventLocation.EventType };

	const auto cameraOffset = GetCameraOffset(player);
	TransformData transformData({ playerPos, cameraOffset }, { halfPointEvent.Location, cameraOffset });
	return std::move(movingPanningSequence(world, *player, { transformData }, duration));
}

PanningAction::PanningAction() {
}

PanningAction::PanningAction(float actionStartOffset, float duration, std::string name)
	: mActionStartOffset(actionStartOffset),
	  mDuration(duration),
	  mName(std::move(name))
{
}

bool PanningAction::tryStart(float sequenceStartTime) {
	if (mIsStarted) {
		return false;
	}
	auto currentTime = mWorld->GetTimeSeconds();
	auto actionStartTime = sequenceStartTime + mActionStartOffset;
	if (currentTime > actionStartTime) {
		mIsStarted = true;
		mStartTimeStamp = currentTime;
		onStart();
		return true;
	}
	return false;
}

void PanningAction::onTickReceived(float deltaTime) {
	if (!mIsStarted) {
		return;
	}
	if (mWorld->GetTimeSeconds() > mStartTimeStamp + mDuration) {
		mIsCompleted = true;
	}
	if (!mIsCompleted) {
		onTick(deltaTime);
	}
}

void PanningAction::setWorld(UWorld& world) {
	mWorld = &world;
}

float PanningAction::duration() const {
	return mDuration;
}

bool PanningAction::isCompleted() const {
	return mIsCompleted;
}

bool PanningAction::isStarted() const {
	return mIsStarted;
}

float PanningAction::actionStartOffset() const {
	return mActionStartOffset;
}

float PanningAction::startTimeStamp() const {
	return mStartTimeStamp;
}

const std::string& PanningAction::name() const {
	return mName;
}

FadeAction::FadeAction(float actionStartOffset, float duration, std::string name, bool fadeIn)
	: PanningAction(actionStartOffset, duration, name), 
	  mFadeIn(fadeIn)
{
}

void FadeAction::onStart() {
	if (!mFader.IsValid()) {
		mFader = mWorld->SpawnActor<AScreenFader>();
	}

	mFader->StartFade(mFadeIn, duration(), FOnFadeComplete::CreateRaw(this, &FadeAction::onFadeComplete));
}

void FadeAction::onFadeComplete() {
	if (mFader.IsValid()) {
		mFader->Destroy();
	}
}

TransformAction::TransformAction() {
}

TransformAction::TransformAction(float actionStartOffset, float duration, std::string name, TransformData transformData, MoveInterpolationInfo interpolationInfo)
	: PanningAction(actionStartOffset, duration, name),
	  mTransformData(std::move(transformData)), 
	  mInterpolationInfo(std::move(interpolationInfo)),
	  mTotalDistance(mTransformData.mStartPosition.GetPosition() - mTransformData.mEndPosition.GetPosition())
{
}

void TransformAction::onTick(float deltaTime) {
	const auto currentTime = mWorld->GetTimeSeconds();
	const auto elapsedTime = currentTime - startTimeStamp();
	const auto timeElapsedFraction = elapsedTime / duration();

	FVector newPosition;
	switch (mInterpolationInfo.Method) {
		case (MoveInterpolationMethod::Linear) : {
			newPosition = FMath::Lerp(mTransformData.mStartPosition.GetPosition(), mTransformData.mEndPosition.GetPosition(), timeElapsedFraction);
			break;
		}
		case (MoveInterpolationMethod::EaseIn): {
			newPosition = FMath::InterpEaseIn(mTransformData.mStartPosition.GetPosition(), mTransformData.mEndPosition.GetPosition(), timeElapsedFraction, mInterpolationInfo.Exponent);
			break;
		}
		case (MoveInterpolationMethod::EaseOut): {
			newPosition = FMath::InterpEaseOut(mTransformData.mStartPosition.GetPosition(), mTransformData.mEndPosition.GetPosition(), timeElapsedFraction, mInterpolationInfo.Exponent);
			break;
		}
	}

	auto cameraComponent = Cast<APlayerCharacter>(mWorld->GetFirstPlayerController()->GetPawn())->FindComponentByClass<UCameraComponent>();

	auto newRotation = mTransformData.mLookAtLocation ? FVector(mTransformData.mLookAtLocation.GetValue() - newPosition).Rotation() : cameraComponent->GetComponentRotation();
	newRotation.Roll = 0.0f;

	cameraComponent->SetWorldLocation(newPosition);
	cameraComponent->SetWorldRotation(newRotation);
}

PanningSequence::PanningSequence() {
}

void PanningSequence::onStart(UWorld& world) {
	mIsStarted = true;
	mStartTimeStamp = world.GetTimeSeconds();
	tryStartActions();
}

void PanningSequence::onTick(float deltaTime) {
	if (!mIsStarted) {
		return;
	}

	tryStartActions();
	for (auto& action : mPanningActions) {
		auto& actionPtr = action.first;
		actionPtr->onTickReceived(deltaTime);
		if (actionPtr->isCompleted() && action.second) {
			onActionComplete(*actionPtr.get());
			action.second = false;
		}
	}
}

bool PanningSequence::isComplete() const {
	return mIsComplete;
}

void PanningSequence::addAction(UWorld& world, Unique<PanningAction> action) {
	action->setWorld(world);
	mPanningActions.emplace_back(std::make_pair(std::move(action), true));
}

void PanningSequence::reset() {
	for (auto& action : mPanningActions) {
		auto ptr = std::move(action.first);
		ptr.reset();
	}
	mPanningActions.clear();
	mIsStarted = false;
	mIsComplete = false;
}

void PanningSequence::onEnd() {
	mIsComplete = true;
}

void PanningSequence::tryStartActions() {
	for (auto& action : mPanningActions) {
		auto& actionPtr = action.first;
		if (actionPtr->tryStart(mStartTimeStamp)) {
			onActionStarted(*actionPtr.get());
		}
	}
}

void PanningSequence::onActionStarted(const PanningAction& action) {
	if (mOnActionStarted) {
		mOnActionStarted(action);
	}
	if (mOnSequenceStarted)
		mOnSequenceStarted();
}

void PanningSequence::onActionComplete(const PanningAction& action) {
	if (mOnActionStarted) {
		mOnActionFinished(action);
	}
	if (isAllCompleted()) {
		onEnd();
		if (mOnSequenceComplete)
			mOnSequenceComplete();
	}
}

bool PanningSequence::isAllCompleted() const {
	return algo::all_of(mPanningActions, RETLAMBDA(it.first->isCompleted()));
}

void ObjectiveEventQueue::add(FObjectiveEventLocation eventLocation, TOptional<float> sequenceDuration) {
	mObjectiveEvents.emplace(std::move(eventLocation), sequenceDuration);
}

bool ObjectiveEventQueue::onStart(UWorld& world, APlayerCharacter* player) {
	mWorld = &world;
	mPlayerCharacter = player;
	if (mObjectiveEvents.size() > 0) {
		mIsStarted = true;
		initEvent(mObjectiveEvents.front());
	}
	else {
		if (mOnQueueCompleted) {
			mOnQueueCompleted();
		}
		return false;
	}

	return true;
}

void ObjectiveEventQueue::onTick(float deltaTime) {
	if (mObjectiveEvents.size() > 0) {
		auto& activeSequence = mObjectiveEvents.front().mSequence;
		if (activeSequence) {
			activeSequence->onTick(deltaTime);
		}
	}
}

bool ObjectiveEventQueue::isActive() const {
	return mIsStarted;
}

void ObjectiveEventQueue::reset() {
	while (mObjectiveEvents.size() > 0) {
		auto& front = mObjectiveEvents.front();
		if (front.mSequence) {
			front.mSequence->reset();
		}
		mObjectiveEvents.pop();
	}
}

void ObjectiveEventQueue::onEventCompleted() {
	if (mObjectiveEvents.size() > 1) {
		next();
	}
	else {
		if (mOnQueueCompleted) {
			mOnQueueCompleted();
		}
		mIsStarted = false;
	}
}

void ObjectiveEventQueue::initEvent(ObjectiveEvent& objectiveEvent) {
	notifyEvent(objectiveEvent);
	onEventCompleted();
}

bool ObjectiveEventQueue::tryAddSequence(ObjectiveEvent& objectiveEvent) {
	if (mPlayerCharacter.IsValid() && actorquery::isAlive(mPlayerCharacter.Get())) {
		const auto furthestThreshold = 5000.0f;
		const auto farThreshold = 2000.0f;

		auto& eventLocation = objectiveEvent.mEventLocation;
		auto delta = eventLocation.Location - mPlayerCharacter->GetActorLocation();

		if (delta.Size() > farThreshold && delta.Size() < furthestThreshold) {
			objectiveEvent.mSequence = createSequence(eventLocation, objectiveEvent.mSequenceDuration.Get(0.0f), *mWorld, mPlayerCharacter.Get());
			objectiveEvent.mSequence->onStart(*mWorld);
			objectiveEvent.mSequence->mOnSequenceComplete = [this]() { this->onEventCompleted(); };
			return true;
		}
	}
	
	return false;
}

void ObjectiveEventQueue::next() {
	mObjectiveEvents.pop();
	auto& active = mObjectiveEvents.front();
	initEvent(active);
}

void ObjectiveEventQueue::notifyEvent(const ObjectiveEvent& eventNotify) {
	const auto& eventLocation = eventNotify.mEventLocation;

	if (auto NotificationBroadcastComponent = actorquery::getFirstActor<ANotificationBroadcastActor>(mWorld)) {
		NotificationBroadcastComponent->BroadcastNotification(FEventNotification(eventLocation.EventType, GetEventTitle(), GetEventMessage(eventLocation.EventType)));
		NotificationBroadcastComponent->TriggerObjectiveUpdatedEffect(eventLocation.Location);
	}

}

void ObjectiveEventHandler::tryStart(UWorld& world, PanningSettings settings) {
	if (!isActive()) {
		start(world, std::move(settings));
	}
}

void ObjectiveEventHandler::start(UWorld& world, PanningSettings settings) {
	mSettings = std::move(settings);
	mWorld = &world;

	auto player = Cast<APlayerCharacter>(mWorld->GetFirstPlayerController()->GetPawn());

	mObjectiveEventQueue.mOnQueueCompleted = [this]() { this->onEnd(); };

	if (mObjectiveEventQueue.onStart(world, player) && isActive()) {
		if (mOnCameraPanningStarted) {
			mOnCameraPanningStarted();
		}

		if (mSettings) {
			if (mSettings->mMakePlayerInvulnerable) {
				togglePlayerInvulnerability(true);
			}
		}
	}
}

void ObjectiveEventHandler::onTick(float deltaTime) {
	mObjectiveEventQueue.onTick(deltaTime);
}

void ObjectiveEventHandler::onEnd() {
	mObjectiveEventQueue.reset();

	if (mSettings) {
		if (mSettings->mMakePlayerInvulnerable) {
			togglePlayerInvulnerability(false);
		}
	}
	
	toggleSpringArmAttachment(true);
}

void ObjectiveEventHandler::queuePanEvent(FObjectiveEventLocation eventLocation, TOptional<float> sequenceDuration) {
	mObjectiveEventQueue.add(std::move(eventLocation), sequenceDuration);
}

bool ObjectiveEventHandler::isActive() const {
	return mObjectiveEventQueue.isActive();
}

void ObjectiveEventHandler::togglePlayerInvulnerability(bool makeInvulnerable) {
	if (auto player = Cast<APlayerCharacter>(mWorld->GetFirstPlayerController()->GetPawn())) {
		if (makeInvulnerable) {
			player->ServerApplyInvulnerability(10.0f);
		}
		else {
			player->ServerRemoveInvulnerability();
		}
	}
}

void ObjectiveEventHandler::toggleSpringArmAttachment(bool attach) {
	if (attach) {
		if (mCachedSpringArm.IsValid() && mCachedCamera.IsValid()) {
			mCachedCamera->AttachTo(mCachedSpringArm.Get());
			mCachedCamera->SetRelativeTransform(mCachedCameraTransform);
		}
	}
	else {
		if (auto player = Cast<APlayerCharacter>(mWorld->GetFirstPlayerController()->GetPawn())) {
			if (auto springArm = componentutils::GetComponentByTag<ULovikaSpringArmComponent>(player, FName{ "CameraSpringArm" })) {
				mCachedSpringArm = springArm;
				if (auto camera = player->FindComponentByClass<UCameraComponent>()) {
					mCachedCameraTransform = camera->GetRelativeTransform();
					camera->DetachFromParent(true);
					mCachedCamera = camera;
				}
			}
		}
	}
}

TransformData::TransformData(FVector lookAt, TransformTarget start, TransformTarget end)
	: mLookAtLocation(std::move(lookAt))
	, mStartPosition(std::move(start))
	, mEndPosition(std::move(end)) {
}

TransformData::TransformData(TransformTarget start, TransformTarget end) 
	: mStartPosition(std::move(start))
	, mEndPosition(std::move(end)) {
}

TransformData::TransformData() {
}

PanningSettings::PanningSettings(bool invulnerable) 
	: mMakePlayerInvulnerable(invulnerable) {
}

MoveInterpolationInfo::MoveInterpolationInfo() 
	: Method(MoveInterpolationMethod::Linear)
	, Exponent(2.0f) {
}

MoveInterpolationInfo::MoveInterpolationInfo(MoveInterpolationMethod method, float exp) 
	: Method(method)
	, Exponent(exp) {
}

TransformTarget::TransformTarget(AActor* actor, FVector offset) 
	: mActor(actor)
	, mOffset(offset){
}

TransformTarget::TransformTarget(FVector position, FVector offset) 
	: mPosition(position)
	, mOffset(offset) {
}

TransformTarget::TransformTarget() {
}

FVector TransformTarget::GetPosition() const {
	return mActor ? mActor.GetValue()->GetActorLocation() + mOffset : mPosition + mOffset;
}

ObjectiveEvent::ObjectiveEvent() {
}

ObjectiveEvent::ObjectiveEvent(FObjectiveEventLocation eventLocation, TOptional<float> sequenceDuration) 
	: mEventLocation(std::move(eventLocation))
	, mSequenceDuration(sequenceDuration) {
}

}}
