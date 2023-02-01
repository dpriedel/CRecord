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
  std::any visitField_entry(CPP_Record_DescParser::Field_entryContext *ctx) override;
  std::any visitCombo_field(CPP_Record_DescParser::Combo_fieldContext *ctx) override;
  std::any visitField_separator_char(CPP_Record_DescParser::Field_separator_charContext *ctx) override;
  std::any visitList_field_name(CPP_Record_DescParser::List_field_nameContext *ctx) override;
  std::any visitSynth_field(CPP_Record_DescParser::Synth_fieldContext *ctx) override;


    // ====================  OPERATORS     ======================================= 

protected:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:
    // ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

	CRecord record_;
    // std::vector<std::string> list_field_names_;
    std::vector<size_t> list_field_numbers_;
    std::string combo_fld_sep_char_;
}; // -----  end of class CRecord_DescVisitor  ----- 


#endif   // ----- #ifndef _CRECORDDESCVISITOR_INC_  ----- 
