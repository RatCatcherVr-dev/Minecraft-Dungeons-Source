#pragma once
#include <WeakObjectPtrTemplates.h>
#include "CommonTypes.h"
#include "game/objective/EventTypes.h"

class AScreenFader;
class ADungeonsPlayerCameraManager;
class APlayerCharacter;
class ULovikaSpringArmComponent;

namespace game { namespace objective {

const FRotator desiredCameraRotation = FRotator(-45.f, 45.f, 0);
const FVector springArmOffset = FVector(0, 0, -80.f);

FVector GetCameraOffset(APlayerCharacter*);

class PanningAction {
public:
	PanningAction();
	PanningAction(float actionStartOffset, float duration, std::string name);
	virtual ~PanningAction() {}
	bool tryStart(float);
	void onTickReceived(float);

	void setWorld(UWorld&);

	float duration() const;
	bool isCompleted() const;
	bool isStarted() const;
	float actionStartOffset() const;
	float startTimeStamp() const;
	const std::string& name() const;
protected:
	virtual void onStart() {};
	virtual void onTick(float) {};

	UWorld* mWorld;
private:
	float mDuration;
	bool mIsCompleted = false;
	bool mIsStarted = false;
	float mActionStartOffset;
	float mStartTimeStamp = 0.0f;
	std::string mName;
};

class FadeAction : public PanningAction {
public:
	FadeAction(float, float, std::string, bool);
	void onStart() override;
private:
	void onFadeComplete();
	TWeakObjectPtr<AScreenFader> mFader;
	bool mFadeIn = false;
};

enum class MoveInterpolationMethod {
	Linear, 
	EaseIn,
	EaseOut
};

struct MoveInterpolationInfo {
	MoveInterpolationMethod Method;
	float Exponent;

	MoveInterpolationInfo();
	MoveInterpolationInfo(MoveInterpolationMethod, float exp = 2.0f);
};

struct TransformTarget {
	TransformTarget();
	TransformTarget(AActor* actor, FVector offset = FVector::ZeroVector);
	TransformTarget(FVector position, FVector offset = FVector::ZeroVector);

	FVector GetPosition() const;
private:
	FVector mOffset;
	FVector mPosition;
	TOptional<AActor*> mActor;
};

struct TransformData {
	TOptional<FVector> mLookAtLocation;
	TransformTarget mStartPosition;
	TransformTarget mEndPosition;

	TransformData();
	TransformData(FVector lookAt, TransformTarget start, TransformTarget end);
	TransformData(TransformTarget start, TransformTarget end);
};

class TransformAction : public PanningAction {
public:
	TransformAction();
	TransformAction(float, float, std::string, TransformData, MoveInterpolationInfo);
	void onTick(float) override;
private:
	TransformData mTransformData;
	FVector mTotalDistance;
	MoveInterpolationInfo mInterpolationInfo;
};


class PanningSequence {
public:
	PanningSequence();

	void onStart(UWorld&);
	void onTick(float);
	bool isComplete() const;
	void addAction(UWorld&, Unique<PanningAction> action);
	void reset();

	std::function<void(const PanningAction&)> mOnActionStarted;
	std::function<void(const PanningAction&)> mOnActionFinished;
	std::function<void()> mOnSequenceStarted;
	std::function<void()> mOnSequenceComplete;

private:
	void onEnd();
	void tryStartActions();
	void onActionStarted(const PanningAction&);
	void onActionComplete(const PanningAction&);
	bool isAllCompleted() const;

	bool mIsStarted = false;
	bool mIsComplete = false;
	std::vector<std::pair<Unique<PanningAction>, bool>> mPanningActions;
	float mStartTimeStamp;
};

struct PanningSettings {
	PanningSettings(bool invulnerable);

	bool mMakePlayerInvulnerable = false;
};

struct ObjectiveEvent {
	ObjectiveEvent();
	ObjectiveEvent(FObjectiveEventLocation eventLocation, TOptional<float> sequenceDuration);

	FObjectiveEventLocation mEventLocation;
	TOptional<PanningSequence> mSequence;
	TOptional<float> mSequenceDuration;
};

class ObjectiveEventQueue {
public:
	void add(FObjectiveEventLocation eventLocation, TOptional<float> sequenceDuration);
	bool onStart(UWorld&, APlayerCharacter*);
	void onTick(float deltaTime);
	bool isActive() const;
	void reset();

	std::function<void()> mOnQueueCompleted;
private:
	void onEventCompleted();
	void initEvent(ObjectiveEvent&);
	bool tryAddSequence(ObjectiveEvent&);
	void next();
	void notifyEvent(const ObjectiveEvent&);

	std::queue<ObjectiveEvent> mObjectiveEvents;
	UWorld* mWorld;
	TWeakObjectPtr<APlayerCharacter> mPlayerCharacter;
	bool mIsStarted = false;
};

class ObjectiveEventHandler {
public:
	void tryStart(UWorld&, PanningSettings);
	void start(UWorld&, PanningSettings);
	void onTick(float);

	std::function<void()> mOnCameraPanningStarted;
	std::function<void()> mOnCameraPanningComplete;

	void queuePanEvent(FObjectiveEventLocation, TOptional<float> sequenceDuration);

	bool isActive() const;
private:
	void togglePlayerInvulnerability(bool);
	void toggleSpringArmAttachment(bool);
	void onEnd();
	UWorld* mWorld;
	TWeakObjectPtr<ULovikaSpringArmComponent> mCachedSpringArm;
	TWeakObjectPtr<UCameraComponent> mCachedCamera;
	FTransform mCachedCameraTransform;
	TOptional<PanningSettings> mSettings;
	ObjectiveEventQueue mObjectiveEventQueue;
};

}}
