// Copyright 2016-2018 Chris Conway (Koderz). All Rights Reserved.

#include "RuntimeMeshSection.h"
#include "RuntimeMeshComponentPlugin.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "RuntimeMeshUpdateCommands.h"

template<typename Type>
struct FRuntimeMeshStreamAccessor
{
	const TArray<uint8>* Data;
	int32 Offset;
	int32 Stride;
public:
	FRuntimeMeshStreamAccessor(const TArray<uint8>* InData, int32 InOffset, int32 InStride)
		: Data(InData), Offset(InOffset), Stride(InStride)
	{
	}
	virtual ~FRuntimeMeshStreamAccessor() { }

	int32 Num() const { return Data->Num() / Stride; }

	Type& Get(int32 Index)
	{
		int32 StartPosition = (Index * Stride + Offset);
		return *((Type*)(&(*Data)[StartPosition]));
	}
};

// Helper for accessing position element within a vertex stream
struct FRuntimeMeshVertexStreamPositionAccessor : public FRuntimeMeshStreamAccessor<FVector>
{
public:
	FRuntimeMeshVertexStreamPositionAccessor(TArray<uint8>* InData, const FRuntimeMeshVertexStreamStructure& StreamStructure)
		: FRuntimeMeshStreamAccessor<FVector>(InData, StreamStructure.Position.Offset, StreamStructure.Position.Stride)
	{
		check(StreamStructure.Position.IsValid());
	}
};

struct FRuntimeMeshVertexStreamUVAccessor
{
	virtual ~FRuntimeMeshVertexStreamUVAccessor() { }

	virtual FVector2D GetUV(int32 Index) = 0;
	virtual int32 Num() = 0;
};

// Helper for accessing position element within a vertex stream
struct FRuntimeMeshVertexStreamUVFullPrecisionAccessor : public FRuntimeMeshStreamAccessor<FVector2D>, public FRuntimeMeshVertexStreamUVAccessor
{
public:
	FRuntimeMeshVertexStreamUVFullPrecisionAccessor(TArray<uint8>* InData, const FRuntimeMeshVertexStreamStructureElement& Element)
		: FRuntimeMeshStreamAccessor<FVector2D>(InData, Element.Offset, Element.Stride)
	{
		check(Element.IsValid());
	}

	virtual FVector2D GetUV(int32 Index) override
	{
		return Get(Index);
	}

	virtual int32 Num() override
	{
		return FRuntimeMeshStreamAccessor<FVector2D>::Num();
	}
};
struct FRuntimeMeshVertexStreamUVHalfPrecisionAccessor : public FRuntimeMeshStreamAccessor<FVector2DHalf>, public FRuntimeMeshVertexStreamUVAccessor
{
public:
	FRuntimeMeshVertexStreamUVHalfPrecisionAccessor(TArray<uint8>* InData, const FRuntimeMeshVertexStreamStructureElement& Element)
		: FRuntimeMeshStreamAccessor<FVector2DHalf>(InData, Element.Offset, Element.Stride)
	{
		check(Element.IsValid());
	}

	virtual FVector2D GetUV(int32 Index) override
	{
		return Get(Index);
	}

	virtual int32 Num() override
	{
		return FRuntimeMeshStreamAccessor<FVector2DHalf>::Num();
	}
};


void FRuntimeMeshSection::FSectionVertexBuffer::FillUpdateParams(FRuntimeMeshSectionVertexBufferParams& Params)
{
	Params.Data = Data;
	Params.NumVertices = GetNumVertices();
}

void FRuntimeMeshSection::FSectionIndexBuffer::FillUpdateParams(FRuntimeMeshSectionIndexBufferParams& Params)
{
	Params.b32BitIndices = b32BitIndices;
	Params.Data = Data;
	Params.NumIndices = GetNumIndices();
}

FRuntimeMeshSection::FRuntimeMeshSection(bool bInUseHighPrecisionTangents, bool bInUseHighPrecisionUVs, int32 InNumUVs, bool b32BitIndices, EUpdateFrequency InUpdateFrequency/*, FRuntimeMeshLockProvider* InSyncRoot*/)
	: UpdateFrequency(InUpdateFrequency)
	, Master_LODMeshdata(bInUseHighPrecisionTangents, bInUseHighPrecisionUVs, InNumUVs, b32BitIndices)
	, LocalBoundingBox(EForceInit::ForceInitToZero)
	, bCollisionEnabled(false)
	, bIsVisible(true)
	, bCastsShadow(true)
//	, SyncRoot(InSyncRoot)
{

	
}

FRuntimeMeshSection::FRuntimeMeshSection(FArchive& Ar)
	: UpdateFrequency(EUpdateFrequency::Average)
	, Master_LODMeshdata(false, false, 1, false)
	, LocalBoundingBox(EForceInit::ForceInitToZero)
	, bCollisionEnabled(false)
	, bIsVisible(true)
	, bCastsShadow(true)
{
	Ar << *this;
}

FRuntimeMeshSection::~FRuntimeMeshSection()
{
	//Clean out LOD data
	for (auto& Entry : LODMeshData)
	{
		delete Entry.Value;
	}
}

FRuntimeMeshSectionCreationParamsPtr FRuntimeMeshSection::GetSectionCreationParams(int32 LOD)
{
	FRuntimeMeshSectionCreationParamsPtr CreationParams = MakeShared<FRuntimeMeshSectionCreationParams, ESPMode::NotThreadSafe>();

	CreationParams->UpdateFrequency = UpdateFrequency;

	auto& MeshLOD = GetLODMeshData(LOD);

	MeshLOD.PositionBuffer.FillUpdateParams(CreationParams->PositionVertexBuffer);
	MeshLOD.TangentsBuffer.FillUpdateParams(CreationParams->TangentsVertexBuffer);
	MeshLOD.UVsBuffer.FillUpdateParams(CreationParams->UVsVertexBuffer);
	MeshLOD.ColorBuffer.FillUpdateParams(CreationParams->ColorVertexBuffer);

	MeshLOD.IndexBuffer.FillUpdateParams(CreationParams->IndexBuffer);
	MeshLOD.AdjacencyIndexBuffer.FillUpdateParams(CreationParams->AdjacencyIndexBuffer);

	CreationParams->bIsVisible = bIsVisible;
	CreationParams->bCastsShadow = bCastsShadow;
	CreationParams->iLOD = LOD;
	return CreationParams;
}

FRuntimeMeshSectionUpdateParamsPtr FRuntimeMeshSection::GetSectionUpdateData(ERuntimeMeshBuffersToUpdate BuffersToUpdate, int32 LOD)
{
	FRuntimeMeshSectionUpdateParamsPtr UpdateParams = MakeShared<FRuntimeMeshSectionUpdateParams, ESPMode::NotThreadSafe>();

	UpdateParams->BuffersToUpdate = BuffersToUpdate;

	auto& MeshLOD = GetLODMeshData(LOD);

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::PositionBuffer))
	{
		MeshLOD.PositionBuffer.FillUpdateParams(UpdateParams->PositionVertexBuffer);
	}

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::TangentBuffer))
	{
		MeshLOD.TangentsBuffer.FillUpdateParams(UpdateParams->TangentsVertexBuffer);
	}

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::UVBuffer))
	{
		MeshLOD.UVsBuffer.FillUpdateParams(UpdateParams->UVsVertexBuffer);
	}

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::ColorBuffer))
	{
		MeshLOD.ColorBuffer.FillUpdateParams(UpdateParams->ColorVertexBuffer);
	}

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::IndexBuffer))
	{
		MeshLOD.IndexBuffer.FillUpdateParams(UpdateParams->IndexBuffer);
	}

	if (!!(BuffersToUpdate & ERuntimeMeshBuffersToUpdate::AdjacencyIndexBuffer))
	{
		MeshLOD.AdjacencyIndexBuffer.FillUpdateParams(UpdateParams->AdjacencyIndexBuffer);
	}

	UpdateParams->iLOD = LOD;

	return UpdateParams;
}

TSharedPtr<struct FRuntimeMeshSectionPropertyUpdateParams, ESPMode::NotThreadSafe> FRuntimeMeshSection::GetSectionPropertyUpdateData()
{
	FRuntimeMeshSectionPropertyUpdateParamsPtr UpdateParams = MakeShared<FRuntimeMeshSectionPropertyUpdateParams, ESPMode::NotThreadSafe>();

	UpdateParams->bCastsShadow = bCastsShadow;
	UpdateParams->bIsVisible = bIsVisible;
	UpdateParams->iLOD = 0; //todo

	return UpdateParams;
}

void FRuntimeMeshSection::UpdateBoundingBox()
{
	auto& MainMeshLOD = GetLODMeshData(0);
	FBox NewBoundingBox(reinterpret_cast<FVector*>(MainMeshLOD.PositionBuffer.GetData().GetData()), MainMeshLOD.PositionBuffer.GetNumVertices());
	
	//make sure the bounding box fits all mesh LODs
	for (auto& MeshLodVAL : LODMeshData)
	{
		auto MeshLOD = MeshLodVAL.Value;
		FBox LodBoundingBox(reinterpret_cast<FVector*>(MeshLOD->PositionBuffer.GetData().GetData()), MeshLOD->PositionBuffer.GetNumVertices());
		NewBoundingBox += LodBoundingBox;
	}
	
	LocalBoundingBox = NewBoundingBox;
}

int32 FRuntimeMeshSection::GetCollisionData(TArray<FVector>& OutPositions, TArray<FTriIndices>& OutIndices, TArray<FVector2D>& OutUVs)
{ 
 	int32 StartVertexPosition = OutPositions.Num();

	auto& Master_MeshLOD = GetLODMeshData(0);
	
	if (Master_MeshLOD.PositionBuffer.GetNumVertices() > 0)
	{
		OutPositions.Append(reinterpret_cast<FVector*>(Master_MeshLOD.PositionBuffer.GetData().GetData()), Master_MeshLOD.PositionBuffer.GetNumVertices());

		bool bCopyUVs = UPhysicsSettings::Get()->bSupportUVFromHitResults;

		TArray<uint8>& IndexData = Master_MeshLOD.IndexBuffer.GetData();

		if (Master_MeshLOD.IndexBuffer.Is32BitIndices())
		{
			int32 NumIndices = Master_MeshLOD.IndexBuffer.GetNumIndices();
			for (int32 Index = 0; Index < NumIndices; Index += 3)
			{
				// Add the triangle
				FTriIndices& Triangle = *new (OutIndices) FTriIndices;
				Triangle.v0 = (*((int32*)&IndexData[(Index + 0) * 4])) + StartVertexPosition;
				Triangle.v1 = (*((int32*)&IndexData[(Index + 1) * 4])) + StartVertexPosition;
				Triangle.v2 = (*((int32*)&IndexData[(Index + 2) * 4])) + StartVertexPosition;
			}
		}
		else
		{
			int32 NumIndices = Master_MeshLOD.IndexBuffer.GetNumIndices();
			for (int32 Index = 0; Index < NumIndices; Index += 3)
			{
				// Add the triangle
				FTriIndices& Triangle = *new (OutIndices) FTriIndices;
				Triangle.v0 = (*((uint16*)&IndexData[(Index + 0) * 2])) + StartVertexPosition;
				Triangle.v1 = (*((uint16*)&IndexData[(Index + 1) * 2])) + StartVertexPosition;
				Triangle.v2 = (*((uint16*)&IndexData[(Index + 2) * 2])) + StartVertexPosition;
			}
		}
		
		return Master_MeshLOD.IndexBuffer.GetNumIndices() / 3;
	}
	else
	{
		//Main LOD doesnt have any verts, lets use the next one we can find

		for (auto& MeshLodVAL : LODMeshData)
		{
			auto MeshLOD = MeshLodVAL.Value;

			if (MeshLOD->PositionBuffer.GetNumVertices() > 0)
			{
				OutPositions.Append(reinterpret_cast<FVector*>(MeshLOD->PositionBuffer.GetData().GetData()), MeshLOD->PositionBuffer.GetNumVertices());

				bool bCopyUVs = UPhysicsSettings::Get()->bSupportUVFromHitResults;

				TArray<uint8>& IndexData = MeshLOD->IndexBuffer.GetData();

				if (MeshLOD->IndexBuffer.Is32BitIndices())
				{
					int32 NumIndices = MeshLOD->IndexBuffer.GetNumIndices();
					for (int32 Index = 0; Index < NumIndices; Index += 3)
					{
						// Add the triangle
						FTriIndices& Triangle = *new (OutIndices) FTriIndices;
						Triangle.v0 = (*((int32*)&IndexData[(Index + 0) * 4])) + StartVertexPosition;
						Triangle.v1 = (*((int32*)&IndexData[(Index + 1) * 4])) + StartVertexPosition;
						Triangle.v2 = (*((int32*)&IndexData[(Index + 2) * 4])) + StartVertexPosition;
					}
				}
				else
				{
					int32 NumIndices = MeshLOD->IndexBuffer.GetNumIndices();
					for (int32 Index = 0; Index < NumIndices; Index += 3)
					{
						// Add the triangle
						FTriIndices& Triangle = *new (OutIndices) FTriIndices;
						Triangle.v0 = (*((uint16*)&IndexData[(Index + 0) * 2])) + StartVertexPosition;
						Triangle.v1 = (*((uint16*)&IndexData[(Index + 1) * 2])) + StartVertexPosition;
						Triangle.v2 = (*((uint16*)&IndexData[(Index + 2) * 2])) + StartVertexPosition;
					}
				}

				return MeshLOD->IndexBuffer.GetNumIndices() / 3;
			}

		}
	}

	
	return 0;
}


void FRuntimeMeshSection::FSectionTangentsVertexBuffer::FillUpdateParams(FRuntimeMeshSectionTangentVertexBufferParams& Params)
{
	Params.bUsingHighPrecision = bUseHighPrecision;
	FSectionVertexBuffer::FillUpdateParams(Params);
}

void FRuntimeMeshSection::FSectionUVsVertexBuffer::FillUpdateParams(FRuntimeMeshSectionUVVertexBufferParams& Params)
{
	Params.bUsingHighPrecision = bUseHighPrecision;
	Params.NumUVs = UVCount;
	FSectionVertexBuffer::FillUpdateParams(Params);
}

void FRuntimeMeshSection::CreateLODMeshData(int32 LOD, bool bInUseHighPrecisionTangents, bool bInUseHighPrecisionUVs, int32 InNumUVs, bool b32BitIndices)
{
	if (LOD != 0) //dont need to create lod 0
	{
		if (!LODMeshData.Contains(LOD))
		{
			LODMeshData.Add(LOD, new FSectionLOD(bInUseHighPrecisionTangents, bInUseHighPrecisionUVs, InNumUVs, b32BitIndices));
		}
	}
}

void FRuntimeMeshSection::ClearAllMeshBuffers()
{
	//clear out LOD0
	Master_LODMeshdata.PositionBuffer.GetData().Empty();
	Master_LODMeshdata.TangentsBuffer.GetData().Empty();
	Master_LODMeshdata.UVsBuffer.GetData().Empty();
	Master_LODMeshdata.ColorBuffer.GetData().Empty();
	Master_LODMeshdata.IndexBuffer.GetData().Empty();
	Master_LODMeshdata.AdjacencyIndexBuffer.GetData().Empty();

	//CLear out other LODS
	for (auto& Entry : LODMeshData)
	{
		FSectionLOD* pSectionLOD = Entry.Value;
		pSectionLOD->PositionBuffer.GetData().Empty();
		pSectionLOD->TangentsBuffer.GetData().Empty();
		pSectionLOD->UVsBuffer.GetData().Empty();
		pSectionLOD->ColorBuffer.GetData().Empty();
		pSectionLOD->IndexBuffer.GetData().Empty();
		pSectionLOD->AdjacencyIndexBuffer.GetData().Empty();
	}
}

