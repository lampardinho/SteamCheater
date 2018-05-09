#include "StatsAndAchievements.h"
#include <iostream>


StatsAndAchievements::StatsAndAchievements(std::function<void(std::vector<std::string>& ach)> onStatsReceived) :
	m_CallbackUserStatsReceived(this, &StatsAndAchievements::OnUserStatsReceived),
	onStatsReceived_(onStatsReceived)
{
	SteamUserStats()->RequestCurrentStats();
}

void StatsAndAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (pCallback->m_eResult != k_EResultOK)
	{
		std::cerr << "OnUserStatsReceived fail";
		return;
	}

	int numAchievements = SteamUserStats()->GetNumAchievements();
	for (int i = 0; i < numAchievements; ++i)
	{
		const char *achName = SteamUserStats()->GetAchievementName(i);
		if (achName)
		{
			achievements.push_back(achName);
		}
	}

	onStatsReceived_(achievements);
}
