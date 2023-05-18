// =====================================================================================
//
//       Filename:  CRecordDescVisitor.h
//
//    Description:  Custom visitor class to generate CRecord based on 
//		            data from parsing record Description file.
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
  std::any visitLength_data_type(CPP_Record_DescParser::Length_data_typeContext *ctx) override;
  std::any visitFixed_field_entry(CPP_Record_DescParser::Fixed_field_entryContext *ctx) override;
  std::any visitCombo_field(CPP_Record_DescParser::Combo_fieldContext *ctx) override;
  std::any visitField_separator_char(CPP_Record_DescParser::Field_separator_charContext *ctx) override;
  std::any visitVirtual_list_field_name(CPP_Record_DescParser::Virtual_list_field_nameContext  *ctx) override;
  std::any visitSynth_field(CPP_Record_DescParser::Synth_fieldContext *ctx) override;
  std::any visitArray_field(CPP_Record_DescParser::Array_fieldContext *ctx) override;
  std::any visitField_modifier(CPP_Record_DescParser::Field_modifierContext *ctx) override;
  std::any visitVariable_header(CPP_Record_DescParser::Variable_headerContext *ctx) override;
  std::any visitField_names_used(CPP_Record_DescParser::Field_names_usedContext *ctx) override;
  std::any visitVariable_list_field_name(CPP_Record_DescParser::Variable_list_field_nameContext *ctx) override;

    // ====================  OPERATORS     ======================================= 

protected:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

    RecordTypes record_type_ = RecordTypes::e_Unknown;
	CRecord record_;
    // std::vector<std::string> list_field_names_;
    std::vector<size_t> list_field_numbers_;
    std::string combo_fld_sep_char_;

    FieldModifiers field_modifier_ = FieldModifiers::e_Unknown;
}; // -----  end of class CRecord_DescVisitor  ----- 


#endif   // ----- #ifndef _CRECORDDESCVISITOR_INC_  ----- 
