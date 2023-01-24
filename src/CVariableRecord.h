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
	// ====================  LIFECYCLE     ======================================= 
	CVariableRecord () = default;                             // constructor 

	// ====================  ACCESSORS     ======================================= 

	// ====================  MUTATORS      ======================================= 

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

}; // -----  end of class CVariableRecord  ----- 

#endif   // ----- #ifndef _CVARIABLERECORD_INC_  ----- 
