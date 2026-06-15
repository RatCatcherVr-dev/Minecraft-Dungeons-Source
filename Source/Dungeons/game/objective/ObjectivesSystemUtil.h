#pragma once

class FOutputDevice;

namespace io {
enum class ObjectiveConsiderTilesType;
}

namespace game {

class Tile;
using TilePtr = const Tile*;
	
namespace objective {
	
class Objective;
enum class ValidationType : int;

namespace util {

bool validate(const std::string& prefix, const Objective&, ValidationType, FOutputDevice* = nullptr);
std::vector<TilePtr> getTiles(const std::vector<TilePtr>&, io::ObjectiveConsiderTilesType, const Tile* base = nullptr);

}}}

FOutputDevice& thisOrGlobalLog(FOutputDevice*);
