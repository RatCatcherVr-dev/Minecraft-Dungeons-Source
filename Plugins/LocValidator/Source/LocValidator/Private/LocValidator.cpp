// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "ILocValidator.h"
#include "LocValidatorCommands.h"

#include "Internationalization/TextNamespaceUtil.h"
#include "PortableObjectFormatDOM.h"
#include <regex>
#include <set>
#include <sstream>
#include "FileHelper.h"

#include "Commandlets/Commandlet.h"
#include "Class.h"

#include "MessageDialog.h"
#include "UICommandList.h"
#include "MultiboxExtender.h"
#include "MultiboxBuilder.h"
#include "LevelEditor.h"

class FToolBarBuilder;
class FMenuBuilder;

//# include LocTextHelper.h??
#define LOCTEXT_NAMESPACE "FLocValidator"

namespace {

bool LoadPOFile(const FString& POFilePath, FPortableObjectFormatDOM& OutPortableObject)
{
	if (!FPaths::FileExists(POFilePath))
	{
		UE_LOG(LogLocValidator, Log, TEXT("Could not find file %s"), *POFilePath);
		return false;
	}

	FString POFileContents;
	if (!FFileHelper::LoadFileToString(POFileContents, *POFilePath))
	{
		UE_LOG(LogLocValidator, Error, TEXT("Failed to load file %s."), *POFilePath);
		return false;
	}

	if (!OutPortableObject.FromString(POFileContents))
	{
		UE_LOG(LogLocValidator, Error, TEXT("Failed to parse Portable Object file %s."), *POFilePath);
		return false;
	}

	return true;
}

//
FString ConditionPoStringForArchive(const FString& InStr)
{
	FString Result = InStr;
	Result.ReplaceInline(TEXT("\\t"), TEXT("\t"), ESearchCase::CaseSensitive);
	Result.ReplaceInline(TEXT("\\n"), TEXT("\n"), ESearchCase::CaseSensitive);
	Result.ReplaceInline(TEXT("\\r"), TEXT("\r"), ESearchCase::CaseSensitive);
	Result.ReplaceInline(TEXT("\\\""), TEXT("\""), ESearchCase::CaseSensitive);
	Result.ReplaceInline(TEXT("\\\\"), TEXT("\\"), ESearchCase::CaseSensitive);
	return Result;
}

void ParsePOMsgCtxtForIdentity(const FString& MsgCtxt, FString& OutNamespace, FString& OutKey)
{
	auto UnescapeMsgCtxtParticleInline = [](FString& InStr)
	{
		InStr.ReplaceInline(TEXT("\\,"), TEXT(","), ESearchCase::CaseSensitive);
	};

	const FString ConditionedMsgCtxt = ConditionPoStringForArchive(MsgCtxt);

	// Find the unescaped comma that defines the breaking point between the namespace and the key
	int32 CommaIndex = INDEX_NONE;
	{
		bool bIsEscaped = false;
		for (int32 Index = 0; Index < ConditionedMsgCtxt.Len(); ++Index)
		{
			if (bIsEscaped)
			{
				// No longer escaped, and skip this character
				bIsEscaped = false;
				continue;
			}

			if (ConditionedMsgCtxt[Index] == TEXT(','))
			{
				// Found the unescaped comma
				CommaIndex = Index;
				break;
			}

			if (ConditionedMsgCtxt[Index] == TEXT('\\'))
			{
				// Next character will be escaped
				bIsEscaped = true;
				continue;
			}
		}
	}

	if (CommaIndex == INDEX_NONE)
	{
		OutNamespace = ConditionedMsgCtxt;
		OutKey.Reset();
	}
	else
	{
		OutNamespace = ConditionedMsgCtxt.Mid(0, CommaIndex);
		OutKey = ConditionedMsgCtxt.Mid(CommaIndex + 1);
	}

	UnescapeMsgCtxtParticleInline(OutNamespace);
	UnescapeMsgCtxtParticleInline(OutKey);
}

//
//// get the text location 
////
void extractLocTextSource(const TSharedPtr<FPortableObjectEntry>& POEntry, FString& assetfile, FString& info)
{
	std::string source; // = POEntry->ExtractedComments[1];
	FString sourcestr;
	for (auto comment : POEntry->ExtractedComments)
	{
		if (comment.Contains("SourceLocation"))
		{
			source = std::string(TCHAR_TO_UTF8(*comment));
			sourcestr = comment;
		}
	}

	std::regex infoRegex(":[\\\\t\\s]?([\\w\\.\\/]+):(.+)$");
	std::smatch matches;

	if (std::regex_search(source, matches, infoRegex))
	{
		if (matches.size() >= 2) {
			assetfile = FString(matches[1].str().c_str());
			info = FString(matches[2].str().c_str());
		}
	};

}

bool UpdateManifestPaths()
{
	FString configfile = FPaths::ProjectConfigDir() + "Localization/Game_Gather.ini";
	if (!FPaths::FileExists(configfile))
	{
		UE_LOG(LogLocValidator, Log, TEXT("Could not find file %s"), *configfile);
		return false;
	}

	FString fileContent;
	if (!FFileHelper::LoadFileToString(fileContent, *configfile))
	{
		UE_LOG(LogLocValidator, Error, TEXT("Failed to load file %s."), *configfile);
		return false;
	}
	static const TCHAR* NewLineDelimiter = TEXT("\n");
	fileContent.ReplaceInline(TEXT("\r\n"), NewLineDelimiter, ESearchCase::CaseSensitive);

	TArray<FString> LinesToProcess;
	fileContent.ParseIntoArray(LinesToProcess, NewLineDelimiter, false);

	std::stringstream ss;
	std::smatch matches;
	const char* engineLocation = TCHAR_TO_UTF8(*FPaths::ConvertRelativePathToFull(FPaths::EngineDir()));
	int updates = 0;
	std::regex mainf_path("(ManifestDependencies=)(.+/Engine/)(Content/Localization.+)$");

	for (auto FSLine: LinesToProcess)
	{
		std::string line = TCHAR_TO_UTF8(*FSLine);
		if (std::regex_search(line, matches, mainf_path))
		{
			//compare to current file and set flag
			if (matches.size() >= 2 && matches[2] != engineLocation)
			{
				ss << matches[1] << engineLocation << matches[3] << std::endl;
				updates += 1;
			}
		}
		else
			ss << line << std::endl;
	}

	if (updates > 0)
	{
		FFileHelper::SaveStringToFile(FString(ss.str().c_str()), *configfile, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	return true; // Manifest file OK
}

} // ( anonymous ) namespace
 
namespace locvalidator {
	bool inWhitelist(const std::string& line) {
		// check line against known broken UMG false negatives for TEST
//## key for ,'Progress', at,'/Game/UI/MissionSelectMap/Marker/UMG_MissionMarker.ApplyOutstandingCorrection', K2Node_FunctionEntry_169.MetaData.Category
//## key for ,'SalvageX', at,'/Game/UI/Inventory/Salvage/UMG_SalvageButtonToggle.UMG_SalvageButtonToggle_C', WidgetTree.UMG_Button.text
//## key for ,'Hooks', at,'/Game/UI/Hotbar2/UMG_HotbarSlotGeneric.OnItemInstanceChanged', K2Node_FunctionEntry_12.MetaData.Category
//## key for ,'Private', at,'/Game/UI/MissionSelectMap/Marker/UMG_MissionMarker.UpdateGraphics', K2Node_FunctionEntry_1.MetaData.Category
//## key for ,'Everyone', at,'/Game/UI/Menu/SettingsMenu/UMG_AdvancedGraphics.UMG_AdvancedGraphics_C', WidgetTree.vSync.SelectionOptions(1).SelectionOptions
//## key for ,'No One', at,'/Game/UI/Menu/SettingsMenu/UMG_AdvancedGraphics.UMG_AdvancedGraphics_C', WidgetTree.vSync.SelectionOptions(2).SelectionOptions
//## key for ,'Friends Only', at,'/Game/UI/Menu/SettingsMenu/UMG_AdvancedGraphics.UMG_AdvancedGraphics_C', WidgetTree.vSync.SelectionOptions(0).SelectionOptions
//## #. Key:	1DFA61554D95DCE782DB1FB9DF55E6FC

	static const std::set<std::string> whitelisted = {
		"1DFA61554D95DCE782DB1FB9DF55E6FC"
		,"340F730A4FF0C696AFAA6A9F5B606FF7"
		,"4D934F2B4AC62B15C4A2E1AD911948E7"
		,"E0A0D9354A0ACD79AE06F2B3202AD035"
		,"E385C65444C2796BAB1AD2A79302B3AC"
		,"ED70D87D4EEF5F7F7202CAB1822A4C00"
		,"F776589442EB369BDB0810844EF391BD"
	};
	return whitelisted.find(line) != whitelisted.end();
	}
}

DEFINE_LOG_CATEGORY(LogLocValidator);

class FLocValidator : public ILocValidator
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void HelpActionClicked();
	void RunLocStringCheck();
private:
	TSharedPtr<class FUICommandList> PluginCommands;

	void AddMenuBarExtension(FMenuBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

};

IMPLEMENT_MODULE( FLocValidator, LocValidator )

void FLocValidator::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(LogLocValidator, Log, TEXT("LocValidator Module 'StartupModule'"));

	FLocValidatorCommands::Register();


	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLocValidatorCommands::Get().LocValidateAction,
		FExecuteAction::CreateRaw(this, &FLocValidator::RunLocStringCheck),
		FCanExecuteAction());
	PluginCommands->MapAction(
		FLocValidatorCommands::Get().HelpAction,
		FExecuteAction::CreateRaw(this, &FLocValidator::HelpActionClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, PluginCommands, FMenuBarExtensionDelegate::CreateRaw(this, &FLocValidator::AddMenuBarExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

}

void FLocValidator::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


void FLocValidator::AddMenuBarExtension(FMenuBarBuilder& Builder)
{
	Builder.AddPullDownMenu(
		LOCTEXT("LocValidatorMenu", "Localization"),
		LOCTEXT("LocValidatorMenu_Tooltip", "Localization tools plugin"),
		FNewMenuDelegate::CreateRaw(this, &FLocValidator::AddMenuExtension),
		"Localization");
}

void FLocValidator::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection("ValidatorTools", LOCTEXT("LocValidatorMenu_Tools", "Tools"));

	//Builder.AddMenuEntry(FLocValidatorCommands::Get().MarketplaceAction);
	Builder.AddMenuEntry(FLocValidatorCommands::Get().LocValidateAction);
	Builder.AddMenuEntry(FLocValidatorCommands::Get().HelpAction);
	Builder.EndSection();

}

void FLocValidator::RunLocStringCheck()
{
	// NEED: getting an up to date version of all text labels (in Data files) e.g. run gatherText macro...
	// TODO: allow for checking all locales, from e.g. settings file or USER CHOICE (Manifest)
	// todo: read filename from Game_Export.ini / PortableObjectName
	UE_LOG(LogLocValidator, Log, TEXT("Running LocValidator to check for Bad Strings (using default HASH ids)"));
	FString fn = "Game.po";

	FString basePath = "Content/Localization/Game/";
	if (true) { // use SE locale
		basePath += "sv-SE/";
	}

	FString filename = FPaths::ProjectDir() + basePath + fn;

	// Patch the Game_Gather.ini with correct engine path for Manifests
	int errorlevel = 0;
	if (UpdateManifestPaths())
	{
		// Set up Commandlet from Plugin code
		FString classname = "GatherTextCommandlet";
		UClass* CommandletClass = FindObject<UClass>(ANY_PACKAGE, *classname, false);

		//FString configFile = "-Config=Config/Localization/Game_Gather.ini";
		UCommandlet* Commandlet = NewObject<UCommandlet>(GetTransientPackage(), CommandletClass);
		check(Commandlet);
		Commandlet->AddToRoot();
		///Commandlet->ParseParms(CommandletCommandLine);

		// Run Commandlet 'Main' to scrape Strings
		errorlevel += Commandlet->Main("-Config=Config/Localization/Game_Gather.ini");

		// Run the Export script to create the 'new' Game.po
		errorlevel += Commandlet->Main("-Config=config/localization/Game_Export.ini");
	}

	// Logic stolen from Engine - PortableObjectPipeline 
	FPortableObjectFormatDOM poObject;
	if (LoadPOFile(filename, poObject))
	{
		int failCount = 0;
		TArray<FString> failStringSet;

		for (auto EntryPairIter = poObject.GetEntriesIterator(); EntryPairIter; ++EntryPairIter)
		{
			auto POEntry = EntryPairIter->Value;
			//// MsgStr ~ the translated string...
			if (POEntry->MsgId.IsEmpty()) // || POEntry->MsgStr.Num() == 0 || POEntry->MsgStr[0].IsEmpty())
			{
				// We ignore the header entry or entries with no translation.
				continue;
			}

			const FString SourceText = ConditionPoStringForArchive(POEntry->MsgId);
			const FString Translation = ConditionPoStringForArchive(POEntry->MsgStr[0]);

			FString ParsedNamespace;
			FString ParsedKey;
			ParsePOMsgCtxtForIdentity(POEntry->MsgCtxt, ParsedNamespace, ParsedKey);

			if (!ParsedKey.IsEmpty())
			{
				const std::regex hashCode("\\,?[A-F0-9]{10,32}$");
				std::string parsed_key_string = std::string(TCHAR_TO_UTF8(*ParsedKey));
				if (std::regex_match(parsed_key_string, hashCode)
					&& !locvalidator::inWhitelist(parsed_key_string))
				{
					// print
					failCount += 1;
					FString info;
					FString assetfile;
					extractLocTextSource(POEntry, assetfile, info);

					failStringSet.Add(TEXT("## key for ,'" + SourceText + "', at,'" + assetfile + "', "+info));
				}
			}
			//else
			//{
			//	// Legacy non-keyed PO entry - need to look-up the expanded namespace key/pairs via the namespace and source string
			//}

		}

		FString message = "All OK.";
		FString runWarning = " ## WARNING: Couldn't run TextExport scripts - results could be from an old .po-file";
		UE_LOG(LogLocValidator, Log, TEXT("FOUND '%d' loc FText:s with HASH keys"), failCount);

		if (failCount > 0)
		{
			message = "Found User Facing FText:s in .po with BAD keys:\n"; // @attn: concat in loop etc
			for (auto& errKey : failStringSet) {
				message += errKey + "\n";
			}
		}
		
		FString titleString = (failCount > 0)? "Loctext Search FAILED!": "Loctext Search";
		if (errorlevel != 0)
			titleString += runWarning;
		FText title(FText::FromString(titleString));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(message), &title);
	}


}

void FLocValidator::HelpActionClicked()
{
	FPlatformProcess::LaunchURL(TEXT("https://github.com/Mojang/Dungeons/blob/master/BuildTools/loc_README.md"), NULL, NULL);
}

#undef LOCTEXT_NAMESPACE
