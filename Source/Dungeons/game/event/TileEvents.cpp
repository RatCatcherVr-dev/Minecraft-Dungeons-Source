#include "Dungeons.h"
#include "TileEvents.h"

namespace game { namespace events {

TileEvents::TileEvents()
	: mRunningId(0) {
}

void TileEvents::unregister(int registrationId) {
	//@todo: sigh! lookup to get list? single list with unions? (and binary search)?
	_unregisterFrom(mTileEnter, registrationId);
	_unregisterFrom(mTileEnterNew, registrationId);
	_unregisterFrom(mTileLeave, registrationId);
	_unregisterFrom(mTileTransition, registrationId);
	_unregisterFrom(mStretchEnter, registrationId);
	_unregisterFrom(mStretchEnterNew, registrationId);
	_unregisterFrom(mStretchLeave, registrationId);
	_unregisterFrom(mStretchTransition, registrationId);
}

void TileEvents::unregister(std::vector<int> registrationIds) {
	for (auto id : registrationIds) {
		unregister(id);
	}
}

int TileEvents::tileEnter(const TileFunc& f) {
	int id = _nextId();
	return mTileEnter.emplace_back(f, id), id;
}

int TileEvents::tileEnterNew(const TileFunc& f) {
	int id = _nextId();
	return mTileEnterNew.emplace_back(f, id), id;
}

int TileEvents::tileLeave(const TileFunc& f) {
	int id = _nextId();
	return mTileLeave.emplace_back(f, id), id;
}

int TileEvents::tileTransition(const TileTransitionFunc& f) {
	int id = _nextId();
	return mTileTransition.emplace_back(f, id), id;
}

int TileEvents::stretchEnter(const StretchFunc& f) {
	int id = _nextId();
	return mStretchEnter.emplace_back(f, id), id;
}

int TileEvents::stretchEnterNew(const StretchFunc& f) {
	int id = _nextId();
	return mStretchEnterNew.emplace_back(f, id), id;
}

int TileEvents::stretchLeave(const StretchFunc& f) {
	int id = _nextId();
	return mStretchLeave.emplace_back(f, id), id;
}

int TileEvents::stretchTransition(const StretchTransitionFunc& f) {
	int id = _nextId();
	return mStretchTransition.emplace_back(f, id), id;
}

int TileEvents::_nextId() {
	return ++mRunningId;
}

}}
