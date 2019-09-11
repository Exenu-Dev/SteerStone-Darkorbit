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
#include "Utility/UtiString.hpp"
#endif /* _SERVER_STRING_BUFFER_HPP_ */


namespace SteerStone { namespace Game { namespace Server {

    class ClientPacket
    {
        DISALLOW_COPY_AND_ASSIGN(ClientPacket);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor
            /// @p_String : String of packet
            ClientPacket(const std::string& p_String)
            {
                /// The first byte is the header
                m_Header        = p_String[0];
                m_Payload       = Core::Utils::SplitAll(p_String, "|", false); ///< Header is also apart of the payload
                m_ReadPosition  = 0;

                /// Skip header, it's in payload
                ReadSkip();
            }
            /// Deconstructor
            ~ClientPacket()
            {

            }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// String to UInt32
            const uint32 ReadUInt32()
            {
                return std::stoul(m_Payload[m_ReadPosition++]);
            }
            /// String to Int32
            const int32 ReadInt32()
            {
                return std::stoi(m_Payload[m_ReadPosition++]);
            }
            /// String to UInt64
            const int64 ReadUInt64()
            {
                return std::stoull(m_Payload[m_ReadPosition++]);
            }
            /// String to Int64
            const int64 ReadInt64()
            {
                return std::stoll(m_Payload[m_ReadPosition++]);
            }
            /// String to Double
            const int64 ReadDouble()
            {
                return std::stod(m_Payload[m_ReadPosition++]);
            }
            /// String to Float
            const int64 ReadFloat()
            {
                return std::stof(m_Payload[m_ReadPosition++]);
            }
            /// String
            const std::string ReadString()
            {
                return m_Payload[m_ReadPosition++];
            }
            /// Skip to next read position
            void ReadSkip()
            {
                m_ReadPosition++;
            }

            /// Get Packet Header
            uint8 GetHeader() const
            {
                return m_Header;
            }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            uint8 m_Header;                     ///< Header
            std::vector<std::string> m_Payload; ///< Payload
            uint32 m_ReadPosition;              ///< Read Position
    };
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone