#pragma once

#include "game/level/InstancedMeshBase.h"
#include "game/level/TilePreparer.h"

namespace game { namespace tile {

// D11.DB - Spawn instanced static meshes from regions defined in Lovika
class StaticMeshSpawner {
public:
	StaticMeshSpawner(UWorld&);
	void operator()(tile::TilePreparationState);

private:
    UWorld& mWorld;
    TMap<FName, AInstancedMeshBase*> mInstanceMap;
};

}}
