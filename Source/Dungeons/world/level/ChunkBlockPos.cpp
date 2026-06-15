#include "Dungeons.h"

#include "world/level/ChunkBlockPos.h"

ChunkBlockPos::ChunkBlockPos(const BlockPos& pos) :
	ChunkBlockPos( pos.x & (CHUNK_WIDTH - 1), pos.y, pos.z & (CHUNK_WIDTH - 1) ){

}

BlockPos ChunkBlockPos::operator+(const BlockPos& p) const {
	return BlockPos(p.x + x, p.y + y, p.z + z);
}
