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

#include <charconv>

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
    // Since we got here, we know we are constructing a FixedRecord object.

    record_ = CFixedRecord();

    auto result = visitChildren(ctx);

    auto buffer_len = ctx->buffer_length()->getText();
    int buf_len;
 
    auto [ptr, ec] { std::from_chars(buffer_len.data(), buffer_len.data() + buffer_len.size(), buf_len) };
    if (ec == std::errc())
    {
        auto& the_record = std::get<RecordTypes::e_FixedRecord>(record_);
        the_record.SetBufferSize(buf_len);
    }
    return result;
}

std::any CRecord_DescVisitor::visitLength_data_type (CPP_Record_DescParser::Length_data_typeContext *ctx)
{
    auto& the_record = std::get<RecordTypes::e_FixedRecord>(record_);

    if (ctx->STARTEND())
    {
        the_record.SetPositionType(CFixedRecord::FixedRecordMode::e_Start_End);
    }
    else if (ctx->STARTLEN())
    {
        the_record.SetPositionType(CFixedRecord::FixedRecordMode::e_Start_Len);
    }
    else if (ctx->LENONLY())
    {
        the_record.SetPositionType(CFixedRecord::FixedRecordMode::e_Len);
    }
    else
    {
        throw std::invalid_argument{fmt::format("Unexpected record length type: {}", ctx->getText())};
    }
    auto result = visitChildren(ctx);
	return result;
}		// -----  end of method CRecord_DescVisitor::VisitLength_data_type  ----- 


std::any CRecord_DescVisitor::visitField_entry (CPP_Record_DescParser::Field_entryContext *ctx)
{
	auto xxx = ctx->a->getText();
    fmt::print("a: {}\n", xxx);

    auto& the_record = std::get<RecordTypes::e_FixedRecord>(record_);

    auto position_type = the_record.GetRecordMode();
    if (position_type != CFixedRecord::FixedRecordMode::e_Len)
    {
        auto yyy = ctx->b->getText();
        fmt::print("b: {}\n", yyy);
    }
    auto result = visitChildren(ctx);
	return result;
}		// -----  end of method CRecord_DescVisitor::visitField_entry  ----- 

