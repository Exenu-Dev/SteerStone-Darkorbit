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

#ifndef _SERVER_CLIENT_PACKET_HPP_
#define _SERVER_CLIENT_PACKET_HPP_
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#endif /* _SERVER_STRING_BUFFER_HPP_ */


namespace SteerStone { namespace Policy { namespace Server {

    class ClientPacket
    {
        DISALLOW_COPY_AND_ASSIGN(ClientPacket);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor
            /// @p_String : String of packet
            ClientPacket(const std::string_view p_String)
            {
                m_Header = p_String[0];
                m_Payload = p_String; ///< Header is also apart of the payload
            }
            /// Deconstructor
            ~ClientPacket()
            {

            }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Get Packet Header
            uint64 GetHeader() const
            {
                return static_cast<uint64>(m_Header);
            }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            uint8 m_Header;         ///< Header
            std::string m_Payload;  ///< Payload
    };
}   ///< namespace Server
}   ///< namespace Policy
}   ///< namespace SteerStone