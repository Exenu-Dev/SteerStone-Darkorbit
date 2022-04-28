/*
* Liam Ashdown
* Copyright (C) 2021
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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.f
*/

#include <openssl/crypto.h>
#include <openssl/opensslv.h>

#include "Network/Server/Listener.hpp"
#include "Logger/Base.hpp"
#include "Config/Config.hpp"

#include "Socket.hpp"

int main()
{
    /// Log Enablers
    LOG_ENABLE_TIME(true);
    LOG_ENABLE_THREAD_ID(true);
    LOG_ENABLE_FUNCTION(true);

    if (!sConfigManager->SetFile("chat.conf"))
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

    /// Loading
    sOpCode->InitializePackets();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
        
    /// Network
    static const std::string l_Address          = sConfigManager->GetString("BindIP", "127.0.0.1");
    static const int32 l_Port                   = sConfigManager->GetInt("Chat", 843);
    static const int32 l_ChildListeners         = sConfigManager->GetInt("ChildListeners", 1);

    SteerStone::Core::Network::Listener<SteerStone::Chat::Server::ChatSocket> l_Listener(l_Address, l_Port, l_ChildListeners);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    LOG_INFO("Policy", "Chat Server successfully booted, listening on %0 %1", l_Address, l_Port);

    while (true)
    {
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(std::numeric_limits<int>::max()));
    }
}