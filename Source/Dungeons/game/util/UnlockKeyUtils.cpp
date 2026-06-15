#include "Dungeons.h"
#include "UnlockKeyUtils.h"
#include "DungeonsGameInstance.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "util/parse/eval/Boolean.h"
#include "util/parse/ParserFactory.h"
#include "util/Algo.hpp"
#include "util/StringUtil.h"

namespace unlockkey {

using Keys = const std::set<FString>&;

int countKeys(const FString& keyPattern, Keys keys) {
	return algo::count_if(keys, RETLAMBDA(it.MatchesWildcard(keyPattern)));
}

TOptional<parse::eval::number::Number> findValue(const FString& keyPattern, Keys keys) {
	for (auto& key : keys) {
		int separatorIndex;
		if (!key.FindChar('=', separatorIndex)) {
			continue;
		}
		if (key.Left(separatorIndex).MatchesWildcard(keyPattern)) {
			if (auto value = ArgAsFloat(key.RightChop(separatorIndex + 1))) {
				return value.GetValue();
			}
		}
	}
	return {};
}

parse::eval::number::Provider<Keys> unlockKeyNumberFactory(const parse::token::Token& token) {
	if (parse::token::Type::Value == token.type) {
		auto s = FString(token.data.c_str()).ToLower();

		if (s.StartsWith("#")) {
			return[id = std::move(s)](Keys keys)->parse::eval::number::Number {
				return findValue(id, keys).Get(0);
			};
		}
		return[value = std::move(s)](Keys keys)->parse::eval::number::Number {
			return countKeys(value, keys);
		};
	}
	return {};
}

namespace {
	bool unlocks(const std::string& expr, Keys keys) {
		auto f = parse::numberDefault<Keys>(expr, unlockKeyNumberFactory);
		return f && f->eval(keys) >= 1;
	}

	UDungeonsGameInstance* getGameInstance(UObject* WorldContextObject) {
		return WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	}
}

//
// Public interface
//
bool unlocks(const TArray<FString>& unlockKeys, const FString& expr) {
	const auto f = createPredicate(unlockKeys);
	return f && f(stringutil::toStdString(expr));
}

TOptional<bool> unlocksAny(const Pred<std::string>& unlocker, const std::vector<std::string>& unlockConditions) {
	if (unlockConditions.empty()) {
		return {};
	}
	return algo::any_of(unlockConditions, unlocker);
}

Pred<std::string> createPredicate(const TArray<FString>& keys) {
	std::set<FString> keySet;
	for (auto& s : keys) {
		keySet.insert(s.ToLower());
	}
	return [keySet](const std::string& expr) {
		return unlocks(expr, keySet);
	};
}

}

//
// UUnlockKeyUtils blueprint function library
//
bool UUnlockKeyUtils::IsUnlocked(UObject* WorldContextObject, const FString& Expression) {
	if (auto* gameInstance = unlockkey::getGameInstance(WorldContextObject)) {
		return gameInstance->QueryUnlockKeys(Expression);
	}
	return false;
}

bool UUnlockKeyUtils::GiveUnlockKeyToLocalPlayer(APlayerCharacter* Player, const FString& Key) {
	if (Player && Player->IsLocallyControlled()) {
		return Player->GetCharacterSerializeComponent()->AddUnlockKey(Key);
	}
	return false;
}

void UUnlockKeyUtils::GiveUnlockKeyToAllLocalPlayers(UObject* WorldContextObject, const FString& Key) {
	for (auto* player : InstanceTracker<APlayerCharacter>::GetList(WorldContextObject->GetWorld())) {
		GiveUnlockKeyToLocalPlayer(player, Key);
	}
}
