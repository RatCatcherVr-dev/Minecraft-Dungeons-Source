#pragma once

#include <Optional.h>
#include <UnrealString.h>
#include <json/json.h>

namespace binarysavedata {

TOptional<Json::Value> loadBinaryFileToJson(const FString& filename);
bool                   writeJsonToBinaryFile(const Json::Value&, const FString& filename);

}

namespace binarysavedata { namespace safer {

enum class WriteResult { Saved, Failed, Untouched };

struct SafeWriteResult {
	WriteResult regular;
	WriteResult backup;

	bool anyWriteSucceeded() const;
};

TArray<uint8> loadJsonStringAndDecryptSafer(const FString& filename);
TOptional<Json::Value> loadBinaryFileToJsonSafer(const FString& filename);
SafeWriteResult        writeJsonToBinaryFileSafer(const Json::Value&, const FString& filename, bool log = true);
SafeWriteResult		   writeJsonToBinaryFileSafer(const std::string& contents, const FString& filename, bool log = true);
std::string			   encryptString(const std::string& text);
}}
