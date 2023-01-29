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
    else
    {
        throw std::invalid_argument{fmt::format("Invalid buffer length: {}", buffer_len)};
    }
    return result;
}

std::any CRecord_DescVisitor::visitLength_data_type (CPP_Record_DescParser::Length_data_typeContext *ctx)
{
    auto& the_record = std::get<RecordTypes::e_FixedRecord>(record_);

    if (ctx->STARTEND())
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_Start_End);
    }
    else if (ctx->STARTLEN())
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_Start_Len);
    }
    else if (ctx->LENONLY())
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_Len);
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
    // we will get here once for each field so this is where we build
    // our field list.

    std::any result = visitChildren(ctx);

    FieldData new_field;

    auto fld_name = ctx->FIELD_NAME()->getText();

	// we could have a start/end, start/len or len only value
	// we already know which to expect from information obtained earlier
	// in the parse.
	
	auto start_or_len_only = ctx->a->getText();
    int len_a;
    int len_b = 0;
    auto [ptr, ec] { std::from_chars(start_or_len_only.data(), start_or_len_only.data() + start_or_len_only.size(), len_a) };
    if (ec != std::errc())
    {
        throw std::invalid_argument{fmt::format("Invalid buffer length: {}", start_or_len_only)};
    }
    // fmt::print("a: {}\n", start_or_len);

    // we will get here for multiple record types so we may need to do 
    // type-specific logic 

    if (auto which_variant = record_.index(); which_variant == RecordTypes::e_FixedRecord)
    {
        auto& the_record = std::get<RecordTypes::e_FixedRecord>(record_);

        auto position_mode = the_record.GetPositionMode();
        if (position_mode != CFixedField::PositionMode::e_Len)
        {
            auto end_or_len = ctx->b->getText();
            auto [ptr, ec] { std::from_chars(end_or_len.data(), end_or_len.data() + end_or_len.size(), len_b) };
            if (ec != std::errc())
            {
                throw std::invalid_argument{fmt::format("Invalid buffer length: {}", end_or_len)};
            }
            // fmt::print("b: {}\n", end_or_len);
        }

        // put it all together

        new_field.field_name_ = fld_name;
        new_field.field_ = CFixedField(position_mode, len_a, len_b);

        the_record.AddField(new_field);

    }

	return result;

}		// -----  end of method CRecord_DescVisitor::visitField_entry  ----- 

