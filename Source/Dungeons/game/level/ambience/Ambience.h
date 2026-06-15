#pragma once

#include <UnrealString.h>
#include "Ambience.generated.h"

UENUM(BlueprintType)
enum class EAmbienceID : uint8 {
	AMBIENCE_DEFAULT,
	AMBIENCE_ALPHA,
	AMBIENCE_BRAVO,
	AMBIENCE_DELTA,
	AMBIENCE_ECHO,
	AMBIENCE_FOXTROT,
};
ENUM_NAME(EAmbienceID);

UENUM(BlueprintType)
enum class EAmbienceAudioID : uint8 {
	AUDIO_FROM_AMBIENCE,
	AUDIO_1,
	AUDIO_2,
	AUDIO_3,
	AUDIO_4,
	AUDIO_5,
	AUDIO_6,
	AUDIO_7,
	AUDIO_8,
	AUDIO_9,
	AUDIO_10,
};
ENUM_NAME(EAmbienceAudioID);

template <typename AmbienceId>
struct FAmbienceIDGroupBase {
	FAmbienceIDGroupBase(AmbienceId id, FString groupName)
		: Id(id)
		, GroupName(std::move(groupName)) {}

	AmbienceId Id;
	FString GroupName;

	bool operator==(const FAmbienceIDGroupBase& rhs) const { return Id == rhs.Id && GroupName == rhs.GroupName; }
	bool operator!=(const FAmbienceIDGroupBase& rhs) const { return !(*this == rhs); }
};

struct FAmbienceIDGroup : public FAmbienceIDGroupBase<EAmbienceID> {
	using FAmbienceIDGroupBase::FAmbienceIDGroupBase;
	FAmbienceIDGroup() : FAmbienceIDGroupBase(EAmbienceID::AMBIENCE_DEFAULT, "") {}
};
struct FAmbienceAudioIDGroup: public FAmbienceIDGroupBase<EAmbienceAudioID> {
	using FAmbienceIDGroupBase::FAmbienceIDGroupBase;
	FAmbienceAudioIDGroup() : FAmbienceIDGroupBase(EAmbienceAudioID::AUDIO_FROM_AMBIENCE, "") {}
};
