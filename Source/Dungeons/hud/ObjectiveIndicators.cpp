#include "Dungeons.h"
#include "Engine.h"
#include "CanvasUtility.h"
#include "hud/ObjectiveIndicators.h"
#include "GameFramework/HUD.h"
#include "game/component/WorldspaceHUDComponent.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/GameProgress.h"
#include "game/Conversion.h"
#include "game/component/ObjectiveHintTargetComponent.h"
#include "game/LobbyBP.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/level/GameTiles.h"
#include "game/util/ActorQuery.h"
#include "util/Algo.hpp"
#include "util/CollectionUtils.h"
#include "game/actor/ObjectiveInterestPoint.h"
#include "game/merchant/MerchantDefComponent.h"
#include "game/component/OxygenComponent.h"

const float UObjectiveIndicators::MASTER_SCALE = 0.5f;
const float UObjectiveIndicators::SWORD_ROTATION_OFFSET_DEGREES = 90.0f;
const float UObjectiveIndicators::WORLD_OBJECTIVE_PROJECTION_OFFSET = 300.0f;
const float UObjectiveIndicators::WORLD_HINT_TARGET_PROJECTION_OFFSET = 400.0f;
const float UObjectiveIndicators::WORLD_MERCHANT_PROJECTION_OFFSET = 425.0f;
const float UObjectiveIndicators::WORLD_NOTIFY_PROJECTION_OFFSET = 100.0f;
const float UObjectiveIndicators::WORLD_DOOR_PROJECTION_OFFSET = 100.0f;
const float UObjectiveIndicators::WORLD_OXYGEN_PROJECTION_OFFSET = 150.0f;

//Box region where objective indicator will be smaller, unit is factor of entire viewport.
const FBox2D UObjectiveIndicators::VIEWPORT_HIDE_PORTRAIT_REGION = {
	FVector2D(0.22f, 0.92f), //top left
	FVector2D(0.78f, 1.0f)  //bottom right
};

void FObjectiveIndicatorIcon::Draw(AHUD* hud, const FVector2D& drawPosition, float scale, float rotation, const FLinearColor& color) const {
	const auto drawOffsetPosition = drawPosition - size * scale * 0.5f;
	hud->DrawTexture(tex,
		drawOffsetPosition.X, drawOffsetPosition.Y,
		size.X, size.Y,
		0.0f, 0.0f,
		1.0f, 1.0f,
		color, EBlendMode::BLEND_Translucent,
		scale,
		false,
		rotation,
		FVector2D(0.5f, 0.5f));
}

const float UObjectiveIndicators::VIEWPORT_HIDE_HEIGHT_FRACTION = UObjectiveIndicators::VIEWPORT_HIDE_PORTRAIT_REGION.Max.Y - UObjectiveIndicators::VIEWPORT_HIDE_PORTRAIT_REGION.Min.Y;
const float UObjectiveIndicators::VIEWPORT_HIDE_WIDTH_FRACTION = UObjectiveIndicators::VIEWPORT_HIDE_PORTRAIT_REGION.Max.X - UObjectiveIndicators::VIEWPORT_HIDE_PORTRAIT_REGION.Min.X;

namespace {

TOptional<FObjectiveLocations> currentOnScreenObjectiveLocationsFor(AGameBP& gameBp, APlayerCharacter& player) {
	const auto objectiveLocations = gameBp.GetObjectiveLocations();

	if (gameBp.IsA<ALobbyBP>()) {
		return objectiveLocations;
	}
	if (gameBp.IsPlayerInRangeOfObjective(&player)) {
		return objectiveLocations;
	}
	return (objectiveLocations.Locations.Num() <= 0 && objectiveLocations.bUseLocations) ? objectiveLocations : TOptional<FObjectiveLocations>();
}

}


namespace objectiveindicators {
	const float outlineTimeLoopProgress(UWorld* world, const float loopDuration, const float timeLoopBias, const float loopFractionOffset) {
		const auto time = world->GetTimeSeconds();
		const auto timeLoop = FMath::Frac(time / loopDuration + loopFractionOffset);
		return timeLoop / timeLoopBias;
	};

	const float outlineScale(float prg) {
		const float PulseFraction = 0.33f;
		const float ScaleFactor = 1.25f;
		return Math::min(1.0f, (1.0f - PulseFraction) + PulseFraction * prg * ScaleFactor);
	};

	const float outlineOpacity(float prg) {
		const float PulseFraction = 1.0f;
		return Math::max(0.0f, 1.0f - (1.0f - PulseFraction) - PulseFraction * FMath::Pow(prg, 3.0f));
	};

	const float jumpOffset(UWorld* world) {
		const float PulseFraction = 1.0f;
		return FMath::Pow(Math::max(0.0f, FMath::Sin(world->GetRealTimeSeconds() * 8.0f)/0.5f-0.5f), 0.4f);
	};

	const float notifyOpacity(UWorld* world) {
		return FMath::Sin(world->GetRealTimeSeconds() * 8.0f);
	}
}

UObjectiveIndicators::UObjectiveIndicators(const FObjectInitializer& ObjectInitializer) {	
	{
		//Objective
		const ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveMarkerBgTexture(TEXT("/Game/UI/Materials/Hints/cue_icon_objectivemarker"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveMarkerOutlineTexture(TEXT("/Game/UI/Materials/Hints/cue_icon_objective_outline"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveMarkerDirectionTexture(TEXT("/Game/UI/Materials/Hints/cue_icon_objectivepointer"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveMarkerReachedTexture(TEXT("/Game/UI/Materials/Hints/cue_icon_objective_reached"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveMarkerSmallTexture(TEXT("/Game/UI/Materials/HUD/Objectivemarker_under_HUD"));

		if (!ObjectiveMarkerBgTexture.Succeeded() || !ObjectiveMarkerOutlineTexture.Succeeded() || !ObjectiveMarkerDirectionTexture.Succeeded() || !ObjectiveMarkerReachedTexture.Succeeded() || !ObjectiveMarkerSmallTexture.Succeeded()) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load objective indicator assets"));
			return;
		}
	
		ObjectiveMarker = FObjectiveIndicatorIcon(ObjectiveMarkerBgTexture.Object);
		ObjectiveReached = FObjectiveIndicatorIcon(ObjectiveMarkerReachedTexture.Object);
		ObjectiveOutline = FObjectiveIndicatorIcon(ObjectiveMarkerOutlineTexture.Object);
		ObjectiveDirection = FObjectiveIndicatorIcon(ObjectiveMarkerDirectionTexture.Object);
		ObjectiveSmall = FObjectiveIndicatorIcon(ObjectiveMarkerSmallTexture.Object);

		ObjectiveIndicatorBig = {
			ObjectiveMarker.GetSize() + 3.0f,
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				const auto distancePrgFadeMax = 1800.f;
				const FloatRange alphaDistanceRange(200, 400);
				const float drawOutlinePrg = indicatorState.inViewport ? Math::clamp((indicatorState.distance - alphaDistanceRange.min()) / distancePrgFadeMax, 0.0f, 1.0f) : objectiveindicators::outlineTimeLoopProgress(world);
				const float alpha = FMath::Sin(HALF_PI * alphaDistanceRange.clampedFractionAt(indicatorState.distance));

				const float drawScaleBg = drawScale;
				const float drawScaleIcon = drawScale;
				const float drawScaleOutline = drawScale * objectiveindicators::outlineScale(drawOutlinePrg);
				const float drawOpacityOutline = objectiveindicators::outlineOpacity(drawOutlinePrg) * alpha;

				ObjectiveOutline.Draw(hud, indicatorState.drawPosition, drawScaleOutline, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, drawOpacityOutline));
				ObjectiveMarker.Draw(hud, indicatorState.drawPosition, drawScaleBg, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, alpha));
				const auto& Icon = indicatorState.inViewport ? ObjectiveReached : ObjectiveDirection;
				const auto IconRotation = indicatorState.inViewport ? 0.0f : indicatorState.degreeToObjective + SWORD_ROTATION_OFFSET_DEGREES;
				Icon.Draw(hud, indicatorState.drawPosition, drawScaleIcon, IconRotation, FLinearColor(1.0f, 1.0f, 1.0f, alpha));
			}
		};

		ObjectiveIndicatorSmall = {
			ObjectiveSmall.GetSize(),
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				ObjectiveSmall.Draw(hud, indicatorState.drawPosition, drawScale, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			}
		};
	}

	{
		//Events
		const ConstructorHelpers::FObjectFinder<UTexture2D> NotifyMarkerOutlineTexture(TEXT("/Game/UI/Materials/Notification/node_event_outline"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> NotifyMarkerInlineTexture(TEXT("/Game/UI/Materials/Notification/node_event_inline"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> NotifyMarkerSmallTexture(TEXT("/Game/UI/Materials/HUD/Eventmarker_under_HUD"));

		if (!NotifyMarkerOutlineTexture.Succeeded() || !NotifyMarkerInlineTexture.Succeeded() || !NotifyMarkerSmallTexture.Succeeded()) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load notify indicator assets"));
			return;
		}

		NotifyOutline = FObjectiveIndicatorIcon(NotifyMarkerOutlineTexture.Object);
		NotifyInline = FObjectiveIndicatorIcon(NotifyMarkerInlineTexture.Object);
		NofitySmall = FObjectiveIndicatorIcon(NotifyMarkerSmallTexture.Object);

		NotifyIndicatorBig = {
			NotifyOutline.GetSize() + 3.0f,
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				NotifyOutline.Draw(hud, indicatorState.drawPosition, drawScale, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));				
				NotifyInline.Draw(hud, indicatorState.drawPosition, drawScale, 0.0f, FLinearColor(1.0f, 1.0f, 1.0f, objectiveindicators::notifyOpacity(world)));
			}
		};

		NotifyIndicatorSmall = {
			NofitySmall.GetSize(),
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {								
				NofitySmall.Draw(hud, indicatorState.drawPosition, drawScale, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, objectiveindicators::notifyOpacity(world)));
			}
		};
	}

	{
		const ConstructorHelpers::FObjectFinder<UTexture2D> MerchantMarkerBubbleTexture(TEXT("/Game/UI/Materials/Merchant/objective/merchant_bubble"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> MerchantMarkerExclamationTexture(TEXT("/Game/UI/Materials/Merchant/objective/merchant_exclam"));		

		if (!MerchantMarkerBubbleTexture.Succeeded() || !MerchantMarkerExclamationTexture.Succeeded()) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load merchant indicator assets"));
			return;
		}

		MerchantBubble = FObjectiveIndicatorIcon(MerchantMarkerBubbleTexture.Object);
		MerchantExclamation = FObjectiveIndicatorIcon(MerchantMarkerExclamationTexture.Object);

		MerchantIndicatorBig = {
			MerchantBubble.GetSize() + 3.0f,
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				const float finalDrawScale = drawScale * (1.0f + objectiveindicators::jumpOffset(world)*0.05f);
				const FVector2D pos1 = { indicatorState.drawPosition.X, indicatorState.drawPosition.Y - 8.0f*objectiveindicators::jumpOffset(world) * finalDrawScale };
				MerchantBubble.Draw(hud, pos1, drawScale, 0.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
				const FVector2D pos2 = { indicatorState.drawPosition.X, indicatorState.drawPosition.Y -4.0f*finalDrawScale - 12.0f*objectiveindicators::jumpOffset(world) * finalDrawScale };
				MerchantExclamation.Draw(hud, pos2, finalDrawScale, 0.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0));
			}
		};

		MerchantIndicatorSmall = {
			MerchantBubble.GetSize() * 0.25f + 1.0f,
			[&](UWorld * world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				const float finalDrawScale = drawScale * 0.25f;
				MerchantBubble.Draw(hud, indicatorState.drawPosition, finalDrawScale, 0.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
				const FVector2D pos = { indicatorState.drawPosition.X, indicatorState.drawPosition.Y - 4.0f*finalDrawScale - 12.0f*objectiveindicators::jumpOffset(world) * finalDrawScale };
				MerchantExclamation.Draw(hud, pos, finalDrawScale, 0.0f, FLinearColor(1.0f, 1.0f, 1.0f, 1.0));
			}
		};
	}

	{
		const ConstructorHelpers::FObjectFinder<UTexture2D> OxygenMarkerBigTexture(TEXT("/Game/Content_DLC5/Decor/Prefabs/_HiddenDepths/BubbleColumn/cue_icon_bubblecolummarker"));
		const ConstructorHelpers::FObjectFinder<UTexture2D> OxygenMarkerSmallTexture(TEXT("/Game/Content_DLC5/Decor/Prefabs/_HiddenDepths/BubbleColumn/BubbleColumnMarker_under_HUD"));

		if (!OxygenMarkerBigTexture.Succeeded() || !OxygenMarkerSmallTexture.Succeeded()) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load oxygen indicator assets"));
			return;
		}

		OxygenMarkerBig = FObjectiveIndicatorIcon(OxygenMarkerBigTexture.Object);
		OxygenMarkerSmall = FObjectiveIndicatorIcon(OxygenMarkerSmallTexture.Object);

		OxygenIndicatorBig = {
			OxygenMarkerBig.GetSize() + 3.0f,
			[&](UWorld* world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				OxygenMarkerBig.Draw(hud, indicatorState.drawPosition, drawScale, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			}
		};

		OxygenIndicatorSmall = {
			OxygenMarkerSmall.GetSize(),
			[&](UWorld* world, AHUD* hud, const IndicatorState& indicatorState, float drawScale) {
				OxygenMarkerSmall.Draw(hud, indicatorState.drawPosition, drawScale, indicatorState.rotation, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			}
		};
	}

}

IndicatorState& UObjectiveIndicators::GetOrCreateIndicatorState(int index) {
	if (!IndicatorStates.Contains(index)) {		
		IndicatorStates.Add(index, IndicatorState{ 0.0f, 0.0f, 0.0f, nullptr, FVector2D(0,0), false });
	}
	return IndicatorStates[index];
}


TArray<FVector> GetAllHintTargetLocations(UWorld* world) {
	const auto& components = InstanceTracker<UObjectiveHintTargetComponent>::GetList(world);
	TArray<FVector> locations;
	for (auto component : components) {
		if (component->IsVisible()) {
			locations.Add(component->GetOwner()->GetActorLocation());
		}
	}
	return locations;
}

TArray<FVector> GetAllMerchantObjectiveLocations(UWorld* world, APlayerCharacter* owner) {
	const auto& components = InstanceTracker<UMerchantDefComponent>::GetList(world);
	TArray<FVector> locations;
	for (auto component : components) {
		if (component->ShouldShowObjectiveMarkerFor(owner)) {
			locations.Add(component->GetOwner()->GetActorLocation());
		}
	}
	return locations;
}

void EnableOxygenIndicatorAudio(AOxygenSourceComponent* oxygenIndicator, bool enable) {
	if (oxygenIndicator) {
		oxygenIndicator->EnableSound(enable);
	}
}

bool ShouldShowOxygenIndicator(UWorld* world, TArray<APlayerCharacter*>& players) {
	players = actorquery::getActors<APlayerCharacter>(world).FilterByPredicate([](APlayerCharacter* player) {
		if (UOxygenComponent* oxygenComponent = Cast<UOxygenComponent>(player->GetComponentByClass(UOxygenComponent::StaticClass()))) {
			return (player->IsLocallyControlled() && oxygenComponent->GetIsUnderwater() && oxygenComponent->GetIsOxygenLow());
		}
		return false;
	});
	return players.Num() > 0;
}

FVector GetPlayersCentrePosition(TArray<APlayerCharacter*>& players) {
	FVector centrePosition = { 0, 0, 0 };
	for (APlayerCharacter* player : players) {
		centrePosition += player->GetActorLocation();
	}

	return (centrePosition / players.Num());
}

TArray<AOxygenSourceComponent*> GetAllOxygenSources(UWorld* world, TArray<AOxygenSourceComponent*>& sources) {
	TArray<AOxygenSourceComponent*>& oxygenSources = InstanceTracker<AOxygenSourceComponent>::GetList(world);
	for (AOxygenSourceComponent* oxygenSource : oxygenSources) {
		if (oxygenSource->GetIsEnabled()) {
			sources.Add(oxygenSource);
		}
	}

	return sources;
}

FVector UObjectiveIndicators::GetNearestOxygenLocation(FVector playerLocation) {
	TArray<AOxygenSourceComponent*> sources;
	GetAllOxygenSources(GetWorld(), sources);

	if (sources.Num()) {
		AOxygenSourceComponent* nearestOxygenSource = sources[0];
		for (AOxygenSourceComponent* source : sources) {
			if (FVector::Dist(playerLocation, nearestOxygenSource->GetActorLocation()) > FVector::Dist(playerLocation, source->GetActorLocation())) {
				nearestOxygenSource = source;
			}
		}

		if (nearestOxygenSource != CurrentNearestOxygenSource) {
			EnableOxygenIndicatorAudio(CurrentNearestOxygenSource.Get(), false);
		}

		CurrentNearestOxygenSource = nearestOxygenSource;
		EnableOxygenIndicatorAudio(CurrentNearestOxygenSource.Get(), true);
		return CurrentNearestOxygenSource->GetActorLocation();
	}
	return FVector{ 0, 0, 0 };
}

void UObjectiveIndicators::UpdateIndicatorState(UCanvas* canvas, AHUD* hud, const FVector2D& viewportSize, const FVector& indicatorLocation, const FVector& playerLocation, const FObjectiveIndicator& indicatorBig, const FObjectiveIndicator& indicatorSmall, const float scale, IndicatorState& OutIndicatorState) {
	const FVector2D paddingSize = indicatorBig.GetBounds() * .5f * scale;

	//moj.db: a bit unsure about these offsets (WORLD_OBJECTIVE_PROJECTION_OFFSET) and why we have them, but it looks good.. so. 	
	auto projectedPlayerPosition = CanvasUtility::ProjectLocationToCanvas(playerLocation, 0, canvas);
	auto projectedObjectivePosition = CanvasUtility::ProjectLocationToCanvas(indicatorLocation, 0.0f, canvas);
	auto isInViewport = CanvasUtility::IsInToViewport(projectedObjectivePosition, viewportSize);
	auto projectedViewportPosition = CanvasUtility::ClampToViewport(projectedObjectivePosition, viewportSize);

	FVector2D clampedPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, paddingSize);

	//Rotate hexagons to be flat against viewport edges
	const float rotationBg = 30.0f * FMath::Sign(clampedPosition.Y - projectedViewportPosition.Y);
	OutIndicatorState.rotation = FMath::FInterpConstantTo(OutIndicatorState.rotation, rotationBg, hud->RenderDelta, 360.0f);
	
	//rotate towards objective
	OutIndicatorState.degreeToObjective = FMath::RadiansToDegrees(FMath::Atan2(projectedObjectivePosition.Y - projectedPlayerPosition.Y, projectedObjectivePosition.X - projectedPlayerPosition.X));

	//distance
	const float distanceToObjective = (indicatorLocation - playerLocation).Size();	
	OutIndicatorState.distance = distanceToObjective;	

	bool underHotbar = false;
	FVector2D percentage = clampedPosition / viewportSize;

	// #D11.CM - Check if the objective is under a hotbar
	if (auto* primaryPlayer = GetWorld()->GetFirstPlayerController()) {
		if (auto* worldHUD = primaryPlayer->FindComponentByClass<UWorldspaceHUDComponent>()) {
			if (worldHUD->IsPointHiddenByHotbar(clampedPosition)) {

				// We are under a hotbar
				auto smallerPaddingSize = indicatorSmall.GetBounds() * .5f * scale;

				// Find objective rotation
 				if (percentage.X > 0.5) {
 					if (1 - percentage.Y < 1 - percentage.X) {
 						projectedViewportPosition += FVector2D(0.0f, viewportSize.Y * VIEWPORT_HIDE_HEIGHT_FRACTION);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, smallerPaddingSize);
 						OutIndicatorState.rotation = 0;
 					}
 					else if (percentage.Y < 1 - percentage.X) {
 						projectedViewportPosition -= FVector2D(0.0f, viewportSize.Y * VIEWPORT_HIDE_HEIGHT_FRACTION);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, smallerPaddingSize);
 						OutIndicatorState.rotation = 180;
 					}
 					else {
 						projectedViewportPosition += FVector2D(viewportSize.X * VIEWPORT_HIDE_WIDTH_FRACTION, 0.0f);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, FVector2D{ smallerPaddingSize.Y, smallerPaddingSize.X });
 						OutIndicatorState.rotation = 270;
 					}
 				}
 				else {
 					if (1 - percentage.Y < percentage.X) {
 						projectedViewportPosition += FVector2D(0.0f, viewportSize.Y * VIEWPORT_HIDE_HEIGHT_FRACTION);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, smallerPaddingSize);
 						OutIndicatorState.rotation = 0;
 					}
 					else if (percentage.Y < percentage.X) {
 						projectedViewportPosition -= FVector2D(0.0f, viewportSize.Y * VIEWPORT_HIDE_HEIGHT_FRACTION);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, smallerPaddingSize);
 						OutIndicatorState.rotation = 180;
 					}
 					else {
 						projectedViewportPosition -= FVector2D(viewportSize.X * VIEWPORT_HIDE_WIDTH_FRACTION, 0.0f);
 						OutIndicatorState.drawPosition = CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, FVector2D{ smallerPaddingSize.Y, smallerPaddingSize.X });
 						OutIndicatorState.rotation = 90;
 					}
 				}

				// Update indicator information
				OutIndicatorState.inViewport = isInViewport;
				OutIndicatorState.indicator = &indicatorSmall;
				underHotbar = true;
			}
		}
	}

	if (!underHotbar) {
		//We are not under the hide portrait region
		OutIndicatorState.drawPosition = clampedPosition;
		OutIndicatorState.inViewport = isInViewport;
		OutIndicatorState.indicator = &indicatorBig;
	}
	
}

void UObjectiveIndicators::DrawIndicator(UWorld* world, UCanvas* canvas, AHUD* hud, const FVector2D& viewportSize, const FVector& indicatorLocation, const FVector& playerLocation, const FObjectiveIndicator& indicatorBig, const FObjectiveIndicator& indicatorSmall, const float scale, IndicatorState& OutIndicatorState) {
	UpdateIndicatorState(canvas, hud, viewportSize, indicatorLocation, playerLocation, indicatorBig, indicatorSmall, scale, OutIndicatorState);
	if(OutIndicatorState.indicator){
		OutIndicatorState.indicator->Draw(world, hud, OutIndicatorState, scale);
	}
};

void UObjectiveIndicators::Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, AHUD* hud) {
	if (!playerOwner->GetPawn()) {
		return;
	}

	const auto world = playerOwner->GetWorld();
	if (!gameBp) {
		gameBp = actorquery::getFirstActor<AGameBP>(world);
		if (!gameBp) {
			return;
		}
	}

	
	auto playerCharacter = Cast<APlayerCharacter>(playerOwner->GetCharacter());
	if (gameBp->bUsesObjectives) {
		if (auto game = gameBp->GetGame()) {
			if (auto playerTile = game->progress().currentlyLocalFurthest()) {
				playerCharacter = playerTile->player;
			}
		}
	}

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	const auto playerLocation = playerOwner->GetPawn()->GetActorLocation();
	const float resolutionScale = MASTER_SCALE * dpi_scale;	
	auto interestPointActors = actorquery::getActors<AObjectiveInterestPoint>(GetWorld());

	int index = 0;

	auto drawIndicatorLocal = [&](const FVector& indicatorLocation, const FObjectiveIndicator& indicatorBig, const FObjectiveIndicator& indicatorSmall) {
		auto& indicatorState = GetOrCreateIndicatorState(index);
		DrawIndicator(world, canvas, hud, viewportSize, indicatorLocation, playerLocation, indicatorBig, indicatorSmall, resolutionScale, indicatorState);
		index++;
	};

	if (interestPointActors.Num() != 0) {
		for (auto point : interestPointActors) {
			drawIndicatorLocal(point->GetActorLocation() + FVector(0.f, 0.f, WORLD_NOTIFY_PROJECTION_OFFSET), NotifyIndicatorBig, NotifyIndicatorSmall);
		}
	}
	else {
		FVector doorLocation { 0 };
		const auto currentObjectiveLocations = currentOnScreenObjectiveLocationsFor(*gameBp, *playerCharacter);

		if (!gameBp->bUsesObjectives || currentObjectiveLocations) {
			if (currentObjectiveLocations->bUseLocations) {
				for (auto&& worldLocation : currentObjectiveLocations.GetValue().Locations) {
					drawIndicatorLocal(worldLocation + FVector(0.f, 0.f, WORLD_OBJECTIVE_PROJECTION_OFFSET), ObjectiveIndicatorBig, ObjectiveIndicatorSmall);
				}
			}
		}
		else if (!currentObjectiveLocations && gameBp->GetNextDoorIndicatorDoor(playerCharacter, doorLocation)) {
			drawIndicatorLocal(doorLocation + FVector(0.f, 0.f, WORLD_DOOR_PROJECTION_OFFSET), ObjectiveIndicatorBig, ObjectiveIndicatorSmall);
		}

		const auto hintLocations = GetAllHintTargetLocations(world);
		for (auto location : hintLocations) {
			drawIndicatorLocal(location + FVector(0.f, 0.f, WORLD_HINT_TARGET_PROJECTION_OFFSET), ObjectiveIndicatorBig, ObjectiveIndicatorSmall);
		}
		
		for (auto location : GetAllMerchantObjectiveLocations(world, playerCharacter)) {
			drawIndicatorLocal(location + FVector(0.f, 0.f, WORLD_MERCHANT_PROJECTION_OFFSET), MerchantIndicatorBig, MerchantIndicatorSmall);
		}

		TArray<APlayerCharacter*> players;
		if (ShouldShowOxygenIndicator(world, players)) {
			drawIndicatorLocal(GetNearestOxygenLocation(GetPlayersCentrePosition(players)) + FVector(0.f, 0.f, WORLD_OXYGEN_PROJECTION_OFFSET), OxygenIndicatorBig, OxygenIndicatorSmall);
		} else {
			EnableOxygenIndicatorAudio(CurrentNearestOxygenSource.Get(), false);
		}
	}
}

bool UObjectiveIndicators::IsInsideCoopHotbar_Implementation(FVector2D& pos)
{
	return false;
}

void UObjectiveIndicators::ClearOxygenSource() {
	CurrentNearestOxygenSource = nullptr;
}
