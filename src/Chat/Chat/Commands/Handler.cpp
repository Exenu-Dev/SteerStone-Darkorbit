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
	void Handler::HandleInput(const std::string& p_Input, Entity::Player const* p_Player)
	{
		auto [l_Command, l_Arguments] = ParseCommand(p_Input);

		HandleCommand(l_Command, p_Player, l_Arguments);
	}

	/// Handle Command
	/// @p_Command : Command to handle
	/// @p_Player : Player who sent the command
	/// @p_Arguments : Arguments for the command
	void Handler::HandleCommand(const std::string& p_Command, Entity::Player const* p_Player, std::vector<std::string>& p_Arguments)
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
	void Handler::Commands(Entity::Player const* p_Player, const std::vector<std::string>& p_Args)
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
	void Handler::UsersTotal(Entity::Player const* p_Player, const std::vector<std::string>& p_Args /*= std::vector<std::string>()*/)
	{
		Server::Packets::UserCount l_Packet;
		l_Packet.Total = sChatManager->TotalPlayers();
	}

	/// Send an announcement to all players
	/// @p_Player : Player who sent the command
	/// @p_Args : Arguments for the command
	void Handler::Announce(Entity::Player const* p_Player, const std::vector<std::string>& p_Args)
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
	void Handler::Ban(Entity::Player const* p_Player, const std::vector<std::string>& p_Args)
	{
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
	void Handler::Save(Entity::Player const* p_Player, const std::vector<std::string>& p_Args)
	{
		sChatManager->AddProcessCommand(p_Player->GetId(), "save");
	}
	/// Teleport Player
	/// @p_Player : Player to teleport
	/// @p_Arguments : Arguments for the command
	void Handler::Teleport(Entity::Player const* p_Player, const std::vector<std::string>& p_Args)
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

		nlohmann::json l_Json = p_Args;

		sChatManager->AddProcessCommand(p_Player->GetId(), "teleport", l_Json);
	}
} ///< namespace Commands
} ///< namespace Chat
} ///< namespace SteerStone