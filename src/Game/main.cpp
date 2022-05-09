/*
* Liam Ashdown
* Copyright (C) 2022
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <openssl/crypto.h>
#include <openssl/opensslv.h>

#include "Network/Server/Listener.hpp"
#include "Logger/Base.hpp"
#include "Config/Config.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Diagnostic/DiaServerWatch.hpp"

#include "World.hpp"
#include "ZoneManager.hpp"
#include "Socket.hpp"

DatabaseType GameDatabase;

int main()
{
    /// Log Enablers
    LOG_ENABLE_TIME(true);
    LOG_ENABLE_THREAD_ID(true);
    LOG_ENABLE_FUNCTION(false);
    LOG_ADD_APPENDER(new ::SteerStone::Core::Logger::FileAppender("GameLog.txt"));

    if (!sConfigManager->SetFile("game.conf"))
        return -1;

    SteerStone::Core::Logger::Base::GetSingleton()->ShowBanner(
        []()
        {
            LOG_INFO("Engine", "Using configuration file %0.", sConfigManager->GetFilename().c_str());
            LOG_INFO("Engine", "Using SSL version: %0 (library: %1)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
            LOG_INFO("Engine", "Using Boost version: %0.%1.%2", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
        });

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    static std::string l_DatabaseInfo  = sConfigManager->GetString("GameDatabaseInfo").c_str();
    static int32       l_WorkerThreads = sConfigManager->GetInt("GameWorkerThreads", 1);
    static int32       l_Instances     = sConfigManager->GetInt("MySQLInstances", 5);

    if (!GameDatabase.Start(l_DatabaseInfo.c_str(), l_Instances, l_WorkerThreads))
         return -1;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Loading
    sWorldManager->Load();
    sOpCode->InitializePackets();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Network
    static const std::string l_Address          = sConfigManager->GetString("BindIP", "0.0.0.0");
    static const int32       l_Port             = sConfigManager->GetInt("GamePort", 5152);
    static const int32       l_ChildListeners   = sConfigManager->GetInt("ChildListeners", 1);

    SteerStone::Core::Network::Listener<SteerStone::Game::Server::GameSocket> l_Listener(l_Address, l_Port, l_ChildListeners);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    LOG_INFO("Game", "Game Server successfully booted, listening on %0 %1", l_Address, l_Port);

    uint32 l_RealCurrTime  = 0;
    uint32 l_RealPrevTime  = sServerTimeManager->Tick();
    uint32 l_PrevSleepTime = 0;

    while (!sWorldManager->StopWorld())
    {
        l_RealCurrTime = sServerTimeManager->GetServerTime();
        uint32 const& l_Diff = sServerTimeManager->Tick();

        sWorldManager->Update(l_Diff);

        l_RealPrevTime = l_RealCurrTime;

        /// Update the world every 60 ms
        if (l_Diff <= WORLD_SLEEP_TIMER + l_PrevSleepTime)
        {
            l_PrevSleepTime = WORLD_SLEEP_TIMER + l_PrevSleepTime - l_Diff;
            std::this_thread::sleep_for(std::chrono::milliseconds((uint32)(l_PrevSleepTime)));
        }
        else
            l_PrevSleepTime = 0;
    }

    return 0;
}