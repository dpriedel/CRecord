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

#ifndef _BASERECORD_INC_
#define _BASERECORD_INC_

#include <algorithm>
#include <charconv>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>

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
template <class T>
class RecordBase
{
   public:
    // ====================  LIFECYCLE     =======================================

    // ====================  ACCESSORS     =======================================

    [[nodiscard]] const FieldList& GetFields() const { return fields_; }
    [[nodiscard]] size_t GetBufferLen() const { return buffer_size_; }

    [[nodiscard]] size_t ConvertFieldNameToNumber(std::string_view field_name) const
    {
        namespace rng = std::ranges;
        // namespace vws = std::ranges::views;

        auto pos = rng::find_if(
            fields_, [&field_name](const auto& fld)
            { return std::visit([&field_name](const auto& fld) { return field_name == fld.GetFieldName(); }, fld); });
        if (pos != fields_.end())
        {
            return rng::distance(rng::begin(fields_), pos);
        }
        throw std::invalid_argument(std::format("Unknown field name: {}", field_name));
    }

    template <typename FLD_TYPE>
    const FLD_TYPE& GetField(std::string_view field_name) const
    {
        namespace rng = std::ranges;
        // namespace vws = std::ranges::views;

        auto pos = rng::find_if(
            fields_, [&field_name](const auto& fld)
            { return std::visit([&field_name](const auto& fld) { return field_name == fld.GetFieldName(); }, fld); });
        if (pos != fields_.end())
        {
            return std::get<FLD_TYPE>(*pos);
        }
        throw std::invalid_argument(std::format("Unknown field name: {}", field_name));
    }

    template <typename FLD_TYPE>
    const FLD_TYPE& GetField(size_t which) const
    {
        return std::get<FLD_TYPE>(fields_.at(which));
    }

    // convert the data to the desired numerical type -- INT or FLOAT most likely.
    // this WILL THROW if the underlying data is not convertable to the specified
    // numeric type

    template <typename NBR_TYPE>
    NBR_TYPE ConvertFieldToNumber(std::string_view field_name) const
    {
        const auto fld_data = (*this)[field_name];
        NBR_TYPE base_fld_nbr;
        auto [ptr, ec] = std::from_chars(fld_data.data(), fld_data.data() + fld_data.size(), base_fld_nbr);
        if (ec != std::errc())
        {
            throw std::invalid_argument{std::format("Can't convert ->{}<- to a number.", fld_data)};
        }
        return base_fld_nbr;
    }

    template <typename NBR_TYPE>
    NBR_TYPE ConvertFieldToNumber(size_t which) const
    {
        const auto fld_data = (*this)[which];
        NBR_TYPE base_fld_nbr;
        auto [ptr, ec] = std::from_chars(fld_data.data(), fld_data.data() + fld_data.size(), base_fld_nbr);
        if (ec != std::errc())
        {
            throw std::invalid_argument{std::format("Can't convert ->{}<- to a number.", fld_data)};
        }
        return base_fld_nbr;
    }

    // ====================  MUTATORS      =======================================

    void SetBufferSize(size_t buf_size) { buffer_size_ = buf_size; }
    void AddField(const CField& new_field) { fields_.push_back(new_field); }

    // ====================  OPERATORS     =======================================

    template <class U>
    bool operator==(const RecordBase<U>& rhs) const
    {
        return (std::is_same_v<T, U> == true && rhs.fields_ == fields_);
    }

    std::string_view operator[](std::string_view field_name) const
    {
        namespace rng = std::ranges;
        // namespace vws = std::ranges::views;

        auto pos = rng::find_if(
            fields_, [field_name](const auto& field)
            { return std::visit([field_name](const auto& fld) { return field_name == fld.GetFieldName(); }, field); });
        if (pos != fields_.end())
        {
            return std::visit([](const auto& fld) { return fld.GetFieldData(); }, *pos);
        }
        throw std::invalid_argument(std::format("Unknown field name: {}", field_name));
    }

    std::string_view operator[](size_t which) const
    {
        return std::visit([&which](const auto& fld) { return fld.GetFieldData(); }, fields_.at(which));
    }

   protected:
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

   private:
    RecordBase() = default;

    friend T;
    // ====================  METHODS       =======================================

    // ====================  DATA MEMBERS  =======================================

    FieldList fields_;

    std::string buffer_;

    size_t buffer_size_ = 0;
};  // ----------  end of template class RecordBase  ----------

#endif  // ----- #ifndef _BASERECORD_INC_  -----
