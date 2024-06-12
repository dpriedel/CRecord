// =====================================================================================
//
//       Filename:  CFixedField.h
//
//    Description:  Fixed Field header
//
//        Version:  1.0
//        Created:  03/27/2023 04:47:52 PM
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

#ifndef _CFIXEDFIELD_INC_
#define _CFIXEDFIELD_INC_

#include <string_view>

#include "FieldBase.h"

// =====================================================================================
//        Class:  CFixedField
//  Description:  Interface class for fields
//
// =====================================================================================

class CFixedField : public BaseField<CFixedField>
{
   public:
    enum class PositionMode : int32_t
    {
        e_StartLen = 12,
        e_StartEnd,
        e_Len,
        e_Unknown
    };

    // ====================  LIFECYCLE     =======================================

    CFixedField() = default;
    CFixedField(PositionMode position_mode, size_t a, size_t b = 0);  // constructor
                                                                      //
    // ====================  ACCESSORS     =======================================

    // ====================  MUTATORS      =======================================

    void UseData(std::string_view record_data);

    // ====================  OPERATORS     =======================================

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

};  // ----------  end of template class CField  ----------

// custom formatter for Fixed Fields
//
template <>
struct std::formatter<CFixedField> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CFixedField& field, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s), "\tfixed fld:\tname: {}, mod: {} offset: {} length: {}, content: ->{}<-.",
                       field.GetFieldName(), field.GetFieldModifier(), field.GetFieldOffset(), field.GetFieldLength(),
                       field.GetFieldData());

        return formatter<std::string>::format(s, ctx);
    }
};

#endif  // ----- #ifndef _CFIXEDFIELD_INC_  -----
