// =====================================================================================
//
//       Filename:  CRecordDescVisitor.h
//
//    Description:  Custom visitor class to generate CRecord based on 
//		    data from parsing record Description file.
//
//        Version:  1.0
//        Created:  01/20/2023 04:40:08 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================


#ifndef  _CRECORDDESCVISITOR_INC_
#define  _CRECORDDESCVISITOR_INC_

#include "CPP_Record_DescBaseVisitor.h"
#include "CRecord.h"
// =====================================================================================
//        Class:  CRecord_DescVisitor
//  Description:  
// =====================================================================================
class CRecord_DescVisitor : public CPP_Record_DescBaseVisitor
{
public:
    // ====================  LIFECYCLE     ======================================= 
    CRecord_DescVisitor ();                             // constructor 
    ~CRecord_DescVisitor () = default;                             // constructor 

    // ====================  ACCESSORS     ======================================= 

    CRecord GetCRecord() const { return record_; }

    // ====================  MUTATORS      ======================================= 

  std::any visitFixed_header(CPP_Record_DescParser::Fixed_headerContext *ctx) override;

    // ====================  OPERATORS     ======================================= 

protected:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

	CRecord record_;
}; // -----  end of class CRecord_DescVisitor  ----- 


#endif   // ----- #ifndef _CRECORDDESCVISITOR_INC_  ----- 
