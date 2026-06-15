#include "Dungeons.h"
#include "CloudFileUtil.h"

#if CLOUDSAVE_ENABLED


#if CONSOLE_SAVE_SYSTEM 

#define LOCTEXT_NAMESPACE "Save_Data_Character"

SaveMetaData GetCharacterSaveMetaData(FString Name)
{
	// D11.SSN
	SaveMetaData metaData;
	metaData.saveName = Name;
	metaData.title = FText(LOCTEXT("Save_Character_Title", "Minecraft Dungeons - Character Save")).ToString();
	metaData.subTitle = FString("Loaded from cloud");
	metaData.details = FText(LOCTEXT("Save_Character_Detail", "The save file for one of your characters.")).ToString();
#if PLATFORM_PS4
	metaData.iconPath = TEXT("../../../dungeons/Content/SaveIcons/save_character.png");
#endif
	return metaData;
}
#undef	LOCTEXT_NAMESPACE


class FCloudFileUtilConsole : public FCloudFileUtil
{
protected:


	virtual bool IsLocalFileNameImpl(FString& FileName)
	{
		auto Index = FileName.Find(ConsoleSave::SaveFileAppend);
		if (Index != 0)
			return false;
		FileName = FileName.Mid(ConsoleSave::SaveFileAppend.Len());
		return true;
	}

	virtual SerializedDataPtr LoadLocalFileImpl(FStringArg f)
	{
		auto pData = MakeSerializedDataPtr();
		DungeonsConsoleSave::Instance()->LoadNonASync(f, GetSystemId(), *pData);
		return pData;
	}

	virtual void DeleteLocalFileImpl(FStringArg f)
	{
		DungeonsConsoleSave::Instance()->DeleteAsync(f, GetSystemId());
	}

	virtual TArray<FString> GetLocalFileListImpl()
	{
		TArray<FString> LocalFileList;
		DungeonsConsoleSave::Instance()->ListSaves(GetSystemId(), LocalFileList);
		return LocalFileList;
	}

	virtual void WriteToLocalFileImpl(FStringArg FileName, StringArg json)
	{
		auto metaData = GetCharacterSaveMetaData(FileName);
		DungeonsConsoleSave::Instance()->SaveAsync(metaData, GetSystemId(), json);
	}

	virtual bool AreLocalFileOperationsCompletedImpl() const
	{
		return DungeonsConsoleSave::Instance()->GetNumAsyncTasksInflight() == 0;
	}


};

std::unique_ptr<FCloudFileUtil> FCloudFileUtil::CreateInstance()
{
	return std::make_unique<FCloudFileUtilConsole>();
}
#endif

#endif