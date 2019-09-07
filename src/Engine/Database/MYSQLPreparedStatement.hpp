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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

#include "Database/PreparedStatement.hpp"
#include "Database/SQLCommon.hpp"
#include <mutex>

namespace SteerStone { namespace Core { namespace Database {

    /// Used for bind index, easy readability
    enum Index
    {
        INDEX_0,
        INDEX_1,
        INDEX_2,
        INDEX_3,
        INDEX_4,
        INDEX_5,
        INDEX_6,
        INDEX_7,
        INDEX_8,
        INDEX_9,
        INDEX_10,
        INDEX_11,
        INDEX_12,
        INDEX_13,
        INDEX_14,
        INDEX_15,
    };

    class Base;

    class MYSQLPreparedStatement
    {
    public:
        friend class PreparedStatements;

    public:
        /// Constructor
        /// @p_Database : Keep reference of database
        MYSQLPreparedStatement(Base& p_Database);

        /// Constructor
        ~MYSQLPreparedStatement();

    public:
        /// SetupStatements
        /// @p_Username : Name of user
        /// @p_Password : Password of user
        /// @p_Port     : Port we are connecting to
        /// @p_Host     : Address we are connecting to
        /// @p_Database : Database we are querying to
        uint32 Connect(std::string const p_Username, std::string const p_Password,
            uint32 const p_Port, std::string const p_Host, std::string const p_Database);

    public:
        /// PrepareStatement
        /// Prepare the statement
        /// @p_Query : Query which will be executed to database
        bool Prepare(PreparedStatement* p_StatementHolder);

        /// Execute
        /// @p_Stmt : Statement we are executing
        /// @p_Result : Result set
        /// @p_FieldCount : How many columns
        bool Execute(MYSQL_STMT* p_Stmt, MYSQL_RES ** p_Result, uint32* p_FieldCount);

        /// GetHandle
        /// Return MySQL Connection Handle
        MYSQL* GetHandle() { return m_Connection; }

        /// GetDatabase
        /// Return database
        Base& GetDatabase() const { return m_Database; }

    private:
        MYSQL* m_Connection;

    private:
        PreparedStatement* m_Statements[MAX_PREPARED_STATEMENTS];
        Base& m_Database;
        std::mutex m_Mutex;
    };

}   ///< namespace Database
}   ///< namespace Core
}   ///< namespace SteerStone