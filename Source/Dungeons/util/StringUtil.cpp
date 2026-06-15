#include "Dungeons.h"
#include "StringUtil.h"

namespace stringutil {

FString toFString(const std::string& str) {
	return FString(str.c_str());
}

FString utf8ToFString(const std::string & str) {
	return FString(UTF8_TO_TCHAR(str.c_str()));
}

FString toFString(const char* cstr) {
	return FString(cstr);
}

FString toFString(const std::exception& exception) {
	return FString(UTF8_TO_TCHAR(exception.what()));
}

std::string toStdString(const FString& fstr) {
	return std::string(TCHAR_TO_UTF8(*fstr));
}

std::string toStdString(const FText& ftext) {
	return toStdString(ftext.ToString());
}

std::string toStdString(const FName& fname) {
	return toStdString(fname.ToString());
}

FName toFName(const std::string& str) {
	return FName(*toFString(str));
}

FName toFName(const char* cstr) {
	return FName(*toFString(cstr));
}

FText toFText(const std::string& str) {
	return FText::FromString(toFString(str));
}

FText toFText(const char* cstr) {
	return FText::FromString(toFString(cstr));
}

}
