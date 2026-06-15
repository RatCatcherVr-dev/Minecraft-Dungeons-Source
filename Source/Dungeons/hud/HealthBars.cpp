#include "Dungeons.h"
#include "Engine.h"
#include "CanvasUtility.h"
#include "hud/HealthBars.h"
#include "game/component/GlobalHealthBarComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/component/HealthBarComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/util/Tags.h"


//Scale of the entire health bar indicator
const float UHealthBars::MASTER_SCALE = 0.5f;

const FColor UHealthBars::HEALTH_BAR_COLOR = FColor(236, 58, 43);
const FColor UHealthBars::HEALTH_BAR_HEAL_COLOR = FColor(255, 138, 102);
const FColor UHealthBars::HEALTH_BAR_DAMAGE_COLOR = FColor(255, 255, 255);
const FColor UHealthBars::HEALTH_BAR_BEHIND_COLOR = FColor(100, 27, 20);

UHealthBars::UHealthBars(const FObjectInitializer& ObjectInitializer) {
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarFrame(TEXT("/Game/UI/Materials/Healthbar/health_bar_frame"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBar(TEXT("/Game/UI/Materials/Healthbar/health_bar"));	

	if (!HealthBarFrame.Succeeded() || !HealthBar.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load health bar assets"));
		return;
	}

	//Assign local textures
	HealthBarTexture = HealthBar.Object;
	
	//Create all icons from full texture.	
	FVector2D healthBarFrameSize;
	
	HealthBarFrameIcon = CanvasUtility::MakeIconFullTexture(HealthBarFrame.Object, healthBarFrameSize, 1.0f);

	//Assume render offset for health bar is the difference between content and its border.
	//Hard-code these values for a different bar-thickness in the art	
	auto CalculatedHealthBarBorderThickness = (healthBarFrameSize.X - HealthBar.Object->GetSizeX()) * 0.5f;
	
	HealthBarSize = FVector2D(HealthBar.Object->GetSizeX(), HealthBar.Object->GetSizeY());
	HealthBarFrameOffset = FVector2D(-healthBarFrameSize.X*0.5, -healthBarFrameSize.Y*0.5);
	HealthBarOffset = FVector2D(-HealthBarSize.X*0.5, -HealthBarSize.Y*0.5);
}

void UHealthBars::Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, AHUD* hud) {
	if (playerOwner && playerOwner->GetPawn()) {
		FVector2D viewportSize;
		GEngine->GameViewport->GetViewportSize(viewportSize);

		const auto owner = playerOwner->GetPawn();

		for (auto actor : InstanceTracker< AMobCharacter >::GetList(owner->GetWorld())) {
			//Never show player owner health bar here
			if (actor == owner) {
				continue;
			}

			const auto HealthBarComponent = actor->GetHealthBarComponent();
			if (HealthBarComponent != nullptr && !actor->ActorHasTag(tags::cosmetic)) {

				const HealthBarAppearance appearance = [&]() -> HealthBarAppearance {
					const auto master = actor->GetMaster();
					if (master) {
						if (auto avatarData = master->FindComponentByClass<UPlayerAvatarComponent>()) {
							return { avatarData->GetPlayerColor(), avatarData->GetPlayerAvatarData().FrameBackgroundColor, 1.0f, actor->IsTargetable() ? 0.5f : 0.125f };
						}
					}
					return { HEALTH_BAR_COLOR , HEALTH_BAR_BEHIND_COLOR, HealthBarComponent->GetOpacity(), HealthBarComponent->GetScale() };
				}();

				if (appearance.Opacity <= 0.0f) {
					continue;
				}

				float healthPercentageCurrent = HealthBarComponent->GetHealthPercentage();
				if (healthPercentageCurrent <= 0.0f) {
					continue;
				}

				const auto barLocation = actorquery::getActorHealthBarPosition(playerOwner, actor);
				auto projectedPosition = CanvasUtility::ProjectLocationToCanvas(barLocation, 0.0f, canvas);

				const bool isInViewport = CanvasUtility::IsInToViewport(projectedPosition, viewportSize);

				if (!isInViewport) {
					continue;
				}


				auto projectedViewportPosition = CanvasUtility::ClampToViewport(projectedPosition, viewportSize);
				const auto scale = MASTER_SCALE * dpi_scale;

				//Origin of art is top left.
				auto textureDrawPosition = projectedViewportPosition;
				textureDrawPosition.X += HealthBarOffset.X * scale * appearance.Scale;
				textureDrawPosition.Y += HealthBarOffset.Y * scale;

				//snap to whole pixels
				textureDrawPosition.X = roundf(textureDrawPosition.X);
				textureDrawPosition.Y = roundf(textureDrawPosition.Y);

				//Health bar
				const auto healthBarPos = textureDrawPosition;
				const auto healthBarWidth = HealthBarSize.X * scale * appearance.Scale;
				const auto healthBarHeight = HealthBarSize.Y * scale;

				const uint8 Alpha = appearance.Opacity * 255;

				//Background
				canvas->SetDrawColor({ appearance.BackgroundColor.R, appearance.BackgroundColor.G, appearance.BackgroundColor.B, Alpha });
				canvas->DrawTile(HealthBarTexture, healthBarPos.X, healthBarPos.Y, healthBarWidth, healthBarHeight, 0.0f, 0.0f, 1.0f, 1.0f);

				//Current health bar
				canvas->SetDrawColor({ appearance.BarColor.R, appearance.BarColor.G, appearance.BarColor.B, Alpha });
				canvas->DrawTile(HealthBarTexture, healthBarPos.X, healthBarPos.Y, healthBarWidth*healthPercentageCurrent, healthBarHeight, 0.0f, 0.0f, healthPercentageCurrent, 1.0f);

				canvas->SetDrawColor(AHUD::WhiteColor);
			}
		}
	}
}
