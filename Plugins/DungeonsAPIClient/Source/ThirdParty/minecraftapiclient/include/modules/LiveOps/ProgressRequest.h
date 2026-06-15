#pragma once

#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "core/Datetime.h"
#include <Anticheat.hpp>

namespace minecraft {
namespace api {

struct ProgressRequest {
	std::shared_ptr<std::string> name;
	std::shared_ptr<int64> score;
	std::shared_ptr<std::string> id;

	void toJson(Json::Value &j) {
		ANTICHEAT_PROTECT_STRINGS_BEGIN;
		writeJson(j, "name", name);
		writeJson(j, "score", score);
		writeJson(j, "id", id);
		ANTICHEAT_PROTECT_STRINGS_END;
	}
};

struct ProgressPost {
	std::shared_ptr<vector<ProgressRequest>> progress;
	
	void toJson(Json::Value &j) {
		ANTICHEAT_PROTECT_STRINGS_BEGIN;
		writeArrayJson<ProgressRequest>(j, "progress", progress);
		ANTICHEAT_PROTECT_STRINGS_END;
	}
};
}
}

