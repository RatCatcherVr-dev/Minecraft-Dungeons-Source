#pragma once

#include "BtTypes.h"
#include "BtRunResult.h"
#include "util/IntChanged.h"
#include "BtNodeLogger.h"

class UBtNode {
public:
	
	virtual ~UBtNode();

	bool IsRunning() const;
	EBtRunResult Run(FBtEvalState&);


	void InitialiseNode(FBtEvalState&);
	bool DoWillRunCheck(FBtEvalState&);
	bool DoPredicateExtraRunCheck(FBtEvalState&);
	EBtRunResult TickPostWillRunCheck(FBtEvalState&);

	void Stop(bt::StateRef);

	bool _WillRun(bt::StateRef);
	void _Tick(bt::StateRef);
	void _Initialise(bt::StateRef);

// public state
	void SetName(const FString& name);
	FString name;
	UBtNode* parent = nullptr;
	bool isCurrentlyLatent = false;


	virtual FString ToString() const;
protected:
	virtual bool OnWillRun(bt::StateRef) = 0;
	virtual void OnTick(bt::StateRef) {}
	virtual void OnStart(bt::StateRef) {}
	virtual void OnStop(bt::StateRef) {}
	virtual void OnInitialise(bt::StateRef) {}

	virtual void FillData(FBtEvalTraversalState&) {}
private:
	void _Start(bt::StateRef);

protected:
	bool isGroup = false;
	FString type;
private:
	EBtRunResult runState = EBtRunResult::Stopped;
	friend class UBtNodeLogger;

	bool willRun = false;
	IntChanged lastWillRunTick;
	IntChanged lastRunTick;
	IntChanged lastStartTick;
	IntChanged lastTickTick;
	IntChanged lastStopTick;
	STAT(TStatId tickStatId;)
	STAT(TStatId willRunStatId;)
};
