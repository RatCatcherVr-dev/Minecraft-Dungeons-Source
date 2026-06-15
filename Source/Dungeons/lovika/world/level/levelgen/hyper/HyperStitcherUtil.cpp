#include "Dungeons.h"
#include "HyperStitcherUtil.h"
#include "lovika/io/LevelFile.h"

namespace levelgen { namespace hajper {

template <typename DstCollection, typename SrcCollection, typename LessOf>
void appendAllAndAssureUnique(DstCollection& dst, const SrcCollection& src, const LessOf& lessOf) {
	auto out = dst;
	algo::append_all(out, src);
	dst = algo::unique_by_less_of(out, lessOf);
}

template <typename DstCollection, typename SrcCollection>
void appendAllAndAssureUnique(DstCollection& dst, const SrcCollection& src) {
	return appendAllAndAssureUnique(dst, src, RETLAMBDA(it));
}

void appendLevel(io::Level& dst, const io::Level& src, bool isPlayedLevel) {
	if (isPlayedLevel) {
		algo::append_all(dst.passThrough.definedLevelIds, src.passThrough.definedLevelIds);
		algo::append_all(dst.passThrough.objectives, src.passThrough.objectives);
	}
	appendAllAndAssureUnique(dst.tileDefs, src.tileDefs, RETLAMBDA(it.lowerId));
	appendAllAndAssureUnique(dst.propDefs, src.propDefs, RETLAMBDA(it.lowerId));
	appendAllAndAssureUnique(dst.objectGroups, src.objectGroups, RETLAMBDA(it.path.lowerId));
	appendAllAndAssureUnique(dst.mobGroups, src.mobGroups, RETLAMBDA(it.lowerId));
	appendAllAndAssureUnique(dst.mobGroupFileIds, src.mobGroupFileIds, RETLAMBDA(it.lowerId));

	// Since we're appending independent levels, we need to rewrite all indices
	// (e.g. io::Dungeon::index, io::Stretch::dungeonIndex, @todo objective index)
	const int dungeonSrcStartIndex = dst.dungeons.size();
	const int dungeonIndexOffset = dst.dungeons.empty() ? 0 : dst.dungeons.back().index + 1;
	algo::append_all(dst.dungeons, src.dungeons);
	for (auto i = dungeonSrcStartIndex; i < dst.dungeons.size(); ++i) {
		dst.dungeons[i].index += dungeonIndexOffset;
		for (auto& stretch : dst.dungeons[i].stretches) {
			stretch.dungeonIndex += dungeonIndexOffset;
		}
	}
	assign_all(dst.passThrough.dungeons, dst.dungeons);
	assign_all(dst.passThrough.objectGroups, dst.objectGroups);
}

}}
