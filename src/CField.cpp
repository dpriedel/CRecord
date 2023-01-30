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

#include <stdexcept>

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
	// TODO: this behaviour can be overridden with field modifiers.
	
    std::string_view updated_data{record.data() + offset_, length_};
    auto pos = updated_data.find_first_not_of(' ');
    if (pos == std::string_view::npos)
    { // no non-blank characters in field so it is empty.
        // make it so.

        updated_data = {};
    }
    else
    {
        updated_data.remove_prefix(pos);
        updated_data.remove_suffix(updated_data.size() - updated_data.find_last_not_of(' ') - 1);

    }
	return updated_data;
}		// -----  end of method CFixedField::UseData  ----- 


//--------------------------------------------------------------------------------------
//       Class:  CVirtualField
//      Method:  CVirtualField
// Description:  constructor
//--------------------------------------------------------------------------------------
CVirtualField::CVirtualField (NameOrNumber reference_type, const std::string& field_sep_char,
        const std::vector<std::string>& field_names)
    : field_reference_type_{reference_type}, field_sep_char_{field_sep_char},
    real_field_names_{field_names}
{
    if (field_reference_type_ == NameOrNumber::e_UseNumbers)
    {
        throw std::runtime_error("COMBO fields using field numbers not implmented yet.");
    }
}  // -----  end of method CVirtualField::CVirtualField  (constructor)  ----- 

std::string_view CVirtualField::UseData (std::string_view record_data, const std::vector<std::string_view>& fields_data)
{
	return {} ;
}		// -----  end of method CVirtualField::UseData  ----- 


