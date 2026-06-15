// Copyright 2016-2018 Chris Conway (Koderz). All Rights Reserved.

#include "RuntimeMeshComponentProxy.h"
#include "RuntimeMeshComponentPlugin.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshProxy.h"
#include "PhysicsEngine/BodySetup.h"
#include "TessellationRendering.h"
#include "PrimitiveSceneProxy.h"


//#pragma optimize("",off)

#define USE_CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID (!WITH_EDITOR)

FRuntimeMeshComponentSceneProxy::FRuntimeMeshComponentSceneProxy(URuntimeMeshComponent* Component) 
	: FPrimitiveSceneProxy(Component)
	, BodySetup(Component->GetBodySetup())
{
	bStaticElementsAlwaysUseProxyPrimitiveUniformBuffer = true;

	check(Component->GetRuntimeMesh() != nullptr);

	RuntimeMeshProxy = Component->GetRuntimeMesh()->EnsureProxyCreated(GetScene().GetFeatureLevel());

	// Setup our material map


	for (auto SectionId : Component->GetRuntimeMesh()->GetSectionIds())
	{
		UMaterialInterface* Mat = Component->GetMaterial(SectionId);
		if (Mat == nullptr)
		{
			Mat = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		SectionRenderData.Add(SectionId, FRuntimeMeshSectionRenderData{ Mat, false });

		MaterialRelevance |= Mat->GetRelevance(GetScene().GetFeatureLevel());
	}
}

FRuntimeMeshComponentSceneProxy::~FRuntimeMeshComponentSceneProxy()
{

}

void FRuntimeMeshComponentSceneProxy::CreateRenderThreadResources()
{
	RuntimeMeshProxy->CalculateViewRelevance(bHasStaticSections, bHasDynamicSections, bHasShadowableSections);

	for (auto& Entry : SectionRenderData)
	{
		if (RuntimeMeshProxy->GetSections().Contains(Entry.Key) && RuntimeMeshProxy->GetSections()[Entry.Key].Contains(0))
		{
			FRuntimeMeshSectionProxyPtr Section = RuntimeMeshProxy->GetSections()[Entry.Key][0];

			auto& RenderData = SectionRenderData[Entry.Key];

			RenderData.bWantsAdjacencyInfo = RequiresAdjacencyInformation(RenderData.Material, RuntimeMeshProxy->GetSections()[Entry.Key][0]->GetVertexFactory()->GetType(), GetScene().GetFeatureLevel());
		}
	}

	FPrimitiveSceneProxy::CreateRenderThreadResources();
}

FPrimitiveViewRelevance FRuntimeMeshComponentSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);

	bool bForceDynamicPath = !IsStaticPathAvailable() || IsRichView(*View->Family) || IsSelected() || View->Family->EngineShowFlags.Wireframe;
	Result.bStaticRelevance = !bForceDynamicPath && bHasStaticSections;
	Result.bDynamicRelevance = bForceDynamicPath || bHasDynamicSections;

	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}

void FRuntimeMeshComponentSceneProxy::CreateMeshBatch(FMeshBatch& MeshBatch, const FRuntimeMeshSectionProxyPtr& Section, const FRuntimeMeshSectionRenderData& RenderData, FMaterialRenderProxy* Material, FMaterialRenderProxy* WireframeMaterial) const
{
	bool bRenderWireframe = WireframeMaterial != nullptr;
	bool bWantsAdjacency = !bRenderWireframe && RenderData.bWantsAdjacencyInfo;


	Section->CreateMeshBatch(MeshBatch, bWantsAdjacency);
	MeshBatch.bWireframe = WireframeMaterial != nullptr;
	MeshBatch.MaterialRenderProxy = MeshBatch.bWireframe ? WireframeMaterial : Material;

	MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
	MeshBatch.bCanApplyViewModeOverrides = true;	

	//HORU: 4.22 compat
	//FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
	//BatchElement.PrimitiveUniformBufferResource = &GetUniformBuffer();
}

void FRuntimeMeshComponentSceneProxy::DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
{
	for (const auto& SectionEntry : RuntimeMeshProxy->GetSections())
	{
		if (SectionRenderData.Contains(SectionEntry.Key))
		{
			auto& Section = SectionEntry.Value;

			if (Section.Num() > 0)
			{
				const FRuntimeMeshSectionRenderData& RenderData = SectionRenderData[SectionEntry.Key];
				FMaterialRenderProxy* Material = RenderData.Material->GetRenderProxy();

				for (const auto& SectionLOD : Section)
				{
					if (SectionLOD.Value.IsValid() && SectionLOD.Value->ShouldRender() && SectionLOD.Value->WantsToRenderInStaticPath())
					{
						//draw main render lod mesh
						FMeshBatch MeshBatch_DrawLOD;
						CreateMeshBatch(MeshBatch_DrawLOD, SectionLOD.Value, RenderData, Material, nullptr);
						MeshBatch_DrawLOD.LODIndex = SectionLOD.Key;

						if (MeshBatch_DrawLOD.LODIndex == URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID)
						{
							PDI->DrawMesh(MeshBatch_DrawLOD, 0.0f);
						}
						else
						{
							PDI->DrawMesh(MeshBatch_DrawLOD, FLT_MAX);
						}
					}
						
				}
			}
			
		}
		
	}
	
	//Draw Shadow LOD sections
	//for (const auto& ShadowSectionEntry : RuntimeMeshProxy->GetShadowSections())
	//{
	//	FRuntimeMeshSectionProxyPtr Section = ShadowSectionEntry.Value;
	//	if (SectionRenderData.Contains(ShadowSectionEntry.Key) && Section.IsValid() && Section->ShouldRender() && Section->WantsToRenderInStaticPath())
	//	{
	//		const FRuntimeMeshSectionRenderData& RenderData = SectionRenderData[ShadowSectionEntry.Key];
	//		FMaterialRenderProxy* Material = RenderData.Material->GetRenderProxy(false);
	//					
	//		//Draw shadow render lod mesh
	//		FMeshBatch MeshBatch_ShadowLOD;
	//		CreateMeshBatch(MeshBatch_ShadowLOD, Section, RenderData, Material, nullptr);
	//		MeshBatch_ShadowLOD.LODIndex = CUSTOM_SHADOW_LOD_ID;
	//		PDI->DrawMesh(MeshBatch_ShadowLOD, FLT_MAX);
	//	}
	//}
}

FLODMask FRuntimeMeshComponentSceneProxy::GetCustomWholeSceneShadowLOD(const FSceneView& InView, float InViewLODScale, int32 InForcedLODLevel, const struct FLODMask& InVisibilePrimitiveLODMask, float InShadowMapTextureResolution, float InShadowMapCascadeSize, int8 InShadowCascadeId, bool InHasSelfShadow) const
{
	FLODMask Lods;
#if USE_CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID
	Lods.SetLOD(URuntimeMeshComponent::CUSTOM_WHOLE_SCENE_SHADOW_LOD_ID);
#else
	Lods.SetLOD(0);
#endif
	return Lods;
}

void FRuntimeMeshComponentSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	// Set up wireframe material (if needed)
	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

	FColoredMaterialRenderProxy* WireframeMaterialInstance = nullptr;
	if (bWireframe)
	{
		WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : nullptr,
			FLinearColor(0, 0.5f, 1.f)
		);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
	}

	// Iterate over sections
	for (const auto& SectionEntry : RuntimeMeshProxy->GetSections())
	{
		if (SectionRenderData.Contains(SectionEntry.Key))
		{
			auto& Section = SectionEntry.Value;

			if (Section.Contains(0) && Section[0].IsValid() && Section[0]->ShouldRender())
			{
				// Add the mesh batch to every view it's visible in
				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
				{
					if (VisibilityMap & (1 << ViewIndex))
					{
						bool bForceDynamicPath = IsRichView(*Views[ViewIndex]->Family) || Views[ViewIndex]->Family->EngineShowFlags.Wireframe || IsSelected() || !IsStaticPathAvailable();

						if (bForceDynamicPath || !Section[0]->WantsToRenderInStaticPath())
						{
							const FRuntimeMeshSectionRenderData& RenderData = SectionRenderData[SectionEntry.Key];
							FMaterialRenderProxy* Material = RenderData.Material->GetRenderProxy();

							FMeshBatch& MeshBatch = Collector.AllocateMesh();
							CreateMeshBatch(MeshBatch, Section[0], RenderData, Material, WireframeMaterialInstance);

							Collector.AddMesh(ViewIndex, MeshBatch);
						}
					}
				}
			}
		}
	}

	// Draw bounds
#if RUNTIMEMESH_ENABLE_DEBUG_RENDERING
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			// Draw simple collision as wireframe if 'show collision', and collision is enabled, and we are not using the complex as the simple
			if (ViewFamily.EngineShowFlags.Collision && IsCollisionEnabled() && BodySetup && BodySetup->GetCollisionTraceFlag() != ECollisionTraceFlag::CTF_UseComplexAsSimple)
			{
				FTransform GeomTransform(GetLocalToWorld());
				BodySetup->AggGeom.GetAggGeom(GeomTransform, GetSelectionColor(FColor(157, 149, 223, 255), IsSelected(), IsHovered()).ToFColor(true), NULL, false, false, UseEditorDepthTest(), ViewIndex, Collector);
			}

			// Render bounds
			RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
		}
	}
#endif
}


//#pragma optimize("",on)
