#pragma once

#include "CoreMinimal.h"
#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <map>

namespace minecraft
{
	namespace api
	{
		struct LeaderboardResponseEntry {
			std::shared_ptr<int64> mRank;
			std::shared_ptr<int64> mValue;
			std::shared_ptr<std::string> mName;

			LeaderboardResponseEntry() {}
	
			LeaderboardResponseEntry(int64 rank, int64 value, const std::string& name){
				mRank = std::make_shared<int64>(rank);
				mValue = std::make_shared<int64>(value);
				mName = std::make_shared<std::string>(name);
			}
			
			void fromJson(const Json::Value& j) {
				mRank = parseJson<int64>(j, "rank");
				mValue = parseJson<int64>(j, "value");
				mName = parseJson<std::string>(j, "name");
			}
		};

		struct LeaderboardResponseList{
			std::shared_ptr<std::vector<minecraft::api::LeaderboardResponseEntry>> List;
			LeaderboardResponseList() {
				List = std::make_shared<std::vector<minecraft::api::LeaderboardResponseEntry>>();
			}

			void fromJson(const Json::Value& j) {
				List = parseArrayJson<LeaderboardResponseEntry>(j);
			}
		};

		struct LeaderboardResponse {
			std::shared_ptr<std::unordered_map<std::string, minecraft::api::LeaderboardResponseList>> Map;
			LeaderboardResponse() {
				Map = std::make_shared<std::unordered_map<std::string, minecraft::api::LeaderboardResponseList>>();
			}
	
			void fromJson(const Json::Value &mapJson) {
				auto keys = mapJson.getMemberNames();
				for (const auto& key : keys) {
					LeaderboardResponseList rList;
					rList.fromJson(key);
					Map->insert({key, std::move(rList)});
				}
			}
		};
	}
}
	

		