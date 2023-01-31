// =====================================================================================
//
//       Filename:  CFixedRecord.cpp
//
//    Description:  FixedRecord specific code 
//
//        Version:  1.0
//        Created:  01/29/2023 11:46:23 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================

#include <iostream>

#include "CFixedRecord.h"

void CFixedRecord::UseData (std::string_view record_data)
{
    // fmt::print("input: {:30}\n", record_data);
    // auto xxx = [record_data](auto&& a_field) -> std::string_view { return a_field.UseData(record_data); };
	for (auto& field_data : fields_)
	{
	    field_data.field_data_ = std::visit(
                overloaded {
                    [](std::monostate&) -> std::string_view { return {}; },
                    [this, record_data](CVirtualField& a_field) -> std::string_view { return a_field.UseData(record_data, GetComboFieldData(a_field.GetFieldNames())); },
                    [record_data](auto& a_field) -> std::string_view { return a_field.UseData(record_data); }
                }, field_data.field_);
	}
	return ;
}		// -----  end of method CFixedRecord::UseData  ----- 

std::vector<std::string_view> CFixedRecord::GetComboFieldData (const std::vector<std::string>& field_names) const
{
    std::vector<std::string_view> combo_field_data;
    for (const auto& fld_name : field_names)
    {
       combo_field_data.push_back((*this)[fld_name]); 
    }
	return combo_field_data;
}		// -----  end of method CFixedRecord::GetComboFieldNames  ----- 

