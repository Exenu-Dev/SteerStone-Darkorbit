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
#include <time.h>
#include <iomanip>

#include "ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Server { namespace Packets {

    /// Write the packet
	PacketBuffer const* SendMessageToRoom::Write()
	{
        m_Buffer.AppendUInt16(RoomId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
	}

    /// Write the packet
    PacketBuffer const* DeveloperMessage::Write()
    {
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* SendAdminMessage::Write()
    {
        m_Buffer.AppendUInt16(RoomId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar("1"); //< Admin message type? TODO; Research this
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* Ping::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* BanUser::Write()
    {
        std::string l_Message = "Your banned";
        m_Buffer.AppendChar(l_Message.c_str());
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* IPBan::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// Write the packet
    PacketBuffer const* BanMessage::Write()
    {
        struct std::tm l_TM;
        std::istringstream l_Stream(StartedAt);
        l_Stream >> std::get_time(&l_TM, "%d/%m/%Y %H:%M:%S");
        std::time_t l_StartTime = mktime(&l_TM);

        char l_StartDateString[26];
        std::strftime(l_StartDateString, sizeof l_StartDateString, "%d %b %Y", localtime(&l_StartTime));

        std::istringstream l_StreamEndsAt(EndsAt);
        l_StreamEndsAt >> std::get_time(&l_TM, "%d/%m/%Y %H:%M:%S");
        std::time_t l_EndTime = mktime(&l_TM);

        char l_EndDateString[26];
        std::strftime(l_EndDateString, sizeof l_EndDateString, "%d %b %Y", localtime(&l_EndTime));

        char l_EndTimeString[26];
        std::strftime(l_EndTimeString, sizeof l_EndTimeString, "%H:%M", localtime(&l_EndTime));

        std::time_t l_TimeLeft = l_EndTime - l_StartTime;
        l_TimeLeft = l_TimeLeft / 60; ///< Transform to minutes

        m_Buffer.AppendChar(l_StartDateString); ///< Banned on
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(l_EndDateString); ///< Ban ends without time
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(l_StartDateString); ///< Banned start
        m_Buffer.AppendSplit(); 
        m_Buffer.AppendChar(l_EndTimeString); ///< Time in hours ban ends
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(std::to_string(l_TimeLeft).c_str()); ///< Hours
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(" "); // unknown
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    PacketBuffer const* KickUser::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }

    /// SERVER_PACKET_SET_ROOMLIST
    PacketBuffer const* SetRoomList::Write()
    {
        for (auto l_Itr : Rooms)
        {
            m_Buffer.AppendUInt32(l_Itr.RoomId);
            m_Buffer.AppendSplit(true);
            m_Buffer.AppendChar(l_Itr.RoomName.c_str());
            m_Buffer.AppendSplit(true);
            m_Buffer.AppendInt32(l_Itr.TabOrder);
            m_Buffer.AppendSplit(true);
            m_Buffer.AppendInt32(static_cast<int32>(l_Itr.Company));
            m_Buffer.AppendSplit(true);
            m_Buffer.AppendUInt32(static_cast<uint32>(l_Itr.RoomType));
            m_Buffer.AppendSplit(true);
            m_Buffer.AppendBool(l_Itr.NewComerRoom);

            m_Buffer.AppendChar("}");
        }
        
        m_Buffer.PopBack();

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* RoomStatusMessage::Write(RoomStatusType p_RoomStatusType)
    {
        switch (p_RoomStatusType)
        {
            case RoomStatusType::CMD_PRIVATE_ROOM_EXIST:
                m_Buffer.AppendChar("i");
                break;
            case RoomStatusType::CMD_PRIVATE_ROOM_NOT_EXIST:
                m_Buffer.AppendChar("l");
				break;
        }

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* PrivateRoomCreated::Write()
    {
        m_Buffer.AppendUInt32(RoomId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(RoomName.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendUInt32(OwnerId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(OwnerName.c_str());

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserLogin::Write()
    {
        m_Buffer.AppendUInt32(UserId);

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* DeleteRoom::Write()
    {
        m_Buffer.AppendUInt32(RoomId);

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* NotRoomOwner::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* CannotLeaveRoom::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* RoomNameTooShort::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* RoomNameTooLong::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserInviteNotFound::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserInviteNotYourRoom::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* CannotInviteYourSelf::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserJoinedInvitedRoom::Write()
    {
        m_Buffer.AppendChar(Username.c_str());

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserLeftInvitedRoom::Write()
    {
        m_Buffer.AppendUInt32(UserId);
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendUInt32(RoomId);

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* RoomLimitReached::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* CreateRoomWrongArguments::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserNotExist::Write()
    {
        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* YouWhisper::Write()
    {
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
    PacketBuffer const* UserWhispers::Write()
    {
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendSplit();
        m_Buffer.AppendChar(Message.c_str());

        m_Buffer.AppendChar("#");
        m_Buffer.AppendEndSplitter();

        return &m_Buffer;
    }
}   ///< Packets
}   ///< Server
}   ///< Chat
}   ///< SteerStone