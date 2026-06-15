#include "Dungeons.h"
#include "BoundaryTesselator.h"
#include "lovika/BlockCuboid.h"

BoundaryTessellator::BoundaryTessellator(Tessellator& tessellator)
	: mTessellator(tessellator) {
}

void BoundaryTessellator::tessellateInWorld(const Boundary& boundary) {
	tessellateInWorld(boundary.position, Vec3{ 1, static_cast<float>(boundary.height), 1 });
}

void BoundaryTessellator::tessellateInWorld(const BlockCuboid& volume) {
	tessellateInWorld(volume.minInclusive, volume.size());
}

void BoundaryTessellator::tessellateInWorld(Vec3 position, Vec3 size) {
	const auto x = Vec3::UNIT_X * size.x;
	const auto y = Vec3::UNIT_Y * size.y;
	const auto z = Vec3::UNIT_Z * size.z;

	if(bProduceMesh)	
	{
		mTessellator.vertex(position);
		mTessellator.vertex(position + y);
		mTessellator.vertex(position + x + y);
		mTessellator.vertex(position + x);

		mTessellator.vertex(position + x);
		mTessellator.vertex(position + x + y);
		mTessellator.vertex(position + x + y + z);
		mTessellator.vertex(position + x + z);

		mTessellator.vertex(position + x + z);
		mTessellator.vertex(position + x + y + z);
		mTessellator.vertex(position + y + z);
		mTessellator.vertex(position + z);

		mTessellator.vertex(position + z);
		mTessellator.vertex(position + y + z);
		mTessellator.vertex(position + y);
		mTessellator.vertex(position);
	}

	if (bProduceSimpleCollision)
	{
		//is this alignment required? Il keep it here to match above functionality
		const auto AlignedExtent = x + y + z;
		FVector BoxExtent(AlignedExtent.x, AlignedExtent.y, AlignedExtent.z);
		FVector BoxPos(position.x, position.y, position.z);
		mTessellator.AddBasicAxisAlignedCollisionVolume(BoxPos, BoxExtent);
	}
}

void BoundaryTessellator::clear() {
	mTessellator.clear();
}

void BoundaryTessellator::SetColor(float r, float g, float b, float a) {
	mTessellator.color(r, g, b, a);
}
