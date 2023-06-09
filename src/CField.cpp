// =====================================================================================
//
//       Filename:  CField.cpp
//
//    Description:  Implementation of basic CField functions 
//
//        Version:  1.0
//        Created:  12/30/2022 02:34:22 PM
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


#include "CField.h"



//--------------------------------------------------------------------------------------
//       Class:  CFixedField
//      Method:  CFixedField
// Description:  constructor
//--------------------------------------------------------------------------------------
CFixedField::CFixedField (PositionMode position_mode, size_t a, size_t b)
{
    if (position_mode == PositionMode::e_Start_End)
    {
        offset_ = a - 1;
        length_ = b - a;
    }
    else if (position_mode == PositionMode::e_Start_Len)
    {
        offset_ = a - 1;
        length_ = b;
    }
    else
    {
        throw std::runtime_error("Length only not implmented yet.");
    }
}  // -----  end of method CFixedField::CFixedField  (constructor)  ----- 


std::string_view CFixedField::UseData (std::string_view record)
{
	// our default behavior is to remove leading and trailing blanks.
	// this can result in an 'empty' field.
	// TODO(dpriedel): this behaviour can be overridden with field modifiers.
	
    std::string_view updated_data{record.data() + offset_, length_};
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimLeft)
    {
        auto pos = updated_data.find_first_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_prefix(pos);
        }
    }
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimRight)
    {
        auto pos = updated_data.find_last_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_suffix(updated_data.size() - pos - 1);
        }
    }
	return updated_data;
}		// -----  end of method CFixedField::UseData  ----- 

//--------------------------------------------------------------------------------------
//       Class:  CVariableField
//      Method:  CVariableField
// Description:  constructor
//--------------------------------------------------------------------------------------
std::string_view CVariableField::UseData (std::string_view field_data)

{
	// our default behavior is to remove leading and trailing blanks.
	// this can result in an 'empty' field.
	// TODO(dpriedel): this behaviour can be overridden with field modifiers.
	
    std::string_view updated_data{field_data};
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimLeft)
    {
        auto pos = updated_data.find_first_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_prefix(pos);
        }
    }
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimRight)
    {
        auto pos = updated_data.find_last_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_suffix(updated_data.size() - pos - 1);
        }
    }
	return updated_data;
}		// -----  end of method CVariableField::UseData  ----- 

