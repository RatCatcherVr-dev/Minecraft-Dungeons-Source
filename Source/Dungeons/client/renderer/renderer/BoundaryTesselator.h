#pragma once


#include "Tessellator.h"
#include "lovika/tile/Boundary.h"
#include "world/phys/Vec3.h"

struct BlockCuboid;

class BoundaryTessellator {
public:
	BoundaryTessellator(Tessellator&);
	void tessellateInWorld(const Boundary&);
	void tessellateInWorld(const BlockCuboid&);
	void clear();
	void SetProduceMesh(bool val){ bProduceMesh  = val;};
	void SetProduceSimpleCollision(bool val) { bProduceSimpleCollision = val; };
	void SetColor(float r, float g, float b, float a);

private:
	void tessellateInWorld(Vec3 pos, Vec3 size);
	bool bProduceMesh = true;
	bool bProduceSimpleCollision = false;
	Tessellator& mTessellator;
};
