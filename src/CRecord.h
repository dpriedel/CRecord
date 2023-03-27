// =====================================================================================
//
//       Filename:  CRecord.h
//
//    Description:  Provides interface to CRecord related classes 
//
//        Version:  1.0
//        Created:  12/30/2022 09:44:00 AM
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
    /* along with Extractor_Markup.  If not, see <http://www.gnu.org/licenses/>. */


#ifndef  _CRECORD_INC_
#define  _CRECORD_INC_

// #include <vector>
// #include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "CField.h"

#include "CFixedRecord.h"
#include "CVariableRecord.h"

	//	for error reporting
	
class Short_Record	:	public	std::length_error
{
	public:
		explicit Short_Record(const char* what_arg=nullptr) : length_error{what_arg} {}
		explicit Short_Record(const std::string& what_arg) : length_error{what_arg} {}
};
class Long_Record	:	public	std::length_error
{
	public:
		explicit Long_Record(const char* what_arg=nullptr) : length_error{what_arg} {}
		explicit Long_Record(const std::string& what_arg) : length_error{what_arg} {}
};

// class	CUnionRecord;
// 			

// include monostate first so CRecord can be default constructed.

using CRecord = std::variant<std::monostate, CFixedRecord, CVariableRecord>;

#endif   // ----- #ifndef _CRECORD_INC_  ----- 
