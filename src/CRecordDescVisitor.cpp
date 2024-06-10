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

#include "utilities.h"
#include <charconv>
#include <cstddef>
#include <format>
#include <print>

namespace rng = std::ranges;
namespace vws = std::ranges::views;

#include "CField.h"
#include "CRecordDescVisitor.h"

// // helper type for the visitor #4
// template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::any CRecord_DescVisitor::visitFixed_header(CPP_Record_DescParser::Fixed_headerContext *ctx)
{
    // Since we got here, we know we are constructing a FixedRecord object.

    record_type_ = RecordTypes::e_FixedRecord;
    record_ = CFixedRecord();

    auto result = visitChildren(ctx);

    auto buffer_len = ctx->buffer_length()->getText();
    int buf_len = 0;

    auto [ptr, ec] = std::from_chars(buffer_len.data(), buffer_len.data() + buffer_len.size(), buf_len);
    if (ec == std::errc())
    {
        auto &the_record = std::get<RecordTypes::e_FixedRecord>(record_);
        the_record.SetBufferSize(buf_len);
    }
    else
    {
        throw std::invalid_argument{std::format("Invalid buffer length: {}", buffer_len)};
    }
    return result;
}

std::any CRecord_DescVisitor::visitVariable_header(CPP_Record_DescParser::Variable_headerContext *ctx)
{
    record_type_ = RecordTypes::e_VariableRecord;
    record_ = CVariableRecord{};

    auto result = visitChildren(ctx);

    auto &variable_rec = std::get<RecordTypes::e_VariableRecord>(record_);

    auto variable_rec_fld_delim = ctx->variable_record_delim()->getText();
    if (variable_rec_fld_delim == "Tab")
    {
        variable_rec.SetFieldDeimChar('\t');
    }
    else if (variable_rec_fld_delim == "Comma")
    {
        variable_rec.SetFieldDeimChar(',');
    }
    else
    {
        variable_rec.SetFieldDeimChar(variable_rec_fld_delim[0]);
    }

    auto fld_cnt = ctx->a->getText();
    int fld_count = 0;
    auto [ptr, ec] = std::from_chars(fld_cnt.data(), fld_cnt.data() + fld_cnt.size(), fld_count);
    if (ec != std::errc())
    {
        throw std::invalid_argument{std::format("Invalid 'number of fields': {}", fld_cnt)};
    }

    variable_rec.SetNumberOfFields(fld_count);

    // if we are not using field names, then we need to construct our field list here so it
    // can be used by any Virtual fields which might be defined.

    if (variable_rec.GetFielNamesUsed() != CVariableRecord::FiedlNamesUsed::e_FieldNames)
    {
        // set our default modifier

        field_modifier_ = FieldModifiers::e_TrimBoth;

        for (size_t indx = 0; indx < fld_count; ++indx)
        {
            CVariableField new_field;
            new_field.SetFieldModifier(field_modifier_);
            variable_rec.AddField(new_field);
        }
    }
    return result;
}  // -----  end of method CRecord_DescVisitor::visitVariable_header  -----

std::any CRecord_DescVisitor::visitField_names_used(CPP_Record_DescParser::Field_names_usedContext *ctx)
{
    auto result = visitChildren(ctx);
    if (record_type_ == RecordTypes::e_VariableRecord)
    {
        CVariableRecord::FiedlNamesUsed fld_names = {};
        if (ctx->YES() != nullptr)
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_FieldNames;
        }
        else if (ctx->NO() != nullptr)
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_FieldNumbers;
        }
        else
        {
            fld_names = CVariableRecord::FiedlNamesUsed::e_UseHeader;
        }

        std::get<RecordTypes::e_VariableRecord>(record_).SetUseFieldNames(fld_names);
    }

    return result;
}  // -----  end of method CRecord_DescVisitor::visitField_names_used  -----

std::any CRecord_DescVisitor::visitLength_data_type(CPP_Record_DescParser::Length_data_typeContext *ctx)
{
    // currently, this field is only used with FixedRecord record types so the below line is safe.

    auto &the_record = std::get<RecordTypes::e_FixedRecord>(record_);

    if (ctx->STARTEND() != nullptr)
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_StartEnd);
    }
    else if (ctx->STARTLEN() != nullptr)
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_StartLen);
    }
    else if (ctx->LENONLY() != nullptr)
    {
        the_record.SetPositionType(CFixedField::PositionMode::e_Len);
    }
    auto result = visitChildren(ctx);
    return result;
}  // -----  end of method CRecord_DescVisitor::VisitLength_data_type  -----

std::any CRecord_DescVisitor::visitFixed_field_entry(CPP_Record_DescParser::Fixed_field_entryContext *ctx)
{
    // we will get here once for each field so this is where we build
    // our field list.

    // set our default modifier

    field_modifier_ = FieldModifiers::e_TrimBoth;

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->FIELD_NAME()->getText();

    // we could have a start/end, start/len or len only value
    // we already know which to expect from information obtained earlier
    // in the parse.

    auto start_or_len_only = ctx->a->getText();
    int len_a = 0;
    int len_b = 0;
    auto [ptr, ec] =
        std::from_chars(start_or_len_only.data(), start_or_len_only.data() + start_or_len_only.size(), len_a);
    if (ec != std::errc())
    {
        throw std::invalid_argument{std::format("Invalid buffer length: {}", start_or_len_only)};
    }
    // std::print("a: {}\n", start_or_len);

    // we will get here for multiple record types so we may need to do
    // type-specific logic

    if (record_type_ == RecordTypes::e_FixedRecord)
    {
        auto &the_record = std::get<RecordTypes::e_FixedRecord>(record_);

        auto position_mode = the_record.GetPositionMode();
        if (position_mode != CFixedField::PositionMode::e_Len)
        {
            auto end_or_len = ctx->b->getText();
            auto [ptr, ec] = std::from_chars(end_or_len.data(), end_or_len.data() + end_or_len.size(), len_b);
            if (ec != std::errc())
            {
                throw std::invalid_argument{std::format("Invalid buffer length: {}", end_or_len)};
            }
            // std::print("b: {}\n", end_or_len);
        }

        // put it all together

        CFixedField new_field(position_mode, len_a, len_b);
        new_field.SetFieldName(fld_name);
        new_field.SetFieldModifier(field_modifier_);
        the_record.AddField(new_field);
    }
    else
    {
        throw std::runtime_error("Unimplemented use of 'fixed_field_entry'.");
    }
    return result;
}  // -----  end of method CRecord_DescVisitor::visitField_entry  -----

std::any CRecord_DescVisitor::visitField_separator_char(CPP_Record_DescParser::Field_separator_charContext *ctx)
{
    std::any result = visitChildren(ctx);
    if (!ctx->isEmpty())
    {
        combo_fld_sep_char_ = ctx->getText();
    }
    return result;
}  // -----  end of method CRecord_DescVisitor::visitField_separator_char  -----

std::any CRecord_DescVisitor::visitVariable_list_field_name(CPP_Record_DescParser::Variable_list_field_nameContext *ctx)
{
    // set our default modifier

    field_modifier_ = FieldModifiers::e_TrimBoth;

    std::any result = visitChildren(ctx);

    std::string fld_name = ctx->FIELD_NAME()->getText();

    if (record_type_ == RecordTypes::e_VariableRecord)
    {
        CVariableField new_field;
        new_field.SetFieldName(fld_name);
        new_field.SetFieldModifier(field_modifier_);
        std::get<RecordTypes::e_VariableRecord>(record_).AddField(new_field);
    }
    return result;
}  // -----  end of method CRecord_DescVisitor::visitVariable_list_field_name  -----

std::any CRecord_DescVisitor::visitField_modifier(CPP_Record_DescParser::Field_modifierContext *ctx)
{
    std::any result = visitChildren(ctx);

    if (ctx->TRIM_LEFT() != nullptr)
    {
        field_modifier_ = FieldModifiers::e_TrimLeft;
    }
    else if (ctx->TRIM_RIGHT() != nullptr)
    {
        field_modifier_ = FieldModifiers::e_TrimRight;
    }
    else if (ctx->NO_TRIM() != nullptr)
    {
        field_modifier_ = FieldModifiers::e_NoTrim;
    }
    else if (ctx->repeating_field()->REPEATING_FIELD() != nullptr)
    {
        // TODO(dpriedel): need to pick up repeat count and keep that somewhere

        field_modifier_ = FieldModifiers::e_Repeating;
    }
    else
    {
        // default value
        field_modifier_ = FieldModifiers::e_TrimBoth;
    }
    return result;
}  // -----  end of method CRecord_DescVisitor::visitField_modifier  -----

std::any CRecord_DescVisitor::visitVirtual_list_field_name(CPP_Record_DescParser::Virtual_list_field_nameContext *ctx)
{
    std::any result = visitChildren(ctx);
    std::string fld_name = ctx->FIELD_NAME()->getText();

    // let's edit fld_name here to be sure it has been defined
    // earlier in the field list for this record type.
    // that way we can avoid runtime checks and catch an
    // error in the RecordDesc file.

    // const FieldList &flds_list = std::visit(
    //     Overloaded{[](std::monostate &) { return FieldList{}; }, [](auto &&arg) { return arg.GetFields(); }},
    //     record_);
    //
    // auto pos = rng::find_if(
    //     flds_list, [&fld_name](const auto &e)
    //     { return std::visit([&fld_name](const auto &fld) { return fld.GetFieldName() == fld_name; }, e); });
    // if (pos == flds_list.end())
    // {
    //     throw std::invalid_argument(std::format("Virtual field element: {} was not previously defined.", fld_name));
    // }
    //
    // // list_field_names_.push_back(fld_name);
    //
    // // let's translate the name to an index into the fields list so we can
    // // avoid searching by name when we are mapping record data to fields.
    //
    // auto distance = std::distance(flds_list.begin(), pos);

    // use logic from BaseRecord

    auto fld_nbr = std::visit(Overloaded{[](std::monostate &) { return size_t{0}; }, [&fld_name](const auto &rec)
                                         { return rec.ConvertFieldNameToNumber(fld_name); }},
                              record_);

    list_field_numbers_.push_back(fld_nbr);

    // std::print("virt source fld: {}. source fld nbr: {}\n", fld_name, fld_nbr);
    return result;
}  // -----  end of method CRecord_DescVisitor::visitVirtual_list_field_name  -----

std::any CRecord_DescVisitor::visitCombo_field(CPP_Record_DescParser::Combo_fieldContext *ctx)
{
    // collect the data necessary to contruct combo fields at runtime.
    // but first, clear out any previously collected data.

    // list_field_names_.clear();
    list_field_numbers_.clear();
    combo_fld_sep_char_.clear();

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->FIELD_NAME()->getText();

    CVirtualField::NameOrNumber names_or_numbers = {};
    if (ctx->NAME_WORD() != nullptr)
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNames;
    }
    else if (ctx->NUMBER_WORD() != nullptr)
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNumbers;
    }

    // field_separator_char and field_name_list are collected in
    // their respective visitors.

    CVirtualField new_field{names_or_numbers, combo_fld_sep_char_, list_field_numbers_};
    new_field.SetFieldName(fld_name);

    // we can get here for any record type so we'll just use a visitor
    // to pass the data to our CRecord object.

    std::visit(Overloaded{[&new_field](std::monostate &) { /* do nothing */ },
                          [&new_field](auto &arg) { arg.AddField(new_field); }},
               record_);

    return result;
}  // -----  end of method CRecord_DescVisitor::visitCombo_field  -----

std::any CRecord_DescVisitor::visitSynth_field(CPP_Record_DescParser::Synth_fieldContext *ctx)
{
    // collect the data necessary to contruct synth fields at runtime.
    // but first, clear out any previously collected data.

    // NOTE: this is the same as combo fields (except field_separator_char is required)
    // and the field name which can have leading '_'s
    // and gets mapped to a Virtual field.

    // list_field_names_.clear();
    list_field_numbers_.clear();
    combo_fld_sep_char_.clear();

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->synth_field_name()->getText();

    CVirtualField::NameOrNumber names_or_numbers = {};
    if (ctx->NAME_WORD() != nullptr)
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNames;
    }
    else if (ctx->NUMBER_WORD() != nullptr)
    {
        names_or_numbers = CVirtualField::NameOrNumber::e_UseNumbers;
    }

    // field_separator_char and field_name_list are collected in
    // their respective visitors.

    if (combo_fld_sep_char_.empty())
    {
        throw std::invalid_argument(std::format("Synth field: {} must specify a 'field_separator_char.", fld_name));
    }

    CVirtualField new_field{names_or_numbers, combo_fld_sep_char_, list_field_numbers_};
    new_field.SetFieldName(fld_name);

    // we can get here for any record type so we'll just use a visitor
    // to pass the data to our CRecord object.

    std::visit(Overloaded{[&new_field](std::monostate &) { /* do nothing */ },
                          [&new_field](auto &arg) { arg.AddField(new_field); }},
               record_);

    return result;

}  // -----  end of method CRecord_DescVisitor::visitSynth_field  -----

std::any CRecord_DescVisitor::visitArray_field(CPP_Record_DescParser::Array_fieldContext *ctx)
{
    // collect the data necessary to contruct array fields at runtime.
    // but first, clear out any previously collected data.

    // list_field_names_.clear();
    list_field_numbers_.clear();

    std::any result = visitChildren(ctx);

    auto fld_name = ctx->FIELD_NAME()->getText();

    auto fld_width = ctx->a->getText();
    size_t field_width = 0;
    auto [ptr, ec] = std::from_chars(fld_width.data(), fld_width.data() + fld_width.size(), field_width);
    if (ec != std::errc())
    {
        throw std::invalid_argument{std::format("Invalid 'field width': {}", fld_width)};
    }

    auto fld_count = ctx->b->getText();
    size_t field_count = 0;
    auto [ptr2, ec2] = std::from_chars(fld_count.data(), fld_count.data() + fld_count.size(), field_count);
    if (ec2 != std::errc())
    {
        throw std::invalid_argument{std::format("Invalid 'array field count': {}", fld_count)};
    }

    // if we are using a field name for the base field, it will be picked up and converted
    // to a field number during children processing.

    if (ctx->NUMBER_WORD() != nullptr)
    {
        auto base_fld = ctx->d->getText();
        int base_fld_nbr = 0;
        auto [ptr, ec] = std::from_chars(base_fld.data(), base_fld.data() + base_fld.size(), base_fld_nbr);
        if (ec != std::errc())
        {
            throw std::invalid_argument{std::format("Invalid 'array base field number': {}", base_fld)};
        }
        list_field_numbers_.push_back(base_fld_nbr);
    }

    CArrayField new_field{field_width, field_count, list_field_numbers_};
    new_field.SetFieldName(fld_name);

    // we can get here for any record type so we'll just use a visitor
    // to pass the data to our CRecord object.

    std::visit(Overloaded{[&new_field](std::monostate &) { /* do nothing */ },
                          [&new_field](auto &arg) { arg.AddField(new_field); }},
               record_);

    return result;

}  // -----  end of method CRecord_DescVisitor::visitSynth_field  -----
