// =====================================================================================
//
//       Filename:  CVirtualField.cpp
//
//    Description:  Virtual Field implementation 
//
//        Version:  1.0
//        Created:  03/27/2023 05:21:58 PM
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


#include <ranges>

namespace rng = std::ranges;
namespace vws = std::ranges::views;

#include <range/v3/range/conversion.hpp>

#include "CVirtualField.h"

//--------------------------------------------------------------------------------------
//       Class:  CVirtualField
//      Method:  CVirtualField
// Description:  constructor
//--------------------------------------------------------------------------------------
CVirtualField::CVirtualField (NameOrNumber reference_type, const std::string& field_sep_char,
        const std::vector<size_t>& field_numbers)
    : field_reference_type_{reference_type}, field_sep_char_{field_sep_char},
    real_field_numbers_{field_numbers}
{
    if (field_reference_type_ == NameOrNumber::e_UseNumbers)
    {
        throw std::runtime_error("COMBO fields using field numbers not implmented yet.");
    }
}  // -----  end of method CVirtualField::CVirtualField  (constructor)  ----- 

std::string_view CVirtualField::UseData (std::string_view record_data, const std::vector<std::string_view>& fields_data)
{
    field_data_ = vws::join_with(fields_data, field_sep_char_) | ranges::to<std::string>();
	return {field_data_} ;
}		// -----  end of method CVirtualField::UseData  ----- 

