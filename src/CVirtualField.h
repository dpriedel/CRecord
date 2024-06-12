// =====================================================================================
//
//       Filename:  CVirtualField.h
//
//    Description:  Virtual Field interface
//
//        Version:  1.0
//        Created:  03/27/2023 04:55:56 PM
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

#ifndef _CVIRTUALFIELD_INC_
#define _CVIRTUALFIELD_INC_

#include <string>
#include <string_view>
#include <vector>

#include "FieldBase.h"

// =====================================================================================
//        Class:  CVirtualField
//  Description:  Replaces COMBO and SYNTH fields
//
// =====================================================================================
class CVirtualField : public BaseField<CVirtualField>
{
   public:
    enum class NameOrNumber
    {
        e_UseNames,
        e_UseNumbers,
        e_Unknown
    };

    // ====================  LIFECYCLE     =======================================
    CVirtualField() = default;  // constructor
    CVirtualField(NameOrNumber reference_type, const std::string& field_sep_char,
                  const std::vector<size_t>& field_numbers);

    // ====================  ACCESSORS     =======================================

    // const std::vector<std::string>& GetFieldNames() const { return real_field_names_; }
    [[nodiscard]] const std::vector<size_t>& GetFieldNumbers() const { return real_field_numbers_; }

    // ====================  MUTATORS      =======================================

    void UseData(std::string_view record_data, const std::vector<std::string_view>& fields_data);

    // ====================  OPERATORS     =======================================

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

    std::string virt_field_data_;
    // std::vector<std::string> real_field_names_;
    std::vector<size_t> real_field_numbers_;

    NameOrNumber field_reference_type_ = NameOrNumber::e_Unknown;
    std::string field_sep_char_;

};  // -----  end of class CVirtualField  -----

// custom formatter for Variable Fields
//
template <>
struct std::formatter<CVirtualField> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CVirtualField& field, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s), "\tvirtual fld:\tname: {}, # component flds: {}, content: ->{}<-.",
                       field.GetFieldName(), field.GetFieldNumbers().size(), field.GetFieldData());

        return formatter<std::string>::format(s, ctx);
    }
};

#endif  // ----- #ifndef _CVIRTUALFIELD_INC_  -----
