#pragma once

#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "RewardResponse.h"
#include <map>

namespace minecraft {
namespace api {

struct Progress {
	std::string name;
	int64 score;

	void fromJson(const Json::Value& j) {
		name = parseRequired<std::string>(j, "name");
		score = parseRequired<int64>(j, "score");
	}
};

struct GetProgressResponse {
	std::vector<Progress> progresses;

	void fromJson(const Json::Value& j) {
		progresses = parseRequiredArray<Progress>(j, "progress");
	}
};

struct PostProgressResponse {
	void fromJson(const Json::Value& j) {
	}
};

#if !UE_BUILD_SHIPPING
struct ResetAllProgressResponse {
	void fromJson(const Json::Value& j) {
	}
};
#endif

}
}

