#if defined(PLATFORM_WINDOWS) && !defined(STEAM_BUILD) && (!defined(GDK_API_ENABLED) || GDK_API_ENABLED == 0)

#include "PCHOnlineDungeonsSubsystem.h"
#include "XalStorage.h"
#include "OnlineSubsystem.h"

#include "xal_platform.h"
#include "Xal/xal.h"
#include "HrLog.h"

#include "CommandLine.h"

#include "Parse.h"
#include "utils.h"

#include <regex>
#include <json/json.h>
#include <FileHelper.h>

namespace Detail
{
	//Some Tokens are not related to a single user and refer to the whole device or system.
	const FString credentialFileName = TEXT("launcher_msa_credentials.json");
	const FString protectedCredentialFileName = TEXT("launcher_msa_credentials.bin");
	bool IsUserAgnostic(const std::string& key)
	{
		static const std::regex DEVICE_IDENTITY_KEY_RE("Xal\\.Production\\..+\\.DeviceIdentity");
		return std::regex_search(key, DEVICE_IDENTITY_KEY_RE) || key == "ClockSkew";
	}

	TOptional<TArray<uint8>> ProtectData(const std::string& data)
	{
#if !PLATFORM_WINDOWS
		return {};
#else
		DATA_BLOB dataIn;
		dataIn.pbData = const_cast<BYTE*>(reinterpret_cast<const BYTE*>(data.data()));
		dataIn.cbData = data.size();
		DATA_BLOB dataOut{};
		scopedLambda::ScopedLambda localFreeOnExit([&dataOut]() {LocalFree(dataOut.pbData);});
		if (!CryptProtectData(
			&dataIn,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			CRYPTPROTECT_LOCAL_MACHINE,
			&dataOut)) {
			HrLog(HRESULT_FROM_WIN32(GetLastError()), "[XalStorage] Unable to protect data");
			return {};
		}
		
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] Successfully protected data"));
		return TArray<uint8>(reinterpret_cast<const uint8*>(dataOut.pbData), dataOut.cbData);
#endif
	}

	TOptional<std::string> UnprotectData(const TArray<uint8>& data)
	{
#if !PLATFORM_WINDOWS
		return {};
#else
		DATA_BLOB dataIn;
		dataIn.pbData = const_cast<BYTE*>(reinterpret_cast<const BYTE*>(data.GetData()));
		dataIn.cbData = data.Num();
		DATA_BLOB dataOut{};
		scopedLambda::ScopedLambda localFreeOnExit([&dataOut]() {LocalFree(dataOut.pbData);});
		if (!CryptUnprotectData(
			&dataIn,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			0,
			&dataOut)) {
			HrLog(HRESULT_FROM_WIN32(GetLastError()), "[XalStorage] Unable to unprotect data");
			return {};
		}

		UE_LOG_ONLINE(Log, TEXT("[XalStorage] Successfully unprotected data"));
		return std::string(reinterpret_cast<const char*>(dataOut.pbData), dataOut.cbData);
#endif
	}

	TOptional<TArray<uint8>> read(const FString& filename)
	{
		TArray<uint8> fileContents;
		if (FFileHelper::LoadFileToArray(fileContents, *filename))
		{
			return { fileContents };
		}
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] Failed to load contents of credential file '%s'"), *filename);
		return {};
	}

	TOptional<std::string> readAsString(const FString& filename)
	{
		if (const auto& content = read(filename))
		{
			const auto& arr = content.GetValue();
			return std::string(arr.GetData(), arr.GetData() + arr.Num());
		}
		return {};
	}

	TOptional<std::string> readCredentials(const FString& credentialFilePath)
	{
		if (const auto& fileContents = read(credentialFilePath / protectedCredentialFileName))
		{
			return UnprotectData(fileContents.GetValue());
		}
		return readAsString(credentialFilePath / credentialFileName);
	}
	
} //namespace Detail


bool XalStorage::Initialize(XTaskQueueHandle taskQueue)
{
	//The launcher should have provided a directory and active user to the game.
	FString launcherDir;
	FString activeUser;
	const TCHAR* commandLine = FCommandLine::Get();

	FParse::Value(commandLine, TEXT("launcherDir"), launcherDir);
	FParse::Value(commandLine, TEXT("activeUser"), activeUser);
	if (launcherDir.IsEmpty() || activeUser.IsEmpty())
	{
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] No launcherDir or activeUser passed to command line"));
		return false;
	}

	
	activeUserXuid = TCHAR_TO_UTF8(*activeUser);
	credentialFilePath = launcherDir;

	if (!LoadCredentials())
	{
		return false;
	}

	XalPlatformStorageEventHandlers handlers;
	handlers.write = [](void* context, void* userContext, XalPlatformOperation operation, char const* key,
		size_t dataSize, void const* data) {
			static_cast<XalStorage*>(context)->Write(operation, key, dataSize, data);
	};
	handlers.read = [](void* context, void* userContext, XalPlatformOperation operation, char const* key) {
		static_cast<XalStorage*>(context)->Read(operation, key);
	};
	handlers.clear = [](void* context, void* userContext, XalPlatformOperation operation, char const* key) {
		static_cast<XalStorage*>(context)->Clear(operation, key);
	};
	handlers.context = this;

	return HrSuccess(XalPlatformStorageSetEventHandlers(taskQueue, &handlers), "[XalStorage] XalPlatformStorageSetEventHandlers");
}

void XalStorage::Write(XalPlatformOperation operation, const std::string& key, size_t dataSize, void const* data)
{
	std::string userMapKey = GetUserMapKey(key);
	credentials[userMapKey][key] = std::string(static_cast<const char*>(data), dataSize);
	SaveCredentials();

	HrLog(XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Success), "[XalStorage] XalPlatformStorageWriteComplete");
}

void XalStorage::Read(XalPlatformOperation operation, const std::string& key)
{
	std::string userMapKey = GetUserMapKey(key);
	auto it = credentials.find(userMapKey);
	if (it != credentials.end())
	{
		std::string content = it->second[key];
		HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, content.size(),
			content.data()), "[XalStorage] XalPlatformStorageReadComplete");
	}
	else
	{
		HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr),
			"[XalStorage] XalPlatformStorageReadComplete");
	}
}

void XalStorage::Clear(XalPlatformOperation operation, const std::string& key)
{
	std::string userMapKey = GetUserMapKey(key);
	auto it = credentials.find(userMapKey);
	if (it != credentials.end())
	{
		if (it->second.erase(key) > 0)
		{
			SaveCredentials();
		}
	}

	HrLog(XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success), "[XalStorage] XalPlatformStorageClearComplete");
}

void XalStorage::SaveCredentials()
{
	Json::Value node;
	if (!credentials.empty())
	{
		Json::Value credentialsJson;
		for (const auto& user : credentials)
		{
			Json::Value userData;
			for (const auto& data : user.second)
			{
				userData[data.first] = data.second;
			}

			credentialsJson[user.first] = userData;
		}

		node["credentials"] = credentialsJson;
	}

	std::ostringstream jsonStringStream;
	Json::StyledStreamWriter writer(" ");
	writer.write(jsonStringStream, node);

	/**
	 * The credentials are written to a separate file to ease contention to the launcher's managed credential storage.
	 * Once the game process has exited, or when the launcher starts, this file will be merged into the main credential storage.
	 */
	if (const auto& savedContents = Detail::ProtectData(jsonStringStream.str())) {
		auto result = FFileHelper::SaveArrayToFile(savedContents.GetValue(), *(credentialFilePath / Detail::protectedCredentialFileName + ".dungeons"));
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] Credential saving %s"), (result ? TEXT("Succeded") : TEXT("Failed")));
	}
}
	

bool XalStorage::LoadCredentials()
{
	const auto& maybeCredentials = Detail::readCredentials(credentialFilePath);
	if (!maybeCredentials)
	{
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] No launcher credentials present '%s'"), *credentialFilePath);
		return false;
	}

	Json::Value node;
	Json::Reader reader;
	std::string filedataUTF = maybeCredentials.GetValue();
	if (!reader.parse(filedataUTF, node, false))
	{
		//Failed to parse file contents, might have been edited by third-party/user
		UE_LOG_ONLINE(Log, TEXT("[XalStorage] Failed to parse contents of credential file '%s'"), *credentialFilePath);
		return false;
	}

	Json::Value users = node["credentials"];
	for (const auto& user : users.getMemberNames())
	{
		//Add the user to the data map
		auto result = credentials.emplace(user, XalUserCredentialMap());
		auto& userMap = result.first->second;
		Json::Value userData = users[user];
		for (const auto& data : userData.getMemberNames())
		{
			if (!userData[data].isString())
			{
				UE_LOG_ONLINE(Log, TEXT("[XalStorage] Expected string type for token [%s] in credentials for (%s)"), data.c_str(), user.c_str());
				return false;
			}

			userMap[data] = userData[data].asString();
		}
	}

	return true;
}

std::string XalStorage::GetUserMapKey(const std::string& key) const
{
	if (Detail::IsUserAgnostic(key)) 
	{
		return "common";
	}

	return activeUserXuid;
}

#endif
