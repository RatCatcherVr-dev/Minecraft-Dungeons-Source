/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"

#include "client/renderer/renderer/Tessellator.h"

#include "legacy/Core/Math/Color.h"

#include "util/Math.h"
#include "util/StringUtils.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"

#include "Async/ParallelFor.h"

#include <functional>

#if !UE_BUILD_SHIPPING
#define TESSELATION_TIMINGS(x) x
#else
#define TESSELATION_TIMINGS(x)
#endif

//PRAGMA_DISABLE_OPTIMIZATION_ACTUAL
//#pragma optimize("", off)

#define THRESH_FACE_POINTS_ARE_SAME			(0.001f)	/* Two points are same if within this distance */

static const ShadowQuadFaceTessellator::ProcessingAlignments sAlignmentStructs[Facing::MAX] = {
															 { 0, 1, 2,		1, 2, 3, 0 } //Facing::DOWN
															,{ 0, 1, 2,		2, 1, 3, 0 } //Facing::UP		
															,{ 0, 2, 1,		3, 2, 0, 1 } //Facing::NORTH  
															,{ 0, 2, 1,		1, 2, 0, 3  } //Facing::SOUTH
															,{ 2, 1, 0,		2, 1, 3, 0 } //Facing::WEST 
															,{ 2, 1, 0,		1, 2, 0, 3 } //Facing::EAST 
															};


template<typename T>
T packNormalized(float x) {
	return (T)std::ceil(x * std::numeric_limits<T>::max());
}
Tessellator::Tessellator(MemoryTracker* parent)
	: MemoryTracker("Tessellator", parent)
	//, mCurrentPointers()
	, mIndexPhase(false)
	, mIndexSize(0)
	, mIndexCount(0)
	//, mVertexFormat()
	, mVertexCountMax(0)
	, mOffset()
	, mScale(1.f)
	, mScale2D(1.f)
	, mColor(0)
	, mNormal(0)
	, mTilted(false)
	, mTiltedMatrix(FMatrix())
	, mNoColor(false)
	, mVoidBeginEnd(false)
	, mVertexCount(0)
	, mCount(0)
	, mTessellating(false)
	, mGeometryVolumeCollector(nullptr)
	, mMode(mce::PrimitiveMode::None) {

	// TODO: rherlitz fix
	//mTiltedMatrix = glm::rotate(glm::mat4(1.0f), 180 + 30.0f, glm::vec3(1, 0, 0));
	//mTiltedMatrix = glm::rotate(mTiltedMatrix, 45.0f, glm::vec3(0, 1, 0));
}

Tessellator::~Tessellator() {
}

void Tessellator::init() {

}

void Tessellator::clear() {
	vertices.Empty();
	indices.Empty();
	colors.Empty();
	normals.Empty();
	uv1.Empty();
	uv2.Empty();

	mCount = 0;
	mIndexCount = 0;
	mVertexCount = 0;
	mVoidBeginEnd = false;
	mTessellating = false;
	mIndexPhase = false;

	mVertexArray.clear();
	mCoordinates[0] = Vec2();
	mCoordinates[1] = Vec2();
	//mCurrentPointers = CurrentVertexPointers();

	//mVertexFormat = mce::VertexFormat::EMPTY;	
}

void Tessellator::cancel() {
	mTessellating = false;
}

void Tessellator::begin(mce::PrimitiveMode mode, int maxVertices) {
	if (mTessellating || mVoidBeginEnd) {
		if (mTessellating && !mVoidBeginEnd) {
			DEBUG_FAIL("already mTessellating!\n");
		}
		return;
	}
	//if (mTessellating) {
	//    throw /*new*/ IllegalStateException("Already mTessellating!");
	//}
	clear();
	mMode = mode;
	mNoColor = false;

	mTessellating = true;

// 	mVertexFormat.enableField(mce::VertexField::Position);	//position is always enabled

	mIndexSize = 0;	//let the user decide
	mIndexCount = 0;
	mVertexCountMax = maxVertices;
	mCurQuadVertex = 0;
	mCurQuadVertex_QuadFace = 0;
}

void Tessellator::begin(int maxVertices = 0) {
	begin(mce::PrimitiveMode::QuadList, maxVertices);
}

void Tessellator::_tex(const Vec2& coord, int unit) {
// 	DEBUG_ASSERT(coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f, "Invalid texture coordinates");
	mCoordinates[unit] = coord;

	//const mce::VertexField field = static_cast<mce::VertexField>(enum_cast(mce::VertexField::UV0) + unit);
	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(field);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(field), "Can't add UV coordinates at this point");
	//}
}


void Tessellator::AddBasicRotatedCollisionVolume(const FVector& Center, const FRotator& Rotation, const FVector& Extent)
{
	if (mGeometryVolumeCollector)
	{
		const float Center_x = float(mScale2D.x * ((Center.X * mScale.x) + mOffset.x));
		const float Center_y = float(mScale2D.y * ((Center.Y * mScale.x) + mOffset.y));
		const float Center_z = float((Center.Z * mScale.x) + mOffset.z);

		const float Extent_x = float(mScale2D.x * Extent.X * mScale.x);
		const float Extent_y = float(mScale2D.y * Extent.Y * mScale.x);
		const float Extent_z = float(Extent.Z * mScale.x);

		FVector NewCenter(Center_x, Center_z, Center_y);//yz flip here
		FVector NewExtent(Extent_x, Extent_z, Extent_y);//yz flip here

		mGeometryVolumeCollector->AddBasicRotatedCollisionVolume(NewCenter,Rotation, NewExtent);
	}
}

void Tessellator::AddBasicAxisAlignedCollisionVolume(const FVector& CornerPos, const FVector& Extent)
{
	if (mGeometryVolumeCollector)
	{
		FVector NewExtent(float(mScale2D.x * Extent.X * mScale.x), float(Extent.Z * mScale.x), float(mScale2D.y * Extent.Y * mScale.x)); //yz flip here

		FVector NewBoxMin(float(mScale2D.x * ((CornerPos.X * mScale.x) + mOffset.x)), float((CornerPos.Z * mScale.x) + mOffset.z), float(mScale2D.y * ((CornerPos.Y * mScale.x) + mOffset.y))); //yz flip here
	
		mGeometryVolumeCollector->AddBasicAxisAlignedCollisionVolume(NewBoxMin, NewExtent);
	}
}

void Tessellator::AddBasicAxisAlignedCollisionVolume(const Vec3& CornerPos, const Vec3& Extent)
{
	AddBasicAxisAlignedCollisionVolume(CornerPos.asFVector(), Extent.asFVector());
}

void Tessellator::tex(const Vec2& v) {
	_tex(v, 0);
}

void Tessellator::tex1(const Vec2& v) {
	_tex(v, 1);
}

void Tessellator::tex2(const Vec2& v) {
	_tex(v, 2);
}

void Tessellator::tex(float u, float v) {
	tex(Vec2(u, v));
}

void Tessellator::tex1(float u, float v) {
	tex1(Vec2(u, v));
}

void Tessellator::tex2(float u, float v) {
	tex2(Vec2(u, v));
}

int Tessellator::getColor() {
	return mColor;
}

bool Tessellator::isTessellating() const {
	return mTessellating;
}

void Tessellator::CreateFullAxisAlignedBoundingBoxCollisionVolume()
{
	if (vertices.Num() > 1 && mGeometryVolumeCollector)
	{
		mGeometryVolumeCollector->clear();

		//Find the min and max vertex positions and make a complete aabb collision volume around it (this will replace all other collision volumes in the tesselator)
		FVector VecMin = vertices[0];
		FVector VecMax = vertices[0];

		for (int i = 1; i < vertices.Num(); ++i)
		{
			const FVector& SourceVec = vertices[i];

			for (int j = 0; j < 3; ++j)
			{
				if (SourceVec[j] < VecMin[j]) { VecMin[j] = SourceVec[j]; }
				if (SourceVec[j] > VecMax[j]) { VecMax[j] = SourceVec[j]; }
			}
		}

		FBox SimpleBoundsBox(VecMin, VecMax);
		mGeometryVolumeCollector->AddBasicAxisAlignedCollisionVolume(VecMin, (VecMax-VecMin));
	}
}


void Tessellator::color(const Color& c) {
	color((byte)(c.r * 255), (byte)(c.g * 255), (byte)(c.b * 255), (byte)(c.a * 255));
}

void Tessellator::color(float r, float g, float b, float a) {
	color(Color(r, g, b, a));
}

void Tessellator::color(byte r, byte g, byte b, byte a) {
	if (mNoColor) {
		return;
	}

 	mColor = (a << 24) | (b << 16) | (g << 8) | (r);
}

void Tessellator::color(int r, int g, int b, int a) {
	color(
		(byte)Math::clamp(r, 0, 255),
		(byte)Math::clamp(g, 0, 255),
		(byte)Math::clamp(b, 0, 255),
		(byte)Math::clamp(a, 0, 255));
}

void Tessellator::color(int c) {
	int r = ((c >> 16) & 255);
	int g = ((c >> 8) & 255);
	int b = ((c) & 255);
	color(r, g, b);
}

//@note: doesn't care about endianess
void Tessellator::colorABGR(int c) {
	if (mNoColor) {
		return;
	}
	mColor = c;

	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(mce::VertexField::Color);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(mce::VertexField::Color), "Can't add Vertex Color at this point");
	//}
}

void Tessellator::color(int c, int alpha) {
	int r = ((c >> 16) & 255);
	int g = ((c >> 8) & 255);
	int b = ((c) & 255);
	color(r, g, b, alpha);
}

void Tessellator::vertexUV(float x, float y, float z, float u, float v) {
	tex(u, v);
	vertex(x, y, z);
}

void Tessellator::vertexUV(const Vec3& pos, float u, float v) {
	tex(u, v);
	vertex(pos);
}


void Tessellator::scale2d(float sx, float sy) {
	mScale2D.x *= sx;
	mScale2D.y *= sy;
}

void Tessellator::scale3d(float sx, float sy, float sz) {
	mScale.x *= sx;
	mScale.y *= sy;
	mScale.z *= sz;
}

void Tessellator::resetScale() {
	mScale2D.x = mScale2D.y = 1;
	mScale.x = 1;
	mScale.y = 1;
	mScale.z = 1;
}

void Tessellator::beginIndices(int extimateCount) {
	DEBUG_ASSERT(mTessellating && mIndexPhase == false, "invalid state!");
	mIndexPhase = true;

	mIndexSize = (getVertexCount() <= std::numeric_limits<unsigned short>::max()) ? sizeof(unsigned short) : sizeof(unsigned int);

	if (extimateCount == 0 && mMode == mce::PrimitiveMode::QuadList) {
		extimateCount = (getVertexCount() / 4 * 6 * mIndexSize);
	}

	mVertexArray.reserve(mVertexArray.size() + extimateCount * mIndexSize);
}

byte* Tessellator::_allocateIndices(int n) {
	int sz = n * mIndexSize;
	mVertexArray.resize(mVertexArray.size() + sz);
	return &mVertexArray[mVertexArray.size() - sz];
}

template<typename I>
void _triangle(void* ptr, Index i1, Index i2, Index i3) {
	auto base = (I*)ptr;

	base[0] = (I)i1;
	base[1] = (I)i2;
	base[2] = (I)i3;
}

void Tessellator::triangle(Index i1, Index i2, Index i3) {
	DEBUG_ASSERT(mIndexPhase, "Can't draw indices now");
	DEBUG_ASSERT(i1 < getVertexCount(), "Vertex outside mesh");
	DEBUG_ASSERT(i2 < getVertexCount(), "Vertex outside mesh");
	DEBUG_ASSERT(i3 < getVertexCount(), "Vertex outside mesh");

	auto ptr = _allocateIndices(3);

	switch (mIndexSize) {
		case 1:
			_triangle<byte>(ptr, i1, i2, i3);
			break;
		case 2:
			_triangle<unsigned short>(ptr, i1, i2, i3);
			break;
		case 4:
			_triangle<unsigned int>(ptr, i1, i2, i3);
			break;
		default:
			DEBUG_FAIL("Index size not supported");
	}

	mIndexCount += 3;
}

void Tessellator::quad(Index i1, Index i2, Index i3, Index i4) {
	triangle(i1, i2, i3);
	triangle(i4, i1, i3);
}

void Tessellator::quad(Index baseIdx, bool flipped) {

	if (!flipped) {
		quad(
			baseIdx,
			baseIdx + 1,
			baseIdx + 2,
			baseIdx + 3);
	}
	else {
		quad(
			baseIdx + 3,
			baseIdx + 2,
			baseIdx + 1,
			baseIdx);
	}
}

void Tessellator::quad(bool flipped) {
	quad(mCount - 4, flipped);
}

void Tessellator::vertex(float x, float y, float z) {
	if (mCount >= 1000000) {	//filled up :( stop adding vertices
		return;
	}

	mCount++;

	// TODO: rherlitz fix
	//if (mTilted) {
	//	glm::vec4 rotatePos = glm::vec4(x, y, z, 1.0f);
	//	rotatePos = mTiltedMatrix * rotatePos;
	//	x = rotatePos[0];
	//	y = rotatePos[1];
	//	z = rotatePos[2];
	//}

	x = float(mScale2D.x * ((x * mScale.x) + mOffset.x));
	y = float(mScale2D.y * ((y * mScale.x) + mOffset.y));
	z = float((z * mScale.x) + mOffset.z);

	
	vertices.Add(FVector(x, z, y));
	uv1.Add(FVector2D(mCoordinates[0].x, mCoordinates[0].y));
	uv2.Add(FVector2D(mCoordinates[1].x, mCoordinates[1].y));
	colors.Add(FColor(mColor));

	++mVertexCount;

	if (mCurQuadVertex == 3) {
		mCurQuadVertex = 0;

		mQuadFacing = Facing::NOT_DEFINED;

		auto v1 = mVertexCount - 4;
		auto v2 = mVertexCount - 3;
		auto v3 = mVertexCount - 2;
		auto v4 = mVertexCount - 1;

		if (doOverrideNormal) {
			normals.Add(overrideNormal);
			normals.Add(overrideNormal);
			normals.Add(overrideNormal);
			normals.Add(overrideNormal);
		}
		else {
			auto& vert1 = vertices[v1];
			auto& vert2 = vertices[v2];
			auto& vert3 = vertices[v3];

			FVector normal = FVector::CrossProduct(vert3 - vert1, vert2 - vert1);
			normal.Normalize();

			normals.Add(normal);
			normals.Add(normal);
			normals.Add(normal);
			normals.Add(normal);
		}

		indices.Add(v1);
		indices.Add(v2);
		indices.Add(v3);
		indices.Add(v3);
		indices.Add(v4);
		indices.Add(v1);

	}
	else {
		mCurQuadVertex++;
	}
	
}




void Tessellator::quadFacing(FacingID facing) {
	if (mCurQuadVertex == 0 && mCurQuadVertex_QuadFace == 0) {
		mQuadFacing = facing;
	}
}

void Tessellator::ReserveForNumFaces(uint32 iNumFaces)
{
	const uint32 iVerts = iNumFaces * 4;
	const uint32 iIndices = iNumFaces * 6;

	vertices.Reserve(iVerts);
	normals.Reserve(iVerts);
	colors.Reserve(iVerts);
	uv1.Reserve(iVerts);
	uv2.Reserve(iVerts);
	indices.Reserve(iIndices);

}

void Tessellator::ReserveForNumExtraFaces(uint32 iNumFaces)
{
	const uint32 iVerts = vertices.Num() + (iNumFaces * 4);
	const uint32 iIndices = indices.Num() + (iNumFaces * 6);

	vertices.Reserve(iVerts);
	normals.Reserve(iVerts);
	colors.Reserve(iVerts);
	uv1.Reserve(iVerts);
	uv2.Reserve(iVerts);
	indices.Reserve(iIndices);
}

void Tessellator::vertex(const Vec3& pos) {
	vertex(pos.x, pos.y, pos.z);
}


void Tessellator::noColor() {
	mNoColor = true;
}

void Tessellator::normal(float x, float y, float z) {
	auto byte = (signed char*)(&mNormal);
#if defined(GFX_API_DX11)
	if (mce::RenderContextImmediate::getAsConst().getDevice()->getDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0) {
		// move from -1 to 1, to 0 to 1, undo transform in shader
		x = x * 0.5f + 0.5f;
		y = y * 0.5f + 0.5f;
		z = z * 0.5f + 0.5f;

		byte[0] = packNormalized<unsigned char>(x);
		byte[1] = packNormalized<unsigned char>(y);
		byte[2] = packNormalized<unsigned char>(z);
		byte[3] = 0;
	}
	else {
		byte[0] = packNormalized<signed char>(x);
		byte[1] = packNormalized<signed char>(y);
		byte[2] = packNormalized<signed char>(z);
		byte[3] = 0;
	}
#else
	byte[0] = packNormalized<signed char>(x);
	byte[1] = packNormalized<signed char>(y);
	byte[2] = packNormalized<signed char>(z);
	byte[3] = 0;
#endif

	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(mce::VertexField::Normal);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(mce::VertexField::Normal), "Can't add normals at this point");
	//}
}

void Tessellator::normal(const Vec3& n) {
	normal(n.x, n.y, n.z);
}

void Tessellator::setOffset(float xo, float yo, float zo) {
	mOffset.x = xo;
	mOffset.y = yo;
	mOffset.z = zo;
}

void Tessellator::addOffset(float x, float y, float z) {
	mOffset.x += x;
	mOffset.y += y;
	mOffset.z += z;
}

void Tessellator::setOffset(const Vec3& v) {
	mOffset = v;
}

void Tessellator::addOffset(const Vec3& v) {
	mOffset += v;
}

// TODO: rherlitz fix
void Tessellator::setRotationOffset(float angle, const Vec3& axis) {
	//mTiltedMatrix = glm::rotate(glm::mat4(1.f), angle, (const glm::vec3&)axis);
}

void Tessellator::addRotationOffset(float angle, const Vec3& axis) {
	//mTiltedMatrix = glm::rotate(glm::mat4(1.f), angle, (const glm::vec3&)axis) * mTiltedMatrix;
}

Index Tessellator::getVertexCount() const {
	return mVertexCount;
}

void Tessellator::voidBeginAndEndCalls(bool doVoid) {
	mVoidBeginEnd = doVoid;
}

void Tessellator::enableColor() {
	mNoColor = false;
}

void Tessellator::tilt() {
	mTilted = true;
}

void Tessellator::resetTilt() {
	mTilted = false;

	// TODO: rherlitz fix
	//mTiltedMatrix = glm::rotate(glm::mat4(1.0f), 180 + 30.0f, glm::vec3(1, 0, 0));
	//mTiltedMatrix = glm::rotate(mTiltedMatrix, 45.0f, glm::vec3(0, 1, 0));
}

MemoryStats Tessellator::getStats() const {
	return MemoryStats(sizeof(*this) + mVertexArray.capacity(), 1);
}

int Tessellator::getByteSize() const {
	return mVertexArray.size();
}

void Tessellator::trim() {
	Util::freeVectorMemory(mVertexArray);
}

int Tessellator::getPolygonCount() const {
	int n = mIndexCount ? mIndexCount : getVertexCount();	//get the actual number of primitives

	switch (mMode) {
		case mce::PrimitiveMode::QuadList:
			return (n / 4) * 2;
		case mce::PrimitiveMode::TriangleList:
			return n / 3;
		case mce::PrimitiveMode::TriangleStrip:
			return n - 2;
		case mce::PrimitiveMode::LineList:
			return n / 2;
		case mce::PrimitiveMode::LineStrip:
			return n - 1;
		default:
			return 0;
	}
}


//#pragma optimize("", on)

ShadowQuadFaceTessellator::ShadowQuadFaceTessellator(MemoryTracker* parent /*= nullptr*/)
:
	Tessellator(parent)
{
	mQuadFacesList.SetNumZeroed(Facing::MAX);
	mQuadFacesListDepthIndexCache.SetNumZeroed(Facing::MAX);
}

ShadowQuadFaceTessellator::~ShadowQuadFaceTessellator()
{
}

void ShadowQuadFaceTessellator::vertex(float x, float y, float z /*= 0*/)
{
	if (mQuadFacing < Facing::NOT_DEFINED)
	{

		if (mQuadFacing == Facing::DOWN || mCount >= 1000000) {	//filled up :( stop adding vertices
			return;
		}

		mCount++;

		x = float(mScale2D.x * ((x * mScale.x) + mOffset.x));
		y = float(mScale2D.y * ((y * mScale.x) + mOffset.y));
		z = float((z * mScale.x) + mOffset.z);


		mCurrentQuadFace.vertices[mCurQuadVertex_QuadFace] = FVector(x, z, y);
		mCurrentQuadFace.uv1[mCurQuadVertex_QuadFace] = FVector2D(mCoordinates[0].x, mCoordinates[0].y);
		
		if (mCurQuadVertex_QuadFace == 3) {
			mCurQuadVertex_QuadFace = 0;

			mCurrentQuadFace.mQuadFacing = mQuadFacing;

			if (doOverrideNormal)
			{
				mCurrentQuadFace.mNormalFacing = Facing::UP; //only place this is used its pointing up anyway
			}
			else
			{
				mCurrentQuadFace.mNormalFacing = mQuadFacing;
			}

			AddQuadFace(mCurrentQuadFace);

			mQuadFacing = Facing::NOT_DEFINED;

			
		}
		else {
			mCurQuadVertex_QuadFace++;
		}
	}
	else
	{
		Tessellator::vertex(x,y,z);
	}
}

void ShadowQuadFaceTessellator::clear()
{
	Tessellator::clear();
		
	for (auto& QFL : mQuadFacesList)
	{
		QFL.Empty();
	}

	mQuadFacesListDepthIndexCache.Init(0,Facing::MAX);

	mTotalQuadFaces = 0;
}


void ShadowQuadFaceTessellator::ReduceBlockFaces()
{
	//D11.SC Reduce triangle count by merging consecutive faces together
		
	if (mTotalQuadFaces < 2) { return; } //early out

	TESSELATION_TIMINGS(int iStartFaceStrips = mTotalQuadFaces);

	TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());

	TArray < int32, TFixedAllocator<Facing::MAX> > DirectionsToProcess;

	//merge depth lists
	for (int i(1); i < Facing::MAX; ++i)
	{
		auto& QuadList = mQuadFacesList[i];

		const int iDepthLists = QuadList.Num();
		
		if (iDepthLists)
		{
			int iCount = 0;
			for (auto& DepthList : QuadList)
			{
				iCount += DepthList.Num();
			}

			if (iCount)
			{
				QuadList[0].Reserve(iCount);

				for (int iSrcList = 1; iSrcList < iDepthLists; ++iSrcList)
				{
					QuadList[0] += QuadList[iSrcList];
					QuadList[iSrcList].Empty();
				}

				DirectionsToProcess.Add(i);
			}
		}
	}

	if(DirectionsToProcess.Num())
	{
		
		int32 iFinalFaceCounts[Facing::MAX] = {0};

		ParallelFor(DirectionsToProcess.Num(), [&](int32 threadIndex) //dont bother with downwards, its always empty
		{
			const int32 FacingVal = DirectionsToProcess[threadIndex];
			iFinalFaceCounts[FacingVal] = MergeFaceStrips(FacingVal); //all faces should have been merged into the first depth list by now
		});
	
		//recount final face count
		mTotalQuadFaces = 0;
		for (int i(1); i < Facing::MAX; ++i)
		{
			mTotalQuadFaces += iFinalFaceCounts[i];
		}

		
	}
	TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
	TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### ShadowQuadFaceTessellator::ReduceBlockFaces iStartFaceStrips (%d) iNewFaceCount (%d) %f Ms (%f s) ##################\n\n"), iStartFaceStrips, mTotalQuadFaces, (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));
	
}



int32 ShadowQuadFaceTessellator::MergeFaceStrips(TArray<QuadFace> &TargetList, const ProcessingAlignments &AlignmentStruct, int32 Dir)
{	
	TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());

	const int32 iDestCount = TargetList.Num();
	int32 iFaceCount = iDestCount;

	if (iDestCount > 1)
	{
		int32 iCurrQuad = 0;
		int32 iTestQuad = 1;
		const int32 iLastQuadIndex = iDestCount - 1;
		
		while (iCurrQuad < iLastQuadIndex)
		{
			QuadFace& CurrQuadFace = TargetList[iCurrQuad];

			if (CurrQuadFace.mQuadFacing != Facing::NUM_CULLING_IDS) //skip culled faces
			{
				iTestQuad = iCurrQuad + 1;

				const float fDepth = CurrQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Depth];

				while (iTestQuad < iDestCount)
				{
					QuadFace& TestQuadFace = TargetList[iTestQuad];

					if (TestQuadFace.mQuadFacing != Facing::NUM_CULLING_IDS) //skip culled faces
					{
						const float fDepthTest = TestQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Depth];
						if (FMath::IsNearlyEqual(fDepth, fDepthTest, THRESH_FACE_POINTS_ARE_SAME))
						{
							//on the same depth plane
							if (CurrQuadFace.vertices[AlignmentStruct.Winding_BL][AlignmentStruct.Pos_Forward] < (TestQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward] - THRESH_FACE_POINTS_ARE_SAME) )
							{
								//if my bottom edge is less than your top, break out as these are sorted Z,Y,X (no further quads will be closer in Y)
								break;
							}
							else if(FMath::IsNearlyEqual(CurrQuadFace.vertices[AlignmentStruct.Winding_BL][AlignmentStruct.Pos_Forward], TestQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward], THRESH_FACE_POINTS_ARE_SAME))
							{
								//Test face top matches the bottom of current

								if (CurrQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right] < (TestQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right] - THRESH_FACE_POINTS_ARE_SAME))
								{
									//if my right start edge is less than your start right, break out as these are sorted Z,Y,X (no further quads will be closer in X)
									break;
								}

								//is Start Right & End Right for both quads the same? 
								if (FMath::IsNearlyEqual(CurrQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right], TestQuadFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right], THRESH_FACE_POINTS_ARE_SAME) &&
									FMath::IsNearlyEqual(CurrQuadFace.vertices[AlignmentStruct.Winding_TR][AlignmentStruct.Pos_Right], TestQuadFace.vertices[AlignmentStruct.Winding_TR][AlignmentStruct.Pos_Right], THRESH_FACE_POINTS_ARE_SAME))
								{
									//We align in The Right Axis, can we merge?
									//Match!
									CurrQuadFace.vertices[AlignmentStruct.Winding_BL][AlignmentStruct.Pos_Forward] = TestQuadFace.vertices[AlignmentStruct.Winding_BL][AlignmentStruct.Pos_Forward];
									CurrQuadFace.vertices[AlignmentStruct.Winding_BR][AlignmentStruct.Pos_Forward] = TestQuadFace.vertices[AlignmentStruct.Winding_BR][AlignmentStruct.Pos_Forward];

									TestQuadFace.mQuadFacing = Facing::NUM_CULLING_IDS; //mark for culling later
									--iFaceCount;
								}
							}
						}
						else
						{
							//since these are depth ordered any further ones will be higher
							break;
						}
					}

					++iTestQuad;
				}
			}

			++iCurrQuad;
		}
	}

	TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
	TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### ShadowQuadFaceTessellator::MergeFaceStrips (%d) iDestCount(%d) %f Ms (%f s) ##################\n"), Dir, iDestCount, (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));

	return iFaceCount;
}

int32 ShadowQuadFaceTessellator::MergeFaceStrips(int32 Dir)
{
	return MergeFaceStrips(mQuadFacesList[Dir][0], sAlignmentStructs[Dir], Dir);
}

void ShadowQuadFaceTessellator::BuildBlocksFromFaces()
{
	TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());

	if (mTotalQuadFaces)
	{
		ReserveForNumExtraFaces(mTotalQuadFaces); //we could have verts other than quad faces in here from non reduced elements

		for (int i(0); i < Facing::MAX; ++i)
		{
			if(!mQuadFacesList[i].Num())
				continue;

			TArray< QuadFace >& List = mQuadFacesList[i][0];

			const int FaceCount = List.Num();

			for (int iFace = 0; iFace < FaceCount; ++iFace)
			{
				const QuadFace& targetFace(List[iFace]);

				if(targetFace.mQuadFacing == Facing::NUM_CULLING_IDS) //culled?
					continue;

				//Generate verts from face
				for (int iVx = 0; iVx < 4; ++iVx)
				{
					++mVertexCount;

					vertices.Add(targetFace.vertices[iVx]);
					uv1.Add(targetFace.uv1[iVx]);
					colors.Add(FColor::White);

					const Vec3& NormalVal = (targetFace.mNormalFacing) ? Facing::NORMAL[targetFace.mNormalFacing] : Facing::NORMAL[Facing::UP];

					normals.Add({ NormalVal.x,NormalVal.y,NormalVal.z });

				}

				auto v1 = mVertexCount - 4;
				auto v2 = mVertexCount - 3;
				auto v3 = mVertexCount - 2;
				auto v4 = mVertexCount - 1;

				//Triangle 1
				indices.Add(v1);
				indices.Add(v2);
				indices.Add(v3);

				//Triangle 2
				indices.Add(v3);
				indices.Add(v4);
				indices.Add(v1);

			}

			List.Empty();
		}
	}

	TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
	TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### BuildBlocksFromFaces  %f Ms (%f s) ##################\n"), (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));

}


void ShadowQuadFaceTessellator::AddQuadFace(const QuadFace& SourceFace)
{
	//This will insert ore extend a current quad in the list, maintaining a list of quads sorted by depth, forward, right
	//The resulting extended quadfaces can then be post processed for further combination in depth and forward axis once these strips have been produced (See ReduceBlockFaces)
	if (AddExtendOrAddSortedQuadFace(mQuadFacesList[SourceFace.mQuadFacing], mQuadFacesListDepthIndexCache[SourceFace.mQuadFacing], SourceFace, sAlignmentStructs[SourceFace.mQuadFacing]))
	{
		++mTotalQuadFaces;
	}
}


bool ShadowQuadFaceTessellator::AddExtendOrAddSortedQuadFace(TArray< TArray< QuadFace > > &DestList, int32& CachedDepthIndex_In_Out, const QuadFace &SourceFace, const ProcessingAlignments &AlignmentStruct)
{
	//QUICK_SCOPE_CYCLE_COUNTER(STAT_ShadowQuadFaceTessellator_AddExtendOrAddSortedQuadFace);
	
	int iDestCount = DestList.Num();

	if (iDestCount > 0)
	{
		const float fMyDepth = SourceFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Depth];
		int iDepthIndex = 0;

		if (iDestCount > 1)
		{
			TArray< QuadFace >& CachedDepthList = DestList[CachedDepthIndex_In_Out];
			const float fTestDepth = (fMyDepth - CachedDepthList[0].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Depth]);

			if (fTestDepth < -THRESH_FACE_POINTS_ARE_SAME)
			{
				iDestCount = CachedDepthIndex_In_Out; //search up to this one
			}
			else 
			{
				iDepthIndex = CachedDepthIndex_In_Out; //start at this one
			}
		}

		//find depth array		
		for (; iDepthIndex < iDestCount; ++iDepthIndex)
		{
			TArray< QuadFace >& DepthList = DestList[iDepthIndex];
			const float fTestDepth = (fMyDepth - DepthList[0].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Depth]);

			if (fTestDepth < -THRESH_FACE_POINTS_ARE_SAME)
			{
				//before this depth, insert
				TArray< QuadFace >& NewArray = DestList.InsertDefaulted_GetRef(iDepthIndex);
				NewArray.Push(SourceFace);
				CachedDepthIndex_In_Out = iDepthIndex;
				return true;
			}
			else if (fTestDepth < THRESH_FACE_POINTS_ARE_SAME)
			{
				//depth match
				CachedDepthIndex_In_Out = iDepthIndex;
				const int iDepthCount = DepthList.Num();
				int iInsertPoint = 0;

				if(iDepthCount > 1)
				{
					//test the last one first, just in case
					const int iLastPoint = iDepthCount - 1;

					const float fTestForwardDiff = (SourceFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward] - DepthList[iLastPoint].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward]);

					if (fTestForwardDiff >= THRESH_FACE_POINTS_ARE_SAME)
					{
						DepthList.Add(SourceFace);return true;
					}
					else if (fTestForwardDiff >= -THRESH_FACE_POINTS_ARE_SAME)
					{
						//its in the last ones range, check for merge
						iInsertPoint = iLastPoint;
					}
				}

				while (iInsertPoint < iDepthCount)
				{
					const float fTestForwardDiff = (SourceFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward] - DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Forward]);

					if (fTestForwardDiff < -THRESH_FACE_POINTS_ARE_SAME)
					{
						//We are closer than this Quad, add new one
						DepthList.Insert(SourceFace, iInsertPoint);
						return true;
					}
					else if (fTestForwardDiff < THRESH_FACE_POINTS_ARE_SAME)
					{
						const float fMyRightStart = SourceFace.vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right]; //TL source						
						const float fMyRightEnd = SourceFace.vertices[AlignmentStruct.Winding_TR][AlignmentStruct.Pos_Right]; //TR of source

						//matching Forward, see if we can expand
						float fTestRightStart = DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right];
						float fTestRightEnd = DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_TR][AlignmentStruct.Pos_Right];
						
						if (FMath::IsNearlyEqual(fMyRightStart, fTestRightStart, THRESH_FACE_POINTS_ARE_SAME))
						{
							//this quad is in the same position as one already in the list :/ lets just add it in anyway and continue
							DepthList.Insert(SourceFace, iInsertPoint+1);
							return true;
						}
						else if (FMath::IsNearlyEqual(fMyRightStart, fTestRightEnd, THRESH_FACE_POINTS_ARE_SAME))
						{
							//extend this quad right, set the Right value of the testing quad to match the new quad's rightmost values
							DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_TR][AlignmentStruct.Pos_Right] = fMyRightEnd;
							DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_BR][AlignmentStruct.Pos_Right] = fMyRightEnd;
							return false;
						}
						else if (fMyRightStart < fTestRightStart)
						{
							//We are more left than the testing quad, lets check if we can extend the testing quad left
							if (FMath::IsNearlyEqual(fMyRightEnd, fTestRightStart, THRESH_FACE_POINTS_ARE_SAME))
							{
								//we can extend the dest quad left with this one
								DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_TL][AlignmentStruct.Pos_Right] = fMyRightStart;
								DepthList[iInsertPoint].vertices[AlignmentStruct.Winding_BL][AlignmentStruct.Pos_Right] = fMyRightStart;
								return false;
							}
						}
					}

					++iInsertPoint;
				}

				if (iInsertPoint == iDepthCount)
				{
					//made it to the end
					DepthList.Add(SourceFace);
					return true;
				}

				return false;
			}
		}

	}

	
	//add new list in
	TArray< QuadFace >& NewArray = DestList.InsertDefaulted_GetRef(iDestCount);
	NewArray.Push(SourceFace);
	CachedDepthIndex_In_Out = iDestCount;
	return true;
}





SimpleGeometryCollector::SimpleGeometryCollector()
	:
	basicAxisAlignedCollisionBoxesIndexCache(0)
{

}


SimpleGeometryCollector::~SimpleGeometryCollector()
{

}



void SimpleGeometryCollector::AddBasicAxisAlignedCollisionVolume(const FVector& CornerPos, const FVector& Extent)
{
	// Concatenate Collision Volumes in here? would be nice to have less of them
	FBox NewBoundsBox(CornerPos, CornerPos + Extent);
	AddOrMergeNewAACollisionBox(NewBoundsBox);

}


void SimpleGeometryCollector::AddOrMergeNewAACollisionBox(FBox& NewBoundsBox)
{
	const int32 iDestCount = basicAxisAlignedCollisionBoxes.Num();
	if (iDestCount > 0)
	{
		//lets see if we can merge this box
		int iInsertPoint = 0;

		{
		
			//check cached depth index first
			const float fTestZ = NewBoundsBox.Min.Z - basicAxisAlignedCollisionBoxes[basicAxisAlignedCollisionBoxesIndexCache].Min.Z;
		
			if (fTestZ > THRESH_FACE_POINTS_ARE_SAME)
			{
				iInsertPoint = basicAxisAlignedCollisionBoxesIndexCache + 1;
			}
			else if (fTestZ >= -THRESH_FACE_POINTS_ARE_SAME)
			{
				iInsertPoint = basicAxisAlignedCollisionBoxesIndexCache;
			}
		
		}

		while (iInsertPoint < iDestCount)
		{
			const float fTestZ = basicAxisAlignedCollisionBoxes[iInsertPoint].Min.Z;

			if (FMath::IsNearlyEqual(NewBoundsBox.Min.Z, fTestZ, THRESH_FACE_POINTS_ARE_SAME))
			{
				//matching Depth
				basicAxisAlignedCollisionBoxesIndexCache = iInsertPoint;
				break;
			}
			else
				if (NewBoundsBox.Min.Z < (fTestZ - THRESH_FACE_POINTS_ARE_SAME))
				{
					//We are closer than this, add new one
					basicAxisAlignedCollisionBoxes.Insert(NewBoundsBox, iInsertPoint);
					basicAxisAlignedCollisionBoxesIndexCache = iInsertPoint;
					return;
				}
			++iInsertPoint;
		}

		//iInsertPoint should now be the start of matching depth range
		while (iInsertPoint < iDestCount)
		{
			const float fTestZ = basicAxisAlignedCollisionBoxes[iInsertPoint].Min.Z;
			if (!FMath::IsNearlyEqual(NewBoundsBox.Min.Z, fTestZ, THRESH_FACE_POINTS_ARE_SAME))
			{
				//No Longer in this depth range
				basicAxisAlignedCollisionBoxes.Insert(NewBoundsBox, iInsertPoint);
				return;
			}

			const float fTestY = basicAxisAlignedCollisionBoxes[iInsertPoint].Min.Y;

			if (NewBoundsBox.Min.Y < (fTestY - THRESH_FACE_POINTS_ARE_SAME))
			{
				//We are closer than this , add new one
				basicAxisAlignedCollisionBoxes.Insert(NewBoundsBox, iInsertPoint);
				return;
			}
			else
				if (FMath::IsNearlyEqual(NewBoundsBox.Min.Y, fTestY, THRESH_FACE_POINTS_ARE_SAME))
				{
					//matching plane, see if we can expand
					float fTestXStart = basicAxisAlignedCollisionBoxes[iInsertPoint].Min.X;
					float fTestXEnd = basicAxisAlignedCollisionBoxes[iInsertPoint].Max.X;

					if (FMath::IsNearlyEqual(NewBoundsBox.Min.X, fTestXStart, THRESH_FACE_POINTS_ARE_SAME))
					{
						//same start position, just add it in if not the same
						if (!NewBoundsBox.Min.Equals(basicAxisAlignedCollisionBoxes[iInsertPoint].Min, THRESH_FACE_POINTS_ARE_SAME) &&
							!NewBoundsBox.Max.Equals(basicAxisAlignedCollisionBoxes[iInsertPoint].Max, THRESH_FACE_POINTS_ARE_SAME))
						{
							basicAxisAlignedCollisionBoxes.Insert(NewBoundsBox, iInsertPoint);
						}
						return;
					}
					else
					if (FMath::IsNearlyEqual(NewBoundsBox.Min.X, fTestXEnd, THRESH_FACE_POINTS_ARE_SAME) &&
						FMath::IsNearlyEqual(NewBoundsBox.Max.Y, basicAxisAlignedCollisionBoxes[iInsertPoint].Max.Y, THRESH_FACE_POINTS_ARE_SAME) &&
						FMath::IsNearlyEqual(NewBoundsBox.Max.Z, basicAxisAlignedCollisionBoxes[iInsertPoint].Max.Z, THRESH_FACE_POINTS_ARE_SAME))
					{
						//extend this  right
						basicAxisAlignedCollisionBoxes[iInsertPoint].Max.X = NewBoundsBox.Max.X;
						return;
					}
					else
					if (NewBoundsBox.Min.X < fTestXStart)
					{
						//We are more left than the testing lets check if we can extend the testing left					
						if (FMath::IsNearlyEqual(NewBoundsBox.Max.X, fTestXStart, THRESH_FACE_POINTS_ARE_SAME) &&
							FMath::IsNearlyEqual(NewBoundsBox.Max.Y, basicAxisAlignedCollisionBoxes[iInsertPoint].Max.Y, THRESH_FACE_POINTS_ARE_SAME) &&
							FMath::IsNearlyEqual(NewBoundsBox.Max.Z, basicAxisAlignedCollisionBoxes[iInsertPoint].Max.Z, THRESH_FACE_POINTS_ARE_SAME))
						{
							//extend testing  block  left
							basicAxisAlignedCollisionBoxes[iInsertPoint].Min.X = NewBoundsBox.Min.X;
							return;
						}

						//push in before x of testing
						basicAxisAlignedCollisionBoxes.Insert(NewBoundsBox, iInsertPoint);
						return;
					}
				}

			++iInsertPoint;
		}
	}

	basicAxisAlignedCollisionBoxes.Push(NewBoundsBox);
}

int32 SimpleGeometryCollector::ReduceAACollisionBoxes()
{
	int32 iTotal = basicAxisAlignedCollisionBoxes.Num();
	
	if (iTotal > 1)
	{
		TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());
		TESSELATION_TIMINGS(int iStartNum = iTotal);

		iTotal = InternalReduceAACollisionBoxesY(); //Y Plane merge should create flat ground planes
		iTotal = InternalReduceAACollisionBoxesZ(); //Z Plane merge created ground planes downward

		TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
		TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### SimpleGeometryCollector::ReduceAACollisionBoxes (%d to %d) %f Ms (%f s) ##################\n\n"), iStartNum, iTotal, (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));
	}

	return iTotal;
}

void SimpleGeometryCollector::AddBasicRotatedCollisionVolume(const FVector& Center, const FRotator& Rotation, const FVector& Extent)
{
	FKBoxElem SimpleBoundsBox(Extent.X, Extent.Y, Extent.Z);
	SimpleBoundsBox.Center = Center;
	SimpleBoundsBox.Rotation = Rotation;
	basicCollisionBoxes.Push(SimpleBoundsBox);
}


void SimpleGeometryCollector::clear()
{
	basicAxisAlignedCollisionBoxes.Empty();
	basicAxisAlignedCollisionBoxesIndexCache = 0;
	basicCollisionBoxes.Empty();
}

bool SimpleGeometryCollector::hasBasicCollisionVolumes()
{
	return (basicCollisionBoxes.Num() > 0 || basicAxisAlignedCollisionBoxes.Num() > 0);
}

int32 SimpleGeometryCollector::InternalReduceAACollisionBoxesY()
{
	TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());
	
	uint8 X = 0;
	uint8 Y = 1;
	uint8 Z = 2;

	const int32 iDestCount = basicAxisAlignedCollisionBoxes.Num();
	int32 iTotalCount = 0;

	if (iDestCount > 1)
	{
		int32 iCurr = 0;
		int32 iTest = 1;
		const int32 iLastIndex = iDestCount - 1;

		while (iCurr < iLastIndex)
		{
			FBox& CurrBox = basicAxisAlignedCollisionBoxes[iCurr];

			if (CurrBox.IsValid) //skip invalid
			{
				iTest = iCurr + 1;

				++iTotalCount;

				while (iTest < iDestCount)
				{
					FBox& TestBox = basicAxisAlignedCollisionBoxes[iTest];

					if (TestBox.IsValid) //skip culled
					{
						if (CurrBox.Max[Z] < (TestBox.Min[Z] - THRESH_FACE_POINTS_ARE_SAME))
						{
							//Targets Top plane is further than this ones bottom plane, list is ordered ZYX in min pos, so no box will be closer
							break;
						}

						if (FMath::IsNearlyEqual(CurrBox.Min[Z], TestBox.Min[Z], THRESH_FACE_POINTS_ARE_SAME) &&
							FMath::IsNearlyEqual(CurrBox.Max[Z], TestBox.Max[Z], THRESH_FACE_POINTS_ARE_SAME))
						{
							//on the same plane
							if (CurrBox.Max[Y] < (TestBox.Min[Y] - THRESH_FACE_POINTS_ARE_SAME))
							{
								//Targets left plane is further than this ones right plane, list is ordered ZYX in min pos, so no box will be closer
								break;
							}

							//is Start X & End X for both same? 
							if (FMath::IsNearlyEqual(CurrBox.Min[X], TestBox.Min[X], THRESH_FACE_POINTS_ARE_SAME) &&
								FMath::IsNearlyEqual(CurrBox.Max[X], TestBox.Max[X], THRESH_FACE_POINTS_ARE_SAME))
							{

								//We align in The Right Axis, can we merge forward or backwards?
								if (FMath::IsNearlyEqual(CurrBox.Max[Y], TestBox.Min[Y], THRESH_FACE_POINTS_ARE_SAME))
								{
									//Match!
									CurrBox.Max[Y] = TestBox.Max[Y];
									TestBox.IsValid = 0;
								}
							}
						}
					}
					++iTest;
				}
			}

			++iCurr;
		}
	}

	TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
	TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### SimpleGeometryCollector::InternalReduceAACollisionBoxesY (%d) %f Ms (%f s) ##################\n\n"), iTotalCount, (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));


	return iTotalCount;
}

int32 SimpleGeometryCollector::InternalReduceAACollisionBoxesZ()
{
	TESSELATION_TIMINGS(double TasksTime = FPlatformTime::Seconds());

	uint8 X = 0;
	uint8 Y = 1;
	uint8 Z = 2;

	const int32 iDestCount = basicAxisAlignedCollisionBoxes.Num();
	int32 iTotalCount = 0;

	if (iDestCount > 1)
	{
		int32 iCurr = 0;
		int32 iTest = 1;
		const int32 iLastIndex = iDestCount - 1;

		while (iCurr < iLastIndex)
		{
			FBox& CurrBox = basicAxisAlignedCollisionBoxes[iCurr];
			
			if (CurrBox.IsValid) //skip invalid
			{
				++iTotalCount;

				iTest = iCurr + 1;

				while (iTest < iDestCount)
				{
					FBox& TestBox = basicAxisAlignedCollisionBoxes[iTest];

					if (TestBox.IsValid) //skip culled
					{
						if (CurrBox.Max[Z] < (TestBox.Min[Z] - THRESH_FACE_POINTS_ARE_SAME))
						{
							//Targets Top plane is further than this ones bottom plane, list is ordered ZYX in min pos, so no box will be closer
							break;
						}

						if (FMath::IsNearlyEqual(CurrBox.Max[Z], TestBox.Min[Z], THRESH_FACE_POINTS_ARE_SAME))
						{
							//on the same plane
							if (CurrBox.Max[Y] < TestBox.Min[Y] - THRESH_FACE_POINTS_ARE_SAME)
							{
								//Targets left plane is further than this ones right plane, list is ordered ZYX in min pos, so no box will be closer
								break;
							}

							//is Start X & End X for both same? 
							if (FMath::IsNearlyEqual(CurrBox.Min[X], TestBox.Min[X], THRESH_FACE_POINTS_ARE_SAME) &&
								FMath::IsNearlyEqual(CurrBox.Max[X], TestBox.Max[X], THRESH_FACE_POINTS_ARE_SAME))
							{

								//We align in The Right Axis, can we merge forward or backwards?
								if (FMath::IsNearlyEqual(CurrBox.Min[Y], TestBox.Min[Y], THRESH_FACE_POINTS_ARE_SAME) &&
									FMath::IsNearlyEqual(CurrBox.Max[Y], TestBox.Max[Y], THRESH_FACE_POINTS_ARE_SAME))
								{
									//Match!
									CurrBox.Max[Z] = TestBox.Max[Z];
									TestBox.IsValid = 0;
								}
							}
						}
					}
					++iTest;
				}
			}

			++iCurr;
		}
	}

	TESSELATION_TIMINGS(double EndTasksTime = FPlatformTime::Seconds());
	TESSELATION_TIMINGS(FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### SimpleGeometryCollector::InternalReduceAACollisionBoxesZ (%d) %f Ms (%f s) ##################\n\n"), iTotalCount, (EndTasksTime - TasksTime)*1000.0, (EndTasksTime - TasksTime)));

	return iTotalCount;
}

//PRAGMA_ENABLE_OPTIMIZATION_ACTUAL