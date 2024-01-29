#include <sstream>

#include "Handler.hpp"

namespace SteerStone { namespace Game { namespace World {

	/// Constructor
	Handler::Handler()
	{
		m_Commands["teleport"] = std::bind(&Handler::Teleport, this, std::placeholders::_1, std::placeholders::_2);
		m_Commands["save"] = std::bind(&Handler::Save, this, std::placeholders::_1, std::placeholders::_2);
	}

	/// Deconstructor
	Handler::~Handler()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// Handle Command
	/// @p_Command : Command to handle
	/// @p_Player : Player who sent the command
	/// @p_Arguments : Arguments for the command
	void Handler::HandleCommand(const std::string& p_Command, Entity::Player* p_Player, nlohmann::json const& p_Arguments/*= nlohmann::json()*/)
	{
		auto l_CommandItr = m_Commands.find(p_Command);
		if (l_CommandItr != m_Commands.end())
			l_CommandItr->second(p_Player, p_Arguments);
		else
			p_Player->SendInfoMessage("Command not found");
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/// Save Player to Database
	/// @p_Player : Player to save
	/// @p_Arguments : Arguments for the command
	void Handler::Save(Entity::Player* p_Player, nlohmann::json const& p_Arguments)
	{
		p_Player->SaveToDB();
	}

	/// Teleport Player
	/// @p_Player : Player to teleport
	/// @p_Arguments : Arguments for the command
	void Handler::Teleport(Entity::Player* p_Player, nlohmann::json const& p_Arguments)
	{
		const auto l_MapId = p_Arguments[0].get<std::string>();

		p_Player->Teleport(std::stoi(l_MapId));

		p_Player->SendInfoMessage("You will be teleported in 5 seconds");
	}
} ///< namespace World
} ///< namespace Game
} ///< namespace SteerStone