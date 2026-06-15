#pragma once

#include "game/ai/bt/BtNode.h"
#include "game/ai/bt/BtNodeLogger.h"
#include "game/ai/bt/BtTypes.h"
#include "CommonTypes.h"

class UBtGroup : public UBtNode {
public:
	UBtGroup();
	void AddChild(Unique<UBtNode>);

	

protected:
	void OnStop(bt::StateRef) override;
	virtual void OnInitialise(bt::StateRef) override;

	std::vector<Unique<UBtNode>> children;
private:
	friend class UBtNodeLogger;
};


void handleParam(UBtGroup&, const bt::Action&);
void handleParam(UBtGroup&, const bt::Pred&);
void handleParam(UBtGroup&, std::initializer_list<bt::Action>);
