#pragma once
#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include <unordered_map>
#include <functional>

class StatsAndAchievements
{
public:
	StatsAndAchievements(std::function<void (std::vector<std::string>& ach)> onStatsReceived);
	~StatsAndAchievements() = default;

	STEAM_CALLBACK(StatsAndAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);

private:
	std::vector<std::string> achievements;
	std::function<void(std::vector<std::string>& ach)> onStatsReceived_;
};

