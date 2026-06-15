#pragma once

#include <string>

namespace minecraft
{
namespace api
{
namespace endpoint
{
	
enum class Dungeons {
	CONFIG_SEASONS,
	CONFIG_GAME,
	CHALLENGES,
	PROGRESS_POST,
	PROGRESS_GET,
	REWARDS_POST,
	REWARDS_GET,
#if !UE_BUILD_SHIPPING
	PROGRESS_RESET,
#endif
};
struct Config {
	Config(const std::string& verb, const std::string& path)
	: verb(verb), path(path) {
	}
	const std::string verb;
	const std::string path;
};

template<Dungeons E> Config get() = delete;

template<> inline Config get<Dungeons::CONFIG_SEASONS>() { return { "GET", "/dungeons/config/seasons" }; }
template<> inline Config get<Dungeons::CONFIG_GAME>() { return { "GET", "/dungeons/config/game" }; }
template<> inline Config get<Dungeons::CHALLENGES>() { return { "GET", "/dungeons/challenges" }; }
template<> inline Config get<Dungeons::PROGRESS_POST>() { return { "POST", "/dungeons/progress" }; }
template<> inline Config get<Dungeons::PROGRESS_GET>() { return { "GET", "/dungeons/progress" }; }
template<> inline Config get<Dungeons::REWARDS_POST>() { return { "POST", "/dungeons/rewards/claim" }; }
template<> inline Config get<Dungeons::REWARDS_GET>() { return { "GET", "/dungeons/rewards" }; }
#if !UE_BUILD_SHIPPING
template<> inline Config get<Dungeons::PROGRESS_RESET>() { return { "GET", "/dev/dungeonsseasons/internal/progress/reset" }; }
#endif

		
}
}
}

