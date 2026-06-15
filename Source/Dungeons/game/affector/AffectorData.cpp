#include "Dungeons.h"
#include "AffectorData.h"
#include "AffectorTypes.h"
#include "game/item/ItemTypeDefs.h"
#include "util/EnumUtil.h"

// D11.DB - This is a bit hacky as it would seem thatFCString::ToBool is not 
//			safe to use with this as there are some static initialisation order dependencies.
bool ToBool(const WIDECHAR* String)
{
	if (
			FCStringWide::Stricmp(String, TEXT("True"))==0
		||	FCStringWide::Stricmp(String, TEXT("Yes"))==0
		||	FCStringWide::Stricmp(String, TEXT("On"))==0
		)
	{
		return true;
	}
	else if(
			FCStringWide::Stricmp(String, TEXT("False"))==0
		||	FCStringWide::Stricmp(String, TEXT("No"))==0
		||	FCStringWide::Stricmp(String, TEXT("Off"))==0
		)
	{
		return false;
	}
	else
	{
		return FCStringWide::Atoi(String) ? true : false;
	}
}

bool ToBool(const ANSICHAR* String)
{
	return ToBool( ANSI_TO_TCHAR(String) );
}

//
// AffectorData - Temporary data class for a rule
//
FAffectorData::FAffectorData(FString data)
	: Data(std::move(data))
	, mInt(FCString::Atoi(*Data))
	, mFloat(FCString::Atof(*Data))
	, mBool(ToBool(*Data)) {
}

FAffectorData::FAffectorData(int value) : FAffectorData(FString::FromInt(value)) {}
FAffectorData::FAffectorData(float value) : FAffectorData(FString::SanitizeFloat(value, 0)) {}
FAffectorData::FAffectorData(const char* value) : FAffectorData(FString(value)) {}
FAffectorData::FAffectorData(bool value) : FAffectorData(FString(value ? "1" : "0")) {}

FAffectorData& FAffectorData::SetDefault() {
	ApplicationType = EAffectorApplicationType::DefaultApplication;
	return *this;
}

FAffectorData& FAffectorData::SetApplicationType(EAffectorApplicationType newApplicationType) {
	ApplicationType = newApplicationType;
	return *this;
}

int FAffectorData::AsInt() const {
	return mInt;
}

float FAffectorData::AsFloat() const {
	return mFloat;
}

bool FAffectorData::AsBoolean() const {
	return mBool;
}

const FString& FAffectorData::AsString() const {
	return Data;
}

TOptional<FItemId> FAffectorData::AsItemType() const {
	return GetItemRegistry().Request(*Data);
}

bool FAffectorData::IsDefault() const {
	return ApplicationType == EAffectorApplicationType::DefaultApplication;
}


bool FAffectorData::IsMidGame() const {
	return ApplicationType == EAffectorApplicationType::MidGame;
}

EAffectorApplicationType FAffectorData::GetApplicationType() const {
	return ApplicationType;
}


namespace affector {
//
// RuleData - Temporary data class for a set of rules
//
RuleData::RuleData(EAffectorId key, FAffectorData data) {
	Set(key, std::move(data));
}

RuleData::RuleData(TMap<EAffectorId, FAffectorData> data)
	: Data(std::move(data)) {
}

bool RuleData::Has(EAffectorId key) const {
	return Data.Contains(key);
}

const FAffectorData& RuleData::Get(EAffectorId key) const {
	return *Data.Find(key);
}

RuleData& RuleData::Set(EAffectorId key, FAffectorData data) {
	Data.Add(key, std::move(data));
	return *this;
}

}