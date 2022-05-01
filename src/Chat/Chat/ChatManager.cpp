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

#include "ChatManager.hpp"

namespace SteerStone { namespace Chat { namespace Channel {

    SINGLETON_P_I(Base);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Base::Base()
    {
    }
    /// Deconstructor
    Base::~Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Add User
    ///@ p_UserId: User Id
    ///@ p_Username: Username
    void Base::AddUser(uint32 const p_UserId, std::string const p_Username, std::vector<uint16> const p_RoomIds)
    {
        User l_User;
        l_User.UserId = p_UserId;
        l_User.Name = p_Username;
        l_User.RoomIds = p_RoomIds;
        m_Users[p_UserId] = l_User;
    }
}
}
}