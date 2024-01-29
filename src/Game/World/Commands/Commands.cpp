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

#include "json.hpp"

#include "World.hpp"
#include "Player.hpp"
#include "Commands.hpp"

namespace SteerStone { namespace Game { namespace World {

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Commands::Commands()
    {
        m_CommandsHandler = new Handler();
    }
    /// Deconstructor
    Commands::~Commands()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //              COMMANDS
    ///////////////////////////////////////////

    /// Process the commands
    /// Note: Currently the implementation is very basic
    /// Ideally we need to move the commands into their own classes
    /// Look how the chat server implements the commands
    void Commands::ProcessCommands()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, type, arguments, user_id FROM process_commands WHERE processed = ?");
        l_PreparedStatement->SetBool(0, false);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            do
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                const uint64 l_Id                   = l_Result[0].GetUInt64();
                const std::string l_Type            = l_Result[1].GetString();
                const nlohmann::json l_Arguments    = l_Result[2].GetString().length() ? nlohmann::json::parse(l_Result[2].GetString()) : nlohmann::json();
                const uint64 l_UserId               = l_Result[3].GetUInt64();

                Entity::Player* l_Player = sWorldManager->FindPlayer(l_UserId);

                if (!l_Player)
                {
                    /// Player is not online, so set the command as processed
                    SetCommandAsProcessed(l_Id);
                }
                else
                {
                    m_CommandsHandler->HandleCommand(l_Type, l_Player, l_Arguments);
                    SetCommandAsProcessed(l_Id);
                }

            } while (l_PreparedResultSet->GetNextRow());
        }
    }
    /// Set the command as processed
    /// @p_Id : The Id of the record
    void Commands::SetCommandAsProcessed(const uint64 p_Id)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("UPDATE process_commands SET processed = ? WHERE id = ?");
        l_PreparedStatement->SetBool(0, true);
        l_PreparedStatement->SetUint64(1, p_Id);
        l_PreparedStatement->ExecuteStatement(true);
    }

    /// Clear the commands
    /// This is primarily used when the server is booting up
    /// We don't want to process any commands that were left over from the previous session
    void Commands::ClearCommands()
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
		l_PreparedStatement->PrepareStatement("DELETE FROM process_commands");
		l_PreparedStatement->ExecuteStatement(true);
    }

}   ///< namespace World
}   ///< namespace Game
}   ///< namespace Steerstone