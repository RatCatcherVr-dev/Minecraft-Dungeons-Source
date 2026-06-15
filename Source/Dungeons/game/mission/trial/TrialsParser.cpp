#include "Dungeons.h"
#include "TrialsParser.h"
#include "util/StringUtil.h"

DEFINE_LOG_CATEGORY(LogTrials)

using namespace minecraft::api;

namespace trial {


	//We have a valid none value, but that does not map to our enum currently so we have to do this...
	TOptional<EMissionTheme> parseMissionTheme(const string& inTheme) {
		FString theme = stringutil::toFString(inTheme);

		if (auto enumvalue = EnumValueFromString(EMissionTheme, theme)) {
			return enumvalue;
		}

		if (theme.Equals("None", ESearchCase::IgnoreCase)) {
			return EMissionTheme::Invalid;
		}

		return TOptional<EMissionTheme>();
	}

	ELevelNames parseLevel(string mission, const string& suffix) {
		if (!Util::endsWith(mission, suffix)) {
			mission.append(suffix);
		}
		return level::fromString(stringutil::toFString(mission));
	}

	EExtraChallenge parseExtraChallenge(const shared_ptr<int> challengeAmount) {
		return extrachallengequery::getExtraChallengeFromAmount(*challengeAmount);
	}

	FReplicatableMap parseModifiers(const shared_ptr<vector<Rule>>& modifierRule) {
		FReplicatableMap modifiers;
		if (modifierRule) {
			for (auto& it : *modifierRule) {
				if (it.properties) {
					for (std::pair<std::string, std::string> ruleProperty : *it.properties) {
						modifiers.Add(stringutil::toFString(ruleProperty.first), stringutil::toFString(ruleProperty.second));
					}
				}
			}
		}		
		return modifiers;
	}

	TArray<FReplicatableMap> parseRewards(const shared_ptr<vector<Rule>>& rewardRule) {
		TArray<FReplicatableMap> rewards;
		if (rewardRule) {
			for (auto& it : *rewardRule) {				
				if (it.properties) {
					TMap<FString, FString> rewardProperties;
					for (std::pair<std::string, std::string> ruleProperty : *it.properties) {
						rewardProperties.Add(stringutil::toFString(ruleProperty.first), stringutil::toFString(ruleProperty.second));
					}
					rewards.Add(FReplicatableMap(rewardProperties));
				}
			}
		}		
		return rewards;
	}

	TOptional<FReplicatableTrialDef> parseTrial(Trial& trial) {
		if (trial.mission && trial.type) {
			const auto level = parseLevel(*trial.mission, *trial.type);
			if (level != ELevelNames::Invalid) {
				FReplicatableTrialDef trialDef {};

				trialDef.id = stringutil::toFString(*trial.id);
				trialDef.level = level;
				trialDef.type = stringutil::toFString(*trial.type);
				
				if (trial.theme) {
					if (auto parsedTheme = parseMissionTheme(*trial.theme)) {
						trialDef.Theme = parsedTheme.GetValue();
					}
					else {
						UE_LOG(LogTrials, Warning, TEXT("Unrecognized theme %s defined for trial: %s."), *stringutil::toFString(*trial.theme), *trialDef.id);
						return TOptional<FReplicatableTrialDef>();
					}
				}
				else {
					trialDef.Theme = EMissionTheme::Invalid;
				}


				trialDef.extraChallenge = parseExtraChallenge(trial.extraChallenge);

				if (trial.rules) {
					trialDef.rewards = parseRewards(trial.rules->rewards);
					trialDef.modifiers = parseModifiers(trial.rules->modifiers);
				}				
				return trialDef;
			}
			
			UE_LOG(LogTrials, Warning, TEXT("Invalid mission defined for trial: %s."), *stringutil::toFString(*trial.mission));
		} else {
			UE_LOG(LogTrials, Warning, TEXT("No mission defined for trial."));
		}

		return TOptional<FReplicatableTrialDef>();
	}

	TArray<Trial> findTrials(const shared_ptr<vector<Trial>>& inTrials) {
		TArray<Trial> trials;
		for (const auto& trial : *inTrials) {
			trials.Add(trial);
		}
		return trials;
	}

	TArray<FReplicatableTrialDef> parseDailyTrials(HttpServiceResponse<TrialsApiResponse>& response) {
		TArray<FReplicatableTrialDef> parsedDailyTrials;
		
		if (const auto trialsResponse = response.getBody()) {
			if (trialsResponse->trials) {
				const auto dailyTrials = findTrials(trialsResponse->trials);
				for (auto dailyTrial : dailyTrials) {
					const auto trialDef = parseTrial(dailyTrial);
					if (trialDef.IsSet()) {
						parsedDailyTrials.Add(trialDef.GetValue());
					}
				}
			} else {
				UE_LOG(LogTrials, Warning, TEXT("Backend response didn't contain any data for daily trials."));
			}
		}

		return parsedDailyTrials;
	}
}
