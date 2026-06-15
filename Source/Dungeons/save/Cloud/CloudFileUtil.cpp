#include "Dungeons.h"
#include "CloudFileUtil.h"


#if CLOUDSAVE_ENABLED

FCloudFileUtil::FCloudFileUtil()
{
}

FCloudFileUtil::~FCloudFileUtil()
{
}

void FCloudFileUtil::PostProccessFileList(TArray<FString>& saveFiles)
{
	USERCLOUD_LOGFUNCTION;
	std::set<FString> Dupes;
	for (auto& f : saveFiles)
		Dupes.insert(f);
	saveFiles.Reset();
	for (auto& f : Dupes)
		saveFiles.Add(f);
}

void FCloudFileUtil::SetFilePath(SystemId Id, FStringArg Path)
{
	mCurrentSystemId = Id;
	mCurrentPath = Path;
}

bool FCloudFileUtil::IsHexString(FStringArg FileName)
{
	for (auto c : FileName)
		if (!isxdigit(c))
			return false;
	return true;
}

SystemId FCloudFileUtil::GetSystemId() const
{
	return mCurrentSystemId;
}

void FCloudFileUtil::WriteToLocalFile(FStringArg _FileName, SerializedDataPtr pContents)
{
	USERCLOUD_LOGUSERFUNCTION;
	FString FileName = _FileName;
	check(IsLocalFileName(FileName));
	auto& Contents = *pContents;
	std::string json((const char*)Contents.GetData(), Contents.Num());
	WriteToLocalFileImpl(FileName, json);

}

TArray<FString> FCloudFileUtil::GetLocalFileList()
{
	TArray<FString> LocalFileList, r;
	LocalFileList = GetLocalFileListImpl();
	r.Reserve(LocalFileList.Num());
	for (auto& FileName : LocalFileList)
	{
		if (IsLocalFileName(FileName))
			r.Add(FileName);
	}
	PostProccessFileList(r);
	return r;
}

void FCloudFileUtil::DeleteLocalFile(FStringArg f)
{
	USERCLOUD_LOGUSERFUNCTION;
	check(IsLocalFileName(f));
	DeleteLocalFileImpl(f);
}

SerializedDataPtr FCloudFileUtil::LoadLocalFile(FStringArg f)
{
	USERCLOUD_LOGUSERFUNCTION;
	check(IsLocalFileName(f));
	return LoadLocalFileImpl(f);
}



bool FCloudFileUtil::IsLocalFileName(FString FileName)
{
	if (IsLocalFileNameImpl(FileName) == false)
		return false;
	return IsHexString(FileName);
}

SerializedDataPtr FCloudFileUtil::MakeSerializedDataPtr(SerializedDataInput pBegin, int Size)
{
	return  std::make_shared<SerializedData>(pBegin, Size);
}

SerializedDataPtr FCloudFileUtil::MakeSerializedDataPtr(int Size)
{
	auto r = std::make_shared<SerializedData>();
	r->Reset(Size);
	return  r;
}

SerializedDataPtr FCloudFileUtil::MakeSerializedDataPtr(const TArray<uint8>& Contents)
{
	return std::make_shared<SerializedData>(Contents);
}

SerializedDataPtr FCloudFileUtil::MakeSerializedDataPtr()
{
	return std::make_shared<SerializedData>();
}

SerializedDataPtr FCloudFileUtil::MakeSerializedDataPtr(const std::string& Data)
{
	return  std::make_shared<SerializedData>((const uint8*)Data.data(), Data.length());
}

bool FCloudFileUtil::AreLocalFileOperationsCompleted() const
{
	return AreLocalFileOperationsCompletedImpl();
}

FString FCloudFileUtil::GetPath()
{
	return mCurrentPath;
}

#endif