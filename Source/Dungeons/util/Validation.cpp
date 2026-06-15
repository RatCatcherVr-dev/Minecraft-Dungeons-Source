#include "Dungeons.h"
#include "Validation.h"
#include "StringUtils.h"
#include <LogMacros.h>

DEFINE_LOG_CATEGORY(LogLevelGeneration);

std::string Validation::summary(bool includeTitle) const {
	std::string s;
	if (includeTitle && !mTitle.empty()) {
		s += mTitle + "\n\n";
	}
	if (!mErrors.empty()) {
		s += "Errors\n - " + Util::join(mErrors, "\n - ");
		if (!mWarnings.empty()) {
			s += '\n';
		}
	}
	if (!mWarnings.empty()) {
		s += "Warnings\n - " + Util::join(mWarnings, "\n - ");
	}
	if (isSuccessNoWarnings()) {
		s += "No warnings and no errors.";
	}
	return s;
}

Validation validationErrors(const std::string& title, std::initializer_list<std::string> errors) {
	Validation validation(title);
	for (auto&& error : errors) {
		validation.error(error);
	}
	return validation;
}

Validation validationErrors(std::initializer_list<std::string> errors) {
	return validationErrors("", errors);
}

Validation validationErrorsIf(const std::string& title, std::initializer_list<std::pair<bool, std::string>> errors) {
	Validation validation(title);
	for (auto&& errorIf : errors) {
		if (errorIf.first) {
			validation.error(errorIf.second);
		}
	}
	return validation;
}

Validation validationErrorsIf(std::initializer_list<std::pair<bool, std::string>> errors) {
	return validationErrorsIf("", errors);
}

void validationIssuesIf(Validation& validation, std::initializer_list<std::tuple<IssueType, bool, std::string>> issues) {
	for (auto&& issue : issues) {
		if (!std::get<bool>(issue)) {
			continue;
		}
		if (IssueType::Warning == std::get<IssueType>(issue)) {
			validation.warning(std::get<std::string>(issue));
		} else {
			validation.error(std::get<std::string>(issue));
		}
	}
}

void append(ValidationIssues& issues, const ValidationIssues& extra) {
	issues.insert(issues.end(), extra.begin(), extra.end());
}

ELogVerbosity::Type getDefaultLogVerbosityFor(const Validation& validation) {
	if (validation.isSuccessNoWarnings()) return ELogVerbosity::Display;
	if (validation.isSuccess())           return ELogVerbosity::Warning;
	return ELogVerbosity::Error;
}

void printValidation(const Validation& validation, const FString& prefix) {
	if (validation.isSuccessNoWarnings()) {
		UE_LOG(LogLevelGeneration, Display, TEXT("%s%s"), *prefix, UTF8_TO_TCHAR(validation.summary().c_str()));
	} else if (validation.isSuccess()) {
		UE_LOG(LogLevelGeneration, Warning, TEXT("%s%s"), *prefix, UTF8_TO_TCHAR(validation.summary().c_str()));
	} else {
		UE_LOG(LogLevelGeneration, Error, TEXT("%s%s"), *prefix, UTF8_TO_TCHAR(validation.summary().c_str()));
	}
}

void printValidation(const ValidationIssues& validationIssues, const FString& prefix) {
	for (int i = 0; i < validationIssues.size(); ++i) {
		printValidation(validationIssues[i], i == 0? prefix : "");
	}
}
