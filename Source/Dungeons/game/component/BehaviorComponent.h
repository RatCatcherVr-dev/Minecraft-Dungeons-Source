#pragma once

#include "Components/ActorComponent.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/bt/BtNode.h"
#include "game/ai/bt/BtEvalState.h"
#include "CommonTypes.h"
#include "GameplayTagContainer.h"
#include "BehaviorComponent.generated.h"

class AMobCharacter;
struct FOnAttributeChangeData;

//
// This class and interface is temporary until we solve the "multiple trees
// per mob" issue in a cleaner way. Right now, ALL TREE EXECUTIONS NEEDS TO
// GO VIA _PreUpdate, {run tree}, _PostUpdate. This is correctly handled when
// calling UBehaviorComponent.Update, and what's why it's a friend class. / AN
//
class UBehaviorStack {
public:
	using RemovalKey = int;

	UBehaviorStack(UWorld&, const bool& enabled);

	RemovalKey  Push(Unique<UBtNode>);
	RemovalKey  PushTimed(Unique<UBtNode>, bt::Duration);
	bool        Remove(RemovalKey);
	bool        Remove(const UBtNode*);
	void        InitialiseNodes(FBtEvalState&);
	void        UpdateFromWillRunCheck(FBtEvalState&);
	void        PerformWillRunCheck(FBtEvalState&);
	void        Stop(FBtEvalState&);
private:
	struct TreeItem {
		Unique<UBtNode> node;
		bool removePending;
		RemovalKey removalKey;
		TOptional<float> removalTime;
	};
	bool _CanRemove(struct TreeItem&) const;
	template <typename Pred>
	bool _Remove(Pred pred);
	void _PreUpdate();
	UBtNode* _Current();
	
	void _PostUpdate();
	void _Clear();

	UWorld& mWorld;
	const bool& mEnabled;
	bool mIsUpdating;
	TArray<TreeItem> mBehaviors;
	TQueue< TreeItem* > mTickNodeQueue;

	static RemovalKey currentRemovalKey;
	friend class UBehaviorComponent;
	friend class UDebugComponent;
};

template <typename Pred>
bool UBehaviorStack::_Remove(Pred pred) {
	bool found = false;
	for (auto&& item : mBehaviors) {
		if (!item.removePending && pred(item)) {
			item.removePending = true;
			found = true;
		}
	}
	return found;
}

enum class EStunState {
	None,
	Pending,
	Stunned, 
	Recovered
};

enum class EBubbledState {
	None,
	Pending,
	Bubbled,
	Recovered
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), DisplayName = "BehaviorTree")
class DUNGEONS_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UBehaviorComponent();

	UBehaviorStack& Actions() const;
	UBehaviorStack& Targets() const;

	bool Remove(UBehaviorStack::RemovalKey) const;
	bool Remove(const UBtNode*) const;
	void Clear(); // @btlifecycle

	bool IsEnabled() const;
	void SetEnabled(bool enabled);

	void InitialiseBehaviourNodes();
	void UpdateStunnedState();
	void UpdateBubbledState();

	void UpdateFromWillRunChecks(bool bDoTargetTree);	
	void PerformWillRunChecks(bool bDoTargetTree);


	void CreateCurrentBehaviourEvalState(AMobCharacter&, int tickId);
	
	void Stop(FBtEvalState& state);

	void InitializeComponent() override;

	bool BehaviorCountDown();
	void ResetBehaviorCountDown();
	
	void SetLastWarning(AActor* threateningActor);

private:

	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

	void SetLastDamagedBy(AActor* byWhom, FVector fromLocation, FGameplayTagContainer gameplayTagContainer);

	void OnThreateningActor(AActor& sourceOfWarning);

	UFUNCTION()
	void OnStun(const FGameplayTag tag, int32 tagCount);

	UFUNCTION()
	void OnBubbled(const FGameplayTag tag, int32 tagCount);

	UWorld* mWorld = nullptr;
	bool mEnabled = true;

	FBtEvalState mCurrentBehaviourEvalState;
	Unique<UBehaviorStack> mActions, mTarget;

	struct FMobParams* mobParams = nullptr;
	
	EStunState stunState = EStunState::None;
	EBubbledState bubbleState = EBubbledState::None;

	//Behaviour count down (behavior update is only triggered if this is 0)
	uint8			mBehaviourCountDown = 0;
};
