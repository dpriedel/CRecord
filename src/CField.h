// =====================================================================================
//
//       Filename:  CField.h
//
//    Description:  Fields represent the contents of a piece of a record.
//              	Fields can be fixed or variable or virtual.
//	                Their main function is to construct a string_view which
//	                provides access to their data.
//
//        Version:  1.0
//        Created:  12/30/2022 02:31:16 PM
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
    /* along with Extractor_Markup.  If not, see <http://www.gnu.org/licenses/>. */


#ifndef  _CFIELD_INC_
#define  _CFIELD_INC_

#include <format>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// #include <fmt/format.h>
//
// #include <range/v3/view/chunk.hpp>
// #include <range/v3/view/transform.hpp>

#include "utilities.h"

#include "CFixedField.h"
#include "CVariableField.h"
#include "CVirtualField.h"
#include "CArrayField.h"

using CField = std::variant<std::monostate, CFixedField, CVariableField, CVirtualField, CArrayField>;

struct FieldData
{
    std::string field_name_;
    CField field_;
    std::string_view field_data_;
};

using FieldList = std::vector<FieldData>;

inline FieldModifiers GetFieldModifier (const CField& fld)
{
    return std::visit(overloaded {
            [](std::monostate) { return FieldModifiers::e_Unknown; },
            [](const auto& arg) { return arg.GetModifier(); }
            }, fld);
};

// a custom formater for Modifiers

template <> struct std::formatter<FieldModifiers>: formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const FieldModifiers& modifier, std::format_context& ctx) const
    {
        std::string mod;
        switch(modifier)
        {
            using enum FieldModifiers;
            case e_TrimLeft:
                mod = "TL";
                break;
            case e_TrimRight:
                mod = "TR";
                break;
            case e_TrimBoth:
                mod = "TB";
                break;
            case e_NoTrim:
                mod = "NT";
                break;
            case e_Repeating:
                mod = "RP";
                break;
            case e_Unknown:
                mod = "Unknown";
                break;
        };
        std::string s;
        std::format_to(std::back_inserter(s), "{}", mod);

        return formatter<std::string>::format(s, ctx);
    }
};

// TODO: finish this if it turns out to be needed
//
// template <> struct std::formatter<CArrayField>: formatter<std::string>
// {
//     // parse is inherited from formatter<string>.
//     auto format(const CArrayField& fld, std::format_context& ctx)
//     {
//         std::string s;
//         std::format_to(std::back_inserter(s), "\t{} ", "TBD");
//
//         return formatter<std::string>::format(s, ctx);
//     }
// };

template <> struct std::formatter<FieldData>: formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const FieldData& field_info, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s), "\tname: {}, mod: {} length: {}, content: ->{}<-.",
            field_info.field_name_, GetFieldModifier(field_info.field_), field_info.field_data_.size(), field_info.field_data_);

        return formatter<std::string>::format(s, ctx);
    }
};

#endif   // ----- #ifndef _CFIELD_INC_  ----- 
