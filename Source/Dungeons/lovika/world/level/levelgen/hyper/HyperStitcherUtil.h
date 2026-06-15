#pragma once

#include "util/Algo.h"

namespace io { struct Level; }

namespace levelgen { namespace hajper {

template <typename DstCollection, typename SrcCollection>
void assign_all(DstCollection& dst, const SrcCollection& src) { dst = {}; algo::append_all(dst, src); }

void appendLevel(io::Level& dst, const io::Level&, bool isPlayedLevel);

}}
