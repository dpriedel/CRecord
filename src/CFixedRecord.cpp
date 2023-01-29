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

// helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void CFixedRecord::UseData (std::string_view record_data)
{
    // fmt::print("input: {:30}\n", record_data);
    // auto xxx = [record_data](auto&& a_field) -> std::string_view { return a_field.UseData(record_data); };
	for (auto& field_data : fields_)
	{
	    field_data.field_data_ = std::visit(
                overloaded {
                    [](std::monostate&) -> std::string_view { return {}; },
                    [record_data](auto& a_field) -> std::string_view { return a_field.UseData(record_data); }
                }, field_data.field_);
	}
	return ;
}		// -----  end of method CFixedRecord::UseData  ----- 


