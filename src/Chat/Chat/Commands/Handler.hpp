#include <iostream>
#include <map>
#include <functional>

#include "Player.hpp"

namespace SteerStone { namespace Chat { namespace Commands {

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
		/// Handle Incoming Input
		/// @p_Input : Input to handle
		/// @p_Player : Player who sent the input
		void HandleInput(const std::string& p_Input, Entity::Player* p_Player);

	private:
		/// Handle Command
		/// @p_Command : Command to handle
		/// @p_Player : Player who sent the command
		/// @p_Arguments : Arguments for the command
		void HandleCommand(const std::string& p_Command, Entity::Player* p_Player, std::vector<std::string>& p_Arguments);

		/// Parse Command
		/// @p_Input : Input to parse
		std::pair<std::string, std::vector<std::string>> ParseCommand(const std::string& p_Input);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	private:
		/// List all commands
		/// @p_Player : Player who sent the command
		/// @p_Args : Arguments for the command
		void Commands(Entity::Player* p_Player, const std::vector<std::string>& p_Args = std::vector<std::string>());
		/// Show the current online users
		/// @p_Player : Player who sent the command
		/// @p_Args : Arguments for the command
		void UsersTotal(Entity::Player* p_Player, const std::vector<std::string>& p_Args = std::vector<std::string>());
		/// Send an announcement to all players
		/// @p_Player : Player who sent the command
		/// @p_Args : Arguments for the command
		void Announce(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Ban a player
		/// @p_Player : Player who sent the command
		/// @p_Args : Arguments for the command
		void Ban(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Save Player to Database
		/// @p_Player : Player to save
		/// @p_Arguments : Arguments for the command
		void Save(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Kick Player
		/// @p_Player : Player to kick
		void Kick(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Teleport Player
		/// @p_Player : Player to teleport
		/// @p_Arguments : Arguments for the command
		void Teleport(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Set Player Speed
		/// @p_Player : Player to set speed
		void Speed(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Create Room
		/// @p_Player : Player to create room
		/// @p_Args : Arguments for the command
		void CreateRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Close Room
		/// @p_Player : Player to close room
		/// @p_Args : Arguments for the command
		void CloseRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Leave Room
		/// @p_Player : Player to leave room
		/// @p_Args : Arguments for the command
		void LeaveRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Invite Room
		/// @p_Player : Player who initiated the command
		/// @p_Args : Arguments for the command
		void InviteRoom(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Whisper Player
		/// @p_Player : Player who initiated the command
		/// @p_Args : Arguments for the command
		void Whisper(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Ignore Player
		/// @p_Player : Player who initiated the command
		/// @p_Args : Arguments for the command
		void Ignore(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Unignore Player
		/// @p_Player : Player who initiated the command
		/// @p_Args : Arguments for the command
		void UnIgnore(Entity::Player* p_Player, const std::vector<std::string>& p_Args);
		/// Users
		/// @p_Player : Player who initiated the command
		/// @p_Args : Arguments for the command
		void Users(Entity::Player* p_Player, const std::vector<std::string>& p_Args);

	private:
		std::map<std::string, std::function<void(Entity::Player*, std::vector<std::string>&)>> m_Commands;
	};

} ///< namespace Commands
} ///< namespace Chat
} ///< namespace SteerStone