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

#ifndef _CVARIABLERECORD_INC_
#define _CVARIABLERECORD_INC_

#include "RecordBase.h"

// =====================================================================================
//        Class:  CVariableRecord
//  Description:
//
// =====================================================================================
class CVariableRecord : public RecordBase<CVariableRecord>
{
   public:
    enum class FiedlNamesUsed : int32_t
    {
        e_FieldNames,
        e_FieldNumbers,
        e_UseHeader,
        e_Unknown
    };

    // ====================  LIFECYCLE     =======================================
    CVariableRecord() = default;  // constructor

    // ====================  ACCESSORS     =======================================

    [[nodiscard]] size_t GetFieldCount() const { return field_count_; }
    [[nodiscard]] FiedlNamesUsed GetFielNamesUsed() const { return use_field_names_; }

    // ====================  MUTATORS      =======================================

    void SetFieldDeimChar(char delim) { field_delim_char_ = delim; }
    void SetUseFieldNames(FiedlNamesUsed use_fld_names)
    {
        use_field_names_ = use_fld_names;
        if (use_field_names_ == FiedlNamesUsed::e_UseHeader)
        {
            look_for_header_ = true;
        }
    }
    void SetNumberOfFields(size_t fld_count) { field_count_ = fld_count; }

    void UseData(std::string_view record_data);

    // ====================  OPERATORS     =======================================

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    // ====================  METHODS       =======================================

    void CollectFieldNamesFromHeader(std::vector<std::string_view> field_names);
    [[nodiscard]] std::vector<std::string_view> GetVirtualFieldData(const std::vector<size_t>& field_numbers) const;

    friend std::formatter<CVariableRecord>;

    // ====================  DATA MEMBERS  =======================================

    FiedlNamesUsed use_field_names_ = FiedlNamesUsed::e_Unknown;

    char field_delim_char_ = {};
    size_t field_count_ = 0;
    bool look_for_header_ = false;

};  // -----  end of class CVariableRecord  -----

template <>
struct std::formatter<CVariableRecord::FiedlNamesUsed> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CVariableRecord::FiedlNamesUsed& names_used, std::format_context& ctx) const
    {
        using enum CVariableRecord::FiedlNamesUsed;
        std::string s;
        std::format_to(std::back_inserter(s), "{}",
                       names_used == e_FieldNames     ? "names"
                       : names_used == e_FieldNumbers ? "numbers"
                       : names_used == e_UseHeader    ? "header"
                                                      : "unknown");

        return formatter<std::string>::format(s, ctx);
    }
};

// a custom formater for variable record

template <>
struct std::formatter<CVariableRecord> : std::formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const CVariableRecord& a_record, std::format_context& ctx) const
    {
        std::string s;
        std::format_to(std::back_inserter(s),
                       "VariableRecord\n\tfld names used: {}\tdelimiter: ->{}<-\t# of fields: {}\tlook for header: "
                       "{}\t buffer len: {}\n",
                       a_record.use_field_names_, a_record.field_delim_char_, a_record.field_count_,
                       a_record.look_for_header_, a_record.GetBufferLen());

        for (const auto& fld : a_record.GetFields())
        {
            std::format_to(std::back_inserter(s), "{}\n", fld);
        }
        return formatter<std::string>::format(s, ctx);
    }
};

#endif  // ----- #ifndef _CVARIABLERECORD_INC_  -----
