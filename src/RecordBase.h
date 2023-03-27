// =====================================================================================
//
//       Filename:  BaseRecord.h
//
//    Description:  base class which inherits from various record types 
//
//        Version:  1.0
//        Created:  01/23/2023 02:29:46 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================


#ifndef  _BASERECORD_INC_
#define  _BASERECORD_INC_

#include <charconv>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <range/v3/algorithm/find_if.hpp>

#include "CField.h"

// use this to make accessing the CRecord variant less opaque.

enum RecordTypes
{
    e_Unknown = 0,
    e_FixedRecord = 1,
    e_VariableRecord = 2
};

// =====================================================================================
//        Class:  RecordBase
//  Description: 
//
// =====================================================================================
template < class T >
class RecordBase
{
public:

    // ====================  LIFECYCLE     ======================================= 

	// ====================  ACCESSORS     ======================================= 
	
	const FieldList& GetFields() const { return fields_; }
	size_t GetBufferLen() const { return buffer_size_; }

    template<typename FLD_TYPE>
    const FLD_TYPE& GetField(std::string_view field_name) const
    {
        auto pos = ranges::find_if(fields_, [field_name] (const auto& entry) { return entry.field_name_ == field_name; });
        if (pos != fields_.end())
        {
            return std::get<FLD_TYPE>(pos->field_);
        }
        throw std::invalid_argument(fmt::format("Unknown field name: {}", field_name));
    }

    template<typename FLD_TYPE>
    const FLD_TYPE& GetField(size_t which) const
    {
        return std::get<FLD_TYPE>(fields_.at(which).field_);
    }

    // convert the data to the desired numerical type -- INT or FLOAT most likely.
    // this WILL THROW if the underlying data is not convertable to the specified
    // numeric type

    template<typename NBR_TYPE>
    NBR_TYPE ConvertFieldToNumber(std::string_view field_name) const
    {
        const auto fld_data = (*this)[field_name];
        NBR_TYPE base_fld_nbr;
        auto [ptr, ec] = std::from_chars(fld_data.data(), fld_data.data() + fld_data.size(), base_fld_nbr);
        if (ec != std::errc())
        {
            throw std::invalid_argument{fmt::format("Invalid 'array base field number': {}", fld_data)};
        }
        return base_fld_nbr;
    }
    template<typename NBR_TYPE>
    NBR_TYPE ConvertFieldToNumber(size_t which) const
    {
        const auto fld_data = (*this)[which];
        NBR_TYPE base_fld_nbr;
        auto [ptr, ec] = std::from_chars(fld_data.data(), fld_data.data() + fld_data.size(), base_fld_nbr);
        if (ec != std::errc())
        {
            throw std::invalid_argument{fmt::format("Invalid 'array base field number': {}", fld_data)};
        }
        return base_fld_nbr;
    }

	// ====================  MUTATORS      ======================================= 

    void SetBufferSize(size_t buf_size) { buffer_size_ = buf_size; }
    void AddField(const FieldData& new_field) { fields_.push_back(new_field); }

	// ====================  OPERATORS     ======================================= 
	
    std::string_view operator[](std::string_view field_name) const
    {
        auto pos = ranges::find_if(fields_, [field_name] (const auto& entry) { return entry.field_name_ == field_name; });
        if (pos != fields_.end())
        {
            return pos->field_data_;
        }
        throw std::invalid_argument(fmt::format("Unknown field name: {}", field_name));
    }

    std::string_view operator[](size_t which) const
    {
        return fields_.at(which).field_data_;
    }
    
protected:
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:

    RecordBase () = default;

    friend T;
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

    FieldList fields_;

    std::string buffer_;

    size_t buffer_size_ = 0;
}; // ----------  end of template class RecordBase  ---------- 

#endif   // ----- #ifndef _BASERECORD_INC_  ----- 

