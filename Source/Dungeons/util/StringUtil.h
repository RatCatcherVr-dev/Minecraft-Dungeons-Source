#pragma once

#include <CoreMinimal.h>

namespace stringutil {
	//Deprecated, please use utf8ToFString
	FString toFString(const std::string& str);
	//toFString should probably do utf8->tchar conversion, but since it doesn't, we'll have to create this temporary function for minimizing side-effects
	FString utf8ToFString(const std::string& str);
	FString toFString(const char* cstr);
	FString toFString(const std::exception& exception);
	std::string toStdString(const FString& fstr);
	std::string toStdString(const FText& ftext);
	std::string toStdString(const FName& fname);
	FName toFName(const std::string& str);
	FName toFName(const char* cstr);
	FText toFText(const std::string& str);
	FText toFText(const char* cstr);
}
