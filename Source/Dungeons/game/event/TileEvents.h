#pragma once

#include <utility>

class APlayerCharacter;

namespace generator {
struct Stretch;
}

namespace game {
class Tile;

namespace events {
class TileEventDispatcher;

struct TileState { const Tile& tile; APlayerCharacter& player; };
struct TileTransitionState { const Tile* from; const Tile* to; bool isFirstVisit; APlayerCharacter& player; };
struct StretchState { const generator::Stretch& stretch; APlayerCharacter& player; };
struct StretchTransitionState { const generator::Stretch* from; const generator::Stretch* to; bool isFirstVisit; APlayerCharacter& player; };

using TileFunc = std::function<void(TileState)>;
using TileTransitionFunc = std::function<void(TileTransitionState)>;
using StretchFunc = std::function<void(StretchState)>;
using StretchTransitionFunc = std::function<void(StretchTransitionState)>;

class TileEvents {
	template <typename T>
	using Reg = std::pair<T, int>;
	friend class TileEventDispatcher;

	TileEvents();
public:
	void unregister(int registrationId);
	void unregister(std::vector<int> registrationIds);

	int tileEnter(const TileFunc&);
	int tileEnterNew(const TileFunc&);
	int tileLeave(const TileFunc&);
	int tileTransition(const TileTransitionFunc&);

	int stretchEnter(const StretchFunc&);
	int stretchEnterNew(const StretchFunc&);
	int stretchLeave(const StretchFunc&);
	int stretchTransition(const StretchTransitionFunc&);
private:
	int _nextId();

	template <typename T>
	inline void _unregisterFrom(std::vector<T>& v, int registrationId) {
		v.erase(std::remove_if(begin(v), end(v), [=](auto reg) { return registrationId == reg.second; }));
	}
	template <typename T>
	void _dispatch(const std::vector<Reg<std::function<void(T)>>>& listeners, const T& state) {
		std::for_each(begin(listeners), end(listeners), [=](const auto& f) { f.first(state); });
	}

	std::vector<Reg<TileFunc>> mTileEnter, mTileEnterNew, mTileLeave;
	std::vector<Reg<TileTransitionFunc>> mTileTransition;
	std::vector<Reg<StretchFunc>> mStretchEnter, mStretchEnterNew, mStretchLeave;
	std::vector<Reg<StretchTransitionFunc>> mStretchTransition;

	int mRunningId;
};

}}
