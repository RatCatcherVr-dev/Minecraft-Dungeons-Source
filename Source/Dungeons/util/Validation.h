#pragma once

#include <tuple>

DECLARE_LOG_CATEGORY_EXTERN(LogLevelGeneration, Log, All);

enum class IssueType: uint8_t {
	Warning,
	Error
};

class Validation {
public:
	Validation(std::string title = "")
		: mTitle(title) {
	}

	Validation& error(const std::string& reason) {
		mErrors.push_back(reason);
		return *this;
	}

	Validation& warning(const std::string& reason) {
		mWarnings.push_back(reason);
		return *this;
	}

	bool isSuccess() const {
		return mErrors.empty();
	}

	bool isSuccessNoWarnings() const {
		return mErrors.empty() && mWarnings.empty();
	}

	const std::string& title() const {
		return mTitle;
	}

	const std::vector<std::string>& errors() const {
		return mErrors;
	}

	const std::vector<std::string>& warnings() const {
		return mWarnings;
	}

	std::string summary(bool includeTitle = true) const;

	explicit operator bool() const {
		return isSuccess();
	}
private:
	std::string mTitle;
	std::vector<std::string> mErrors;
	std::vector<std::string> mWarnings;
};

Validation validationErrors(const std::string& title, std::initializer_list<std::string>);
Validation validationErrors(std::initializer_list<std::string>);
Validation validationErrorsIf(const std::string& title, std::initializer_list<std::pair<bool, std::string>>);
Validation validationErrorsIf(std::initializer_list<std::pair<bool, std::string>>);
void validationIssuesIf(Validation&, std::initializer_list<std::tuple<IssueType, bool, std::string>>);

using ValidationIssues = std::vector<Validation>;
void append(ValidationIssues&, const ValidationIssues&);

ELogVerbosity::Type getDefaultLogVerbosityFor(const Validation&);

void printValidation(const Validation&, const FString& prefix);// , FOutputDevice* = nullptr, TOptional<FName> category = {});
void printValidation(const ValidationIssues&, const FString& prefix = "");
