#include "Dungeons.h"
#include "BtInvert.h"

bool UBtInvert::OnWillRun(bt::StateRef state) {
	return !children[0]->_WillRun(state);
}
