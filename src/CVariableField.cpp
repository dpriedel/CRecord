// =====================================================================================
//
//       Filename:  CVariableField.cpp
//
//    Description:  Variable Field implementation
//
//        Version:  1.0
//        Created:  03/27/2023 05:33:04 PM
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

#include "CVariableField.h"

//--------------------------------------------------------------------------------------
//       Class:  CVariableField
//      Method:  CVariableField
// Description:  constructor
//--------------------------------------------------------------------------------------
void CVariableField::UseData(std::string_view field_data)

{
    // our default behavior is to remove leading and trailing blanks.
    // this can result in an 'empty' field.
    // TODO(dpriedel): this behaviour can be overridden with field modifiers.

    field_data_ = field_data;
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimLeft)
    {
        auto pos = field_data_.find_first_not_of(' ');
        if (pos == std::string_view::npos)
        {  // no non-blank characters in field so it is empty.
            // make it so.

            field_data_ = {};
        }
        else if (pos > 0)
        {
            field_data_.remove_prefix(pos);
        }
    }
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimRight)
    {
        auto pos = field_data_.find_last_not_of(' ');
        if (pos == std::string_view::npos)
        {  // no non-blank characters in field so it is empty.
            // make it so.

            field_data_ = {};
        }
        else if (pos < field_data_.size() - 1)
        {
            field_data_.remove_suffix(field_data_.size() - pos - 1);
        }
    }
}  // -----  end of method CVariableField::UseData  -----
