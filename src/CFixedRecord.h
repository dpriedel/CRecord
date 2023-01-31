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


#ifndef  _CFIXEDRECORD_INC_
#define  _CFIXEDRECORD_INC_

#include <string_view>

#include <fmt/format.h>

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
	CFixedRecord () =default;

	// ====================  ACCESSORS     ======================================= 

    CFixedField::PositionMode GetPositionMode() const { return field_position_mode_; }
	// ====================  MUTATORS      ======================================= 
	
	void SetPositionType(CFixedField::PositionMode position_mode) { field_position_mode_ = position_mode; }

    void UseData(std::string_view record_data);

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

    std::vector<std::string_view> GetComboFieldData(const std::vector<std::string>& field_names) const;

	// ====================  DATA MEMBERS  ======================================= 

    CFixedField::PositionMode field_position_mode_ = CFixedField::PositionMode::e_Unknown;

}; // -----  end of class CFixedRecord  ----- 

// a custom formater for fields

template <> struct fmt::formatter<CFixedRecord>: formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CFixedRecord& a_record, fmt::format_context& ctx)
    {
        std::string s;
        fmt::format_to(std::back_inserter(s), "FixedRecord\n");
        for (const auto& fld : a_record.GetFields())
        {
            fmt::format_to(std::back_inserter(s), "{}\n", fld);
        }
        return formatter<std::string>::format(s, ctx);
    }
};

#endif   // ----- #ifndef _CFIXEDRECORD_INC_  ----- 
