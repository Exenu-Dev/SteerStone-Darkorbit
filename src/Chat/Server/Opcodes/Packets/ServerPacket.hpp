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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "PacketBuffer.hpp"

namespace SteerStone { namespace Chat { namespace Server {

    /// ServerPacket which handles encoding packets to be sent to the client
    class ServerPacket
    {
        DISALLOW_COPY_AND_ASSIGN(ServerPacket);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor
            ServerPacket(const char* p_Header, bool p_AppendSplit = true) 
            {
                m_Buffer.AppendChar(p_Header);

                if (p_AppendSplit)
                    m_Buffer.AppendChar("%");
            }
            /// Deconstructor
            ~ServerPacket() {}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Get our buffer
            PacketBuffer GetBuffer()          { return m_Buffer;          }
            /// Get our secondary buffer
            PacketBuffer GetSecondaryBuffer() { return m_SecondaryBuffer; }

        protected:
            PacketBuffer m_Buffer;                  ///< String buffer which holds our data to be sent to the client
            PacketBuffer m_SecondaryBuffer;         ///< Use Secondary buffer if we are pushing data into buffer outside the class packet
                                                    ///< And want to append the secondary buffer after the first buffer
    };
}   ///< namespace Server
}   ///< namespace Chat
}   ///< namespace SteerStone