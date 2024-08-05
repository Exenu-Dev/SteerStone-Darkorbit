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
#include "Common.hpp"
#include "Socket.hpp"

namespace SteerStone { namespace Chat { namespace Server { namespace Packets {

    enum RoomStatusType
    {
        CMD_PRIVATE_ROOM_EXIST      = 0,
        CMD_PRIVATE_ROOM_NOT_EXIST  = 1,
    };

    class UserLogin final : public ServerPacket
    {
    public:
        /// Constructor 
        UserLogin() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_USER_LOGIN).c_str())
        {
            UserId = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 UserId;
    };

    class SendMessageToRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        SendMessageToRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_MESSAGE).c_str())
        {
            RoomId = 0;
            Message = "";
            Username = "";
            Clan = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
        std::string Message;
        std::string Username;
        std::string Clan;
    };

    class DeveloperMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        DeveloperMessage() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_DEVELOPER_MESSAGE).c_str())
        {
            Message = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Message;
    };

    class SendAdminMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        SendAdminMessage() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_ADMIN_MESSAGE).c_str())
        {
            RoomId = 0;
            Message = "";
            Username = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
        std::string Message;
        std::string Username;
    };

    class BanUser final : public ServerPacket
    {
    public:
        /// Constructor 
        BanUser() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_BAN_USER).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class IPBan final : public ServerPacket
    {
    public:
        /// Constructor 
        IPBan() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_IP_BAN_MESSAGE).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class BanMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        BanMessage() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_BAN_MESSAGE).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string StartedAt;
        std::string EndsAt;
    };

    class KickUser final : public ServerPacket
    {
    public:
        /// Constructor 
        KickUser() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_KICK_USER).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class Ping final : public ServerPacket
    {
    public:
        /// Constructor 
        Ping() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SEND_PING).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    struct Room
    {
        Room(uint32 p_RoomId = 0, std::string p_RoomName = "", int32 p_TabOrder = -1, Company p_Company = Company::NOMAD, RoomType p_RoomType = RoomType::ROOM_TYPE_NORMAL, bool p_NewComerRoom = false)
			: RoomId(p_RoomId), RoomName(p_RoomName), TabOrder(p_TabOrder), Company(p_Company), RoomType(p_RoomType), NewComerRoom(p_NewComerRoom) {}

        uint32 RoomId;
        std::string RoomName;
        int32 TabOrder;
        Company Company;
        RoomType RoomType;
        bool NewComerRoom;
    };

    class SetRoomList final : public ServerPacket
    {
    public:
        /// Constructor 
        SetRoomList() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_SET_ROOMLIST).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::vector<Room> Rooms;
    };

    /// SERVER_PACKET_ROOM_STATUS_MESSAGE
    class RoomStatusMessage final : public ServerPacket
    {
    public:
        /// Constructor 
        RoomStatusMessage() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_STATUS_MESSAGE).c_str(), false)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write(RoomStatusType p_RoomStatusType);
    };

    /// SERVER_PACKET_PRIVATE_ROOM_CREATED
    class PrivateRoomCreated final : public ServerPacket
    {
    public:
        /// Constructor 
        PrivateRoomCreated() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_PRIVATE_ROOM_CREATED).c_str())
        {
            RoomId      = 0;
            RoomName    = "";
            OwnerId     = 0;
            OwnerName   = "";
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
        std::string RoomName;
        uint32 OwnerId;
        std::string OwnerName;
    };

    /// SERVER_PACKET_DELETE_ROOM
    class DeleteRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        DeleteRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_DELETE_ROOM).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 RoomId;
    };

    class NotRoomOwner final : public ServerPacket
    {
    public:
        /// Constructor 
        NotRoomOwner() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_NOT_ROOM_OWNER).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class CannotLeaveRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        CannotLeaveRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_CANNOT_LEAVE_ROOM).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class RoomNameTooShort final : public ServerPacket
    {
    public:
        /// Constructor 
        RoomNameTooShort() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_SHORT).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class RoomNameTooLong final : public ServerPacket
    {
    public:
        /// Constructor 
        RoomNameTooLong() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_LONG).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class UserInviteNotFound final : public ServerPacket
    {
    public:
        /// Constructor 
        UserInviteNotFound() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_INVITE_USER_NOT_FOUND).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class UserInviteNotYourRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        UserInviteNotYourRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_INVITE_NOT_YOUR_ROOM).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class CannotInviteYourSelf final : public ServerPacket
    {
    public:
        /// Constructor 
        CannotInviteYourSelf() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_ROOM_CANNOT_INVITE_YOURSELF).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class UserJoinedInvitedRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        UserJoinedInvitedRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_USER_JOINED_INVITED_ROOM).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Username;
    };

    class UserLeftInvitedRoom final : public ServerPacket
    {
    public:
        /// Constructor 
        UserLeftInvitedRoom() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_USER_LEFT_INVITED_ROOM).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 UserId;
        std::string Username;
        uint32 RoomId;
    };

    class RoomLimitReached final : public ServerPacket
    {
    public:
        /// Constructor 
        RoomLimitReached() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_NO_MORE_PRIVATE_ROOMS).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class CreateRoomWrongArguments final : public ServerPacket
    {
    public:
        /// Constructor 
        CreateRoomWrongArguments() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_CREATE_ROOM_WRONG_ARGUMENTS).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class UserNotExist final : public ServerPacket
    {
    public:
        /// Constructor 
        UserNotExist() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_USER_DOES_NOT_EXIST).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    class YouWhisper final : public ServerPacket
    {
    public:
        /// Constructor 
        YouWhisper() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_YOU_WHISPER).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Username;
        std::string Message;
    };

    class UserWhispers final : public ServerPacket
    {
    public:
        /// Constructor 
        UserWhispers() : ServerPacket(sOpCode->GetServerPacketHeader(ServerOpCodes::SERVER_PACKET_USER_WHISPERS_YOU).c_str())
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        std::string Username;
        std::string Message;
    };

}   ///< Packets
}   ///< Server
}   ///< Chat
}   ///< SteerStone