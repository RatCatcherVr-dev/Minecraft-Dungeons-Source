#include "TileDecorator.h"
#include "TileDecoratorModule.h"
#include "TileDecoratorCommands.h"
#include "TileDecoratorUtils.h"
#include "TileDecoratorFilter.h"
#include "ContentBrowserModule.h"
#include "ContentBrowserDelegates.h"
#include "CopyComponentsIntoBlueprint.h"
#include "editor/decoration/DecorationAnchor.h"
#include "editor/decoration/DecorationLevelActor.h"
#include "game/Conversion.h"
#include "game/level/TileDecor.h"
#include "game/level/ambience/AmbienceActor.h"
#include "game/level/sound/AudioManager.h"
#include "game/level/DungeonsLevelStreamingDynamic.h"
#include "game/level/InstancedMeshesActor.h"
#include "game/level/doors/InstantTravelActor.h"
#include "game/util/ActorQuery.h"
#include "lovika/io/ObjectGroupFile.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include <ObjectTools.h>
#include <LevelSequenceActor.h>
#include <EditorLevelUtils.h>
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "UnrealString.h"
#include <AssetRegistryModule.h>


IMPLEMENT_GAME_MODULE(FTileDecoratorModule, TileDecorator);


static TAutoConsoleVariable<int32> CVarShadowCasterWarningTime(
	TEXT("TileDecorator.ShadowCasterWarningTime"),
	5,
	TEXT(" Time to Show Shadow Caster warning for "));


static TAutoConsoleVariable<int32> CVarEnableInstancingOnSave(
	TEXT("Dungeons.EnableInstancingOnSave"),
	1,
	TEXT(" Enable the on save instancing conversion "));

#define LOCTEXT_NAMESPACE "TileDecorator"

namespace {

using namespace filter;

using LevelTile = std::pair<const io::Object*, const ULevel*>;

template <class T>
TArray<T*> GetAllActorsOfType(UWorld* world) {
	TArray<T*> out;
	UGameplayStatics::GetAllActorsOfClass(world, T::StaticClass(), out);
	return out;
}

TArray<LevelTile> getLevelTilesForActors(const TArray<AActor*>& actors) {
	auto level = util::deco::getLevel();
	std::set<LevelTile> objects;
	for (auto&& actor : actors) {
		auto blockPos = conversion::ueToBlock(actor->GetActorLocation());
		if (auto tileObject = level->getObjectFor(blockPos)) {
			objects.insert(LevelTile(tileObject, actor->GetLevel()));
		}
	}
	TArray<LevelTile> out;
	for (auto&& obj : objects) {
		out.Add(obj);
	}
	return out;
}

TArray<LevelTile> getAllLevelTiles() {
	return getLevelTilesForActors(GetAllActorsOfType<AActor>(util::editor::getWorld()));
}

FString groupNameFor(const ULevel* subLevel) {
	auto name = [&] {
		const auto subLevelName = subLevel->GetOuter()->GetName();
		if (subLevelName.EndsWith("decor")) {
			return subLevelName.LeftChop(5);
		}
		if (subLevelName.EndsWith("tiles")) {
			return subLevelName.LeftChop(5);
		}
		return subLevelName;
	}();

	const auto shouldStripCharacter = [](TCHAR ch) {
		return ch == '_';
	};
	while (!name.IsEmpty() && shouldStripCharacter(name[name.Len() - 1])) {
		name.RemoveAt(name.Len() - 1, 1, false);
	}
	return name;
}

FString fullFilePathFor(const FString& filename) {
	auto filenameHACK = filename + ".uasset";
	if (filenameHACK.StartsWith("/Game/")) {
		filenameHACK = FPaths::GameContentDir() + filenameHACK.RightChop(6);
	}
	return filenameHACK;
}

bool saveDecorsFor(const ULevel* subLevel, const FString& tileName, const BlockCuboid& bounds, FString& out_FailReason) {
	auto filename = decor::filenameFor(groupNameFor(subLevel), tileName);
	if (FPaths::FileExists(fullFilePathFor(filename))) {
		out_FailReason = "files already exist?";
		return false;
	}
	TArray<AActor*> saveActors = util::actor::filter(
		InNullableLevel(subLevel) && IsInsideXy(bounds) && IsDecorComponent());

	// output validation messages (move to "filter" next)
	for (auto actor : saveActors) {
		if (actor) {
			
			UStaticMeshComponent* component = Cast<UStaticMeshComponent>(actor);
 			if (component && component->Mobility != EComponentMobility::Static) {
				UE_LOG(TileDecorator, Error, TEXT("Actor %s is dynamic"), *actor->GetName());
				continue;
			}

			UBoxComponent* shape = Cast<UBoxComponent>(actor);
			if( shape && shape->Mobility) {
				UE_LOG(TileDecorator, Error, TEXT("Actor %s is dynamic"), *actor->GetName());
				continue;
			}

			USceneComponent* root = actor->GetRootComponent();
			if (root) {
				TArray<USceneComponent*> children;
				root->GetChildrenComponents(true, children);
				for (auto child : children) {
					UStaticMeshComponent* childMesh = Cast<UStaticMeshComponent>(child);
					if (childMesh && childMesh->Mobility != EComponentMobility::Static) {
						UE_LOG(TileDecorator, Error, TEXT("Actor %s is dynamic"), *actor->GetName());
					}
				}
				continue;
			}
		}
	}
	
	auto anchorstest = actorquery::getActors<ADecorationAnchor>(util::editor::getWorld());
	if (anchorstest.Num() > 1)
	{
		out_FailReason = "Duplicate decoration actor in level -> will break decoration alignment!";
		return false;
	
	}
	AActor* anchor = actorquery::getFirstActor<ADecorationAnchor>(util::editor::getWorld());
	if (!anchor) {
		anchor = util::editor::getWorld()->SpawnActor<AActor>(ADecorationAnchor::StaticClass(), FTransform{});
	}
	anchor->SetActorLocation(conversion::blockToUe(bounds.minInclusive));
	saveActors.AddUnique(anchor);

	auto object = FCopyComponentsIntoBlueprint::CopyBlueprintsTo(filename, saveActors);
	util::editor::closeAllEditorsForAsset(object);
	util::save::saveObject(object);
	return true;
}

void removeAllTileLocalPositionArrows(AActor& actor) {
	util::actor::removeMatchingComponentsOfType(actor, UArrowComponent::StaticClass(), [](auto&& component) {
		return ADecorationAnchor::isDecorationAnchorArrowHACK(Cast<UArrowComponent>(component));
	});
}

bool saveDecorObject(AActor& actor, const ULevel* subLevel, const FString& filename, const BlockCuboid& bounds) {
	if (FPaths::FileExists(fullFilePathFor(filename))) {
		return false;
	}
	removeAllTileLocalPositionArrows(actor);

	UArrowComponent* anchor = NewObject<UArrowComponent>(&actor);
	ADecorationAnchor::initDecorationAnchorArrowHACK(anchor);
	actor.AddInstanceComponent(anchor);
	anchor->RegisterComponent();
	anchor->AttachTo(actor.GetRootComponent());
	anchor->RelativeLocation = conversion::blockToUe(bounds.minInclusive) - actor.GetActorLocation();
	anchor->RelativeRotation = actor.GetActorRotation();
	anchor->RelativeScale3D = actor.GetActorScale3D();

	auto object = FCopyComponentsIntoBlueprint::CopyBlueprintsTo(filename, TArray<AActor*>{&actor});
	util::editor::closeAllEditorsForAsset(object);
	util::save::saveObject(object);

	removeAllTileLocalPositionArrows(actor);
	return true;
}

bool saveDecorActor(AActor& actor, const ULevel* subLevel, const FString& tileName, const BlockCuboid& bounds) {
	return saveDecorObject(actor, subLevel, decor::filenameForActor(groupNameFor(subLevel), tileName), bounds);
}

bool saveDecorEnvironmental(AActor& actor, const ULevel* subLevel, const FString& name) {
	return saveDecorObject(actor, subLevel, decor::filenameForEnvironmental(groupNameFor(subLevel), name), {});
}

struct LevelTileFail
{
	const LevelTile* levelTile;
	FString errorMessage;
};

void showFailedTileExportPopup(TArray<LevelTileFail> tiles) {
	FString titleString = "Failed exporting tiles: ";
	FText title(FText::FromString(titleString));

	FString message; // @attn: concat in loop etc
	for (auto& failedTile : tiles) {
		auto& tile = *failedTile.levelTile;
		message += FString(tile.first->id.c_str()) + " (" + groupNameFor(tile.second) + ")" + failedTile.errorMessage + "\n";
	}
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(message), &title);
}

void saveDecorsForSelectedTiles() {
	auto levelTiles = getLevelTilesForActors(util::editor::getSelectedActors());

	TArray<LevelTileFail> failedTiles;
	for (auto&& levelTile : levelTiles) {
		auto tileObject = levelTile.first;

		if (tileObject->id == "") {
			UE_LOG(TileDecorator, Error, TEXT("Unnamed tile! Skipping"));
			continue;
		}
		FString failreason = "";
		if (!saveDecorsFor(levelTile.second, FString(tileObject->id.c_str()), tileObject->bounds, failreason)) {
			failedTiles.Add({&levelTile, failreason});
		}
	}
	if (failedTiles.Num() > 0) {
		showFailedTileExportPopup(failedTiles);
	}
}

//
// SUB-LEVELS
//
static FString LeftOfLast(const FString& s, char delimiter) {
	int index;
	return s.FindLastChar(delimiter, index) ? s.LeftChop(s.Len() - index) : s;
}

TArray<AActor*> getActorsInTile(LevelTile tile) {
	return util::actor::filter(InLevel(*tile.second) && IsInsideXy(tile.first->bounds) && !IsGlobal());
}

TOptional<LevelTile> getCurrentlySelectedTile() {
	auto preselectedActors = util::editor::getSelectedActors();
	if (preselectedActors.Num() == 0) {
		FString message("At least one actor should be selected");
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(message), nullptr);
		return {};
	}
	auto levelTiles = getLevelTilesForActors(preselectedActors);
	if (levelTiles.Num() != 1) {
		FString message(levelTiles.Num()? "Selected actors must belong to the same tile" : "Actors doesn't belong to any tile");
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(message), nullptr);
		return {};
	}
	return LevelTile{ levelTiles.Top().first, preselectedActors[0]->GetLevel() };
}

ULevelStreamingDynamic* getOrCreateSubLevel(const FString& path) {
	UWorld& world = *util::editor::getWorld();

	for (auto level : world.GetStreamingLevels()) {
		if (level->GetLoadedLevel() && level->GetLoadedLevel()->GetPathName().StartsWith(path)) {
			
			if(level->IsA<UDungeonsLevelStreamingDynamic>())
			{
				return static_cast<ULevelStreamingDynamic*>(level);
			}
			else
			{
				EditorLevelUtils::SetStreamingClassForLevel(level, UDungeonsLevelStreamingDynamic::StaticClass());
				return static_cast<ULevelStreamingDynamic*>(level);
			}
		}
	}
	return static_cast<ULevelStreamingDynamic*>(EditorLevelUtils::CreateNewStreamingLevelForWorld(world, UDungeonsLevelStreamingDynamic::StaticClass(), path, false));
}

ULevelStreamingDynamic* moveSelectedActorsToTileSubLevel(LevelTile tile) {
	if (tile.first->id.empty()) {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("\"Tile\" must have an id (was a prefab region accidentally created instead of something else?)"), nullptr);
		return nullptr;
	}
	auto const path = LeftOfLast(util::editor::getWorld()->GetOutermost()->GetName(), '/') + "/SubLevels/" + tile.first->id.c_str();
	if (auto level = getOrCreateSubLevel(path)) {
		auto offset = conversion::blockToUe(tile.first->bounds.minInclusive);
		level->LevelTransform.SetTranslation(offset);
		EditorLevelUtils::MoveSelectedActorsToLevel(level);
		return level;
	}
	return nullptr;
}

//
// SUB-LEVEL COMMANDS
//
TOptional<LevelTile> selectAllActorsInCurrentTile() {
	if (auto tile = getCurrentlySelectedTile()) {
		util::editor::setSelectedActors(getActorsInTile(tile.GetValue()));
		return tile;
	}
	return {};
}

ULevelStreamingDynamic* moveCurrentTileActorsToTileSubLevel() {
	if (auto tile = selectAllActorsInCurrentTile()) {
		return moveSelectedActorsToTileSubLevel(tile.GetValue());
	}
	return nullptr;
}

TArray<ULevelStreamingDynamic*> moveAllActorsToTileSubLevels() {
	TArray<ULevelStreamingDynamic*> levels;
	
	for (auto&& tile : getAllLevelTiles()) {
		util::editor::setSelectedActors(getActorsInTile(tile));
		if (auto level = moveSelectedActorsToTileSubLevel(tile)) {
			levels.Add(level);
		}
	}
	return levels;
}


void RenameToOld(AActor* pActor)
{
	//rename to unique old ready for deletion so we can maintain original names for instance conversion
	int32 RenTest = 0;
	while (!pActor->Rename(*(pActor->GetName() + TEXT("_OLD") + FString::FromInt(RenTest)), nullptr, REN_Test))
	{
		++RenTest;
	}

	pActor->Rename(*(pActor->GetName() + TEXT("_OLD") + FString::FromInt(RenTest)));
}

void convertCurrentActorsTileToInstanced() {
	if (auto tile = getCurrentlySelectedTile()) {
		
		util::editor::setSelectedActors(TArray<AActor*>());

		UWorld& world = *util::editor::getWorld();

		for (auto level : world.GetStreamingLevels()) 
		{
			if (level->GetLoadedLevel() == tile.GetValue().second)
			{
				if(auto DungeonsStreamingLvl = Cast<UDungeonsLevelStreamingDynamic>(level))
				{
					TArray<AActor*> ActorsToDestroy;
					DungeonsStreamingLvl->ConvertInstancedMeshes(true, ActorsToDestroy);

					for (auto* pActor : ActorsToDestroy)
					{
						util::editor::closeAllEditorsForAsset(pActor);
						RenameToOld(pActor);
					}

					util::editor::deleteActors(ActorsToDestroy);

				}


				GEngine->ForceGarbageCollection(true);
				GEngine->PerformGarbageCollectionAndCleanupActors();

				return;
			}
			
		}
	}
}


void convertCurrentActorsTileFromInstanced() {
	if (auto tile = getCurrentlySelectedTile()) {

		util::editor::setSelectedActors(TArray<AActor*>());

		UWorld& world = *util::editor::getWorld();

		for (auto level : world.GetStreamingLevels())
		{
			if (level->GetLoadedLevel() == tile.GetValue().second)
			{
				if (auto DungeonsStreamingLvl = Cast<UDungeonsLevelStreamingDynamic>(level))
				{
					TArray<AActor*> ActorsToDestroy;
					DungeonsStreamingLvl->ConvertInstancedMeshes(false, ActorsToDestroy);

					for (auto* pActor : ActorsToDestroy)
					{
						util::editor::closeAllEditorsForAsset(pActor);
						RenameToOld(pActor);
					}

					util::editor::deleteActors(ActorsToDestroy);
				}

				GEngine->ForceGarbageCollection(true);
				GEngine->PerformGarbageCollectionAndCleanupActors();

				return;
			}
		}
	}
}


void deleteAllDuplicatedSMs() {

	TArray<AActor*> ActorsToDestroy;
	
	
	TArray<AActor*> SMActors;
	UGameplayStatics::GetAllActorsOfClass(util::editor::getWorld(), AStaticMeshActor::StaticClass(), SMActors);

	TMap< UStaticMesh* , TArray<AActor*> > SM_Map;

	//map StaticMeshActors by instanceable & SM type
	const FName NoInstancingTag = "NoInstancing";
	for (auto* pActor : SMActors)
	{
		AStaticMeshActor* SMActor = Cast<AStaticMeshActor>(pActor);

		if (!SMActor || SMActor->ActorHasTag(NoInstancingTag)) {
			continue;
		}

		auto* mesh = SMActor->GetStaticMeshComponent();
		if (mesh->ComponentHasTag(NoInstancingTag)) {
			continue;
		}
		static const TSet<FName> SkippedNames{
			"Plane",
			"Cube",
			"SM_Ivy_16x16"
		};
		if (!mesh->GetStaticMesh() || mesh->bHiddenInGame || SMActor->bHidden) {
			continue;
		}
		if (SkippedNames.Contains(mesh->GetStaticMesh()->GetFName())) {
			continue;
		}
		
		//add to list
		TArray<AActor*>& SMList = SM_Map.FindOrAdd(mesh->GetStaticMesh());
		SMList.Push(SMActor);
	}

	for (auto SMVal : SM_Map)
	{
		TArray<AActor*>& SMList = SMVal.Value;
		
		//push any actors found to have duplicate transforms into a list to be destroyed
		for (int iRoot = 0; iRoot < SMList.Num(); ++iRoot)
		{
			AActor* pSrc = SMList[iRoot];
			FTransform SrcPos = pSrc->GetTransform();
			for (int iCheck = iRoot+1; iCheck < SMList.Num(); ++iCheck)
			{
				AActor* pCheck = SMList[iCheck];
				FTransform CheckPos = pCheck->GetTransform();

				if (CheckPos.Equals(SrcPos))
				{
					//duplicated SM
					ActorsToDestroy.Push(pCheck);
					SMList.RemoveAt(iCheck);
					--iCheck;
				}
			}
		}
	}

	util::editor::deleteActors(ActorsToDestroy);

	GEngine->ForceGarbageCollection(true);
	GEngine->PerformGarbageCollectionAndCleanupActors();
}

//
// DECOR ACTORS
//
bool LoadLovikaDecorAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects)
{
	bool bAnyObjectsWereLoadedOrUpdated = false;

	// Build a list of unloaded assets
	TArray<FString> UnloadedObjectPaths;
	bool bAtLeastOneUnloadedMap = false;
	for (int32 PathIdx = 0; PathIdx < ObjectPaths.Num(); ++PathIdx) {
		const FString& ObjectPath = ObjectPaths[PathIdx];

		UObject* FoundObject = FindObject<UObject>(NULL, *ObjectPath);
		if (FoundObject) {
			LoadedObjects.Add(FoundObject);
		}
		else {
			// Unloaded asset, we will load it later
			UnloadedObjectPaths.Add(ObjectPath);
			if (FEditorFileUtils::IsMapPackageAsset(ObjectPath))
			{
				bAtLeastOneUnloadedMap = true;
			}
		}
	}

	// Make sure all selected objects are loaded, where possible
	if (UnloadedObjectPaths.Num() > 0) {
		// Get the maximum objects to load before displaying the slow task
		const bool bShowProgressDialog = (UnloadedObjectPaths.Num() > GetDefault<UContentBrowserSettings>()->NumObjectsToLoadBeforeWarning) || bAtLeastOneUnloadedMap;
		FScopedSlowTask SlowTask(UnloadedObjectPaths.Num(), LOCTEXT("LoadingObjects", "Loading Objects..."));
		if (bShowProgressDialog) {
			SlowTask.MakeDialog();
		}

		GIsEditorLoadingPackage = true;

		const ELoadFlags LoadFlags = LOAD_None;

		bool bSomeObjectsFailedToLoad = false;
		for (int32 PathIdx = 0; PathIdx < UnloadedObjectPaths.Num(); ++PathIdx) {
			const FString& ObjectPath = UnloadedObjectPaths[PathIdx];
			SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("LoadingObjectf", "Loading {0}..."), FText::FromString(ObjectPath)));

			// Load up the object
			UObject* LoadedObject = LoadObject<UObject>(NULL, *ObjectPath, NULL, LoadFlags, NULL);
			if (LoadedObject) {
				LoadedObjects.Add(LoadedObject);
			}
			else {
				bSomeObjectsFailedToLoad = true;
			}
		}
	}

	return true;
}

void DeleteDecorActorsForTile(LevelTile levelTile) {
	auto& tileObject = levelTile.first;

	int decorActorsCount = 0;
	FString relPath = FString(tileObject->id.c_str()) + "/" + FString(tileObject->id.c_str()) + FString::FromInt(decorActorsCount);
	FString pathName = decor::filenameForActor(groupNameFor(levelTile.second), relPath);
	FString fullPathName = fullFilePathFor(pathName);
	while (FPaths::FileExists(fullPathName)) {

		FString pathNameBP = pathName + "." + FString(tileObject->id.c_str()) + FString::FromInt(decorActorsCount);
		TArray<FString> paths;
		paths.Add(pathNameBP);

		// Query for a list of assets in the selected paths
		TArray<UObject*> AssetObjects;
		if (LoadLovikaDecorAssetsIfNeeded(paths, AssetObjects)) {
			ObjectTools::DeleteObjects(AssetObjects, false);
		}
		decorActorsCount++;
		relPath = FString(tileObject->id.c_str()) + "/" + FString(tileObject->id.c_str()) + FString::FromInt(decorActorsCount);
		pathName = decor::filenameForActor(groupNameFor(levelTile.second), relPath);
		fullPathName = fullFilePathFor(pathName);
	}
	if (decorActorsCount > 0) {
		FPlatformProcess::Sleep(4.0f);
	}
}

bool saveDecorActorsForTile(LevelTile levelTile) {
	DeleteDecorActorsForTile(levelTile);

	auto& tileObject = levelTile.first;
	auto filter = InNullableLevel(levelTile.second) && IsInsideXy(tileObject->bounds) && NeedsDecorActor();

	int decorActorsCount = 0;
	for (auto&& actor : util::actor::filter(filter)) {
		FString relPath;
		if (actor->IsA<ALevelSequenceActor>()) {
			relPath = "Intro";
		} else {
			relPath = FString(tileObject->id.c_str()) + "/" + FString(tileObject->id.c_str()) + FString::FromInt(decorActorsCount++);
		}
		UE_LOG(TileDecorator, Error, TEXT("Name %s"), *relPath);

		if (!saveDecorActor(*actor, levelTile.second, *relPath, tileObject->bounds)) {
			return false; // @attn: why 'return' and not 'continue'?
		}
	}
	return true;
}

void saveDecorActorsForSelectedTiles() {
	auto levelTiles = getLevelTilesForActors(util::editor::getSelectedActors());
	for (auto&& levelTile : levelTiles) {
		if (!saveDecorActorsForTile(levelTile)) {
			showFailedTileExportPopup({ { &levelTile, "daveDecorActorsForTile() error"} });
		}
	}
}

void saveSingleEnvironmentalEffect(const FString& type, const TArray<AActor*>& actors) {
	if (actors.Num() == 0) {
		//UE_LOG(TileDecorator, Warning, TEXT("TileDecorator: No env effect of type?: %s"), *type);
		return;
	}
	if (actors.Num() >= 2) {
		FText title(FText::FromString("More than one effect of type " + type + ": " + FString::FromInt(actors.Num())));
		FMessageDialog::Open(EAppMsgType::Ok, title, &title);
		return;
	}
	if (!saveDecorEnvironmental(*actors[0], actors[0]->GetWorld()->GetLevel(0), type)) {
		FText title(FText::FromString("Couldn't save actor of type " + type));
		FMessageDialog::Open(EAppMsgType::Ok, title, &title);
	}
}

void clearDirectory_slow(const FString& folder) {
	auto& assetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TArray<FAssetData> allAssets;
	TArray<FAssetData> assetsToDelete;
	assetRegistry.GetAllAssets(allAssets);

	for (auto& asset : allAssets) {
		if (asset.PackageName.ToString().Contains(folder)) {
			assetsToDelete.Add(MoveTempIfPossible(asset));
		}
	}
	TArray<UObject*> packagesToDelete;
	for (auto& asset : assetsToDelete) {
		packagesToDelete.Add(asset.GetAsset());
	}
	//ObjectTools::DeleteAssets(assetsToDelete, false);
	ObjectTools::ForceDeleteObjects(packagesToDelete, false);
}

void clearEnvironmentDirectory() {
	auto folder = decor::filenameForEnvironmental(groupNameFor(util::deco::getLevel()->GetLevel()), "");
	clearDirectory_slow(folder);
}

void saveAmbienceActors(const TArray<AActor*>& actors) {
	clearEnvironmentDirectory();

	int counter = 0;
	for (auto actor : actors) {
		auto ambience = Cast<AAmbienceActor>(actor);
		TArray< AActor* > ChildActors;
		ambience->GetAttachedActors(ChildActors);
		ambience->SetAttachedActorNames(ChildActors);
		for (auto child : ChildActors){
			saveSingleEnvironmentalEffect(child->GetName(), { child });
		}
		const FString filename = "ambience_" + FString::FromInt(counter);
		saveSingleEnvironmentalEffect(filename, { actor });
		counter++;
	}
}

void saveEnvironmentalEffects() {
	//saveSingleEnvironmentalEffect("directional_0", util::actor::filter(IsA<ADirectionalLight>()));
	//saveSingleEnvironmentalEffect("skylight_0",    util::actor::filter(IsA<ASkyLight>()));
	//saveSingleEnvironmentalEffect("postprocess_0", util::actor::filter(IsA<APostProcessVolume>()));
	//saveSingleEnvironmentalEffect("fog_0",         util::actor::filter(IsA<AExponentialHeightFog>()));


	saveAmbienceActors(util::actor::filter(IsA<AAmbienceActor>() && InPersistentLevel()));

	// We skip actors that are attached to AAmbienceActor:s (since they will be exported with them) and
	// skip the actors that aren't in the persistent level (since they will be exported with the tile..)
	// However, we do warn about the latter case for the level designers to have a chance to fix it.
	saveSingleEnvironmentalEffect("bpaudiomanager_0", util::actor::filter(IsA<AAudioManager>() && InPersistentLevel() && !IsAttachedTo<AAmbienceActor>()));

	TArray<AActor*> unsavedActors;
	unsavedActors.Append(util::actor::filter(IsA<AAmbienceActor>() && !InPersistentLevel()));
	unsavedActors.Append(util::actor::filter(IsA<AAudioManager>() && !InPersistentLevel() && !IsAttachedTo<AAmbienceActor>()));

	if (unsavedActors.Num()) {
		FString err = "Won't save objects due to them not being in the Persistent level:\n";
		for (auto actor : unsavedActors) {
			err += "  " + actor->GetName() + "\n";
		}
		FText title(FText::FromString("Couldn't save actors"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(err), &title);
	}
}

}

FTileDecoratorModule::FTileDecoratorModule()
	: mCommands(new FUICommandList())
{
	FTileDecoratorCommands::Register();

	RegisterGenericCommands();
	RegisterTileDecoratorCommands();
	RegisterSubLevelCommands();
}

FTileDecoratorModule::~FTileDecoratorModule() {
}

//
// Command Registration
//
void FTileDecoratorModule::RegisterGenericCommands() {	
	mCommands->MapAction(
		FTileDecoratorCommands::Get().CopyToBlueprint,
		FExecuteAction::CreateLambda([]() { FCopyComponentsIntoBlueprint::OpenDialog(FCopyComponentsIntoBlueprint::EOpenDialogSave::CloseEditorAndSave); }),
		FCanExecuteAction::CreateLambda([]() { return GEditor->GetSelectedActorCount() > 0; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().CopyFrozenActorToBlueprint,
		FExecuteAction::CreateLambda([]() { FCopyComponentsIntoBlueprint::OpenDialog(FCopyComponentsIntoBlueprint::EOpenDialogSave::CloseEditorAndSave); }),
		FCanExecuteAction::CreateLambda([]() { return GEditor->GetSelectedActorCount() == 1; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().ToggleRegions,
		FExecuteAction::CreateLambda([]() { util::deco::getLevel()->ToggleRegions(); 	}),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; }),
		FIsActionChecked::CreateLambda([]() { return util::deco::getLevel() != nullptr && util::deco::getLevel()->IsShowingRegions(); })
	);

	mCommands->MapAction(
		FTileDecoratorCommands::Get().LockFileInGit,
		FExecuteAction::CreateLambda([this]() {	this->ExecuteLockFileInGit();}),
		FCanExecuteAction::CreateLambda([]() { return true; }),
		FIsActionChecked::CreateLambda([]() { return true; })
	);

	mCommands->MapAction(
		FTileDecoratorCommands::Get().UnLockFileInGit,
		FExecuteAction::CreateLambda([this]() {	this->ExecuteUnlockFileInGit(); }),
		FCanExecuteAction::CreateLambda([]() { return true; }),
		FIsActionChecked::CreateLambda([]() { return true; })
	);


}

void FTileDecoratorModule::RegisterTileDecoratorCommands() {
	mCommands->MapAction(
		FTileDecoratorCommands::Get().SaveDecorsForSelectedTiles,
		FExecuteAction::CreateLambda([]() { saveDecorsForSelectedTiles(); }), // @todo: Delete already existing decors?
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() && GEditor->GetSelectedActorCount() > 0; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().SaveDecorActorsForSelectedTiles,
		FExecuteAction::CreateLambda([]() { saveDecorActorsForSelectedTiles(); }), // @todo: Delete already existing decors?
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() && GEditor->GetSelectedActorCount() > 0; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().SaveEnvironmentalEffects,
		FExecuteAction::CreateLambda([]() { saveEnvironmentalEffects(); })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().SelectAllActorsInTile,
		FExecuteAction::CreateLambda([]() { selectAllActorsInCurrentTile(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
	
}

void FTileDecoratorModule::RegisterSubLevelCommands() {
	mCommands->MapAction(
		FTileDecoratorCommands::Get().MoveAllActorsInTileToSubLevel,
		FExecuteAction::CreateLambda([]() { moveCurrentTileActorsToTileSubLevel(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().MoveAllActorsInAllTilesToSubLevels,
		FExecuteAction::CreateLambda([]() { moveAllActorsToTileSubLevels(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().ConvertCurrentActorsTileToInstanced,
		FExecuteAction::CreateLambda([]() { convertCurrentActorsTileToInstanced(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().ConvertCurrentActorsTileFromInstanced,
		FExecuteAction::CreateLambda([]() { convertCurrentActorsTileFromInstanced(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
	mCommands->MapAction(
		FTileDecoratorCommands::Get().DeleteAllDuplicatedSMs,
		FExecuteAction::CreateLambda([]() { deleteAllDuplicatedSMs(); }),
		FCanExecuteAction::CreateLambda([]() { return util::deco::getLevel() != nullptr; })
	);
}


void FTileDecoratorModule::StartupModule() {
	UE_LOG(TileDecorator, Display, TEXT("TileDecorator: StartupModule"));
	FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedRef<FExtender> customToolBarButton(new FExtender());
	customToolBarButton->AddToolBarExtension(
		"Settings",
		EExtensionHook::Before,
		mCommands,
		FToolBarExtensionDelegate::CreateLambda([this, &levelEditorModule](FToolBarBuilder& builder) {
			builder.BeginSection("Dungeons");
			builder.AddComboButton(
				FUIAction(),
				FOnGetContent::CreateLambda([this]() { return SetupMenu(); })
			);
			builder.EndSection();
		})
	);

	levelEditorModule.GetToolBarExtensibilityManager()->AddExtender(customToolBarButton);

	//Add Git LFS lock/unlock context menu options
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.GetAllAssetViewContextMenuExtenders()
		.Add_GetRef(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FTileDecoratorModule::OnExtendContentBrowserAssetSelectionMenu)).GetHandle();

#if WITH_EDITOR	
	FEditorDelegates::PreSaveWorld.AddStatic(&FTileDecoratorModule::OnEditorPreSaveWorld);
	FEditorDelegates::PostSaveWorld.AddStatic(&FTileDecoratorModule::OnEditorPostSaveWorld);
	FEditorDelegates::OnMapOpened.AddStatic(&FTileDecoratorModule::OnEditorMapOpened);
#endif

}

void FTileDecoratorModule::ShutdownModule() {
	UE_LOG(TileDecorator, Display, TEXT("TileDecorator: ShutdownModule"));
}

//
// Menus
//
TSharedRef<SWidget> FTileDecoratorModule::SetupMenu() {
	FMenuBuilder builder(true, mCommands);
	SetupGenericMenu(builder);
	SetupTileDecoratorMenu(builder);
	return builder.MakeWidget();
}

void FTileDecoratorModule::SetupGenericMenu(FMenuBuilder& builder) {
	builder.BeginSection(NAME_None, LOCTEXT("dungeons.menu.blueprints", "Blueprints"));
	{
		builder.AddMenuEntry(FTileDecoratorCommands::Get().CopyToBlueprint);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().CopyFrozenActorToBlueprint);
	}
	builder.EndSection();
}

void beginDecorLevelSection(FMenuBuilder& builder, FString ifHasDecorLevel) {
	FText text = FText::FromString(util::deco::getLevel() ? ifHasDecorLevel : ifHasDecorLevel + " (DecorationLevelActor is missing!)");
	builder.BeginSection(NAME_None, text);
}

void FTileDecoratorModule::SetupTileDecoratorMenu(FMenuBuilder& builder) {
	beginDecorLevelSection(builder, "Tile Decoration"); {
		builder.AddMenuEntry(FTileDecoratorCommands::Get().ToggleRegions);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().SaveDecorsForSelectedTiles);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().SaveDecorActorsForSelectedTiles);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().SaveEnvironmentalEffects);
	}
	builder.EndSection();

	beginDecorLevelSection(builder, "Sub-levels"); {
		builder.AddMenuEntry(FTileDecoratorCommands::Get().SelectAllActorsInTile);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().MoveAllActorsInTileToSubLevel);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().MoveAllActorsInAllTilesToSubLevels);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().ConvertCurrentActorsTileToInstanced);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().ConvertCurrentActorsTileFromInstanced);
		builder.AddMenuEntry(FTileDecoratorCommands::Get().DeleteAllDuplicatedSMs);
	}
	builder.EndSection();
}



struct StaticMeshesSaveData
{
	FSoftObjectPath OldPath;
	FTransform Pos;
	UStaticMesh* pStaticMesh;
};

TMap< UDungeonsLevelStreamingDynamic*, TArray<StaticMeshesSaveData> > sActorSaveProcessingLists;

UDungeonsLevelStreamingDynamic* GetDungeonsLevelStreamingDynamicFromWorld(UWorld* World)
{
	UWorld& world = *util::editor::getWorld();

	for (auto* level : world.GetStreamingLevels())
	{
		if (level->GetLoadedLevel() && World == level->GetLoadedLevel()->GetOuter())
		{
			if (auto DungeonsStreamingLvl = Cast<UDungeonsLevelStreamingDynamic>(level))
			{
				return DungeonsStreamingLvl;

			}
		}
	}

	return nullptr;
}

void ShadowCasterCheck()
{
	int32 WarningTime = CVarShadowCasterWarningTime.GetValueOnAnyThread();

	if (WarningTime == 0)
		return;

	{
		//lets check for shadow casters
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(util::editor::getWorld(), AActor::StaticClass(), AllActors);
		for (AActor* actor : AllActors)
		{
			if (actor)
			{
				//ignore doors for now
				if (actor->IsA(AInstantTravelActor::StaticClass()))
				{
					continue;
				}

				TArray<USpotLightComponent*> Spotlights;
				actor->GetComponents(Spotlights, true);
				TArray<UPointLightComponent*> Pointlights;
				actor->GetComponents(Pointlights, true);

				for (auto* pLight : Spotlights)
				{
					if (pLight && pLight->CastShadows)
					{
						FString WarningMsg = FString::Format(TEXT(
							"!! {0} IS SHADOW CASTING! Please Turn this off to reduce performance and memory Impact (especially on the Switch) )\n"
						), { pLight->GetFullName(), });

						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, WarningTime, FColor::Red, WarningMsg);

						FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### %s IS SHADOW CASTING !####\n"), *pLight->GetFullName());
					}
				}


				for (auto* pLight : Pointlights)
				{
					if (pLight && pLight->CastShadows)
					{
						FString WarningMsg = FString::Format(TEXT(
							"!! {0} IS SHADOW CASTING! Please Turn this off to reduce performance and memory Impact (especially on the Switch) )\n"
						), { pLight->GetFullName(), });

						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, WarningTime, FColor::Red, WarningMsg);

						FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### %s IS SHADOW CASTING !####\n"), *pLight->GetFullName());
					}
				}

			}
		}
	}
}

TSharedRef<FExtender> FTileDecoratorModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
	// Menu extender for Content Browser context menu when an asset is selected
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	if (SelectedAssets.Num() > 0)
	{
		mSelectedAssets = SelectedAssets;	

		Extender->AddMenuExtension("AssetContextSourceControl", EExtensionHook::After, mCommands, FMenuExtensionDelegate::CreateLambda(
			[this](FMenuBuilder& MenuBuilder) mutable
		{
			MenuBuilder.BeginSection("AssetContextExploreMenuOptions", LOCTEXT("AssetContextExploreMenuOptionsHeading", "Lock"));
			{
				// LockFileInGit
				MenuBuilder.AddMenuEntry(
					FTileDecoratorCommands::Get().LockFileInGit,
					NAME_None,
					LOCTEXT("GitLockFile", "Lock File in Git"),
					LOCTEXT("GitLockFileTooltip", "FTileDecoratorModule - Try to lock selected assets in Git.")
				);

				MenuBuilder.AddMenuEntry(
					FTileDecoratorCommands::Get().UnLockFileInGit,
					NAME_None,
					LOCTEXT("GitUnLockFile", "Unlock File in Git"),
					LOCTEXT("GitUnLockFileTooltip", "FTileDecoratorModule - Try to unlock selected assets in Git.")
				);

			}
			MenuBuilder.EndSection();
		}));
	}
	return Extender;
}

void FTileDecoratorModule::OnEditorPreSaveWorld(uint32 SaveFlags, UWorld* World)
{
	ShadowCasterCheck();

	if(CVarEnableInstancingOnSave.GetValueOnAnyThread() == 0)
		return;
	
	auto DungeonsStreamingLvl = GetDungeonsLevelStreamingDynamicFromWorld(World);

	if (DungeonsStreamingLvl)
	{
		
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### OnEditorPreSaveWorld ####\n"));

		FSoftObjectPath SavingWorldPath(World);
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##SavingWorldPath : %s\n"), *SavingWorldPath.ToString());

		UWorld& world = *util::editor::getWorld();

		TArray<AActor*> ActorsToDestroy;
		DungeonsStreamingLvl->ConvertInstancedMeshes(true, ActorsToDestroy);

		FSoftObjectPath PersistantWorldPath(&world);
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##PersistantWorldPath : %s\n"), *PersistantWorldPath.ToString());

		TArray<StaticMeshesSaveData> SaveDataList;

		//Save out soft object paths
		for (AActor* actor : ActorsToDestroy)
		{
			if(auto pSMActor = Cast<AStaticMeshActor>(actor))
			{
				StaticMeshesSaveData NewData;
			
				FSoftObjectPath CurrActor(pSMActor);
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("## CurrActor Actor Test : %s\n"), *CurrActor.ToString());
				NewData.OldPath = CurrActor;
				NewData.Pos = pSMActor->GetTransform();
				NewData.pStaticMesh = pSMActor->GetStaticMeshComponent()->GetStaticMesh();
				SaveDataList.Push(NewData);
			}
		}
		//move actors into global PersistentLevel
		ULevel* pPersistantLvl = world.GetLevel(0);
		int32 iMoved = UEditorLevelUtils::MoveActorsToLevel(ActorsToDestroy, pPersistantLvl);

		check(iMoved == ActorsToDestroy.Num());
		
		for (AActor* pActor : pPersistantLvl->Actors)
		{
			if (pActor && pActor->IsA<AStaticMeshActor>())
			{
				FSoftObjectPath CurrActor(pActor);
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("## persistant Actor : %s\n"), *CurrActor.ToString());
			}
		}

		sActorSaveProcessingLists.FindOrAdd(DungeonsStreamingLvl) = SaveDataList;

	}
}

void FTileDecoratorModule::OnEditorPostSaveWorld(uint32 SaveFlags, UWorld* World, bool bSuccess)
{
	if (CVarEnableInstancingOnSave.GetValueOnAnyThread() == 0)
		return;

	auto DungeonsStreamingLvl = GetDungeonsLevelStreamingDynamicFromWorld(World);

	if (DungeonsStreamingLvl)
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##### OnEditorPostSaveWorld ####\n"));

		FSoftObjectPath SavingWorldPath(World);
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##SavingWorldPath : %s\n"), *SavingWorldPath.ToString());

		UWorld& world = *util::editor::getWorld();
		ULevel* pPersistantLvl = world.GetLevel(0);
		
		//get rid of generated Instance assets
		TArray<AActor*> ActorsToDestroy;
		for (auto* pActor : DungeonsStreamingLvl->GetLoadedLevel()->Actors)
		{
			if (pActor && pActor->IsA<AInstancedMeshesActor>())
			{
				ActorsToDestroy.Push(pActor);
			}
		}

		for (auto* pActor : ActorsToDestroy)
		{
			util::editor::closeAllEditorsForAsset(pActor);
			RenameToOld(pActor);
		}
		util::editor::deleteActors(ActorsToDestroy);

		TArray<AStaticMeshActor*> PersistantLvlSMActors;

		//pull static meshes from persistant level
		for (AActor* pActor : pPersistantLvl->Actors)
		{
			if (auto* SMA = Cast<AStaticMeshActor>(pActor))
			{
				PersistantLvlSMActors.Push(SMA);
			}
		}

		
		if (auto* pActorList = sActorSaveProcessingLists.Find(DungeonsStreamingLvl))
		{
			//refind actors
			TArray<AActor*> NewActors;

			for (auto& actorSaveData : *pActorList)
			{
				AActor* pActor = nullptr;
				for (AStaticMeshActor* SMA : PersistantLvlSMActors)
				{
					if (actorSaveData.pStaticMesh == SMA->GetStaticMeshComponent()->GetStaticMesh() &&
						SMA->GetTransform().Equals(actorSaveData.Pos))
					{
						//FOUND IT!
						pActor = SMA;
						break;
					}					
				}
				
				checkf(pActor, TEXT("Failed to Resolve Actor : %s"), *actorSaveData.OldPath.ToString());//failed to find actor 
				NewActors.Push(pActor);
			}

			//move actors into back into level
			UEditorLevelUtils::MoveActorsToLevel(NewActors, DungeonsStreamingLvl);
		}

		sActorSaveProcessingLists.Remove(DungeonsStreamingLvl);

		GEngine->ForceGarbageCollection(true);
		GEngine->PerformGarbageCollectionAndCleanupActors();

	}

}

void FTileDecoratorModule::OnEditorMapOpened(const FString& Filename, bool bLoadAsTemplate)
{
	ShadowCasterCheck();
}



void FTileDecoratorModule::ExecuteLockFileInGit()
{
	FScopedSlowTask SlowTask(mSelectedAssets.Num(), LOCTEXT("LockingAssets", "Locking Assets..."));
	SlowTask.MakeDialog(false, true);

	FString errorPaths;
	for (int i = 0; i < mSelectedAssets.Num(); ++i)
	{
		FString path = mSelectedAssets[i].ObjectPath.ToString();

		TArray<FString> pathSplit;
		path.ParseIntoArray(pathSplit, TEXT("/"));

		TArray<FString> nameSplit;
		pathSplit[pathSplit.Num() - 1].ParseIntoArray(nameSplit, TEXT("."));
		pathSplit[pathSplit.Num() - 1] = nameSplit[0];
		path = "Content";
		for (int j = 1; j < pathSplit.Num(); ++j)
		{
			path += "/" + pathSplit[j];
		}

		path += TEXT(".uasset");
		FString RelativePath = FPaths::ProjectDir();
		FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

		FullPath += "BuildTools/D11/";

		FString cmd = "cd " + FullPath + " && LockUnlockFile.py 1 ../../" + path + " 2>&1";
		SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("LockingAssetf", "Locking {0}..."), FText::FromString(path)));
		
		FString output;
		char   psBuffer[128];
		FILE   *pPipe;

		if ((pPipe = _popen(TCHAR_TO_ANSI(*cmd), "r")) != NULL)
		{
			while (fgets(psBuffer, 128, pPipe))
			{
				output += psBuffer;
			}
			_pclose(pPipe);
		}
		bool bSuccessful = !output.Contains("Lock failed");
		FString result = !bSuccessful ? "Lock Failed" : "Lock Successful";
		UE_LOG(LogTemp, Log, TEXT("%s : %s"), *path, *result);

		if (!bSuccessful)
		{
			errorPaths += path + "\n";

			FString cmd2 = "cd " + FullPath + " && LockCheckFile.py ../../" + path + " 2>&1";

			FString output2;
			char   psBuffer2[128];
			FILE   *pPipe2;

			if ((pPipe2 = _popen(TCHAR_TO_ANSI(*cmd2), "r")) != NULL)
			{
				while (fgets(psBuffer2, 128, pPipe2))
				{
					output2 += psBuffer2;
				}
				_pclose(pPipe2);
			}

			errorPaths += output2 + "\n";

		}
	}

	if (!errorPaths.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("Core", "Unlocking Errors", "Was not possible to lock this path(s):\n{0}"), FText::FromString(errorPaths)));
	}
}

void FTileDecoratorModule::ExecuteUnlockFileInGit()
{
	FScopedSlowTask SlowTask(mSelectedAssets.Num(), LOCTEXT("UnlockingAssets", "Unlocking Assets..."));
	SlowTask.MakeDialog(false, true);

	FString errorPaths;
	for (int i = 0; i < mSelectedAssets.Num(); ++i)
	{
		FString path = mSelectedAssets[i].ObjectPath.ToString();

		TArray<FString> pathSplit;
		path.ParseIntoArray(pathSplit, TEXT("/"));

		TArray<FString> nameSplit;
		pathSplit[pathSplit.Num() - 1].ParseIntoArray(nameSplit, TEXT("."));
		pathSplit[pathSplit.Num() - 1] = nameSplit[0];
		path = "Content";
		for (int j = 1; j < pathSplit.Num(); ++j)
		{
			path += "/" + pathSplit[j];
		}

		path += TEXT(".uasset");

		FString RelativePath = FPaths::ProjectDir();
		FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

		FullPath += "BuildTools/D11/";

		FString cmd = "cd " + FullPath + " && LockUnlockFile.py 2 ../../" + path + " 2>&1";

		SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("UnlockingAssetf", "Unlocking {0}..."), FText::FromString(path)));

		FString output;
		char   psBuffer[128];
		FILE   *pPipe;

		if ((pPipe = _popen(TCHAR_TO_ANSI(*cmd), "r")) != NULL)
		{
			while (fgets(psBuffer, 128, pPipe))
			{
				output += psBuffer;
			}
			_pclose(pPipe);
		}

		bool bSuccessful = output.Contains("Unlocked Content");
		FString result = !bSuccessful ? "Unlock Failed" : "Unlock Successful";
		UE_LOG(LogTemp, Log, TEXT("%s : %s"), *path, *result);

		if (!bSuccessful)
		{
			errorPaths += path + "\n";
		}
	}

	if (!errorPaths.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("Core", "Unlocking Errors", "Was not possible to unlock this path(s):\n{0}"), FText::FromString(errorPaths)));
	}
}


#undef LOCTEXT_NAMESPACE

