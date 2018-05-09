#include <windows.h>
#include <iostream>

#include "steam/steam_api.h"

#include "StatsAndAchievements.h"
#include <thread>
#include <atomic>
#include <iomanip>
#include <string>
#include <sstream>

std::atomic_bool shouldExit;

class MBuf: public std::stringbuf {
public:
    int sync() {
        fputs( str().c_str(), stdout );
        str( "" );
        return 0;
    }
};

int main()
{
	if (!SteamAPI_Init())
	{
		std::cerr << "SteamAPI_Init fail";
		getchar();
		return EXIT_FAILURE;
	}

	if (!SteamUser()->BLoggedOn())
	{
		std::cerr << "SteamUser()->BLoggedOn fail";
		getchar();
		return EXIT_FAILURE;
	}

	system("cls");	

	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	//https://stackoverflow.com/questions/45575863/how-to-print-utf-8-strings-to-stdcout-on-windows
	//setvbuf( stdout, nullptr, _IONBF, 0 );
	//MBuf buf;
    //std::cout.rdbuf( &buf );

	shouldExit = false;
	std::thread thr([]()
	{
		while (!shouldExit)
		{
			SteamAPI_RunCallbacks();
		}
	});
	
	StatsAndAchievements stats([](std::vector<std::string> ach)
	{
		std::cout << "Press [e] to exit\n";
		std::cout << "Press [s] to show achievements status\n";
		std::cout << "Press [r] to reset stats\n\n";

		int i = 1;
		do 
		{
			std::string input;
			getline(std::cin, input);
			
			if (input == "e")
			{
				shouldExit = true;
			}
			else if (input == "s")
			{
				std::cout << setiosflags(std::ios::left);
				std::cout << std::setw(5) << "[ID]";
				std::cout << std::setw(30) << "name";
				std::cout << std::setw(50) << "desc";
				std::cout << std::setw(10) << "hidden";
				std::cout << std::setw(5) << "unlocked";
				std::cout << std::endl;
				std::cout << std::string(100, '-').c_str() << std::endl;

				i = 1;
				for (auto achName : ach)
				{
					std::string name = SteamUserStats()->GetAchievementDisplayAttribute(achName.c_str(), "name");
					const char* desc = SteamUserStats()->GetAchievementDisplayAttribute(achName.c_str(), "desc");
					const char* hidden = SteamUserStats()->GetAchievementDisplayAttribute(achName.c_str(), "hidden");
					bool unlocked;
					SteamUserStats()->GetAchievement(achName.c_str(), &unlocked);
					
					std::cout << "[" << i++ << "]" << std::setw(3) << "";
					std::cout << std::setw(30) << name;
					std::cout << std::setw(50) << desc;
					std::cout << std::setw(10) << (hidden[0] == '1' ? "true" : "false");
					std::cout << std::setw(5) << (unlocked ? "true" : "false");
					std::cout << std::endl;
				}
			}
			else if (input == "r")
			{
				SteamUserStats()->ResetAllStats(true);
				std::cout << "Stats reseted!" << std::endl;
			}
			else
			{
				i = stoi(input);
				SteamUserStats()->SetAchievement(ach[i - 1].c_str());
				SteamUserStats()->StoreStats();
				std::cout << "Achievement " << i << " unlocked!" << std::endl;
			}
		} while (!shouldExit);		
	});

	thr.join();	

	SteamAPI_Shutdown();
	
	return EXIT_SUCCESS;
}
