// =====================================================================================
//
//       Filename:  CArrayField.h
//
//    Description:  Array Field interface 
//
//        Version:  1.0
//        Created:  03/27/2023 05:00:12 PM
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


#ifndef  _CARRAYFIELD_INC_
#define  _CARRAYFIELD_INC_

#include <vector>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/transform.hpp>

#include "FieldBase.h"

// =====================================================================================
//        Class:  CArrayField
//  Description:  Field which maps a contiguous set of characters into an 'array' of 
//                  fields.
// =====================================================================================
class CArrayField : public BaseField<CArrayField>
{
public:

	// ====================  LIFECYCLE     ======================================= 
	//
	CArrayField () = default;                             // constructor 
	CArrayField (size_t field_width, size_t field_count, const std::vector<size_t>& field_numbers)
	    : field_width_{field_width}, field_count_{field_count}, real_field_numbers_{field_numbers} { }

	// ====================  ACCESSORS     ======================================= 

    const std::vector<size_t>& GetFieldNumbers() const { return real_field_numbers_; }

    auto GetArray() const
    {
        return ranges::views::transform(field_data_ | ranges::views::chunk(field_width_), [] (const auto& fld) { return std::string_view{fld}; });
    }
	// ====================  MUTATORS      ======================================= 

    std::string_view UseData(std::string_view record_data, const std::vector<std::string_view>& fields_data);

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

    std::string field_data_;
    // std::vector<std::string> real_field_names_;
    std::vector<size_t> real_field_numbers_;

    size_t field_width_ = 0;
    size_t field_count_ = 0;

}; // -----  end of class CArrayField  ----- 

#endif   // ----- #ifndef _CARRAYFIELD_INC_  ----- 
