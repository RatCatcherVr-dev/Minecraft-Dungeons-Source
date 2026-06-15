#if defined(PLATFORM_WINDOWS) && !defined(STEAM_BUILD) && (!defined(GDK_API_ENABLED) || GDK_API_ENABLED == 0)
#pragma once

#include "CoreMinimal.h"
#include "xsapiServicesInclude.h"
#include "xal_platform_types.h"

#include <map>

/**
* Acts as an intermediary between file-based storage of XAL credentials for
* Microsoft accounts and the game.
* The file and format are both expected to be managed by the Launcher.
* While the credentials on file as stored per-user, the game may only handle a single
* user 'Logged' in at any time.
*/
class XalStorage
{
	using XalUserCredentialMap = std::map<std::string, std::string>;
	using XalUserMap = std::map<std::string, XalUserCredentialMap>; //Map of user XUIDs to Credentials

public:
	XalStorage() = default;
	~XalStorage() = default;

	bool Initialize(XTaskQueueHandle);

private:
	void Write(XalPlatformOperation, const std::string& key, size_t dataSize, void const* data);
	void Read(XalPlatformOperation, const std::string& key);
	void Clear(XalPlatformOperation, const std::string& key);
	
	void SaveCredentials();
	bool LoadCredentials();

	std::string GetUserMapKey(const std::string& key) const;

	std::string activeUserXuid; //Which XUID should be used when writing/reading credentials.
	XalUserMap credentials;
	FString credentialFilePath; //Path to the folder containing credentials, managed by the Launcher.
};

#endif
