#include "Dungeons.h"
#include "EndGameContentDefs.h"
#include "EndGameContent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/difficulty/DifficultyUtil.h"
#include "locale/LocTableFromFile.h"
#include "util/Algo.h"

#define LOCTEXT_NAMESPACE "EndGame"
namespace endgamecontent {
	auto endGameContentDefs = EndGameContentDefs(enum_cast(EEndGameContentType::ENUM_COUNT), EEndGameContentType::Invalid);
	
	const EndGameContentDefs& defs() {
		return endGameContentDefs;
	}

	TArray<EEndGameContentType> allTypes() {
		return algo::map_tarray(defs().getAllMutable(), RETLAMBDA(it->type()));
	}

	TArray<EEndGameContentType> unannouncedTypes(const TArray<EEndGameContentType>& announced) {
		auto types = allTypes();
		return algo::copy_if(types, RETLAMBDA(!announced.Contains(it)));
	}

	TArray<EEndGameContentType> unlockedUnannouncedTypes(const UCharacterSerializeComponent* serialize) {
		TArray<EEndGameContentType> types;
		const auto& alreadyAnnounced = serialize->ReadAnnouncedEndGameContent().announcedUnlockedContent.Array();
		for (auto type : unannouncedTypes(alreadyAnnounced)) {
			if (isUnlocked(serialize, type)) {
				types.Add(type);
			}
		}
		return types;
	}

	bool isUnlocked(const UCharacterSerializeComponent* serialize, EEndGameContentType type) {
		if (auto def = endgamecontent::defs().getChecked(type)) {
			if (def->isUnlocked(serialize)) {
				return true;
			}
		}
		return false;
	}

	TArray<FString> getAllUnlockedProgressKeys(const UCharacterSerializeComponent* serialize) {
		TArray<FString> keys;
		for (auto& def : defs().getAllMutable()) {
			if (def->isUnlocked(serialize)) {
				keys.Append(def->getGrantedProgressKeys());
			}
		}
		return keys;
	}

	MutableEndGameContentDef& create(EEndGameContentType type) {
		return endGameContentDefs.create(type);
	}

	const EndGameContentDef& invalid = create(EEndGameContentType::Invalid);
	const EndGameContentDef& endlessStruggle = create(EEndGameContentType::EndlessStruggle)
		.unlockPredicate([](const UCharacterSerializeComponent* serialize) { return serialize->IsDifficultyCompleted(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle); })
		.unlockRequirementText([]() { return FText::Format(NSLOCTEXT("EndlessStruggle", "EndlessStruggleTier_BaseUnlock", "Complete any mission on {0}"), UDifficultyUtil::getDifficultyThreatLevelDisplayName(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle)); });
	const EndGameContentDef& hypermission = create(EEndGameContentType::HyperMission)
		.grantedProgressKey("nether-cave-unlocked")
		.unlockPredicate([](const UCharacterSerializeComponent* serialize) { return serialize->IsThreatLevelCompleted(EThreatLevel::Threat_5); })
		.unlockRequirementText([]() { return UDifficultyUtil::getThreatCompletedRequirementText(EThreatLevel::Threat_5); });
	const EndGameContentDef& emergentDifficulty = create(EEndGameContentType::EmergentDifficulty)
		.grantedProgressKey("emergent-difficulty-unlocked")
		.unlockPredicate([](const UCharacterSerializeComponent* serialize) { return serialize->IsThreatLevelCompleted(EThreatLevel::Threat_1); })
		.unlockRequirementText([]() { return UDifficultyUtil::getThreatCompletedRequirementText(EThreatLevel::Threat_1); });
	const EndGameContentDef& enchanterMerchant = create(EEndGameContentType::EnchanterMerchant);
	const EndGameContentDef& endPortal = create(EEndGameContentType::EndPortal)
		.unlockPredicate([](const UCharacterSerializeComponent* serialize) { return serialize->HasUsedPortal(); })
		.unlockRequirementText([]() { return  LocTableFromFile::Get("DLC6UIHints.csv", "hint_eyes_of_ender_lobbyMap"); });
}
#undef LOCTEXT_NAMESPACE
