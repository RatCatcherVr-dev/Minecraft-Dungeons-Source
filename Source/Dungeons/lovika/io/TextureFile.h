#pragma once

#include "Engine/Texture2D.h"
#include <IImageWrapperModule.h>

namespace io {

UTexture2D* loadTexture(const FString& FullFilePath, EImageFormat ImageFormat, bool& IsValid, uint32_t& Width, uint32_t& Height);

};
