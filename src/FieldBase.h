// =====================================================================================
//
//       Filename:  CFieldBase.h
//
//    Description:  Interface class for fields
//
//        Version:  2.0
//        Created:  2024-06-09 10:00 AM
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

#ifndef _CFIELDBASE_INC_
#define _CFIELDBASE_INC_

// use this to make accessing the below variant less opaque.

#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <type_traits>

enum FieldTypes
{
    e_EmptyField = 1,
    e_FixedField,
    e_VariableField,
    e_QuotedField,
    e_VirtualField,
    e_ArrayField
};

enum class VirtualFieldType : int32_t
{
    e_Synth = 1,
    e_Combo,
    e_SkipToDelim,
    e_Array,
    e_Unknown = 99
};

enum class FieldModifiers : int32_t
{
    e_TrimBoth,
    e_TrimLeft,
    e_TrimRight,
    e_NoTrim,
    e_Repeating,
    e_Unknown
};

// =====================================================================================
//        Class:  BaseField
//  Description:  Interface class for fields
//
// =====================================================================================
template <class T>
class BaseField
{
   public:
    // ====================  LIFECYCLE     =======================================

    // ====================  ACCESSORS     =======================================

    FieldModifiers GetFieldModifier() const { return field_modifier_; }
    const std::string& GetFieldName() const { return field_name_; }
    std::string_view GetFieldData() const { return field_data_; }
    size_t GetFieldOffset() const { return offset_; }
    size_t GetFieldLength() const { return length_; }

    // ====================  MUTATORS      =======================================

    void SetFieldModifier(FieldModifiers modifier) { field_modifier_ = modifier; }
    void SetFieldName(std::string_view name) { field_name_ = name; }
    void SetFieldNumbe(size_t number) { field_number_ = number; }

    // ====================  OPERATORS     =======================================

    template <class U>
    bool operator==(const BaseField<U>& rhs) const
    {
        return (std::is_same_v<T, U> == true && offset_ == rhs.offset_ && length_ == rhs.length_ &&
                field_modifier_ == rhs.field_modifier_);
    }

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    BaseField(){};  // constructor
    friend T;

    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

    size_t offset_ = 0;
    size_t length_ = 0;
    size_t field_number_ = 0;

    FieldModifiers field_modifier_ = FieldModifiers::e_Unknown;

    std::string field_name_;
    std::string_view field_data_;

};  // ----------  end of template class CField  ----------

// a custom formater for Modifiers

template <>
struct std::formatter<FieldModifiers> : formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const FieldModifiers& modifier, std::format_context& ctx) const
    {
        std::string mod;
        switch (modifier)
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

template <>
struct std::formatter<VirtualFieldType> : formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const VirtualFieldType& fld_type, std::format_context& ctx) const
    {
        std::string mod;
        switch (fld_type)
        {
            using enum VirtualFieldType;
            case e_Synth:
                mod = "Synth";
                break;
            case e_Combo:
                mod = "Combo";
                break;
            case e_SkipToDelim:
                mod = "SkipToDelim";
                break;
            case e_Array:
                mod = "Array";
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

#endif  // ----- #ifndef _CFIELDBASE_INC_  -----
