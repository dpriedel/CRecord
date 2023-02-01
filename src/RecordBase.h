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

#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <range/v3/algorithm/find_if.hpp>

#include "CField.h"

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
    std::string virtual_fields_buffer;

    size_t buffer_size_ = 0;
    size_t virtual_fields_buffer_size_ = 0;
}; // ----------  end of template class RecordBase  ---------- 

#endif   // ----- #ifndef _BASERECORD_INC_  ----- 

