#pragma once
#include <PCH/Precompiled.hpp>
#include "Singleton/Singleton.hpp"

namespace SteerStone { namespace Chat { namespace Channel {
    struct User
    {
        uint32 UserId;
        std::string Name;
        std::vector<uint16> RoomIds;
    };

    typedef std::unordered_map<uint32, User> UsersMap;

    /// Chat
    class Base
    {
        SINGLETON_P_D(Base);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Add User
        ///@ p_UserId: User Id
        ///@ p_Username: Username
        void AddUser(uint32 const p_UserId, std::string const p_Username, std::vector<uint16> const p_RoomIds);


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        UsersMap m_Users;
    };
}   ///< namespace Channel
}   ///< namespace Chat
}   ///< namespace Steerstone

#define sChatManager SteerStone::Chat::Channel::Base::GetSingleton()