#include "Dungeons.h"
#include "BinarySaveData.h"
#include "util/StringUtil.h"
#include <AES.h>
#include <LogVerbosity.h>
#include <Themida/Anticheat.hpp>
#include <ostream>
#include <sstream>

namespace binarysavedata { namespace internal {

//
// Header
//
struct Header {
	struct Sizes {
		static const int Id = 4;
		static const int Unused = 4;

		static const int FullHeader = Id + Unused;
	};

	Header()
		: mData(Sizes::FullHeader, '\0') {
	}
	Header(const char*) = delete; // Deleted due to zeros (e.g. "ab\0cd") not being handled
	Header(const std::string& header) : Header() {
		setId(header);
	}

	std::string id() const {
		return mData.substr(0, 4);
	}

	void setId(const std::string& id) {
		mData.replace(0, Sizes::Id, id);
	}

	std::string fullHeader() const {
		return mData;
	}
private:
	std::string mData;
};


const Header CurrentHeader(std::string{ "D001" });

//
// Encryption keys
//

ANTICHEAT_NO_OPTIMIZATION_BEGIN

const std::string& CurrentEncryptionKey() {
	static const std::string key = [] {
		ANTICHEAT_PROTECT_STRINGS_BEGIN;
		auto encryptionKey = Util::base64_decode("XOudCuu5WsAnCwr2dT38DuPmi7aUeQIPJDDi6gAr1Mk=");
		ensure(encryptionKey.size() >= FAES::FAESKey::KeySize);
		ANTICHEAT_PROTECT_STRINGS_END;
		return encryptionKey;
	}();
	return key;
}
ANTICHEAT_NO_OPTIMIZATION_END

//
// Helpers
//
int calculateBlockPadCount(int length) {
	if (const auto blockModulo = length % FAES::AESBlockSize) {
		return FAES::AESBlockSize - blockModulo;
	}
	return 0;
}

//
// Binary to Ascii
//
TOptional<std::string> binaryToJsonString(const uint8* data, int size) {
	const int messageSize = size - Header::Sizes::FullHeader; // message/payload/data-after-removing-header size

	if (messageSize < 0) {
		return {};
	}
	if (messageSize % FAES::AESBlockSize != 0) {
		return {};
	}

	// Use explicit sizes due to const char* ctor stopping at first '\0'
	const Header header(std::string{ data, data + Header::Sizes::FullHeader });

	if (header.id() == CurrentHeader.id()) {
		std::string out(data + Header::Sizes::FullHeader, data + size);

		FAES::DecryptData(
			reinterpret_cast<uint8*>(&out[0]),
			out.length(),
			reinterpret_cast<const uint8*>(&CurrentEncryptionKey()[0]),
			CurrentEncryptionKey().size()
		);
		return out;
	}

	checkNoEntry();
	return {};
}

TOptional<std::string> binaryToJsonString(const std::string& binary) {
	return binaryToJsonString(reinterpret_cast<const uint8*>(binary.data()), binary.size());
}

bool writeToBinaryFile(const std::string& data, const FString& filename) {
	std::ofstream out;

	const auto bufferSize = std::min(data.size(), static_cast<std::string::size_type>(65536));
	const auto buffer = std::unique_ptr<char[]>{ new char[bufferSize] };
	out.rdbuf()->pubsetbuf(buffer.get(), bufferSize);

#if PLATFORM_WINDOWS 
	// PC path is already UTF8, toStdString would read it again as TCHAR
	out.open(*filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
#else
	// @hack @attn at time of writing Console save isnt using this save system, but compiles it anyway. 
	// *filename returns a TCHAR of type wchar which switch doesnt accept as argument for ofstream.open
	out.open(stringutil::toStdString(filename), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
#endif

	if (out) {
		out.write(data.c_str(), data.size());
		out.close();
		return !out.fail();
	}
	return false;
}

//
// Ascii to Binary. @note: The "json" in the function name implies that we
//                         should only write json with this (the real condition
//                         is that the format supports trailing white space)
//
std::string jsonStringToBinary(const std::string& text) {
	const int padCount = calculateBlockPadCount(text.length());

	std::string out;
	out.reserve(text.length() + Header::Sizes::FullHeader + padCount);
	out.append(CurrentHeader.fullHeader());
	out.append(text);
	out.append(padCount, ' ');

	const int dataStart = Header::Sizes::FullHeader;
	const int dataSize = out.size() - dataStart;

	FAES::EncryptData(
		reinterpret_cast<uint8*>(&out[dataStart]),
		dataSize,
		reinterpret_cast<const uint8*>(&CurrentEncryptionKey()[0]),
		CurrentEncryptionKey().size()
	);
	return out;
}

std::string jsonToBinary(const Json::Value& node) {
	std::ostringstream jsonStringStream;
	Json::StyledStreamWriter writer(" ");
	writer.write(jsonStringStream, node);

	return jsonStringToBinary(jsonStringStream.str());
}

}}

namespace binarysavedata {


TOptional<Json::Value> loadBinaryFileToJson(const FString& filename) {
	TArray<uint8> fileContents;
	if (!FFileHelper::LoadFileToArray(fileContents, *filename)) {
		return {};
	}

	if (const auto maybeJsonData = internal::binaryToJsonString(fileContents.GetData(), fileContents.Num())) {
		Json::Value node;
		Json::Reader reader;
		if (maybeJsonData && reader.parse(maybeJsonData.GetValue(), node)) {
			return node;
		}
	}
	return {};
}

bool writeJsonToBinaryFile(const Json::Value& node, const FString& filename) {
	return internal::writeToBinaryFile(internal::jsonToBinary(node), filename);
}

}

namespace binarysavedata { namespace safer {


TArray<uint8> loadJsonStringAndDecryptSafer(const FString& filename) {
	TArray<uint8> fileContents;
	if (!FFileHelper::LoadFileToArray(fileContents, *filename)) {
		return {};
	}
	if (const auto maybeJsonData = internal::binaryToJsonString(fileContents.GetData(), fileContents.Num())) {
		auto s = maybeJsonData.GetValue();
		return TArray<uint8>((uint8*)s.data() , s.length() );
	}
	return {};
}

FString getBackupFilename(const FString& filename) {
	static const FString BackupFileExtension = ".bak";
	return filename + BackupFileExtension;
}

FString getTempFilename(const FString& filename) {
	static const FString TempFileExtension = ".tmp";
	return filename + TempFileExtension;
}

TOptional<Json::Value> loadBinaryFileToJsonSafer(const FString& filename) {
	if (auto regular = loadBinaryFileToJson(filename)) {
		return regular;
	}
	return {};
}

void logMoveBaseBackupResults(SafeWriteResult result, const FString& filename) {
	const FString logMessage = [&] {
		const auto toDescription = [&](WriteResult r) -> FString {
			return r == WriteResult::Saved ? "saved successfully" : "failed saving";
		};
		return FString::Printf(TEXT("SaveFile: %s %s."),
			*FPaths::GetCleanFilename(filename),
			*toDescription(result.regular));
	}();

	if (result.anyWriteSucceeded()) {
		UE_LOG(LogDungeons, Log, TEXT("%s"), *logMessage)
	} else {
		UE_LOG(LogDungeons, Error, TEXT("%s"), *logMessage)
	}
}

bool moveFileWithReplace(const FString& from, const FString& to, bool log) {
#if !PLATFORM_WINDOWS
	return false;
#else
	const bool success = MoveFileExW(*from, *to, MOVEFILE_REPLACE_EXISTING) != 0;
	if (!success && log) {
		const DWORD error = GetLastError();
		UE_LOG(LogDungeons, Error, TEXT("SaveFile: Could not move file '%s' -> '%s' Error: %d"), *FPaths::GetCleanFilename(from), *FPaths::GetCleanFilename(to), error);
	}
	return success;
#endif
}


std::string jsonStringToBinary_(const std::string& text)
{
	return internal::jsonStringToBinary(text);
}


std::string encryptString(const std::string& text)
{
	return internal::jsonStringToBinary(text);
}

SafeWriteResult writeJsonToBinaryFileSafer(const std::string& contents, const FString& filename, bool log) {
	SafeWriteResult result{ WriteResult::Untouched, WriteResult::Untouched };
	auto& fileManager = IFileManager::Get();

	const auto tempFilename = getTempFilename(filename);
	if (internal::writeToBinaryFile(contents, tempFilename)) {
		if (moveFileWithReplace(*tempFilename, *filename, log)) {
			result.regular = WriteResult::Saved;
		}
	}
	if (log) {
		logMoveBaseBackupResults(result, filename);
	}
	return result;
}

SafeWriteResult writeJsonToBinaryFileSafer(const Json::Value& node, const FString& filename, bool log) {
	return writeJsonToBinaryFileSafer(internal::jsonToBinary(node), filename, log);
}
	

//
// SaveWriteResult + logging helpers
//
bool SafeWriteResult::anyWriteSucceeded() const {
	return regular == WriteResult::Saved || backup == WriteResult::Saved;
}


}}
