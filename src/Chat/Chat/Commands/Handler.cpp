#include <sstream>

#include "ChatManager.hpp"
#include "Packets/Server/ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Commands {

	/// Constructor
	Handler::Handler()
	{
		m_Commands["commands"] = std::bind(&Handler::Commands, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["users"] = std::bind(&Handler::UsersTotal, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["announce"] = std::bind(&Handler::Announce, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["ban"] = std::bind(&Handler::Ban, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["teleport"] = std::bind(&Handler::Teleport, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["save"] = std::bind(&Handler::Save, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["kick"] = std::bind(&Handler::Kick, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["speed"] = std::bind(&Handler::Speed, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["create"] = std::bind(&Handler::CreateRoom, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["close"] = std::bind(&Handler::CloseRoom, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["leave"] = std::bind(&Handler::LeaveRoom, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["invite"] = std::bind(&Handler::InviteRoom, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["w"] = std::bind(&Handler::Whisper, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["ignore"] = std::bind(&Handler::Ignore, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["allow"] = std::bind(&Handler::UnIgnore, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["users"] = std::bind(&Handler::Users, this, std::placeholders::_1, std::placeholders::_2);
	}

	/// Deconstructor
	Handler::~Handler()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// Handle Incoming Input
	/// @p_Input : Input to handle
	/// @p_Player : Player who sent the input
	void Handler::HandleInput(const std::string& p_Input, Entity::Player* p_Player)
	{
		auto [l_Command, l_Arguments] = ParseCommand(p_Input);

		HandleCommand(l_Command, p_Player, l_Arguments);
	}

	/// Handle Command
	/// @p_Command : Command to handle
	/// @p_Player : Player who sent the command
	/// @p_Arguments : Arguments for the command
	void Handler::HandleCommand(const std::string& p_Command, Entity::Player* p_Player, std::vector<std::string>& p_Arguments)
	{
		auto l_CommandItr = m_Commands.find(p_Command);
		if (l_CommandItr != m_Commands.end())
			l_CommandItr->second(p_Player, p_Arguments);
		else
			p_Player->SendMessageToSelf("Command not found");
	}

	/// Parse Command
	/// @p_Input : Input to parse
	std::pair<std::string, std::vector<std::string>> Handler::ParseCommand(const std::string& p_Input)
	{
		std::istringstream l_Stream(p_Input);
		std::string l_Command;
		std::vector<std::string> l_Arguments;

		// First word is the command
		l_Stream >> l_Command;

		// Rest of the words are arguments
		std::string l_Argument;
		while (l_Stream >> l_Argument)
			l_Arguments.push_back(l_Argument);

		return { l_Command, l_Arguments };
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// List all commands
	/// @p_Player : Player who sent the command
	/// @p_Args : Arguments for the command
	void Handler::Commands(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		std::string l_Commands = "\n";
		for (auto l_Command : m_Commands)
		{
			if (l_Command.first == "commands")
				continue;

			l_Commands += l_Command.first + "\n";
		}

		p_Player->SendMessageToSelf("<b>Commands:<b/> " + l_Commands);
	}

	/// Show the current online users
	/// @p_Player : Player who sent the command
	/// @p_Args : Arguments for the command
	void Handler::UsersTotal(Entity::Player* p_Player, const std::vector<std::string>& p_Args /*= std::vector<std::string>()*/)
	{
		// sChatManager->TotalPlayers();
	}

	/// Send an announcement to all players
	/// @p_Player : Player who sent the command
	/// @p_Args : Arguments for the command
	void Handler::Announce(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
		{
			p_Player->SendMessageToSelf("Usage: /announce <message>");
			return;
		}

		std::string l_Message;
		for (auto l_Argument : p_Args)
			l_Message += l_Argument + " ";

		sChatManager->SendSystemMessage(l_Message);
	}

	/// Ban a player
	/// @p_Player : Player who sent the command
	/// @p_Args : Arguments for the command
	void Handler::Ban(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		return; //< Not supported at the moment
		if (p_Args.size() < 3)
		{
			p_Player->SendMessageToSelf("Usage: /ban <username> <reason> <days>");
			return;
		}

		if (!p_Player->IsAdmin())
		{
			p_Player->SendMessageToSelf("You do not have permission to use this command");
			return;
		}

		std::string l_Username = p_Args[0];
		std::string l_Reason = p_Args[1];
		std::string l_Days = p_Args[2];

		sChatManager->BanPlayer(l_Username, p_Player, l_Reason, l_Days);
	}
	/// Save Player to Database
	/// @p_Player : Player to save
	/// @p_Arguments : Arguments for the command
	void Handler::Save(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		p_Player->SendMessageToSelf("Successfully saved");

		sChatManager->AddProcessCommand(p_Player->GetId(), "save");
	}
	/// Kick Player
	/// @p_Player : Player to kick
	void Handler::Kick(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		return; //< Not supported at the moment
		if (p_Args.size() < 1)
		{
			p_Player->SendMessageToSelf("Usage: /kick <username>");
			return;
		}

		if (!p_Player->IsAdmin())
		{
			p_Player->SendMessageToSelf("You do not have permission to use this command");
			return;
		}

		const std::string l_Username = p_Args[0];

		if (Entity::Player const* l_Player = sChatManager->FindPlayerByUsername(l_Username))
		{
			/// Send Kick Packet
			Server::Packets::KickUser l_KickPacket;
			l_Player->SendPacket(l_KickPacket.Write());

			nlohmann::json l_Json;
			l_Json.push_back(l_Player->GetId());

			sChatManager->AddProcessCommand(p_Player->GetId(), "kick", l_Json);
			return;
		}

		p_Player->SendMessageToSelf("Player not found");
	}
	/// Teleport Player
	/// @p_Player : Player to teleport
	/// @p_Arguments : Arguments for the command
	void Handler::Teleport(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
		{
			p_Player->SendMessageToSelf("Usage: /teleport <mapid>");
			return;
		}

		if (!p_Player->IsAdmin())
		{
			p_Player->SendMessageToSelf("You do not have permission to use this command");
			return;
		}

		const int32 l_MapId = std::stoi(p_Args[0]);

		if (l_MapId <= 0 || l_MapId > 29)
		{
			p_Player->SendMessageToSelf("Invalid map id");
			return;
		}

		nlohmann::json l_Json = p_Args;

		sChatManager->AddProcessCommand(p_Player->GetId(), "teleport", l_Json);
	}
	/// Set Player Speed
	/// @p_Player : Player to set speed
	void Handler::Speed(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
		{
			p_Player->SendMessageToSelf("Usage: /speed <speed>");
			return;
		}

		if (!p_Player->IsAdmin())
		{
			p_Player->SendMessageToSelf("You do not have permission to use this command");
			return;
		}

		// Check if the speed is a valid number
		if (!std::all_of(p_Args[0].begin(), p_Args[0].end(), ::isdigit))
		{
			p_Player->SendMessageToSelf("Invalid speed");
			return;
		}

		const int32 l_Speed = std::stoi(p_Args[0]);

		if (l_Speed <= 0 || l_Speed > 1000)
		{
			p_Player->SendMessageToSelf("Invalid speed");
			return;
		}

		nlohmann::json l_Json;
		l_Json[0] = l_Speed;

		sChatManager->AddProcessCommand(p_Player->GetId(), "speed", l_Json);
	
	}
	/// Create Room
	/// @p_Player : Player to create room
	/// @p_Args : Arguments for the command
	void Handler::CreateRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
		{
			p_Player->SendPacket(Server::Packets::CreateRoomWrongArguments().Write());
			return;
		}

		const std::string l_RoomName = p_Args[0];

		if (l_RoomName.size() < 3)
		{
			p_Player->SendPacket(Server::Packets::RoomNameTooShort().Write());
			return;
		}

		if (l_RoomName.size() > 20)
		{
			p_Player->SendPacket(Server::Packets::RoomNameTooLong().Write());
			return;
		}

		uint32 l_OwnerOfPrivateRoomsCount = 0;

		for (auto l_Itr : p_Player->GetRooms())
		{
			if (l_Itr.second->IsPrivateRoom() && l_Itr.second->GetOwner() == p_Player)
				l_OwnerOfPrivateRoomsCount++;
		}

		if (l_OwnerOfPrivateRoomsCount >= 3)
		{
			p_Player->SendPacket(Server::Packets::RoomLimitReached().Write());
			return;
		}

		if (sChatManager->RoomExistsByName(l_RoomName))
		{
			p_Player->SendPacket(Server::Packets::RoomStatusMessage().Write(Server::Packets::RoomStatusType::CMD_PRIVATE_ROOM_EXIST));
			return;
		}

		// Generate random room id
		uint16 l_RoomId = sChatManager->GenerateRoomId();
		int16 l_TabOrder = 5; //< Not sure about this

		Chat::Channel::Room* l_Room = new Chat::Channel::Room(
			l_RoomId,
			l_RoomName,
			l_TabOrder,
			p_Player->GetCompany(),
			RoomType::ROOM_TYPE_PRIVATE,
			p_Player
		);

		sChatManager->AddRoom(l_Room);
		p_Player->AddRoom(l_Room);
	}
	/// Close Room
	/// @p_Player : Player to close room
	/// @p_Args : Arguments for the command
	void Handler::CloseRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		Chat::Channel::Room* l_Room = p_Player->CurrentRoom();

		if (!l_Room)
		{
			p_Player->SendPacket(Server::Packets::RoomStatusMessage().Write(Server::Packets::RoomStatusType::CMD_PRIVATE_ROOM_NOT_EXIST));
			LOG_WARNING("Chat", "Player is not in a room");
			return;
		}

		if (l_Room->GetOwner() != p_Player)
		{
			p_Player->SendPacket(Server::Packets::NotRoomOwner().Write());
			LOG_WARNING("Chat", "Player is not the owner of the room");
			return;
		}

		p_Player->CloseRoom(l_Room->GetId());
	}
	/// Leave Room
	/// @p_Player : Player to leave room
	/// @p_Args : Arguments for the command
	void Handler::LeaveRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		Chat::Channel::Room* l_Room = p_Player->CurrentRoom();

		if (!l_Room)
		{
			p_Player->SendPacket(Server::Packets::RoomStatusMessage().Write(Server::Packets::RoomStatusType::CMD_PRIVATE_ROOM_NOT_EXIST));
			LOG_WARNING("Chat", "Player is not in a room");
			return;
		}

		if (l_Room->GetOwner() == p_Player || !l_Room->IsPrivateRoom())
		{
			p_Player->SendPacket(Server::Packets::CannotLeaveRoom().Write());
			LOG_WARNING("Chat", "Player is the owner of the room");
			return;
		}

		p_Player->LeaveRoom(l_Room->GetId());
	}
	/// Invite Room
	/// @p_Player : Player who initiated the command
	/// @p_Args : Arguments for the command
	void Handler::InviteRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
		{
			p_Player->SendMessageToSelf("Usage: /invite <username>");
			return;
		}

		const std::string l_Username = p_Args[0];

		if (Entity::Player const* l_Player = sChatManager->FindPlayerByUsername(l_Username))
		{
			Chat::Channel::Room* l_Room = p_Player->CurrentRoom();

			if (!l_Room)
			{
				p_Player->SendPacket(Server::Packets::RoomStatusMessage().Write(Server::Packets::RoomStatusType::CMD_PRIVATE_ROOM_NOT_EXIST));
				LOG_WARNING("Chat", "Player is not in a room");
				return;
			}

			if (!l_Room->IsPrivateRoom())
			{
				p_Player->SendPacket(Server::Packets::UserInviteNotYourRoom().Write());
				LOG_WARNING("Chat", "Player is not in a room");
				return;
			}

			if (l_Room->GetOwner() != p_Player)
			{
				p_Player->SendPacket(Server::Packets::UserInviteNotYourRoom().Write());
				LOG_WARNING("Chat", "Player is not the owner of the room");
				return;
			}

			if (l_Room->GetOwner() == l_Player)
			{
				p_Player->SendPacket(Server::Packets::CannotInviteYourSelf().Write());
				LOG_WARNING("Chat", "Player is inviting themselves");
				return;
			}

			if (l_Room->IsInRoom(l_Player))
			{
				/// Couldn't find a packet to handle this
				LOG_WARNING("Chat", "Player is already in the room");
				return;
			}

			l_Room->InvitePlayer(const_cast<Entity::Player*>(l_Player));
			return;
		}

		p_Player->SendPacket(Server::Packets::UserInviteNotFound().Write());
	}
	/// Whisper Player
	/// @p_Player : Player who initiated the command
	/// @p_Args : Arguments for the command
	void Handler::Whisper(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 2)
			return;

		const std::string l_Username = p_Args[0];
		const std::string l_Message = p_Args[1];

		if (Entity::Player const* l_Player = sChatManager->FindPlayerByUsername(l_Username))
		{
			p_Player->Whisper(l_Player, l_Message);
			return;
		}

		p_Player->SendPacket(Server::Packets::UserNotExist().Write());
	}
	/// Ignore Player
	/// @p_Player : Player who initiated the command
	/// @p_Args : Arguments for the command
	void Handler::Ignore(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
			return;

		const std::string l_Username = p_Args[0];

		if (Entity::Player const* l_Player = sChatManager->FindPlayerByUsername(l_Username))
		{
			p_Player->IgnorePlayer(l_Player);
			return;
		}

		p_Player->SendPacket(Server::Packets::UserNotExist().Write());
	}
	/// Unignore Player
	/// @p_Player : Player who initiated the command
	/// @p_Args : Arguments for the command
	void Handler::UnIgnore(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		if (p_Args.size() < 1)
			return;

		const std::string l_Username = p_Args[0];

		if (Entity::Player const* l_Player = sChatManager->FindPlayerByUsername(l_Username))
		{
			p_Player->UnIgnorePlayer(l_Player);
			return;
		}

		p_Player->SendPacket(Server::Packets::UserNotExist().Write());
	}
	/// Users
	/// @p_Player : Player who initiated the command
	/// @p_Args : Arguments for the command
	void Handler::Users(Entity::Player* p_Player, const std::vector<std::string>& p_Args)
	{
		Chat::Channel::Room* l_Room = p_Player->CurrentRoom();

		if (!l_Room)
		{
			LOG_WARNING("Chat", "Player is not in a room");
			return;
		}

		/// TODO: Turn this into a packet handler
		Server::PacketBuffer l_PacketBuffer;
		l_PacketBuffer.AppendChar("df");
		l_PacketBuffer.AppendChar("%");

		for (auto l_Itr : l_Room->GetRoomPlayers())
		{
			std::string l_ClanName = l_Itr.second->ClanTag();

			if (!l_ClanName.length())
				l_ClanName = "noclan";

			l_PacketBuffer.AppendChar(l_Itr.second->GetUsername().c_str());
			l_PacketBuffer.AppendChar("|");
			l_PacketBuffer.AppendChar(l_ClanName.c_str());
			l_PacketBuffer.AppendChar("}");
		}

		l_PacketBuffer.PopBack();

		l_PacketBuffer.AppendChar("#");
		l_PacketBuffer.AppendEndSplitter();
		p_Player->SendPacket(&l_PacketBuffer);

	}
} ///< namespace Commands
} ///< namespace Chat
} ///< namespace SteerStone