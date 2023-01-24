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

#include "RecordBase.h"

// =====================================================================================
//        Class:  CFixedRecord
//  Description: 
//
// =====================================================================================
class CFixedRecord : public RecordBase<CFixedRecord> 
{
public:
	enum class FixedRecordMode
	{
		e_Start_Len		=	12,
		e_Start_End,
		e_Len,
		e_Unknown
	};

	// ====================  LIFECYCLE     ======================================= 
	CFixedRecord () = default;                             // constructor 

	// ====================  ACCESSORS     ======================================= 

    FixedRecordMode GetRecordMode() const { return field_position_type_; }
	// ====================  MUTATORS      ======================================= 
	
	void SetPositionType(FixedRecordMode position_type) { field_position_type_ = position_type; }

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

    FixedRecordMode field_position_type_ = FixedRecordMode::e_Unknown;

}; // -----  end of class CFixedRecord  ----- 

#endif   // ----- #ifndef _CFIXEDRECORD_INC_  ----- 
