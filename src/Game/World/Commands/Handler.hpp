#include <iostream>
#include <map>
#include <functional>
#include "json.hpp"

#include "Player.hpp"

namespace SteerStone { namespace Game { namespace World {
	class Handler
	{
	public:
		/// Constructor
		Handler();
		/// Deconstructor
		~Handler();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	public:
		/// Handle Command
		/// @p_Command : Command to handle
		/// @p_Player : Player who sent the command
		/// @p_Arguments : Arguments for the command
		void HandleCommand(const std::string& p_Command, Entity::Player* p_Player, nlohmann::json const& p_Arguments = nlohmann::json());

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	private:
		/// Save Player to Database
		/// @p_Player : Player to save
		/// @p_Arguments : Arguments for the command
		void Save(Entity::Player* p_Player, nlohmann::json const& p_Arguments);
		/// Teleport Player
		/// @p_Player : Player to teleport
		/// @p_Arguments : Arguments for the command
		void Teleport(Entity::Player* p_Player, nlohmann::json const& p_Arguments);

	private:
		std::map<std::string, std::function<void(Entity::Player*, nlohmann::json const&)>> m_Commands;
	};

} ///< namespace World
} ///< namespace Game
} ///< namespace SteerStone