#include "Dungeons.h"
#include "BtNode.h"

DECLARE_CYCLE_STAT(TEXT("BtRun"), STAT_BtNodeRun, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("BtThreadedWillRun"), STAT_BtThreadedWillRun, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("BtRunGameThread"), STAT_BtNodeRunGameThread, STATGROUP_AI);
DECLARE_CYCLE_STAT(TEXT("BtInitialiseNode"), STAT_BtInitialiseNode, STATGROUP_AI);


UBtNode::~UBtNode() {
}

void UBtNode::SetName(const FString& nodeName) {
	ensure(name.IsEmpty() && "BtNode name is already set");
	name = nodeName;
	STAT(tickStatId = FDynamicStats::CreateStatId<FStatGroup_STATGROUP_AI>(name + "_tick");)
	STAT(willRunStatId = FDynamicStats::CreateStatId<FStatGroup_STATGROUP_AI>(name + "_willRun");)
}

bool UBtNode::IsRunning() const {
	return runState == EBtRunResult::Running ;
}


EBtRunResult UBtNode::Run(FBtEvalState& state) {
	SCOPE_CYCLE_COUNTER(STAT_BtNodeRun);

	if (!lastRunTick.update(state.tick)) {
		return runState;
	}
	state.root = this;

	_WillRun(state);
	_Tick(state);

	if (willRun) { // @note @ai: Really needed?
		runState = EBtRunResult::Running;
	} else {
		runState = EBtRunResult::Stopped;
	}
	return runState;
}

EBtRunResult UBtNode::TickPostWillRunCheck(FBtEvalState& state) {
	SCOPE_CYCLE_COUNTER(STAT_BtNodeRunGameThread);

	if (!lastRunTick.update(state.tick)) {
		return runState;
	}
	state.root = this;

	_Tick(state);

	return runState;
}

bool UBtNode::DoWillRunCheck(FBtEvalState& state) {
	SCOPE_CYCLE_COUNTER(STAT_BtThreadedWillRun);

	if (lastRunTick.value == state.tick) 
	{
		return runState != EBtRunResult::Stopped;
	}

	state.root = this;
	
	_WillRun(state);

	return willRun;
}

void UBtNode::InitialiseNode(FBtEvalState& state) {
	SCOPE_CYCLE_COUNTER(STAT_BtInitialiseNode);
	state.root = this;
	_Initialise(state);
}


bool UBtNode::_WillRun(bt::StateRef state) {
	STAT(FScopeCycleCounter scopeCounter(willRunStatId);)

	if (!lastWillRunTick.update(state.tick)) {
		return willRun;
	}
	willRun = OnWillRun(state);
	return willRun;
}

void UBtNode::_Tick(bt::StateRef state) {
	STAT(FScopeCycleCounter scopeCounter(tickStatId);)

	if (!lastTickTick.update(state.tick)) {
		return;
	}

	auto oldTraversalState = state.traversalState;
	FillData(state.traversalState);

	if (willRun) {
		if (!IsRunning()) {
			_Start(state);
		}
		OnTick(state);
	} else {
		if (IsRunning()) {
			Stop(state);
		}
	}
	state.traversalState = oldTraversalState;
}



void UBtNode::_Initialise(bt::StateRef state)
{
	OnInitialise(state);
}

FString UBtNode::ToString() const {
	if (name.IsEmpty()) {
		return type;
	}
	FString out = name;
	out.AppendChar(' ');
	out.AppendChar('(');
	out.Append(type);
	out.AppendChar(')');
	return out;
}

void UBtNode::_Start(bt::StateRef state) {
	if (!lastStartTick.update(state.tick)) {
		return;
	}
	runState = EBtRunResult::Running;
	OnStart(state);
}

void UBtNode::Stop(bt::StateRef state) {
	if (!IsRunning()) {
		return;
	}
	if (isCurrentlyLatent) {
		return;
	}
	if (!lastStopTick.update(state.tick)) {
		return;
	}
	runState = EBtRunResult::Stopped;
	isCurrentlyLatent = false;
	OnStop(state);
}
