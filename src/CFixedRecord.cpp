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

#include "CFixedRecord.h"
#include "utilities.h"

void CFixedRecord::UseData(std::string_view record_data)
{
    // fmt::print("input: {:30}\n", record_data);
    // auto xxx = [record_data](auto&& a_field) -> std::string_view { return a_field.UseData(record_data); };
    for (auto& field : fields_)
    {
        std::visit(Overloaded{[this, record_data](CArrayField& a_field)
                              { a_field.UseData(record_data, GetVirtualFieldData(a_field.GetFieldNumbers())); },
                              [this, record_data](CVirtualField& a_field)
                              { a_field.UseData(record_data, GetVirtualFieldData(a_field.GetFieldNumbers())); },
                              [record_data](auto& a_field) { a_field.UseData(record_data); }},
                   field);
    }
}  // -----  end of method CFixedRecord::UseData  -----

std::vector<std::string_view> CFixedRecord::GetVirtualFieldData(const std::vector<size_t>& field_numbers) const
{
    std::vector<std::string_view> virtual_field_data;
    virtual_field_data.reserve(field_numbers.size());
    for (const auto& fld_nbr : field_numbers)
    {
        virtual_field_data.push_back((*this)[fld_nbr]);
    }
    return virtual_field_data;
}  // -----  end of method CFixedRecord::GetComboFieldNames  -----
