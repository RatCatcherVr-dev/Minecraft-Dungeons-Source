/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CoreTypes.h"
#include "Containers/UnrealString.h"

enum class EEntitlementsSource : uint8;

namespace dungeonsversion {
	extern const FString BuildTypeLauncher;
	extern const FString BuildTypeWin10Store;
	extern const FString BuildTypeUndefined;

	/** Helper to get the standard string for the crash type based on crash event bool values. */
	const FString& GetDungeonsBuildTypeString(EEntitlementsSource Type);
	void AddDungeonsVersionToCrashReport();

}
//// jryden copy from FGenericPlatformContext / GenericPlatformCrashContext.h
struct DUNGEONS_API FDungeonsVersionContext
{

public:
	static const ANSICHAR* DungeonsGameXMLNameA;

	static const TCHAR* DungeonsGameXMLNameW;
	static const int32 CrashGUIDLength = 128;
	static const FString DungeonsPropertiesTag;
	
	/** Initializes crash context related platform specific data that can be impossible to obtain after a crash. */
	static void Initialize();

	/**
	 * @return true, if the generic crash context has been initialized.
	 */
	static bool IsInitalized()
	{
		return bIsInitialized;
	}
	
	static FDungeonsVersionContext& Get();

	static void SaveDungeonsVersion();

	/** Default constructor. */
	FDungeonsVersionContext(EEntitlementsSource InType, const TCHAR* ErrorMessage);
	virtual ~FDungeonsVersionContext() { }

	//// could be protected?
	void SerializeContentToBuffer() const;

	/** Serializes crash's informations to the specified filename. Should be overridden for platforms where using FFileHelper is not safe, all POSIX platforms. */
	virtual void SerializeAsXML(const TCHAR* Filename) const;


	/** Writes a common property to the buffer. */
	void AddCrashProperty(const TCHAR* PropertyName, const TCHAR* PropertyValue) const;

	/** Writes a common property to the buffer. */
	template <typename Type>
	void AddCrashProperty(const TCHAR* PropertyName, const Type& Value) const
	{
		AddCrashProperty(PropertyName, *TTypeToString<Type>::ToString(Value));
	}

	/** Escapes and appends specified text to XML string */
	static void AppendEscapedXMLString(FString& OutBuffer, const TCHAR* Text);

protected:

	EEntitlementsSource BuildType;
	const TCHAR* ErrorMessage;

private:

	/** Writes header information to the buffer. */
	void AddHeader() const;

	/** Writes footer to the buffer. */
	void AddFooter() const;

	void BeginSection(const TCHAR* SectionName) const;
	void EndSection(const TCHAR* SectionName) const;

	/** Called once when GConfig is initialized. Opportunity to cache values from config. */
	static void InitializeFromConfig();

	/**	Whether the Initialize() has been called */
	static bool bIsInitialized;

	/** The buffer used to store the crash's properties. */
	mutable FString CommonBuffer;

	// FNoncopyable (from GenericPlatformCrashContext)
	FDungeonsVersionContext(const FDungeonsVersionContext&) = delete;
	FDungeonsVersionContext& operator=(const FDungeonsVersionContext&) = delete;
};
