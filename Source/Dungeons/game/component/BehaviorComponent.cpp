#include "Dungeons.h"
#include "BehaviorComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "HealthComponent.h"
#include "MobAnimationsComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/action/BtAction.h"
#include "game/ai/bt/BtEvalState.h"
#include <Array.h>
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

//
// UBehaviorStack
//
UBehaviorStack::RemovalKey UBehaviorStack::currentRemovalKey = 0;

UBehaviorStack::UBehaviorStack(UWorld& world, const bool& enabled)
	: mWorld(world)
	, mEnabled(enabled)
	, mIsUpdating(false) {
}

bool UBehaviorStack::_CanRemove(TreeItem& tree) const {
	return tree.removalTime && mWorld.GetTimeSeconds() >= mBehaviors.Last().removalTime.GetValue();
}

UBehaviorStack::RemovalKey UBehaviorStack::Push(Unique<UBtNode> tree) {
	const RemovalKey removalKey = ++currentRemovalKey;

	mBehaviors.Add({
		std::move(tree),
		false,
		removalKey
	});
	return removalKey;
}

UBehaviorStack::RemovalKey UBehaviorStack::PushTimed(Unique<UBtNode> tree, bt::Duration duration) {
	auto removalKey = Push(std::move(tree));
	mBehaviors.Last().removalTime = mWorld.GetTimeSeconds() + duration.Seconds();
	return removalKey;
}

bool UBehaviorStack::Remove(RemovalKey key) {
	return _Remove([key](auto&& it) { return key == it.removalKey; });
}

bool UBehaviorStack::Remove(const UBtNode* node) {
	return _Remove([node](auto&& it) { return node == it.node.get(); });
}

void UBehaviorStack::InitialiseNodes(FBtEvalState& state) {
	if (!mEnabled)
	{
		return;
	}

	for (auto& BehaviourTree : mBehaviors)
	{
		BehaviourTree.node->InitialiseNode(state);
	}
	
}

void UBehaviorStack::UpdateFromWillRunCheck(FBtEvalState& state) {
	if (!mEnabled) {
		return;
	}

	_PreUpdate();

	TreeItem* DequeueDataInst;

	//Dequeue any trees that have been queued to execute (may have been done on another thread)
	while (mTickNodeQueue.Dequeue(DequeueDataInst))
	{
		DequeueDataInst->node->TickPostWillRunCheck(state);
	}
	
	_PostUpdate();
}

void UBehaviorStack::PerformWillRunCheck(FBtEvalState& state) {
	if (!mEnabled) 
	{
		return;
	}

	for (int i = mBehaviors.Num() - 1; i >= 0; --i)
	{
		if (mBehaviors[i].node->DoWillRunCheck(state))
		{
			//we can run, lets push the node to be ticked from the game thread
			mTickNodeQueue.Enqueue(&mBehaviors[i]);
			break;
		}
		else if (mBehaviors[i].node->IsRunning())
		{
			//Enqueue the running tick as it may need to shut itself down
			mTickNodeQueue.Enqueue(&mBehaviors[i]);
		}
	}
}

void UBehaviorStack::Stop(FBtEvalState& state) {
	for (int i = mBehaviors.Num() - 1; i >= 0; --i) {
		if (mBehaviors[i].node->IsRunning()) {
			mBehaviors[i].node->Stop(state);
			break;
		}
	}
}

void UBehaviorStack::_PreUpdate() {
	mIsUpdating = true;
	while (mBehaviors.Num() && _CanRemove(mBehaviors.Last())) {
		mTickNodeQueue.Empty(); //best empty this in case our behaviour tree has ended up in  here ready to be ticked
		mBehaviors.Pop(false);
	}
}

UBtNode* UBehaviorStack::_Current() {
	return (mEnabled && mBehaviors.Num() > 0) ? mBehaviors.Last().node.get() : nullptr;
}

void UBehaviorStack::_PostUpdate() {
	mBehaviors.RemoveAll([](TreeItem& it) { return it.removePending; });
	mIsUpdating = false;
}

void UBehaviorStack::_Clear() {

	mTickNodeQueue.Empty(); //clear out ticking behaviour trees

	if (!mIsUpdating) {
		mBehaviors.Empty();
		return;
	}
	for (auto&& it : mBehaviors) {
		it.removePending = true; // If we're updating, remove them afterwards
	}
}


//
// Warn console variables
//
TAutoConsoleVariable<int32> CVarMobsWarnEnabled(
	TEXT("Dungeons.Mob.Warn.Enabled"),
	1,
	TEXT("Should mobs warn surrounding allies if they are hurt or killed.\n")
	TEXT("<=0: off.\n")
	TEXT(">=1: on.\n"),
	ECVF_Cheat
);
TAutoConsoleVariable<float> CVarMobsWarnCheckDelaySeconds(
	TEXT("Dungeons.Mob.Warn.CheckDelaySeconds"),
	20.0f,
	TEXT("The min delay between checking if an threatening actors should warn friendly mobs.\n"),
	ECVF_Cheat
);

TAutoConsoleVariable<float> CVarMobsWarnIfWarnedDelaySeconds(
	TEXT("Dungeons.Mob.Warn.IfWarnedDelaySeconds"),
	2.0f,
	TEXT("The min delay between being warned, and also being able to issue a warning to friendly mobs if attacked yourself.\n"),
	ECVF_Cheat
);

TAutoConsoleVariable<float> CVarMobsWarnRadius(
	TEXT("Dungeons.Mob.Warn.Radius"),
	750.0f,
	TEXT("When a mob is attacked, they will warn surrounding allies that are within the radius of itself.\n"),
	ECVF_Cheat
);


//
// UBehaviorComponent
//
UBehaviorComponent::UBehaviorComponent() {
	bWantsInitializeComponent = true;
}

UBehaviorStack& UBehaviorComponent::Actions() const {
	return *mActions;
}

UBehaviorStack& UBehaviorComponent::Targets() const {
	return *mTarget;
}

bool UBehaviorComponent::Remove(UBehaviorStack::RemovalKey key) const {
	return mActions->Remove(key) || mTarget->Remove(key);
}

bool UBehaviorComponent::Remove(const UBtNode* node) const {
	return node && (mActions->Remove(node) || mTarget->Remove(node));
}

void UBehaviorComponent::Clear() {
	(*mActions)._Clear();
	(*mTarget)._Clear();
}

bool UBehaviorComponent::IsEnabled() const {
	return mEnabled;
}

void UBehaviorComponent::SetEnabled(bool enabled) {
	mEnabled = enabled;
}


void UBehaviorComponent::InitialiseBehaviourNodes()
{
	mTarget->InitialiseNodes(mCurrentBehaviourEvalState);
	mActions->InitialiseNodes(mCurrentBehaviourEvalState);
}

void UBehaviorComponent::UpdateStunnedState()
{
	switch (stunState)
	{
	case EStunState::Stunned:
	{
		return; //return on stunned
	}

	case EStunState::Pending:
	{
		stunState = EStunState::Stunned;

		//stop all behaviour and play stun anim on stun pending

		mTarget->Stop(mCurrentBehaviourEvalState);
		mActions->Stop(mCurrentBehaviourEvalState);

		if (auto anim = mCurrentBehaviourEvalState.anim()) {
			if (auto sequence = mCurrentBehaviourEvalState.animPack()->Common.Stunned) {
				// some stun animation should play instead of novelty
				mCurrentBehaviourEvalState.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")), 0.f, 0.2, 1.f, -1, 0, FMath::FRandRange(0, sequence->GetPlayLength()));
			}
		}

		return;

	}

	case EStunState::Recovered:
	{
		//recovered stun, stop anim and proceed with behavior updates
		mCurrentBehaviourEvalState.owner->MulticastStopSlotAnimation();
		stunState = EStunState::None;
		break;
	}

	default:break;
	}
}

void UBehaviorComponent::UpdateBubbledState()
{
	switch (bubbleState)
	{
	case EBubbledState::Bubbled:
	{
		return; //return on stunned
	}

	case EBubbledState::Pending:
	{
		bubbleState = EBubbledState::Bubbled;

		//stop all behaviour and play stun anim on stun pending

		mTarget->Stop(mCurrentBehaviourEvalState);
		mActions->Stop(mCurrentBehaviourEvalState);

		return;

	}

	case EBubbledState::Recovered:
	{
		//recovered stun, stop anim and proceed with behavior updates
		mCurrentBehaviourEvalState.owner->MulticastStopSlotAnimation();
		bubbleState = EBubbledState::None;
		break;
	}

	default:break;
	}
}

void UBehaviorComponent::UpdateFromWillRunChecks(bool bDoTargetTree)
{	
	if (stunState == EStunState::Stunned || bubbleState == EBubbledState::Bubbled) { return; }

	if (bDoTargetTree)
	{
		mTarget->UpdateFromWillRunCheck(mCurrentBehaviourEvalState);
	}
	else
	{
		mActions->UpdateFromWillRunCheck(mCurrentBehaviourEvalState);
	}
}

void UBehaviorComponent::PerformWillRunChecks(bool bDoTargetTree) {
	if (stunState == EStunState::Stunned) {	return;	}

	if (bDoTargetTree)
	{
		mTarget->PerformWillRunCheck(mCurrentBehaviourEvalState);
	}
	else
	{
		mActions->PerformWillRunCheck(mCurrentBehaviourEvalState);
	}
	
}



void UBehaviorComponent::CreateCurrentBehaviourEvalState(AMobCharacter& mob, int tickId)
{
	mCurrentBehaviourEvalState = bt::createState(mob, tickId);
}


void UBehaviorComponent::InitializeComponent() {
	Super::InitializeComponent();
	if (GetOwnerRole() != ROLE_Authority) return;

	mWorld = GetOwner()->GetWorld();

	mActions = make_unique<UBehaviorStack>(*mWorld, mEnabled);
	mTarget = make_unique<UBehaviorStack>(*mWorld, mEnabled);

	if (auto* mob = Cast<AMobCharacter>(GetOwner())) {
		mobParams = &mob->MobParams;

		if (auto* abilitySystem = mob->GetAbilitySystemComponent()) {
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(this, &UBehaviorComponent::OnAttributeHealthChange);
			abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned")).AddUObject(this, &UBehaviorComponent::OnStun);
			abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Bubbled")).AddUObject(this, &UBehaviorComponent::OnBubbled);
		}
	}	
}

bool UBehaviorComponent::BehaviorCountDown()
{
	if (mBehaviourCountDown > 0)
	{
		--mBehaviourCountDown;
	}
	return (mBehaviourCountDown == 0);
}

void UBehaviorComponent::ResetBehaviorCountDown()
{
	mBehaviourCountDown = 1 + (FMath::Rand() % 3); //random 1-3
}

void UBehaviorComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	if (data.OldValue - data.NewValue > SMALL_NUMBER) {
		if (data.GEModData) {
			const FGameplayEffectSpec& effectSpec = data.GEModData->EffectSpec;
			const FGameplayEffectContextHandle& handle = effectSpec.GetEffectContext();
			
			auto instigator = handle.GetInstigator();
			SetLastDamagedBy(instigator, handle.GetOrigin(), effects::GetDungeonsDamageTypes(*data.GEModData));
			if (instigator && CVarMobsWarnEnabled.GetValueOnGameThread()) OnThreateningActor(*instigator);
		}
	}
}

void UBehaviorComponent::SetLastDamagedBy(AActor* byWhom, FVector fromLocation, FGameplayTagContainer gameplayTagContainer) {
	if(mobParams){
		mobParams->lastDamaged = {
			byWhom,
			fromLocation,
			bt::TimeStamp::Now(mCurrentBehaviourEvalState),
			gameplayTagContainer
		};
	}
}

void UBehaviorComponent::SetLastWarning(AActor* threateningActor) {
	if (mobParams) {
		mobParams->lastWarning = {
			threateningActor,
			bt::TimeStamp::Now(mCurrentBehaviourEvalState)
		};
	}
}

void UBehaviorComponent::OnThreateningActor(AActor& sourceOfThreat) {
	if (const AMobCharacter* owner = Cast<AMobCharacter>(GetOwner())) {
		bt::StateRef state = mCurrentBehaviourEvalState;

		//Stop this check from happening too often.
		if (!mobParams->canWarnTime.IsPassed(state)) {
			return;
		}

		//Always trigger delay, no matter the outcome of the check.
		//Only try to do this quite rarely, we do not need this happen for all mobs.
		mobParams->canWarnTime = bt::TimeStamp::SecondsFromNow(state, CVarMobsWarnCheckDelaySeconds.GetValueOnGameThread());

		//Stop clusters of warnings from happening during the same attack : We trade some mobs which could be warned for performance.
		if (mobParams->lastWarning && (!mobParams->lastWarning->timeStamp.IsPassed(state, bt::Seconds(CVarMobsWarnIfWarnedDelaySeconds.GetValueOnGameThread())))) {
			return;
		}
	
		const float notifyRadius = CVarMobsWarnRadius.GetValueOnGameThread();

		
		// 0.0 means as soon as we enter offensive range, we stop caring about warning allies of threats.
		// 1.0 means we warn even if only a tiny sliver of the warning radius area is outside the offensive range.
		const float minWarnRadiusFractionWhichIsGuaranteed = 0.5f;
				
		//This will knowingly miss some mobs if they have different offensiveRanges - It is ok - this feature does not depend on being 100% exact.
		const float minDistanceWhereWeCareAboutWarningAllies = owner->OffensiveRange - notifyRadius * minWarnRadiusFractionWhichIsGuaranteed;
		const float distanceLimit2 = FMath::Square(minDistanceWhereWeCareAboutWarningAllies);
		if ((sourceOfThreat.GetActorLocation() - owner->GetActorLocation()).SizeSquared() <= distanceLimit2) {
			return;
		}

		for (auto mob : actorquery::getNearbyInstanceTrackedActors<AMobCharacter>(owner, notifyRadius)) {
			if (actorquery::is::alive(mob) && characterquery::is::friendly(owner, mob)) {
				mob->IssueWarning(&sourceOfThreat);
			}
		}
	}	
}

void UBehaviorComponent::OnStun(const FGameplayTag tag, const int32 tagCount) {
	if (tagCount > 0) {
		stunState = stunState != EStunState::Stunned ? EStunState::Pending : stunState;
	}
	else if (stunState != EStunState::None)
	{
		stunState = EStunState::Recovered; //recovered from stun
	}
	
}

void UBehaviorComponent::OnBubbled(const FGameplayTag tag, int32 tagCount)
{
	if (tagCount > 0) 
	{
		bubbleState = bubbleState != EBubbledState::Bubbled ? EBubbledState::Pending : bubbleState;
	}
	else if (bubbleState != EBubbledState::None)
	{
		bubbleState = EBubbledState::Recovered; //recovered from bubbled
	}
}
