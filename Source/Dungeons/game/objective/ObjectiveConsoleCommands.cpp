#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/level/sound/AudioMusicManager.h"
#include "game/level/sound/MusicSetTypes.h"
#include "game/Game.h"
#include "game/Conversion.h"
#include "util/TileDebug.h"
#include "util/EnumUtil.h"
#include "util/Random.h"
#include <Engine/AssetManager.h>


#include "DungeonsGameInstance.h"

static void PrintLevelToConsole(APlayerController* playerController, const Util::TileDebugData& tileDebugData, int indentLevel = 0) {
	const auto indent = [](auto count) {
		FString string;
		for (auto i = 0; i < count; i++) {
			// '\t' does not work; have to reinvent the tab
			string.AppendChars(TEXT("    "), 4);
		}
		return string;
	}(indentLevel);

	playerController->ClientMessage(indent + tileDebugData.name.c_str());

	for (auto&& child : tileDebugData.children) {
		PrintLevelToConsole(playerController, child, indentLevel + 1);
	}
}

static void DoShowMusicQueue(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance()))
		if (auto* musicManager = gi->GetAudioMusicManager())
			musicManager->ToggleShowDebugOutput();
}

static const FAutoConsoleCommand ShowMusicQueue(TEXT("Dungeons.Level.Music")
	, TEXT("Show the music queue and the active AudioComponents")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoShowMusicQueue)
	, ECVF_Cheat
);

static void DoShowSFXQueue(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance()))
		if (auto* manager = gi->GetAudioSFXManager())
			manager->ToggleShowDebugOutput();
}

static const FAutoConsoleCommand ShowSFXQueue(TEXT("Dungeons.Level.SFX")
	, TEXT("Show the track queue for SFX and the active AudioComponents")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoShowSFXQueue)
	, ECVF_Cheat
);

static TOptional<EDungeonsMusicPriority> GetSlot(const FString& slotidentifier)
{
	// get slot, defaults to BackgroundMusic
	EDungeonsMusicPriority slot = EDungeonsMusicPriority::Top;
	if (slotidentifier.IsNumeric())
		slot = EDungeonsMusicPriority(FCString::Atoi(*slotidentifier));
	else
		slot = EnumValueFromString(EDungeonsMusicPriority, slotidentifier).Get(EDungeonsMusicPriority::BackgroundMusic);
	return slot < EDungeonsMusicPriority::Top ? TOptional<EDungeonsMusicPriority>(slot) : TOptional<EDungeonsMusicPriority>();
}

static USoundCue* GetSoundCue(const FString& name) {
	//check possible asset locations:
	UAssetManager& assetManager = UAssetManager::Get();
	auto& assetRegistry = assetManager.GetAssetRegistry();
	FARFilter packageFilter;
	/// list folders in base...
	for (const auto& ppath : { "04_bgm_env_arena","04_bgm_env_dungeons2D", "04_bgm_mob_boss", "04_bgm_env_overworld2D" , "04_bgm_env_overworld3D" , "04_bgm_mob_event"}) {
		packageFilter.PackageNames.Add(*("/Game/AudioForce/04_playback_soundCue/" + FString(ppath) + "/" + name));
	}
	TArray<FAssetData> assetData;
	assetRegistry.GetAssets(packageFilter, assetData);
	auto soundAsset = assetData.FindByPredicate([](const FAssetData& data) { return data.IsValid(); });

	if (soundAsset) {
		FSoftObjectPath objectPath = FSoftObjectPath(soundAsset->ObjectPath.ToString());
		StaticLoadObject(USoundCue::StaticClass(), NULL, *objectPath.GetAssetPathString());
		USoundCue* soundCue = Cast<USoundCue>(objectPath.ResolveObject());
		if (soundCue) 
			return soundCue;
	}
	
	return nullptr;
}

static void DoPopMusicTrack(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!commands.Num()) {
		out.Log(TEXT("Not enough input parameters, call with 'soundcue' to play"));
		return;
	}

	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance()))
	{
		if (auto* manager = gi->GetAudioMusicManager())
		{
			auto slot = GetSlot(commands[0]);
			if (slot.IsSet())
				manager->Pop(slot.GetValue());
		}
	}
}

static const FAutoConsoleCommand PopMusicTrack(TEXT("Dungeons.Music.Pop")
	, TEXT("Push a music track to the desired musicmanagerslot by specifying the asset (and the slot number)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPopMusicTrack)
	, ECVF_Cheat
);


static void DoPushMusicTrack(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!commands.Num()) {
		out.Log(TEXT("Not enough input parameters, call with 'soundcue' to play"));
		return;
	}
	if (commands[0].IsNumeric()) {
		out.Log(TEXT("command should be name of soundCue to play!"));
		return;
	}
	if (commands.Num() < 2) {
		out.Log(TEXT("No priority slot specified -will fall back to BackgroundMusic-slot"));
	}

	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance())) 
	{
		if (auto* manager = gi->GetAudioMusicManager())
		{
			if (auto soundCue = GetSoundCue(commands[0])) {
				EDungeonsMusicPriority slot = GetSlot(commands.Num() > 1?commands[1]: "").Get(EDungeonsMusicPriority::BackgroundMusic);
				manager->PushMusicTrack(soundCue, slot);
			}
			else
				out.Logf(TEXT("Error, no asset '%s' found!"), *commands[0]);
		}
	}
}

static const FAutoConsoleCommand PushMusicTrack(TEXT("Dungeons.Music.PushTrack")
	, TEXT("Push a music track to the desired musicmanagerslot by specifying the asset (and the slot number)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPushMusicTrack)
	, ECVF_Cheat
);

static void DoPlaybackMusicTrack(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!commands.Num()) {
		out.Log(TEXT("Not enough input parameters, call with 'soundcue' to play"));
		return;
	}
	if (commands[0].IsNumeric()) {
		out.Log(TEXT("command should be name of soundCue to play!"));
		return;
	}

	if (auto soundCue = GetSoundCue(commands[0])) {
		UAudioComponent* ac = UGameplayStatics::CreateSound2D(world, soundCue, 1.0f);
		ac->Play();

	}
	else 
		out.Logf(TEXT("Error, no asset '%s' found!"), *commands[0]);
}

static const FAutoConsoleCommand PlaybackMusicTrack(TEXT("Dungeons.Music.PlayTrack")
	, TEXT("Play any music track by specifying the asset ")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPlaybackMusicTrack)
	, ECVF_Cheat
);

static void DoPlayAllMusicTracks(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance()))
		if (auto* manager = gi->GetAudioMusicManager())
		{
			manager->TriggerPlayAllTracks();
		}
}

static const FAutoConsoleCommand PlayAllMusicTracks(TEXT("Dungeons.Music.PlayAll")
	, TEXT("Trigger Playback of all the active AudioComponents")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPlayAllMusicTracks)
	, ECVF_Cheat
);

static void DoPopAllSoundmixes(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* gi = Cast<UDungeonsGameInstance>(world->GetGameInstance()))
		if (auto* manager = gi->GetSoundMixManager())
		{
			manager->USoundMixManager::PopAllSoundMixes();
		}
}

static const FAutoConsoleCommand PopAllSoundmixes(TEXT("Dungeons.Music.PopAllSoundmixes")
	, TEXT("Stop and clear all added soundmixes")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPopAllSoundmixes)
	, ECVF_Cheat
);

static void DoPrintLevel(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto game = actorquery::getGame(world);
	const auto playerController = world->GetFirstPlayerController();

	if (game != nullptr && playerController != nullptr) {
		const auto tileDebugData = Util::collectTiles(game->tiles());
		PrintLevelToConsole(playerController, tileDebugData);
	}
}

static TOptional<Util::TileDebugData> FindTile(APlayerController* playerController, const Util::TileDebugData& tileDebugData, const std::string& target) {
	if (tileDebugData.name.find(target) != std::string::npos) {
		return { tileDebugData };
	}

	for (auto&& child : tileDebugData.children) {
		const auto maybeResult = FindTile(playerController, child, target);
		
		if (maybeResult.IsSet()) {
			return maybeResult;
		}
	}

	return {};
}

static void DoTeleport(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const std::string target { TCHAR_TO_UTF8(*commands[0]) };
	const auto game = actorquery::getGame(world);
	const auto playerController = world->GetFirstPlayerController();

	if (game != nullptr && playerController != nullptr) {
		const auto tileDebugData = Util::collectTiles(game->tiles());
		const auto maybeTile = FindTile(playerController, tileDebugData, target);
		if (maybeTile.IsSet()) {
			const auto& cuboid = maybeTile->location;
			const auto center = (conversion::blockToUe(cuboid.minInclusive) + conversion::blockToUe(cuboid.maxExclusive)) / 2.f;
			playerController->GetPawn()->SetActorLocation(center + FVector { 0.f, 0.f, 1000.f });
		}
	}
}

static const FAutoConsoleCommand PrintLevel(TEXT("Dungeons.Level.Print")
	, TEXT("...")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPrintLevel)
	, ECVF_Cheat
);

static const FAutoConsoleCommand TeleportTile(TEXT("Dungeons.Level.Teleport")
	, TEXT("...")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoTeleport)
	, ECVF_Cheat
);

static void DoDebugRng(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto seed = [&] {
		if (commands.Num() > 0) {
			if (commands[0].IsNumeric()) {
				return FCString::Atoi(*commands[0]);
			}

			out.Log(TEXT("First arg must be a mob name/tag and second argument must be a number (if present)"));
		}

		return 1;
	}();

	Random rnd { static_cast<uint32_t>(seed) };

	const auto iterations = 1000000;
	for (auto i = 0; i < iterations; i++) {
		rnd.nextDouble();
		rnd.nextBoolean();
	}

	const auto randomInt = rnd.nextInt();
	out.Logf(ELogVerbosity::Display, TEXT("Random int after %d iterations: %d"), iterations, randomInt);
}

static const FAutoConsoleCommand DebugRng(TEXT("Dungeons.Level.DebugRng")
	, TEXT("Run the rng for a few iterations and output a random int")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoDebugRng)
	, ECVF_Cheat
);