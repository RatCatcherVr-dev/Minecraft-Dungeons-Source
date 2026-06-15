#include "Dungeons.h"
#include "CloudArchive.h"
#include "DungeonsCloudLoadSave.h"
#include "CloudFileUtil.h"

#if CLOUDSAVE_ENABLED
static const int FCLOUDARCHIVE_HEADER = 0xFEEDC0DE;
static const int FCLOUDARCHIVE_VERSION = 0x33;

FCloudArchive::FCloudArchive()
{
}

FCloudArchive::~FCloudArchive()
{
}


FCloudFileUtil* FCloudArchive::GetFileUtil()
{
	return FDungeonsCloudLoadSave::GetFileUtil();
}

void FCloudArchive::SimpleEncryptDecrypt(std::string& Data)
{
	unsigned int x = 0x55;
	for (auto& c : Data)
	{
		c ^= char(x);
		x = x * 214013 + 2531011;
	}
}

FString FCloudArchive::ReadString(SerializedDataInput& pInputStream)
{
	ExpectedType(pInputStream, ECloudSerializedDataType::String);
	FString r;
	while (*pInputStream)
		r.AppendChar(*pInputStream++);
	pInputStream++;
	return r;
}

SerializedDataPtr FCloudArchive::ReadData(SerializedDataInput& pInputStream)
{
	int Size = ReadInt(pInputStream);
	ExpectedType(pInputStream, ECloudSerializedDataType::Data);
	auto r = GetFileUtil()->MakeSerializedDataPtr(pInputStream, Size);
	pInputStream += Size;
	return r;
}

void FCloudArchive::Clear()
{
	mFiles.clear();
}

bool FCloudArchive::DoesFileExist(ArchiveIndex n)
{
	return mFiles.find(n) != mFiles.end();
}

void FCloudArchive::AddFile(ArchiveIndex n, const FCloudArchiveFile& CloudFile)
{
	mFiles[n] = CloudFile;
	check(GetNumberOfFiles() <= FCloudArchive::CLOUDARCHIVE_MAXFILES);
}


int FCloudArchive::GetNumberOfFiles() const
{
	return int(mFiles.size());
}

void FCloudArchive::Serialize(SerializedDataOutput& pOutputStream, int n)
{
	check(DoesFileExist(n));
	auto pData = mFiles[n];
	WriteInt(pOutputStream, n);
	writeDateTime(pOutputStream, pData.mDateTime);
	WriteData(pOutputStream, pData.mpData);
}


void FCloudArchive::WriteType(SerializedDataOutput& pOutputStream, ECloudSerializedDataType Type)
{
	*pOutputStream++ = uint8(Type);
}

void FCloudArchive::WriteString(SerializedDataOutput& pOutputStream, FStringArg s)
{
	WriteType(pOutputStream, ECloudSerializedDataType::String);
	for (auto c : s)
		*pOutputStream++ = uint8(c);
	*pOutputStream++ = 0;
}

void FCloudArchive::WriteInt(SerializedDataOutput& pOutputStream, int n)
{
	WriteType(pOutputStream, ECloudSerializedDataType::Int);
	*pOutputStream++ = uint8(n >> 24);
	*pOutputStream++ = uint8(n >> 16);
	*pOutputStream++ = uint8(n >> 8);
	*pOutputStream++ = uint8(n >> 0);
}

void FCloudArchive::WriteData(SerializedDataOutput& pOutputStream, SerializedDataPtr pData)
{
	auto& Data = *pData;
	int Size = Data.Num();
	WriteInt(pOutputStream, Size);
	WriteType(pOutputStream, ECloudSerializedDataType::Data);
	memcpy(pOutputStream, Data.GetData(), Size);
	pOutputStream += Size;
}

void FCloudArchive::writeDateTime(SerializedDataOutput& pOutputStream, const FDateTime& DateTime)
{
	WriteType(pOutputStream, ECloudSerializedDataType::DateTime);
	auto Ticks = DateTime.GetTicks();
	WriteInt(pOutputStream, int(Ticks >> 32));
	WriteInt(pOutputStream, int(Ticks));
}

FDateTime FCloudArchive::ReadDateTime(SerializedDataInput& pInputStream)
{
	ExpectedType(pInputStream, ECloudSerializedDataType::DateTime);
	int64 a = ReadInt(pInputStream);
	int64 b = ReadInt(pInputStream);
	b &= 0xffffffff;
	return FDateTime((a << 32) | b);
}

int FCloudArchive::ReadInt(SerializedDataInput& pInputStream)
{
	ExpectedType(pInputStream, ECloudSerializedDataType::Int);
	int a = *pInputStream++;
	int b = *pInputStream++;
	int c = *pInputStream++;
	int d = *pInputStream++;
	return (a << 24) | (b << 16) | (c << 8) | (d << 0);
}

void FCloudArchive::ExpectedType(SerializedDataInput& pInputStream, ECloudSerializedDataType Expected)
{
	uint8 Type = *pInputStream++;
	check(Type <= uint8(ECloudSerializedDataType::Terminate));
	check(ECloudSerializedDataType(Type) == Expected);
}

SerializedDataPtr FCloudArchive::CompressArchive()
{
	const int ApproxHeaderSize = 1024 * 2;
	int ApproxSize = ApproxHeaderSize;
	for (auto it : mFiles)
		ApproxSize += it.second.mpData->Num();
	ApproxSize += mFiles.size() * ApproxHeaderSize;
	auto pSerialidedData = GetFileUtil()->MakeSerializedDataPtr(ApproxSize);
	auto pSerializedDataOutput = pSerialidedData->GetData();
	auto pBegin = pSerializedDataOutput;
	WriteInt(pSerializedDataOutput, FCLOUDARCHIVE_HEADER);
	WriteInt(pSerializedDataOutput, FCLOUDARCHIVE_VERSION);
	for (auto it : mFiles)
		Serialize(pSerializedDataOutput, it.first);
	WriteInt(pSerializedDataOutput, -1);
	WriteType(pSerializedDataOutput, ECloudSerializedDataType::Terminate);
	auto pEnd = pSerializedDataOutput;
	check(pEnd - pBegin <= ApproxSize);
	std::string CompressedData;
	auto SourceString = std::string((char*)pBegin, (char*)pEnd);
	SimpleEncryptDecrypt(SourceString);
	FDungeonsCloudLoadSave::GetCompressor()->compress(SourceString, CompressedData);
	check(CompressedData.size() != 0);
	return GetFileUtil()->MakeSerializedDataPtr(CompressedData);
}

bool FCloudArchive::DecompressArchive(SerializedDataPtr pInput)
{
	auto r = DecompressArchiveImpl(pInput);
	check(r);
	return r;
}

FCloudArchiveFile& FCloudArchive::GetFile(ArchiveIndex n)
{
	check(DoesFileExist(n));
	return mFiles[n];
}

bool FCloudArchive::DecompressArchiveImpl(SerializedDataPtr pInput)
{
	auto& Input = *pInput;
	if (Input.Num() == 0)
		return false;
	auto pBegin = (char*)Input.GetData();
	auto pEnd = pBegin + Input.Num();
	std::string CompressedData(pBegin, pEnd);
	std::string DecompressedData;
	bool r = FDungeonsCloudLoadSave::GetCompressor()->decompress(CompressedData, DecompressedData);
	if (!r)
		return false;
	SimpleEncryptDecrypt(DecompressedData);
	Clear();
	auto DecompressedSize = DecompressedData.size();
	if (DecompressedSize == 0)
		return false;
	auto InputStream = (const unsigned char*)DecompressedData.data();
	auto InputStreamEnd = InputStream + DecompressedSize;
	auto Header = ReadInt(InputStream);
	auto Version = ReadInt(InputStream);
	check(Header == FCLOUDARCHIVE_HEADER);
	if (Version == FCLOUDARCHIVE_VERSION)
	{
		while (true)
		{
			auto FileIndex = ReadInt(InputStream);
			if (FileIndex < 0)
				break;
			auto DateTime = ReadDateTime(InputStream);
			auto Data = ReadData(InputStream);
			AddFile(FileIndex, FCloudArchiveFile(DateTime, Data));
		}
		ExpectedType(InputStream, ECloudSerializedDataType::Terminate);
		check(InputStream == InputStreamEnd);
	}
	else
		check(false);
	return true;
}
#endif