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
#include <cstddef>
#include <iterator>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <range/v3/algorithm/find_if.hpp>

#include "CField.h"
#include "CRecordDescVisitor.h"

// // helper type for the visitor #4
// template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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

    record_type_ = RecordTypes::e_FixedRecord;
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

std::any CRecord_DescVisitor::visitVariable_header (CPP_Record_DescParser::Variable_headerContext *ctx)
{
    record_type_ = RecordTypes::e_VariableRecord;
    record_ = CVariableRecord{};

    auto result = visitChildren(ctx);

    auto& variable_rec = std::get<RecordTypes::e_VariableRecord>(record_);

    auto variable_rec_fld_delim = ctx->variable_record_delim()->getText();
    if (variable_rec_fld_delim == "Tab")
    {
        variable_rec.SetFieldDeimChar('\t');
    }
    else if (variable_rec_fld_delim == "Comma") {
        variable_rec.SetFieldDeimChar(',');
    }
    else
    {
        variable_rec.SetFieldDeimChar(variable_rec_fld_delim[0]);
    }

	auto fld_cnt = ctx->a->getText();
    int fld_count;
    auto [ptr, ec] { std::from_chars(fld_cnt.data(), fld_cnt.data() + fld_cnt.size(), fld_count) };
    if (ec != std::errc())
    {
        throw std::invalid_argument{fmt::format("Invalid 'number of fields': {}", fld_cnt)};
    }


    variable_rec.SetNumberOfFields(fld_count);

    // if we are not using field names, then we need to construct our field list here so it 
    // can be used by any Virtual fields which might be defined. 

    if (variable_rec.GetFielNamesUsed() != CVariableRecord::FiedlNamesUsed::e_FieldNames_)
    {
        for (size_t indx = 0; indx < fld_count; ++indx)
        {
            FieldData new_field;
            new_field.field_ = CVariableField();

            variable_rec.AddField(new_field);
        }
    }
	return result;
}		// -----  end of method CRecord_DescVisitor::visitVariable_header  ----- 

std::any CRecord_DescVisitor::visitField_names_used (CPP_Record_DescParser::Field_names_usedContext *ctx)
{
    auto result = visitChildren(ctx);
    if (record_type_ == RecordTypes::e_VariableRecord)
    {
        CVariableRecord::FiedlNamesUsed fld_names;
        if (ctx->YES())
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_FieldNames_;
        }
        else if (ctx->NO())
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_FieldNumbers_;
        }
        else 
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_UseHeader_;
        }
        
        std::get<RecordTypes::e_VariableRecord>(record_).SetUseFieldNames(fld_names);
    }

	return result;
}		// -----  end of method CRecord_DescVisitor::visitField_names_used  ----- 

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

    if (record_type_ == RecordTypes::e_FixedRecord)
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

std::any CRecord_DescVisitor::visitField_separator_char (CPP_Record_DescParser::Field_separator_charContext *ctx)
{
    std::any result = visitChildren(ctx);
    if (!  ctx->isEmpty())
    {
        combo_fld_sep_char_ = ctx->getText();
    }
	return result;
}		// -----  end of method CRecord_DescVisitor::visitField_separator_char  ----- 

std::any CRecord_DescVisitor::visitVariable_list_field_name (CPP_Record_DescParser::Variable_list_field_nameContext *ctx)
{
    std::any result = visitChildren(ctx);
    std::string fld_name = ctx->FIELD_NAME()->getText();

    if (record_type_ == RecordTypes::e_VariableRecord)
    {
        FieldData new_field;
        new_field.field_name_ = fld_name;
        new_field.field_ = CVirtualField{};
        std::get<RecordTypes::e_VariableRecord>(record_).AddField(new_field);
    }
	return result;
}		// -----  end of method CRecord_DescVisitor::visitVariable_list_field_name  ----- 

std::any CRecord_DescVisitor::visitVirtual_list_field_name (CPP_Record_DescParser::Virtual_list_field_nameContext *ctx)
{
    std::any result = visitChildren(ctx);
    std::string fld_name = ctx->FIELD_NAME()->getText();

    // let's edit fld_name here to be sure it has been defined
    // earlier in the field list for this record type.
    // that way we can avoid runtime checks and catch an 
    // error in the RecordDesc file.

    const FieldList& flds_list = std::visit(overloaded {
            [](std::monostate&) { return FieldList{}; },
            [](auto&& arg) { return arg.GetFields(); }
            }, record_);

    auto pos = ranges::find_if(flds_list, [&fld_name](const auto& e) { return e.field_name_ == fld_name; });
    if (pos == flds_list.end())
    {
        throw std::invalid_argument(fmt::format("Virtual field element: {} was not previously defined.", fld_name));
    }

	// list_field_names_.push_back(fld_name);
	
	// let's translate the name to an index into the fields list so we can
	// avoid searching by name when we are mapping record data to fields.

    auto distance = std::distance(flds_list.begin(), pos);
    list_field_numbers_.push_back(distance);

	return result;
}		// -----  end of method CRecord_DescVisitor::visitVirtual_list_field_name  ----- 

std::any CRecord_DescVisitor::visitCombo_field(CPP_Record_DescParser::Combo_fieldContext *ctx)
{
    // collect the data necessary to contruct combo fields at runtime.
    // but first, clear out any previously collected data.

    // list_field_names_.clear();
    list_field_numbers_.clear();
    combo_fld_sep_char_.clear();

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->FIELD_NAME()->getText();

    CVirtualField::NameOrNumber names_or_numbers;
    if (ctx->NAME_WORD())
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNames;
    }
    else if (ctx->NUMBER_WORD())
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNumbers;
    }

    // field_separator_char and field_name_list are collected in
    // their respective visitors.

    FieldData new_field;
    new_field.field_name_ = fld_name;
    new_field.field_ = CVirtualField{names_or_numbers, combo_fld_sep_char_, list_field_numbers_};

    // we can get here for any record type so we'll just use a visitor
    // to pass the data to our CRecord object.

    std::visit(overloaded {
            [&new_field](std::monostate&) { /* do nothing */},
            [&new_field](auto& arg) { arg.AddField(new_field); }
            }, record_);

	return result;
}		// -----  end of method CRecord_DescVisitor::visitCombo_field  ----- 

std::any CRecord_DescVisitor::visitSynth_field (CPP_Record_DescParser::Synth_fieldContext *ctx)
{
    // collect the data necessary to contruct synth fields at runtime.
    // but first, clear out any previously collected data.

    // NOTE: this is the same as combo fields (except field_separator_char is required)
    // and the field name which can have leading '-'s
    // and gets mapped to a Virtual field.

    // list_field_names_.clear();
    list_field_numbers_.clear();
    combo_fld_sep_char_.clear();

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->synth_field_name()->getText();

    CVirtualField::NameOrNumber names_or_numbers;
    if (ctx->NAME_WORD())
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNames;
    }
    else if (ctx->NUMBER_WORD())
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNumbers;
    }

    // field_separator_char and field_name_list are collected in
    // their respective visitors.

    if (combo_fld_sep_char_.empty())
    {
        throw std::invalid_argument(fmt::format("Synth field: {} must specify a 'field_separator_char.", fld_name));
    }

    FieldData new_field;
    new_field.field_name_ = fld_name;
    new_field.field_ = CVirtualField{names_or_numbers, combo_fld_sep_char_, list_field_numbers_};

    // we can get here for any record type so we'll just use a visitor
    // to pass the data to our CRecord object.

    std::visit(overloaded {
            [&new_field](std::monostate&) { /* do nothing */},
            [&new_field](auto& arg) { arg.AddField(new_field); }
            }, record_);

	return result;

}		// -----  end of method CRecord_DescVisitor::visitSynth_field  ----- 

