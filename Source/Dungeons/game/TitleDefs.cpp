#include "Dungeons.h"
#include "TitleDefs.h"

FTitleRegistry& GetTitleRegistry() {
	static TIdRegistry<ETitle, const UnlockableTitle> instance;
	return instance;
}

const UnlockableTitle& Create(const ETitle& id, const FText& title, const FText& description) {
	auto ID = GetTitleRegistry().Register(id, MakeUnique<const UnlockableTitle>(id, title, description));
	return GetTitleRegistry().Get(ID);
}

TArray<FBPUnlockableTitle> UTitlesBlueprintLibrary::GetAllTitles() {
	TArray<FBPUnlockableTitle> titles;
	for (uint8 i = 0; i < static_cast<uint8>(ETitle::END); i++) {
		ETitle titleID = static_cast<ETitle>(i);
		TOptional<FTitleId> id = GetTitleRegistry().Request(titleID);
		if (id.IsSet()) {
			FBPUnlockableTitle title(titleID, GetTitle(titleID), GetDescription(titleID));
			titles.Emplace(title);
		}
	}
	return titles;
}

FText UTitlesBlueprintLibrary::GetTitle(ETitle title) {
	TOptional<FTitleId> id = GetTitleRegistry().Request(title);
	if (id.IsSet()) {
		return GetTitleRegistry().Get(id.GetValue()).GetTitle();
	}
	return FText();
}

FText UTitlesBlueprintLibrary::GetDescription(ETitle title) {
	TOptional<FTitleId> id = GetTitleRegistry().Request(title);
	if (id.IsSet()) {
		return GetTitleRegistry().Get(id.GetValue()).GetDescription();
	}
	return FText();
}

UnlockableTitle::UnlockableTitle(const ETitle& id, const FText& title, const FText& description) : mId(id), mTitle(title), mDescription(description) {

}

FBPUnlockableTitle::FBPUnlockableTitle() {

}

FBPUnlockableTitle::FBPUnlockableTitle(const ETitle& id, const FText& title, const FText& description) : mId(id), mTitle(title), mDescription(description) {

}

const ETitle& UnlockableTitle::GetID() const {
	return mId;
}

const FText& UnlockableTitle::GetTitle() const {
	return mTitle;
}

const FText& UnlockableTitle::GetDescription() const {
	return mDescription;
}

#define LOCTEXT_NAMESPACE "UnlockableTitles"
namespace game {
	namespace title {
		const UnlockableTitle& None = Create(ETitle::NONE, LOCTEXT("NoneTitle", "None"), LOCTEXT("NoneDescription", "No title."));
		const UnlockableTitle& Boomer = Create(ETitle::BOOMER, LOCTEXT("BoomerTitle", "Boomer"), LOCTEXT("BoomerDescription", "Defeat 1,000 enemies with TNT."));
		const UnlockableTitle& CreepersBane = Create(ETitle::CREEPERS_BANE, LOCTEXT("CreepersBaneTitle", "Creeper's Bane"), LOCTEXT("CreepersBaneDescription", "Defeat 1,000 Creepers."));
	}
}
#undef LOCTEXT_NAMESPACE