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
#include <string>
#include <type_traits>

enum class FieldTypes : int32_t
{
    e_FixedField = 1,
    e_VariableField,
    e_QuotedField,
    e_VirtualField,
    e_ArrayField
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
    size_t GetFieldSize() const { return length_; }

    // ====================  MUTATORS      =======================================

    void SetFieldModifier(FieldModifiers modifier) { field_modifier_ = modifier; }
    void SetFieldName(const std::string& name) { field_name_ = name; }
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

#endif  // ----- #ifndef _CFIELDBASE_INC_  -----
