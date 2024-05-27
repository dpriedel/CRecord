// =====================================================================================
//
//       Filename:  CFieldBase.h
//
//    Description:  Interface class for fields
//
//        Version:  1.0
//        Created:  03/27/2023 04:38:23 PM
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
#include <type_traits>

enum FieldTypes
{
    e_FixedField = 1,
    e_VariableField,
    e_QuotedField,
    e_VirtualField,
    e_ArrayField
};

enum class FieldModifiers
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

    FieldModifiers GetModifier() const { return field_modifier_; }

    // ====================  MUTATORS      =======================================

    void SetModifier(FieldModifiers modifier) { field_modifier_ = modifier; }

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
    FieldModifiers field_modifier_ = FieldModifiers::e_Unknown;

};  // ----------  end of template class CField  ----------

#endif  // ----- #ifndef _CFIELDBASE_INC_  -----
