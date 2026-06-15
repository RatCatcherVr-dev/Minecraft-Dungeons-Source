#pragma once

#include "DungeonsCloudLoadSave.h"

#if CLOUDSAVE_ENABLED

typedef int32 ArchiveIndex;

struct FCloudArchiveFile
{
	FCloudArchiveFile(const FDateTime& DateTime, SerializedDataPtr pData)
	{
		mDateTime = DateTime;
		mpData = pData;
	}
	FCloudArchiveFile()
	{
	}
	FDateTime			mDateTime;
	SerializedDataPtr	mpData;
};

typedef std::map<int, FCloudArchiveFile> FCloudFileMap;

class FCloudArchive
{
public:
	static const int CLOUDARCHIVE_MAXFILES = 20;
	static const int ARCHIVEINDEX_INVALID = -1;

	FCloudArchive();
	~FCloudArchive();
	void Clear();
	void AddFile(ArchiveIndex n, const FCloudArchiveFile& CloudFile);
	FCloudArchiveFile& GetFile(ArchiveIndex ArchiveIndex);
	bool DoesFileExist(ArchiveIndex ArchiveIndex);
	SerializedDataPtr CompressArchive();
	bool DecompressArchive(SerializedDataPtr Output);
	int GetNumberOfFiles() const;
protected:
	enum ECloudSerializedDataType
	{
		Int = 0xbb,
		String,
		DateTime,
		Data,
		Terminate
	};

	static FCloudFileUtil* GetFileUtil();
	bool DecompressArchiveImpl(SerializedDataPtr Output);
	static void SimpleEncryptDecrypt(std::string& Data);

	void Serialize(SerializedDataOutput& pOutputStream, int FileIndex);
	static void WriteString(SerializedDataOutput& pOutputSerializedData, FStringArg s);
	static void WriteInt(SerializedDataOutput& pOutputSerializedData, int n);
	static void WriteData(SerializedDataOutput& pOutputSerializedData, SerializedDataPtr pData);
	static void writeDateTime(SerializedDataOutput& pOutputSerializedData, const FDateTime& DateTime);
	static void WriteType(SerializedDataOutput& pOutputSerializedData, ECloudSerializedDataType Type);

	static FString ReadString(SerializedDataInput& pInputStream);
	static SerializedDataPtr ReadData(SerializedDataInput& pInputStream);
	static int ReadInt(SerializedDataInput& pInputStream);
	static FDateTime ReadDateTime(SerializedDataInput& pInputStream);
	static void ExpectedType(SerializedDataInput& pInputStream, ECloudSerializedDataType Expected);
	FCloudFileMap mFiles;
};
#endif