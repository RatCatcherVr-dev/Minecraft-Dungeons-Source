/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "legacy/Core/Utility/MemoryTracker.h"

#include "CommonTypes.h"
#include "legacy/Core/Math/Color.h"
#include "client/renderer/renderer/PrimitiveMode.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"
#include "world/Facing.h"

#ifdef DEBUG_STATS
#define END() end(__FUNCTION__ )
#else
#define END() end()
#endif

class TextureData;
namespace mce {
	class MaterialPtr;
}


class SimpleGeometryCollector
{
public:
	SimpleGeometryCollector();

	virtual ~SimpleGeometryCollector();

	virtual void AddBasicAxisAlignedCollisionVolume(const FVector& CornerPos, const FVector& Extent);

	void AddOrMergeNewAACollisionBox(FBox& NewBoundsBox);
	int32 ReduceAACollisionBoxes();

	virtual void AddBasicRotatedCollisionVolume(const FVector& Center, const FRotator& Rotation, const FVector& Extent);

	void clear();

	bool hasBasicCollisionVolumes();

	TArray< FBox > basicAxisAlignedCollisionBoxes;
	TArray< FKBoxElem > basicCollisionBoxes;	

	int32 basicAxisAlignedCollisionBoxesIndexCache;

protected:



	int32 InternalReduceAACollisionBoxesY();
	int32 InternalReduceAACollisionBoxesZ();

};




class Tessellator : public MemoryTracker {

	static const int MAX_MEMORY_USE = 16 * 1024 * 1024;
	static const int MAX_FLOATS = MAX_MEMORY_USE / 4 / 2;

public:

	

	Tessellator( MemoryTracker* parent = nullptr );

	virtual ~Tessellator();

	void init();
	virtual void clear();

	void begin( int maxVertices );
	void begin(mce::PrimitiveMode mode, int maxVertices);

	void cancel();

	//method useful to allow the engine to optimize the index mFormat
	void beginIndices( int estimateCount = 0 );

	void color( int c );
	void color( int c, int alpha );
	void color( float r, float g, float b, float a = 1.f );
	void color( int r, int g, int b, int a = 255 );
	void color( byte r, byte g, byte b, byte a = 255 );
	void color( const Color& c );

	void colorABGR( int c );

	void normal( float x, float y, float z );
	void normal( const Vec3& n );
	void voidBeginAndEndCalls( bool doVoid );

	void tex( float u, float v );
	void tex(const Vec2& coord);
	void tex1( float u, float v );
	void tex1( const Vec2& coord);
	void tex2(float u, float v);
	void tex2(const Vec2& coord);

	virtual void vertex( float x, float y, float z = 0 );
	virtual void vertexUV( float x, float y, float z, float u, float v );
	virtual void vertexUV( const Vec3& pos, float u, float v );

	virtual void vertex( const Vec3& pos );
	void quadFacing(FacingID facing);

	void ReserveForNumFaces(uint32 iNumFaces);
	void ReserveForNumExtraFaces(uint32 iNumFaces);
	

	void quad( Index i1, Index i2, Index i3, Index i4 );
	void quad( Index baseIdx, bool flipped = false );
	void quad( bool flipped = false );

	void triangle( Index i1, Index i2, Index i3 );

	void scale2d( float x, float y );
	void scale3d( float sx, float sy, float sz );
	void resetScale();

	void tilt();
	void resetTilt();

	void noColor();
	void enableColor();

	void trim();

	virtual MemoryStats getStats() const override;

	int getByteSize() const;

	void setOffset(float xo, float yo, float zo);
	void setOffset(const Vec3& v);
	void addOffset(float x, float y, float z);
	void addOffset(const Vec3& v);

	void setRotationOffset(float angle, const Vec3& axis);
	void addRotationOffset(float angle, const Vec3& axis);

	Index getVertexCount() const;

	int getPolygonCount() const;

	int getColor();

	bool isTessellating() const;
	
	//Find the min and max vertex positions and make a complete aabb collision volume around it (this will replace all other collision volumes in the tesselator)
	virtual void CreateFullAxisAlignedBoundingBoxCollisionVolume();

	void AddBasicAxisAlignedCollisionVolume(const Vec3& CornerPos, const Vec3& Extent);
	virtual void AddBasicAxisAlignedCollisionVolume(const FVector& CornerPos, const FVector& Extent);

	virtual void AddBasicRotatedCollisionVolume(const FVector& Center, const FRotator& Rotation, const FVector& Extent);

	void SetSimpleGeometryVolumeCollector(SimpleGeometryCollector* val) { mGeometryVolumeCollector = val; }


	bool doOverrideNormal = false;
	FVector overrideNormal;

	
	TArray<FVector> vertices;
	TArray<int32> indices;
	TArray<FVector> normals;
	TArray<FColor> colors;
	TArray<FVector2D> uv1;
	TArray<FVector2D> uv2;


protected:

	Tessellator(const Tessellator& rhs) = delete;

	Tessellator& operator=(const Tessellator& rhs) = delete;

	std::vector<byte> mVertexArray;	//BUFFER
	bool mIndexPhase;
	int mIndexSize;
	int mIndexCount;

	//mce::VertexFormat mVertexFormat;
	int mVertexCountMax;

	Vec3 mOffset;
	Vec3 mScale;
	Vec3 mScale2D;

	Vec2 mCoordinates[3];

	unsigned int mColor;
	int mNormal;

	FacingID mQuadFacing = Facing::NOT_DEFINED;
	int mCurQuadVertex = 0;
	int mCurQuadVertex_QuadFace = 0;
	
	bool mTilted;
	FMatrix mTiltedMatrix;

	bool mNoColor;
	bool mVoidBeginEnd;

	Index mVertexCount;
	Index mCount;

	bool mTessellating;
	SimpleGeometryCollector* mGeometryVolumeCollector;

	mce::PrimitiveMode mMode;

	byte* _allocateIndices(int n);
	void _tex(const Vec2& coord, int unit);
	
	

};


class ShadowQuadFaceTessellator : public Tessellator 
{

public:

	struct QuadFace
	{
		FacingID mQuadFacing = Facing::NOT_DEFINED;
		FacingID mNormalFacing = Facing::NOT_DEFINED;

		FVector vertices[4]; //possibly position and range instead of 4 vecs? (more calcs, less space)
		FVector2D uv1[4];
	};

	struct ProcessingAlignments
	{		
		uint8 Pos_Right		: 2;
		uint8 Pos_Forward	: 2;
		uint8 Pos_Depth		: 2;
		
		uint8 Winding_TL	: 2;
		uint8 Winding_TR	: 2;
		uint8 Winding_BL	: 2;
		uint8 Winding_BR	: 2;
	};

	ShadowQuadFaceTessellator(MemoryTracker* parent = nullptr);

	virtual ~ShadowQuadFaceTessellator();

	void vertex(float x, float y, float z = 0) override;
	
	void clear() override;

	void ReduceBlockFaces();	
	
	void BuildBlocksFromFaces();

	virtual void AddBasicAxisAlignedCollisionVolume(const FVector& CornerPos, const FVector& Extent)override{};//do nothing with these
	virtual void AddBasicRotatedCollisionVolume(const FVector& Center, const FRotator& Rotation, const FVector& Extent)override{}; //do nothing with these

protected:
	int32 mTotalQuadFaces;

	QuadFace mCurrentQuadFace;
	TArray < TArray< TArray< QuadFace > >, TFixedAllocator<Facing::MAX> > mQuadFacesList;
	TArray < int32 , TFixedAllocator<Facing::MAX> > mQuadFacesListDepthIndexCache;
	
	void AddQuadFace(const QuadFace& SourceFace);
	bool AddExtendOrAddSortedQuadFace(TArray< TArray< QuadFace > > &DestList, int32& CachedDepthIndex_In_Out, const QuadFace &SourceFace, const ProcessingAlignments &AlignmentStruct);
	
	int32 MergeFaceStrips(int32 Dir);
	int32 MergeFaceStrips(TArray<QuadFace> &TargetList, const ProcessingAlignments &AlignmentStruct, int32 Dir);


};

