// Copyright 2016-2018 Chris Conway (Koderz). All Rights Reserved.

#pragma once

#include "Engine/Engine.h"
#include "RuntimeMeshCore.h"
#include "RuntimeMeshBuilder.h"

enum class ERuntimeMeshBuffersToUpdate : uint8;
struct FRuntimeMeshSectionVertexBufferParams;
struct FRuntimeMeshSectionTangentVertexBufferParams;
struct FRuntimeMeshSectionUVVertexBufferParams;
struct FRuntimeMeshSectionIndexBufferParams;
class UMaterialInterface;

class RUNTIMEMESHCOMPONENT_API FRuntimeMeshSection
{
	struct FSectionVertexBuffer
	{
	private:
		const int32 Stride;
		TArray<uint8> Data;
	public:
		FSectionVertexBuffer(int32 InStride) : Stride(InStride)
		{

		}
		virtual ~FSectionVertexBuffer() { }

		void SetData(TArray<uint8>& InVertices, bool bUseMove)
		{
			if (bUseMove)
			{
				Data = MoveTemp(InVertices);
			}
			else
			{
				Data = InVertices;
			}
		}

		template<typename VertexType>
		void SetData(const TArray<VertexType>& InVertices)
		{
			if (InVertices.Num() == 0)
			{
				Data.Empty();
				return;
			}
			check(InVertices.GetTypeSize() == GetStride());

			Data.SetNum(InVertices.GetTypeSize() * InVertices.Num());
			FMemory::Memcpy(Data.GetData(), InVertices.GetData(), Data.Num());
		}

		int32 GetStride() const
		{
			return Stride;
		}

		int32 GetNumVertices() const
		{
			return Stride > 0 ? Data.Num() / Stride : 0;
		}

		TArray<uint8>& GetData() { return Data; }

		void FillUpdateParams(FRuntimeMeshSectionVertexBufferParams& Params);

		friend FArchive& operator <<(FArchive& Ar, FSectionVertexBuffer& Buffer)
		{
			Buffer.Serialize(Ar);
			return Ar;
		}

	protected:
		virtual void Serialize(FArchive& Ar)
		{
			if (Ar.CustomVer(FRuntimeMeshVersion::GUID) < FRuntimeMeshVersion::RuntimeMeshComponentUE4_19)
			{
				FRuntimeMeshVertexStreamStructure VertexStructure;
				Ar << const_cast<FRuntimeMeshVertexStreamStructure&>(VertexStructure);
			}
			Ar << const_cast<int32&>(Stride);
			Ar << Data;
		}
	};

	struct FSectionPositionVertexBuffer : public FSectionVertexBuffer
	{
		FSectionPositionVertexBuffer()
			: FSectionVertexBuffer(sizeof(FVector))
		{

		}
	};

	struct FSectionTangentsVertexBuffer : public FSectionVertexBuffer
	{
	private:
		bool bUseHighPrecision;

	public:
		FSectionTangentsVertexBuffer(bool bInUseHighPrecision)
			: FSectionVertexBuffer(bInUseHighPrecision ? (sizeof(FPackedRGBA16N) * 2) : (sizeof(FPackedNormal) * 2))
			, bUseHighPrecision(bInUseHighPrecision)
		{

		}

		bool IsUsingHighPrecision() const { return bUseHighPrecision; }

		void FillUpdateParams(FRuntimeMeshSectionTangentVertexBufferParams& Params);

		virtual void Serialize(FArchive& Ar) override
		{
			if (Ar.CustomVer(FRuntimeMeshVersion::GUID) >= FRuntimeMeshVersion::RuntimeMeshComponentUE4_19)
			{
				Ar << bUseHighPrecision;
			}
			FSectionVertexBuffer::Serialize(Ar);
		}
	};

	struct FSectionUVsVertexBuffer : public FSectionVertexBuffer
	{
	private:
		bool bUseHighPrecision;
		int32 UVCount;

	public:

		FSectionUVsVertexBuffer(bool bInUseHighPrecision, int32 InUVCount)
			: FSectionVertexBuffer((bInUseHighPrecision ? sizeof(FVector2D) : sizeof(FVector2DHalf)) * InUVCount)
			, bUseHighPrecision(bInUseHighPrecision), UVCount(InUVCount)
		{

		}

		bool IsUsingHighPrecision() const { return bUseHighPrecision; }

		int32 NumUVs() const { return UVCount; }

		void FillUpdateParams(FRuntimeMeshSectionUVVertexBufferParams& Params);

		virtual void Serialize(FArchive& Ar) override
		{
			if (Ar.CustomVer(FRuntimeMeshVersion::GUID) >= FRuntimeMeshVersion::RuntimeMeshComponentUE4_19)
			{
				Ar << bUseHighPrecision;
				Ar << UVCount;
			}
			FSectionVertexBuffer::Serialize(Ar);
		}
	};

	struct FSectionColorVertexBuffer : public FSectionVertexBuffer
	{
		FSectionColorVertexBuffer()
			: FSectionVertexBuffer(sizeof(FColor))
		{

		}
	};

	struct FSectionIndexBuffer
	{
	private:
		const bool b32BitIndices;
		TArray<uint8> Data;
	public:
		FSectionIndexBuffer(bool bIn32BitIndices)
			: b32BitIndices(bIn32BitIndices)
		{

		}

		void SetData(TArray<uint8>& InIndices, bool bUseMove)
		{
			if (bUseMove)
			{
				Data = MoveTemp(InIndices);
			}
			else
			{
				Data = InIndices;
			}
		}

		template<typename IndexType>
		void SetData(const TArray<IndexType>& InIndices)
		{
			check(InIndices.GetTypeSize() == GetStride());

			Data.SetNum(InIndices.GetTypeSize() * InIndices.Num());
			FMemory::Memcpy(Data.GetData(), InIndices.GetData(), Data.Num());
		}

		int32 GetStride() const
		{
			return b32BitIndices ? 4 : 2;
		}

		bool Is32BitIndices() const
		{
			return b32BitIndices;
		}

		int32 GetNumIndices() const
		{
			return Data.Num() / GetStride();
		}

		TArray<uint8>& GetData() { return Data; }

		void FillUpdateParams(FRuntimeMeshSectionIndexBufferParams& Params);

		friend FArchive& operator <<(FArchive& Ar, FSectionIndexBuffer& Buffer)
		{
			Ar << const_cast<bool&>(Buffer.b32BitIndices);
			Ar << Buffer.Data;
			return Ar;
		}
	};


	struct FSectionLOD
	{
		/** Vertex buffer containing the positions for this section */
		FSectionPositionVertexBuffer PositionBuffer;

		/** Vertex buffer containing the tangents for this section */
		FSectionTangentsVertexBuffer TangentsBuffer;

		/** Vertex buffer containing the UVs for this section */
		FSectionUVsVertexBuffer UVsBuffer;

		/** Vertex buffer containing the colors for this section */
		FSectionColorVertexBuffer ColorBuffer;

		FSectionIndexBuffer IndexBuffer;
		FSectionIndexBuffer AdjacencyIndexBuffer;


		FSectionLOD(bool bInUseHighPrecisionTangents = true, bool bInUseHighPrecisionUVs = true, int32 InNumUVs = 0, bool b32BitIndices = true) :
			  TangentsBuffer(bInUseHighPrecisionTangents)
			, UVsBuffer(bInUseHighPrecisionUVs, InNumUVs)
			, IndexBuffer(b32BitIndices)
			, AdjacencyIndexBuffer(b32BitIndices)
		{}

	};


	const EUpdateFrequency UpdateFrequency;

	FSectionLOD						Master_LODMeshdata;		//LOD 0
	TMap < int32, FSectionLOD* >	LODMeshData;			//other LODs

	FBox LocalBoundingBox;

	bool bCollisionEnabled;

	bool bIsVisible;

	bool bCastsShadow;

	//	TUniquePtr<FRuntimeMeshLockProvider> SyncRoot;
public:
	FRuntimeMeshSection(FArchive& Ar);
	FRuntimeMeshSection(bool bInUseHighPrecisionTangents, bool bInUseHighPrecisionUVs, int32 InNumUVs, bool b32BitIndices, EUpdateFrequency InUpdateFrequency/*, FRuntimeMeshLockProvider* InSyncRoot*/);

	~FRuntimeMeshSection();

	// 	void SetNewLockProvider(FRuntimeMeshLockProvider* NewSyncRoot)
	// 	{
	// 		SyncRoot.Reset(NewSyncRoot);
	// 	}

	//	FRuntimeMeshLockProvider GetSyncRoot() { return SyncRoot->Get(); }

	bool IsCollisionEnabled() const { return bCollisionEnabled; }
	bool IsVisible() const { return bIsVisible; }
	bool ShouldRender() const { return IsVisible() && HasValidMeshData(); }
	bool CastsShadow() const { return bCastsShadow; }
	EUpdateFrequency GetUpdateFrequency() const { return UpdateFrequency; }
	FBox GetBoundingBox() const { return LocalBoundingBox; }

	FSectionLOD& GetLODMeshData(int32 LOD) { return (LOD == 0) ? Master_LODMeshdata : *LODMeshData[LOD]; }
	const FSectionLOD& GetLODMeshData(int32 LOD)const { return (LOD == 0)? Master_LODMeshdata : *LODMeshData[LOD]; }

	void CreateLODMeshData(int32 LOD, bool bInUseHighPrecisionTangents, bool bInUseHighPrecisionUVs, int32 InNumUVs, bool b32BitIndices);

	void ClearAllMeshBuffers();

	TMap < int32, FSectionLOD* >& GetSubLODS() { return LODMeshData; };

	int32 GetNumVertices(int32 LOD) const { return GetLODMeshData(LOD).PositionBuffer.GetNumVertices(); }
	int32 GetNumIndices(int32 LOD) const { return GetLODMeshData(LOD).IndexBuffer.GetNumIndices(); }

	bool HasValidMeshData() const {

		//Check Main LOD first
		auto& MainLODMeshData = GetLODMeshData(0);
		if (MainLODMeshData.IndexBuffer.GetNumIndices() > 0 &&
			MainLODMeshData.PositionBuffer.GetNumVertices() > 0 &&
			MainLODMeshData.TangentsBuffer.GetNumVertices() > 0 &&
			MainLODMeshData.TangentsBuffer.GetNumVertices() == MainLODMeshData.PositionBuffer.GetNumVertices() &&
			MainLODMeshData.UVsBuffer.GetNumVertices() > 0 &&
			MainLODMeshData.UVsBuffer.GetNumVertices() == MainLODMeshData.PositionBuffer.GetNumVertices() &&
			MainLODMeshData.ColorBuffer.GetNumVertices() > 0 &&
			MainLODMeshData.ColorBuffer.GetNumVertices() == MainLODMeshData.PositionBuffer.GetNumVertices())
		{
			return true;
		}

		//Check other LODS if main fails
		for (auto& MeshDataLod : LODMeshData)
		{
			auto MeshData = MeshDataLod.Value;
			if (MeshData->IndexBuffer.GetNumIndices() > 0 &&
				MeshData->PositionBuffer.GetNumVertices() > 0 &&
				MeshData->TangentsBuffer.GetNumVertices() > 0 &&
				MeshData->TangentsBuffer.GetNumVertices() == MeshData->PositionBuffer.GetNumVertices() &&
				MeshData->UVsBuffer.GetNumVertices() > 0 &&
				MeshData->UVsBuffer.GetNumVertices() == MeshData->PositionBuffer.GetNumVertices() &&
				MeshData->ColorBuffer.GetNumVertices() > 0 &&
				MeshData->ColorBuffer.GetNumVertices() == MeshData->PositionBuffer.GetNumVertices())
			{
				return true; //found a valid mesh LOD
			}
		}

		return false;
	}

	void SetVisible(bool bNewVisible)
	{
		bIsVisible = bNewVisible;
	}
	void SetCastsShadow(bool bNewCastsShadow)
	{
		bCastsShadow = bNewCastsShadow;
	}
	void SetCollisionEnabled(bool bNewCollision)
	{
		bCollisionEnabled = bNewCollision;
	}

	void UpdatePositionBuffer(TArray<uint8>& InVertices,int32 LOD, bool bUseMove)
	{
		GetLODMeshData(LOD).PositionBuffer.SetData(InVertices, bUseMove);
		UpdateBoundingBox();
	}

	template<typename VertexType>
	void UpdatePositionBuffer(const TArray<VertexType>& InVertices, int32 LOD, const FBox* BoundingBox = nullptr)
	{
		GetLODMeshData(LOD).PositionBuffer.SetData(InVertices);
	
		if (BoundingBox)
		{
			LocalBoundingBox = *BoundingBox;
		}
		else
		{
			UpdateBoundingBox();
		}
		
	}

	void UpdateTangentsBuffer(TArray<uint8>& InVertices, int32 LOD, bool bUseMove)
	{
		GetLODMeshData(LOD).TangentsBuffer.SetData(InVertices, bUseMove);
	}

	template<typename VertexType>
	void UpdateTangentsBuffer(const TArray<VertexType>& InVertices, int32 LOD)
	{
		GetLODMeshData(LOD).TangentsBuffer.SetData(InVertices);
	}

	void UpdateUVsBuffer(TArray<uint8>& InVertices, int32 LOD, bool bUseMove)
	{
		GetLODMeshData(LOD).UVsBuffer.SetData(InVertices, bUseMove);
	}

	template<typename VertexType>
	void UpdateUVsBuffer(const TArray<VertexType>& InVertices,int32 LOD)
	{
		GetLODMeshData(LOD).UVsBuffer.SetData(InVertices);
	}

	void UpdateColorBuffer(TArray<uint8>& InVertices, int32 LOD, bool bUseMove)
	{
		GetLODMeshData(LOD).ColorBuffer.SetData(InVertices, bUseMove);
	}

	template<typename VertexType>
	void UpdateColorBuffer(const TArray<VertexType>& InVertices, int32 LOD)
	{
		GetLODMeshData(LOD).ColorBuffer.SetData(InVertices);
	}

	void UpdateIndexBuffer(TArray<uint8>& InIndices, int32 LOD, bool bUseMove)
	{
		GetLODMeshData(LOD).IndexBuffer.SetData(InIndices, bUseMove);
	}

	template<typename IndexType>
	void UpdateIndexBuffer(const TArray<IndexType>& InIndices, int32 LOD)
	{
		GetLODMeshData(LOD).IndexBuffer.SetData(InIndices);
	}

	template<typename IndexType>
	void UpdateAdjacencyIndexBuffer(const TArray<IndexType>& InIndices, int32 LOD)
	{
		GetLODMeshData(LOD).AdjacencyIndexBuffer.SetData(InIndices);
	}

	TSharedPtr<FRuntimeMeshAccessor> GetSectionMeshAccessor(int32 LOD)
	{
		auto& MeshLOD = GetLODMeshData(LOD);
		return MakeShared<FRuntimeMeshAccessor>(MeshLOD.TangentsBuffer.IsUsingHighPrecision(), MeshLOD.UVsBuffer.IsUsingHighPrecision(), MeshLOD.UVsBuffer.NumUVs(), MeshLOD.IndexBuffer.Is32BitIndices(),
			&MeshLOD.PositionBuffer.GetData(), &MeshLOD.TangentsBuffer.GetData(), &MeshLOD.UVsBuffer.GetData(), &MeshLOD.ColorBuffer.GetData(), &MeshLOD.IndexBuffer.GetData());
	}

	TUniquePtr<FRuntimeMeshScopedUpdater> GetSectionMeshUpdater(const FRuntimeMeshDataPtr& ParentData, int32 SectionIndex, int32 LOD, ESectionUpdateFlags UpdateFlags, FRuntimeMeshLockProvider* LockProvider, bool bIsReadonly)
	{
		auto& MeshLOD = GetLODMeshData(LOD);
		return TUniquePtr<FRuntimeMeshScopedUpdater>(new FRuntimeMeshScopedUpdater(ParentData, SectionIndex, LOD, UpdateFlags, MeshLOD.TangentsBuffer.IsUsingHighPrecision(), MeshLOD.UVsBuffer.IsUsingHighPrecision(), MeshLOD.UVsBuffer.NumUVs(), MeshLOD.IndexBuffer.Is32BitIndices(),
			&MeshLOD.PositionBuffer.GetData(), &MeshLOD.TangentsBuffer.GetData(), &MeshLOD.UVsBuffer.GetData(), &MeshLOD.ColorBuffer.GetData(), &MeshLOD.IndexBuffer.GetData(), LockProvider, bIsReadonly));
	}

	TSharedPtr<FRuntimeMeshIndicesAccessor> GetTessellationIndexAccessor(int32 LOD)
	{
		auto& MeshLOD = GetLODMeshData(LOD);
		return MakeShared<FRuntimeMeshIndicesAccessor>(MeshLOD.AdjacencyIndexBuffer.Is32BitIndices(), &MeshLOD.AdjacencyIndexBuffer.GetData());
	}








	bool CheckTangentBuffer(bool bInUseHighPrecision, int32 LOD) const
	{
		auto& MeshLOD = GetLODMeshData(LOD);

		return MeshLOD.TangentsBuffer.IsUsingHighPrecision() == bInUseHighPrecision;
	}

	bool CheckUVBuffer(bool bInUseHighPrecision, int32 InNumUVs, int32 LOD) const
	{
		auto& MeshLOD = GetLODMeshData(LOD);

		return MeshLOD.UVsBuffer.IsUsingHighPrecision() == bInUseHighPrecision && MeshLOD.UVsBuffer.NumUVs() == InNumUVs;
	}

	bool CheckIndexBufferSize(bool b32BitIndices, int32 LOD) const
	{
		auto& MeshLOD = GetLODMeshData(LOD);

		return b32BitIndices == MeshLOD.IndexBuffer.Is32BitIndices();
	}



	TSharedPtr<struct FRuntimeMeshSectionCreationParams, ESPMode::NotThreadSafe> GetSectionCreationParams(int32 LOD);

	TSharedPtr<struct FRuntimeMeshSectionUpdateParams, ESPMode::NotThreadSafe> GetSectionUpdateData(ERuntimeMeshBuffersToUpdate BuffersToUpdate, int32 LOD);

	TSharedPtr<struct FRuntimeMeshSectionPropertyUpdateParams, ESPMode::NotThreadSafe> GetSectionPropertyUpdateData();

	void UpdateBoundingBox();
	void SetBoundingBox(const FBox& InBoundingBox) { LocalBoundingBox = InBoundingBox; }

	int32 GetCollisionData(TArray<FVector>& OutPositions, TArray<FTriIndices>& OutIndices, TArray<FVector2D>& OutUVs);


	friend FArchive& operator <<(FArchive& Ar, FRuntimeMeshSection& MeshData)
	{
		Ar << const_cast<EUpdateFrequency&>(MeshData.UpdateFrequency);

		//D11.SC TODO!! Only archive LOD0 for now

		if (Ar.CustomVer(FRuntimeMeshVersion::GUID) >= FRuntimeMeshVersion::RuntimeMeshComponentUE4_19)
		{
			Ar << MeshData.Master_LODMeshdata.PositionBuffer;
			Ar << MeshData.Master_LODMeshdata.TangentsBuffer;
			Ar << MeshData.Master_LODMeshdata.UVsBuffer;
			Ar << MeshData.Master_LODMeshdata.ColorBuffer;
		}
		else
		{
			// This is a hack to read the old data and ignore it
			Ar << MeshData.Master_LODMeshdata.PositionBuffer;
			Ar << MeshData.Master_LODMeshdata.PositionBuffer;
			Ar << MeshData.Master_LODMeshdata.PositionBuffer;
		}


		Ar << MeshData.Master_LODMeshdata.IndexBuffer;
		Ar << MeshData.Master_LODMeshdata.AdjacencyIndexBuffer;

		Ar << MeshData.LocalBoundingBox;

		Ar << MeshData.bCollisionEnabled;
		Ar << MeshData.bIsVisible;
		Ar << MeshData.bCastsShadow;

		// This is a hack to read the old data and ignore it
		if (Ar.CustomVer(FRuntimeMeshVersion::GUID) < FRuntimeMeshVersion::RuntimeMeshComponentUE4_19)
		{
			TArray<FVector> NullPositions;
			TArray<uint8> NullIndices;
			MeshData.Master_LODMeshdata.PositionBuffer.SetData(NullPositions);
			MeshData.Master_LODMeshdata.IndexBuffer.SetData(NullIndices, false);
			MeshData.Master_LODMeshdata.AdjacencyIndexBuffer.SetData(NullIndices, false);
		}

		return Ar;
	}
};




/** Smart pointer to a Runtime Mesh Section */
using FRuntimeMeshSectionPtr = TSharedPtr<FRuntimeMeshSection, ESPMode::ThreadSafe>;




FORCEINLINE static FArchive& operator <<(FArchive& Ar, FRuntimeMeshSectionPtr& Section)
{
	if (Ar.IsSaving())
	{
		bool bHasSection = Section.IsValid();
		Ar << bHasSection;
		if (bHasSection)
		{
			Ar << *Section.Get();
		}
	}
	else if (Ar.IsLoading())
	{
		bool bHasSection;
		Ar << bHasSection;
		if (bHasSection)
		{
			Section = MakeShared<FRuntimeMeshSection, ESPMode::ThreadSafe>(Ar);
		}
	}
	return Ar;
}