// =====================================================================================
//
//       Filename:  CVariableRecord.h
//
//    Description:  Header for variable lenght fields delimited record 
//
//        Version:  1.0
//        Created:  01/07/2023 09:42:29 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================



#ifndef  _CVARIABLERECORD_INC_
#define  _CVARIABLERECORD_INC_

#include "RecordBase.h"

// =====================================================================================
//        Class:  CVariableRecord
//  Description: 
//
// =====================================================================================
class CVariableRecord : public RecordBase<CVariableRecord>
{
public:

    enum class FiedlNamesUsed
    {
        e_FieldNames_,
        e_FieldNumbers_,
        e_Unknown
    };

	// ====================  LIFECYCLE     ======================================= 
	CVariableRecord () = default;                             // constructor 

	// ====================  ACCESSORS     ======================================= 

    size_t GetFieldCount() const { return field_count_; }

	// ====================  MUTATORS      ======================================= 

    void SetFieldDeimChar (char delim) { field_delim_char_ = delim; }
    void SetNumberOfFiedls (size_t fld_count) { field_count_ = fld_count; }
    void SetUseFieldNames(FiedlNamesUsed use_fld_names) { use_fieldNames_ = use_fld_names; }

    void UseData(std::string_view record_data) {};

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

    FiedlNamesUsed use_fieldNames_ = FiedlNamesUsed::e_Unknown;

    char field_delim_char_;
    size_t field_count_ = 0;

}; // -----  end of class CVariableRecord  ----- 

#endif   // ----- #ifndef _CVARIABLERECORD_INC_  ----- 
