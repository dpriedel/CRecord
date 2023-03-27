// =====================================================================================
//
//       Filename:  CArrayField.cpp
//
//    Description:  Array Field implementation  
//
//        Version:  1.0
//        Created:  03/27/2023 05:17:46 PM
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

#include <range/v3/algorithm/find.hpp>

#include "CArrayField.h"

//--------------------------------------------------------------------------------------
//       Class:  CArrayField
//--------------------------------------------------------------------------------------

std::string_view CArrayField::UseData (std::string_view record_data, const std::vector<std::string_view>& fields_data)
{
	// we get only 1 field passed to us in the fields_data value (by definition of our field type)
    // we need to right trim BUT must take into account the 'field' width of our array entries.	

    field_data_ = fields_data[0];

    std::string empty_field(field_width_, ' ');
    auto fields = field_data_ | ranges::views::chunk(field_width_) | ranges::views::transform([] (const auto& fld) { return std::string_view{fld}; });
    const auto pos = ranges::find(fields, empty_field);
	if (pos != fields.end())
	{
        field_data_.resize(ranges::distance(fields.begin(), pos) * field_width_);
	}
	return {field_data_} ;
}		// -----  end of method CArrayField::UseData  ----- 

