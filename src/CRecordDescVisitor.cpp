// =====================================================================================
//
//       Filename:  CRecordDescVisitor.cpp
//
//    Description:  Where we do the actual work 
//
//        Version:  1.0
//        Created:  01/21/2023 10:24:15 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================

#include <fmt/format.h>

#include "CRecordDescVisitor.h"


// --------------------------------------------------------------------------------------
//       Class:  CRecord_DescVisitor
//      Method:  CRecord_DescVisitor
// Description:  constructor
// --------------------------------------------------------------------------------------
CRecord_DescVisitor::CRecord_DescVisitor () 
{
}  // -----  end of method CRecord_DescVisitor::CRecord_DescVisitor  (constructor)  ----- 

std::any CRecord_DescVisitor::visitFixed_header(CPP_Record_DescParser::Fixed_headerContext *ctx)
{
    fmt::print("Got here!") ;
    return {};
}
