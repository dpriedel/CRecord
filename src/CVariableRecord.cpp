// =====================================================================================
//
//       Filename:  CVariableRecord.cpp
//
//    Description:  Implementation for Delimited data type record 
//
//        Version:  1.0
//        Created:  02/07/2023 09:48:33 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================

#include "CVariableRecord.h"
#include "utilities.h"
#include <fmt/core.h>
#include <string_view>

void CVariableRecord::UseData (std::string_view record_data)
{
    // our file contains simple single-character Delimited data
    // so we can use our routine to split into string views and then 
    // dole them out to our fields.

    // fmt::print("a record: {}\n", record_data);

    auto fields = split_string<std::string_view>(record_data, field_delim_char_);
    BOOST_ASSERT_MSG(fields.size() == field_count_, fmt::format("Wrong number of fields in record. Found: {}. Expected: {}", fields.size(), field_count_).c_str());

    if (look_for_header_)
    {
        CollectFieldNamesFromHeader(fields);
        return;
    }
    // fmt::print("input: {:30}\n", record_data);
    // auto xxx = [record_data](auto&& a_field) -> std::string_view { return a_field.UseData(record_data); };
    for(size_t indx = 0; indx < fields_.size(); ++indx)
	{
	    fields_[indx].field_data_ = std::visit(
                overloaded {
                    [](std::monostate&) -> std::string_view { return {}; },
                    [this, record_data](CVirtualField& a_field) -> std::string_view { return a_field.UseData(record_data, GetVirtualFieldData(a_field.GetFieldNumbers())); },
                    [this, record_data](CArrayField& a_field) -> std::string_view { return {}; },
                    [&fields, indx](auto& a_field) -> std::string_view { return a_field.UseData(fields[indx]); }
                }, fields_[indx].field_);
	}
    return ;
}		// -----  end of method CVariableRecord::UseData  ----- 

void CVariableRecord::CollectFieldNamesFromHeader(std::vector<std::string_view> field_names)
{
    for(size_t indx = 0; indx < field_count_; ++indx)
    {
        fields_[indx].field_name_ = field_names[indx];
    }

    look_for_header_ = false;
}		// -----  end of method CVariableRecord::CollectFieldNamesFromHeader  ----- 

std::vector<std::string_view> CVariableRecord::GetVirtualFieldData (const std::vector<size_t>& field_numbers) const
{
    std::vector<std::string_view> virtual_field_data;
    for (const auto& fld_nbr : field_numbers)
    {
       virtual_field_data.push_back((*this)[fld_nbr]); 
    }
	return virtual_field_data;
}		// -----  end of method CFixedRecord::GetComboFieldNames  ----- 
