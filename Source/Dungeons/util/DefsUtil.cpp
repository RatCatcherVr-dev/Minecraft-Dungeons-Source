#include "DefsUtil.h"
#include <UnrealString.h>
#include <NoExportTypes.h>

namespace game {
namespace defs {
	FName genereateAssetPath(const FString& Prefix, const FString& Name, const FString& Suffix) {
		auto name = Name;
		if (name.Contains("/")) {
			int32 index = name.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			name = name.RightChop(index + 1);
		}
		name = Prefix + name + Suffix;

		return *(Name + "/" + name);
	}

	FName generateBlueprintRelativePath(const FString & name, const FString & Suffix)
	{
		return genereateAssetPath("BP_", name, Suffix);
	}

	FName generateTextureRelativePath(const FString & name, const FString & Suffix)
	{
		return genereateAssetPath("T_", name, Suffix);
	}

	FName generateMaterialInstanceRelativePath(const FString & name, const FString & Suffix)
	{
		return genereateAssetPath("MI_", name, Suffix);
	}

}}