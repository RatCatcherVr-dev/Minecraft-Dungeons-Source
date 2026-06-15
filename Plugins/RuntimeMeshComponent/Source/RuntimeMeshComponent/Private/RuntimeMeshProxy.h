// Copyright 2016-2018 Chris Conway (Koderz). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshSectionProxy.h"
#include "RuntimeMeshUpdateCommands.h"


template<typename Type>
struct FRuntimeMeshRenderThreadDeleter
{
	void operator()(Type* Object) const
	{
		// This is a custom deleter to make sure the runtime mesh proxy is only ever deleted on the rendering thread.
		if (IsInRenderingThread())
		{
			delete Object;
		}
		else
		{
			// HORU: 4.22 rendering
			ENQUEUE_RENDER_COMMAND(FRuntimeMeshProxyDeleterCommand)(
				[Object](FRHICommandListImmediate & RHICmdList)
				{
					delete static_cast<Type*>(Object);
				}
			);
		}
	}
};

/**
 *
 */
class FRuntimeMeshProxy
{
	ERHIFeatureLevel::Type FeatureLevel;

	TMap < int32, TMap< int32, FRuntimeMeshSectionProxyPtr > > Sections; //D11.SC Changed this to LOD Mapping [SectorID][LOD]

public:
	FRuntimeMeshProxy(ERHIFeatureLevel::Type InFeatureLevel);
	~FRuntimeMeshProxy();

	ERHIFeatureLevel::Type GetFeatureLevel() const { return FeatureLevel; }


	void CreateSection_GameThread(int32 SectionId, const FRuntimeMeshSectionCreationParamsPtr& SectionData);
	void CreateSection_RenderThread(int32 SectionId, const FRuntimeMeshSectionCreationParamsPtr& SectionData);
	void UpdateSection_GameThread(int32 SectionId, const FRuntimeMeshSectionUpdateParamsPtr& SectionData);
	void UpdateSection_RenderThread(int32 SectionId, const FRuntimeMeshSectionUpdateParamsPtr& SectionData);
	void UpdateSectionProperties_GameThread(int32 SectionId, const FRuntimeMeshSectionPropertyUpdateParamsPtr& SectionData);
	void UpdateSectionProperties_RenderThread(int32 SectionId, const FRuntimeMeshSectionPropertyUpdateParamsPtr& SectionData);
	void DeleteSection_GameThread(int32 SectionId);
	void DeleteSection_RenderThread(int32 SectionId);



	TMap < int32, TMap< int32, FRuntimeMeshSectionProxyPtr > >& GetSections()			{ return Sections; } //TODO error checking
	

	void CalculateViewRelevance(bool& bHasStaticSections, bool& bHasDynamicSections, bool& bHasShadowableSections)
	{
		check(IsInRenderingThread());
		bHasStaticSections = false;
		bHasDynamicSections = false;
		bHasShadowableSections = false;
		for (const auto& SectionEntry : Sections)
		{
			if (SectionEntry.Value.Contains(0) && SectionEntry.Value[0].IsValid())
			{
				bool bWantsStaticPath = SectionEntry.Value[0]->WantsToRenderInStaticPath();
				bHasStaticSections |= bWantsStaticPath;
				bHasDynamicSections |= !bWantsStaticPath;
				bHasShadowableSections = SectionEntry.Value[0]->CastsShadow();
			}
		}
	}
private:
	void UpdateCachedValues();

};

