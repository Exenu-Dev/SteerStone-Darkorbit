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

#include "Database/Database.hpp"
#include "Database/SQLCommon.hpp"
#include "Utility/UtiString.hpp"
#include "Threading/ThrTaskManager.hpp"

namespace SteerStone { namespace Core { namespace Database {

    /// Constructor
    Base::Base()
    {
    }

    /// Deconstructor
    Base::~Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// @p_InfoString : Database user details; username, password, host, database, l_Port
    /// @p_PoolSize : How many pool connections database will launch
    uint32 Base::Start(char const* p_InfoString, uint32 p_PoolSize)
    {
        /// Check if pool size is within our requirements
        if (p_PoolSize < MIN_CONNECTION_POOL_SIZE)
            p_PoolSize = MIN_CONNECTION_POOL_SIZE;
        else if (p_PoolSize > MAX_CONNECTION_POOL_SIZE)
            p_PoolSize = MAX_CONNECTION_POOL_SIZE;
        else
            p_PoolSize = p_PoolSize;

        std::string l_Username;
        std::string l_Password;
        std::string l_Database;
        std::string l_Host;
        std::string l_Port;

        std::vector<std::string> l_Tokens = Utils::SplitAll(p_InfoString, ";", false);

        auto& l_Itr = l_Tokens.begin();

        if (l_Itr != l_Tokens.end())
            l_Host = *l_Itr++;
        if (l_Itr != l_Tokens.end())
            l_Port = *l_Itr++;
        if (l_Itr != l_Tokens.end())
            l_Username = *l_Itr++;
        if (l_Itr != l_Tokens.end())
            l_Password = *l_Itr++;
        if (l_Itr != l_Tokens.end())
            l_Database = *l_Itr++;

        if (!Connect(l_Username, l_Password, std::stoi(l_Port), l_Host, l_Database, p_PoolSize, *this))
        {
            Threading::TaskManager::GetSingleton()->PushRunOnceTask("DATABASE_WORKER_THREAD", std::bind(&Base::ProcessOperators, this));
            return true;
        }
        else
            return false;
    }

    /// Returns a Prepare Statement from Pool
    PreparedStatement* Base::GetPrepareStatement()
    {
       return Prepare();
    }
    /// @p_PreparedStatement : Connection we are freeing
    void Base::FreePrepareStatement(PreparedStatement* p_PreparedStatement)
    {
        Free(p_PreparedStatement);
    }

    /// Execute query on worker thread
    /// @p_PrepareStatementHolder : PrepareStatement which will be executed on database worker thread
    CallBackOperator Base::PrepareOperator(PreparedStatement* p_PrepareStatementHolder)
    {
        /// PrepareStatement keeps reference of MYSQLConnection -- keep note
        PrepareStatementOperator* l_PrepareStatementOperator = new PrepareStatementOperator(p_PrepareStatementHolder);

        /// Keep reference of statement operator to execute query
        EnqueueOperator(l_PrepareStatementOperator);

        /// Return our CallBackOperator which gets the result from database worker thread
        return CallBackOperator(std::move(l_PrepareStatementOperator->GetFuture()));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Pass operator to worker thread
    /// @p_Operator : Operator we are adding to be processed on database worker thread
    void Base::EnqueueOperator(Operator* p_Operator)
    {
        m_Producer.Push(p_Operator);
    }
    /// Process future operations
    void Base::ProcessOperators()
    {
        while (true)
        {
            Operator* l_Operator = nullptr;
            m_Producer.WaitAndPop(l_Operator);

            if (l_Operator)
            {
                l_Operator->Execute();

                delete l_Operator;
            }
        }
    }

}   ///< namespace Database
}   ///< namespace Core
}   ///< namespace SteerStone