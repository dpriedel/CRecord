// =====================================================================================
//
//       Filename:  CVariableField.h
//
//    Description:  Variable Field interfac3
//
//        Version:  1.0
//        Created:  03/27/2023 04:52:31 PM
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

#ifndef _CVARIABLEFIELD_INC_
#define _CVARIABLEFIELD_INC_

#include <string_view>

#include "FieldBase.h"

// =====================================================================================
//        Class:  CVariableField
//  Description:  Simple delimited fields -- NOT quoted
//
// =====================================================================================
class CVariableField : public BaseField<CVariableField>
{
   public:
    // ====================  LIFECYCLE     =======================================
    CVariableField() = default;  // constructor

    // ====================  ACCESSORS     =======================================

    // ====================  MUTATORS      =======================================

    void UseData(std::string_view field_data);

    // ====================  OPERATORS     =======================================

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

};  // -----  end of class CVariableField  -----

#endif  // ----- #ifndef _CVARIABLEFIELD_INC_  -----
