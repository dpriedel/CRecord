// =====================================================================================
//
//       Filename:  CFixedRecord.h
//
//    Description:  Header for Fixed length record 
//
//        Version:  1.0
//        Created:  01/07/2023 09:38:48 AM
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


#ifndef  _CFIXEDRECORD_INC_
#define  _CFIXEDRECORD_INC_

#include <format>
#include <string_view>

#include "RecordBase.h"

// =====================================================================================
//        Class:  CFixedRecord
//  Description: 
//
// =====================================================================================
class CFixedRecord : public RecordBase<CFixedRecord> 
{
public:
	// ====================  LIFECYCLE     ======================================= 
	// CFixedRecord () =default;

	// ====================  ACCESSORS     ======================================= 

    [[nodiscard]] CFixedField::PositionMode GetPositionMode() const { return field_position_mode_; }
	// ====================  MUTATORS      ======================================= 
	
	void SetPositionType(CFixedField::PositionMode position_mode) { field_position_mode_ = position_mode; }

    void UseData(std::string_view record_data);

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

    [[nodiscard]] std::vector<std::string_view> GetVirtualFieldData(const std::vector<size_t>& field_numbers) const;

	// ====================  DATA MEMBERS  ======================================= 

    CFixedField::PositionMode field_position_mode_ = CFixedField::PositionMode::e_Unknown;

}; // -----  end of class CFixedRecord  ----- 

// a custom formater for fields

template <> struct std::formatter<CFixedRecord>: std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CFixedRecord& a_record, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s), "FixedRecord\n");
        for (const auto& fld : a_record.GetFields())
        {
            std::format_to(std::back_inserter(s), "{}\n", fld);
        }
        return formatter<std::string>::format(s, ctx);
    }
};

#endif   // ----- #ifndef _CFIXEDRECORD_INC_  ----- 
