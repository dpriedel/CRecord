// =====================================================================================
//
//       Filename:  CRecord.h
//
//    Description:  Provides interface to CRecord related classes
//
//        Version:  1.0
//        Created:  12/30/2022 09:44:00 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:
//
// =====================================================================================

/* This file is part of ModernCRecord. */

/* ModernCRecord is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* ModernCRecord is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with ModernCRecord.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _CRECORD_INC_
#define _CRECORD_INC_

#include <format>
#include <stdexcept>
#include <string>
#include <variant>

// use this to make accessing the CRecord variant less opaque.

enum RecordTypes
{
    e_EmptyRecord = 0,
    e_FixedRecord = 1,
    e_VariableRecord = 2,
    e_Unknown = 99
};

#include "CFixedRecord.h"
#include "CVariableRecord.h"

//	for error reporting

class Short_Record : public std::length_error
{
   public:
    explicit Short_Record(const char* what_arg = nullptr) : length_error{what_arg} {}
    explicit Short_Record(const std::string& what_arg) : length_error{what_arg} {}
};
class Long_Record : public std::length_error
{
   public:
    explicit Long_Record(const char* what_arg = nullptr) : length_error{what_arg} {}
    explicit Long_Record(const std::string& what_arg) : length_error{what_arg} {}
};

// class	CUnionRecord;
//

// Placeholder.

class CEmptyRecord : public RecordBase<CEmptyRecord>
{
   public:
    CEmptyRecord() = default;
};

using CRecord = std::variant<CEmptyRecord, CFixedRecord, CVariableRecord>;

// a custom formater for EmptyRecord

template <>
struct std::formatter<CEmptyRecord> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CEmptyRecord& a_record, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s), "EmptyRecord\n");
        // for (const auto& fld : a_record.GetFields())
        // {
        //     std::format_to(std::back_inserter(s), "{}\n", fld);
        // }
        return formatter<std::string>::format(s, ctx);
    }
};

// a custom formater for CRecord

template <>
struct std::formatter<CRecord> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.

    auto format(const CRecord& a_record, std::format_context& ctx) const
    {
        std::string s = std::visit([](const auto& rec) { return std::format("{}", rec); }, a_record);
        return formatter<std::string>::format(s, ctx);
    }
};

#endif  // ----- #ifndef _CRECORD_INC_  -----
