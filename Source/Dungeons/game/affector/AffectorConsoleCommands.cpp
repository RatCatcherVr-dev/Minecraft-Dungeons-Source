#include "Dungeons.h"
#include "game/abilities/effects/AffectorGameplayEffect.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/affector/Affectors.h"
#include "game/util/ActorQuery.h"
#include "util/RandomUtil.h"
#include "util/Algo.h"

static void DoActivateAffector(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() == 0 || args.Num() > 2) {
		out.Log(TEXT("Requires 2 arguments, the affector {name} and optionally {data}"));
		return;
	}

	if (const auto pc = world->GetFirstPlayerController()) {
		if (!pc->HasAuthority()) {
			out.Log(TEXT("Only the server can activate affectors."));
			return;
		}
	}

	const auto& affectorName = args[0];
	const auto* type = affector::findType(affectorName);
	if (type == nullptr) {
		out.Logf(TEXT("Could not find any affector with the name '%s'"),  *affectorName);
		return;
	}

	const auto& affectorData = args.Num() == 2 ? args[1] : "";
	auto& affectors = affector::getDebug(world);
	affectors.Set(type->Id, affectorData);
	if (type->EffectType) {
		for (auto character : actorquery::getActors<ABaseCharacter>(world)) {
			const auto target = character->IsA<APlayerCharacter>() ? affector::EAffectorTarget::Player : affector::EAffectorTarget::Mob;
			if (target == type->Target) {
				affector::effect::applyEffect(character->GetAbilitySystemComponent(), *type);
			}
		}
	}
}

static const FAutoConsoleCommand ActivateAffector(TEXT("Dungeons.Affector.Activate")
	, TEXT("Activate an affector usage: Activate {name} {data}. Example 'Activate PlayerSpeedMul 1.8'")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoActivateAffector)
	, ECVF_Cheat);


static void DoListAffectors(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (const auto& at : affector::getTypes()) {
		out.Logf(TEXT("%s: %s"), *at->RuleId, *at->GetBasicDescription());
	}
}

static const FAutoConsoleCommand ListAffectors(TEXT("Dungeons.Affector.List")
	, TEXT("List affectors")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoListAffectors)
	, ECVF_Cheat);


static void DoListActiveAffectors(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	out.Logf(TEXT("--- default affectors ---"));
	for (const auto& at : affector::get(world).GetActive()) {
		if (at.Data.IsDefault()) {
			out.Logf(TEXT("%s: %s"), *at.Type.RuleId, *at.Data.AsString());
		}
	}

	out.Logf(TEXT("--- daily affectors ---"));
	for (const auto& at : affector::get(world).GetActive()) {
		if (!at.Data.IsDefault()) {
			out.Logf(TEXT("%s: %s"), *at.Type.RuleId, *at.Data.AsString());
		}
	}
}

static const FAutoConsoleCommand ListActiveAffectors(TEXT("Dungeons.Affector.ListActive")
	, TEXT("List active affectors")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoListActiveAffectors)
	, ECVF_Cheat);


struct DescData {
	FText description;
	FString data;
	EAffectorModus modus;
};

static TOptional<DescData> GetDescData(const affector::AffectorType& type, const FString& arg, bool allowModusFallback = true) {
	Random& rnd = Util::sharedRandom();

	if (!algo::contains(TArray<FString> { "*", "+", "-" }, arg)) {
		return DescData{ type.GetDescription(arg), arg, type.GetModusForData(arg) };
	}

	auto requestedModus = [&]() {
		if (arg == "+") { return EAffectorModus::Easier; }
		if (arg == "-") { return EAffectorModus::Harder; }
		return EAffectorModus::Any; }();

	if (!type.HasDataGenerator(requestedModus) && allowModusFallback) {
		requestedModus = EAffectorModus::Any;
	}
	if (!type.HasDataGenerator(requestedModus)) {
		return {};
	}
	const auto data = type.GetDataGenerator(requestedModus, rnd.nextBoolean())({ rnd });
	return DescData{ type.GetDescription(data), data, type.GetModusForData(data) };
}

static FString CreateDefaultDescription(const affector::AffectorType& type, const FString& arg, bool allowModusFallback = true) {
	bool isQuery = arg.EndsWith("?");
	auto inData = isQuery ? arg.LeftChop(1) : arg;

	if (const auto description = GetDescData(type, inData)) {
		if (isQuery) {
			return FString::Format(TEXT("Description for '{0} {1}' ({2}): {3}"), {
				type.RuleId,
				description->data,
				FString(description->modus == EAffectorModus::Easier? "easier" : "harder"),
				description->description.ToString()
			});
		} else {
			return FString::Format(TEXT("Description: {0}"), { description->description.ToString() });
		}
	}
	return FString::Format(TEXT("Invalid data for '{0}': {1}"), { type.RuleId, arg });
}

static void DoDescribeAffector(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() != 2) {
		out.Log(TEXT("Exactly two arguments are needed: affector-name data"));
		return;
	}
	const auto& affectorName = args[0];
	const auto* type = affector::findType(affectorName);
	if (type == nullptr) {
		out.Logf(TEXT("Could not find any affector with the name '%s'"), *affectorName);
		return;
	}
	out.Log(CreateDefaultDescription(*type, args[1]));
}

static const FAutoConsoleCommand DescribeAffector(TEXT("Dungeons.Affector.Desc")
	, TEXT("Get description for an affector with a given data value")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoDescribeAffector)
	, ECVF_Cheat);


static void DoDescribeAllAffectors(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const FString data = args.Num() > 0 ? args[0] : "*";

	for (auto* type : affector::getTypes()) {
		out.Log(CreateDefaultDescription(*type, data));
	}
}

static const FAutoConsoleCommand DescribeAllAffectors(TEXT("Dungeons.Affector.DescAll")
	, TEXT("Get description for all affector with a sample data value")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoDescribeAllAffectors)
	, ECVF_Cheat);
