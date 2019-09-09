/*
* Liam Ashdown
* Copyright (C) 2019
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

#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Constructor 
    /// @p_Service : Boost Service
    /// @p_CloseHandler : Close Handler Custom function
    GameSocket::GameSocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler)
        : Socket(p_Service, std::move(p_CloseHandler))
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Handle incoming data
    bool GameSocket::ProcessIncomingData()
    {
        return true;
    }

}   ///< namespace Server
}   ///< namespace Core
}   ///< namespace Steerstone