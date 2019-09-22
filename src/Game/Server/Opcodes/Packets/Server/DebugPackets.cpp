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

#include "MiscPackets.hpp"
#include "DebugPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {
    
    /// SERVER_PACKET_HEADLESS_MOVE
    PacketBuffer const* DebugHeadlessMove::Write()
    {
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone