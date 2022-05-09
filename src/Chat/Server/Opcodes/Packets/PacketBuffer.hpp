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

#ifndef _SERVER_PACKET_BUFFER_HPP_
#define _SERVER_PACKET_BUFFER_HPP_
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Logger/LogDefines.hpp"
#endif /* _SERVER_PACKET_BUFFER_HPP_ */

#define STORAGE_INITIAL_SIZE 4096

namespace SteerStone { namespace Chat { namespace Server {

    /// Buffer which is used to hold data to be sent to the client
    class PacketBuffer
    {
    public:
        /// Constructor
        /// @p_ReserveSize : Reserve size for our m_Storage
        explicit PacketBuffer(std::size_t p_ReserveSize = STORAGE_INITIAL_SIZE) : m_WritePosition(0), m_ReadPosition(0)
        {
            m_Storage.reserve(p_ReserveSize);
        }
        /// Deconstructor
        ~PacketBuffer() {}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
     
        /// Append Bool
        /// @p_Header : Header of packet
        void AppendHeader(char* p_Header)
        {
            AppendChar(p_Header);
        }
        /// Append String
        /// @p_Value : Append a string to our storage
        /// @p_Delimeter : Append "/x2" to our storage if true
        void AppendChar(const char* p_Buffer)
        {
            if (std::size_t l_Length = strlen(p_Buffer))
                Append(p_Buffer, l_Length);
        }
        /// Append Bool
        /// @p_Bool : Bool being appended
        void AppendBool(bool const p_Bool)
        {
            AppendChar(p_Bool ? "1" : "0");
        }
        /// Append UInt16
        /// @p_Value : Value being appended
        void AppendUInt16(uint16 p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Append UInt32
        /// @p_Value : Value being appended
        void AppendUInt32(uint32 p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Append Int32
        /// @p_Value : Value being appended
        void AppendInt32(int32 p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Append UInt64
        /// @p_Value : Value being appended
        void AppendUInt64(uint64 p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Append Int64
        /// @p_Value : Value being appended
        void AppendUInt64(int64 p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Append Float
        /// @p_Value : Value being appended
        void AppendFloat(float p_Value)
        {
            std::string l_Value = std::to_string(p_Value);
            AppendChar(l_Value.c_str());
        }
        /// Apennd Splitter
        void AppendSplit()
        {
            std::string l_Split = "@";
            Append(l_Split.c_str(), l_Split.length());
        }
        /// Knock off last byte
        void AppendEndSplitter()
        {
            m_Storage.push_back('\0');
        }
        /// Append end of packet
        void AppendCarriage()
        {
            AppendChar("\r");
        }
        /// Append
        /// @p_Buffer : Append another PacketBuffer into our storage
        void Append(PacketBuffer const& p_Buffer)
        {
            if (p_Buffer.GetWritePosition())
                Append(p_Buffer.GetContents(), p_Buffer.GetWritePosition());
        }
        /// Append
        /// @param T the source type to convert.
        void Append(const char* p_Buffer, const std::size_t& p_Size)
        {
            return Append((uint8 const*)p_Buffer, p_Size);
        }
        /// Append
        /// @p_Buffer : Buffer which will be appended to into our storage
        /// @p_Size : Size of Buffer
        void Append(uint8 const* p_Buffer, std::size_t const& p_Size)
        {
            if (!p_Size)
                return;

            LOG_ASSERT(GetSize() < 10000000, "PacketBuffer", "Size is larger than 10000000!");

            if (m_Storage.size() < m_WritePosition + p_Size)
                m_Storage.resize(m_WritePosition + p_Size);

            memcpy(&m_Storage[m_WritePosition], p_Buffer, p_Size);
            m_WritePosition += p_Size;
        }

        /// Resize storage
        /// @p_Size : Size
        void Resize(std::size_t const p_Size)
        {
            m_Storage.resize(p_Size);
        }
        /// Reserve Storage
        /// @p_Size : Size
        void Reserve(std::size_t const p_Size)
        {
            if (p_Size > GetSize())
                m_Storage.reserve(p_Size);
        }

        /// Get Write Position
        std::size_t GetWritePosition() const
        {
            return m_WritePosition;
        }
        /// Get Read Position
        std::size_t GetReadPositino()
        {
            return m_ReadPosition;
        }

        /// Get Size of storage
        std::size_t GetSize() const
        {
            return m_Storage.size();
        }
        /// Get contents
        uint8 const* GetContents() const
        {
            return &m_Storage[0];
        }

        /// Clear Storage
        void Clear()
        {
            m_WritePosition = 0;
            m_ReadPosition = 0;
            m_Storage.clear();
        }

    private:
        std::size_t m_WritePosition;  ///< Write position in our stroage
        std::size_t m_ReadPosition;   ///< Read position in our storage
        std::vector<uint8> m_Storage; ///< Vector Storage
    };

}   ///< namespace Server
}   ///< namespace Chat 
}   ///< namespace SteerStone