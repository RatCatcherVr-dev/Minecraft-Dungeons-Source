#include "Dungeons.h"
#include "Engine.h"
#include "hud/PlayerInfo.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/ActorQuery.h"
#include "game/Game.h"
#include "game/GameProgress.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "hud/DebugList.h"
#include "game/component/DebugComponent.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "game/GameBP.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "game/conversion.h"
#include "world/level/material/Material.h"
#include "game/actor/character/mob/MobCharacter.h"
#include <GameFramework/Controller.h>
#include <AIController.h>
#include "game/util/TileQuery.h"
#include "util/StringUtil.h"

void PlayerInfo::PrintTileDebug(DebugList& tileTree, const Util::TileDebugData& tileDebugData, int indentLevel/* = 0*/) {
	tileTree.Draw({ tileDebugData.name.c_str() }, FColor::White, indentLevel * 16);

	for (auto&& child : tileDebugData.children) {
		PrintTileDebug(tileTree, child, indentLevel + 1);
	}
}

void PlayerInfo::PrintMobDebug(DebugList& mobInfo, AActor* mob) {
	if (const auto* healthComponent = mob->FindComponentByClass<UHealthComponent>()) {
		mobInfo.Draw(
			FString::Printf(
				TEXT("HP: %d/%d"),
				static_cast<int>(healthComponent->GetCurrentHealth()),
				static_cast<int>(healthComponent->GetMaximumHealth())
			),
			FColor::Orange
		);
	}
	if (const auto* debugComponent = mob->FindComponentByClass<UDebugComponent>()) {
		for (const auto& line : debugComponent->lines) {
			mobInfo.Draw(line, FColor::White);
		}
	}

	/* //Nice targeting visualization for mobs
	if (AMobCharacter* chararacter = Cast<AMobCharacter>(mob)){
		if(auto aiController = chararacter->AiController()){
			auto origo = mob->GetActorLocation() + FVector(0.f,0.f,chararacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

			
			auto drawTo = [&](AActor* target, FColor color, float thickness, float arrowSize, float marginFactor = 1.0f) {
				if (auto targetChar = Cast<ABaseCharacter>(target)) {
					auto dirto = targetChar->GetActorLocation() - mob->GetActorLocation();
					auto targetLocation = origo + dirto.GetSafeNormal() * (dirto.Size() - targetChar->GetCapsuleComponent()->GetScaledCapsuleRadius()*marginFactor);
					DrawDebugDirectionalArrow(mob->GetWorld(), origo, targetLocation, arrowSize, color, false, -1.0f, 0, thickness);
				}
			};

			if(auto target = chararacter->MobParams.target){
				drawTo(target, FColor::White, 1.f, 75.f, 1.0f);
			}
			if (auto target = chararacter->MobParams.targetSecondary) {
				drawTo(target, FColor::Silver, 1.f, 60.f, 1.5f);
			}
			if(auto focusActor = aiController->GetFocusActor()){
				drawTo(focusActor, FColor::Red, 1.5f, 40.f, 2.0f);
			}
			DrawDebugLine(mob->GetWorld(), origo, origo + aiController->GetControlRotation().Vector()*200.f, FColor::Green, false, -1.0f, 0, 2);
			DrawDebugLine(mob->GetWorld(), origo, origo + mob->GetActorRotation().Vector()*100.f, FColor::Yellow, false, -1.0f, 0, 4);
		}
	}
	*/

}

static FVector GetPointUnderCapsule(ABaseCharacter* actor, float offset = -5.0f) {
	const auto halfHeight = actor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	return actor->GetActorLocation() + FVector(0, 0, offset - halfHeight);
}

void PlayerInfo::Draw(AHUD* hud, APlayerController* playerOwner) {
#if UE_BUILD_SHIPPING
	return;
#endif

	if (!playerOwner) {
		return;
	}

	if (!Cast<ABasePlayerController>(playerOwner)->GetDebugState()) {
		return;
	}

	if (!playerOwner->GetPawn()) {
		return;
	}

	const auto playerCharacter = Cast<APlayerCharacter>(playerOwner->GetCharacter());

	for (const auto nearbyMob : actorquery::getNearbyActors<AMobCharacter>(playerCharacter, 2000.f)) {
		FVector2D screenLocation;
		UGameplayStatics::ProjectWorldToScreen(
			playerOwner,
			nearbyMob->GetActorLocation() + FVector { 0.f, 0.f, 200.f },
			screenLocation,
			false
		);

		DebugList mobInfo { hud, screenLocation };
		PrintMobDebug(mobInfo, nearbyMob);
	}

	const auto game = actorquery::getGame(playerOwner->GetWorld());
	if (!game) {
		return;
	}

	if (!TileDebugData.IsSet()) {		
		TileDebugData = Util::collectTiles(game->tiles());
	}

	
	const auto& tile = actorquery::getGame(hud->GetWorld())->progress().current(*playerCharacter);
	const auto& stretch = tile.stretch();
	const auto& dungeon = tile.dungeon();

	DebugList tileInfo(hud, { 370, 8 });

	tileInfo.Draw({ (game->levelName() + ", seed: " + std::to_string(game->settings().randomSeed)).c_str() }, FColor::White);

	tileInfo.Draw({ tile.tile().id().c_str() }, FColor::White);
	tileInfo.Draw({ tile.bounds().toString().c_str() }, { 128, 128, 128 });
	const auto tileRotation = UTileQuery::GetMyTileDegrees(playerCharacter);
	tileInfo.Draw({ (std::string("Rotation, UE/Json: ") + std::to_string(static_cast<int>(tileRotation.DegreesUE4)) + "/" + std::to_string(static_cast<int>(tileRotation.DegreesJson))).c_str() }, { 128, 128, 128 });

	tileInfo.Draw({ stretch.def.id.c_str() }, FColor::White);
	tileInfo.Draw({ stringutil::toFString(dungeon.def().id.id) + '@' + dungeon.def().level.id }, FColor::White);
	if (dungeon.resourcePack()) {
		tileInfo.Draw("resource-pack: " + stringutil::toFString(dungeon.def().resourcePack), FColor::White);
	} else {
		tileInfo.Draw("resource-pack: " + stringutil::toFString(dungeon.def().resourcePack) + "(no EResourcePack)", FColor::White);
	}
	{
		static const auto ambienceString = [prefixLength = std::string("AMBIENCE_").length()](TOptional<EAmbienceID> ambience) {
			return ambience ? GetEnumValueToStringStripped(ambience.GetValue()).RightChop(prefixLength).ToLower() : "-";
		};
		static const auto ambienceAudioString = [prefixLength = std::string("AUDIO_").length()](TOptional<EAmbienceAudioID> ambience) {
			return ambience ? GetEnumValueToStringStripped(ambience.GetValue()).RightChop(prefixLength).ToLower() : "-";
		};
		tileInfo.Draw("ambience: " + ambienceString(playerCharacter->GetAmbience().Id) + "," + ambienceAudioString(playerCharacter->GetAmbienceAudio().Id) + '@' + tile.ambienceGroupName(), FColor::White);
	}
	{
		const auto musicOverrideString = tile.musicLevelOverride() ? "music-override: " + GetEnumValueToStringStripped(tile.musicLevelOverride().GetValue()).ToLower() : " ";
		tileInfo.Draw(musicOverrideString, FColor::White);
	}

	FVector mouseWorld, direction;
	playerOwner->DeprojectMousePositionToWorld(mouseWorld, direction);

	FHitResult res;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

	playerOwner->GetWorld()->LineTraceSingleByObjectType(res, mouseWorld + (direction * 2000.0f), mouseWorld + (direction * 100000.0f), params);
	auto& blockSource = *actorquery::getFirstActor<AGameBP>(playerOwner->GetWorld())->BlockSource();

	const auto& impactBlock = blockSource.getBlock(conversion::ueToBlock(res.ImpactPoint + direction));
	_PrintBlockMaterial(TEXT("MouseOn: "), tileInfo, impactBlock, dungeon.blockGraphicsPack(playerOwner->GetWorld()));

	const auto& standOnBlock = blockSource.getBlock(conversion::ueToBlock(GetPointUnderCapsule(playerCharacter)));
	_PrintBlockMaterial(TEXT("StandOn: "), tileInfo, standOnBlock, dungeon.blockGraphicsPack(playerOwner->GetWorld()));

	for (const auto& group : stretch.def.mobs.groups) {
		for (const auto& type : group.types) {
			tileInfo.Draw({ type.expr.c_str() }, FColor::Yellow);
		}

		tileInfo.NewLine();
	}

	if (TileDebugData.IsSet()) {
		DebugList tileTree(hud, { 10, 8 });
		PrintTileDebug(tileTree, TileDebugData.GetValue());
	}

	// click to navigate response
	{
		FHitResult hitResult;
	
		if (playerOwner->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::IgnorePlayer), false, hitResult)) {
			_PrintComponent(TEXT("IgnorePlayer: "), tileInfo, hitResult.Component.Get());
		}
		
		if (playerOwner->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult)) {
			_PrintComponent(TEXT("PlayerPlane: "), tileInfo, hitResult.Component.Get());
		}
	}
}

void PlayerInfo::_PrintBlockMaterial(const FString& header, DebugList& tileInfo, const Block& block, const BlockGraphicsPack& blockGraphicsPack) const {
	FString line;
	line.Reserve(64);

	line.Append(header);

	if (block.getId() == BlockID::AIR) {
		line.Append("Unknown blocking geometry");
	} else {
		line.Append(block.getRawNameId().c_str());
		line.AppendChar('(');
		line.AppendInt(block.getId());
		line.Append(TEXT("), material: "));
		const auto materialType = blockGraphicsPack.get(block.getId()).getMaterialType();
		line.Append(GetEnumValueToStringStripped(materialType).ToLower());
	}
	tileInfo.Draw(line, { 255, 128, 0 });
}

void PlayerInfo::_PrintComponent(const FString& header, DebugList& tileInfo, const UPrimitiveComponent* component) const {
	FString line;
	line.Reserve(64);

	line.Append(header);

	if (component != nullptr) {
		line.Append(component->GetName());
		line.Append(" of ");
		line.Append(component->GetOwner()->GetName());
	}

	tileInfo.Draw(line, { 255, 128, 0 });
}