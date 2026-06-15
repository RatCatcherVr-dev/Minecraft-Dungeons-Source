#pragma once

namespace game {
namespace defs {
	FName generateBlueprintRelativePath(const FString& name, const FString& Suffix);
	FName generateTextureRelativePath(const FString& name, const FString& Suffix);
	FName generateMaterialInstanceRelativePath(const FString& name, const FString& Suffix);
}
}