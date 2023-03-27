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

#include <fmt/format.h>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/algorithm/find.hpp>

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
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimLeft)
    {
        auto pos = updated_data.find_first_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_prefix(pos);
        }
    }
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimRight)
    {
        auto pos = updated_data.find_last_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_suffix(updated_data.size() - pos - 1);
        }
    }
	return updated_data;
}		// -----  end of method CFixedField::UseData  ----- 

//--------------------------------------------------------------------------------------
//       Class:  CVariableField
//      Method:  CVariableField
// Description:  constructor
//--------------------------------------------------------------------------------------
std::string_view CVariableField::UseData (std::string_view field_data)

{
	// our default behavior is to remove leading and trailing blanks.
	// this can result in an 'empty' field.
	// TODO: this behaviour can be overridden with field modifiers.
	
    std::string_view updated_data{field_data};
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimLeft)
    {
        auto pos = updated_data.find_first_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_prefix(pos);
        }
    }
    if (field_modifier_ == FieldModifiers::e_TrimBoth || field_modifier_ == FieldModifiers::e_TrimRight)
    {
        auto pos = updated_data.find_last_not_of(' ');
        if (pos == std::string_view::npos)
        { // no non-blank characters in field so it is empty.
            // make it so.

            updated_data = {};
        }
        else
        {
            updated_data.remove_suffix(updated_data.size() - pos - 1);
        }
    }
	return updated_data;
}		// -----  end of method CVariableField::UseData  ----- 

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
    field_data_ = ranges::views::join(fields_data, field_sep_char_) | ranges::to<std::string>();
	return {field_data_} ;
}		// -----  end of method CVirtualField::UseData  ----- 


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

