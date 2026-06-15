#pragma once

#include "NameTypes.h"

class LocTableFromFile
{
public:
	static void LoadCsvs();
	static FText Get(const FName& InTableId, const FString& InKey);
};
