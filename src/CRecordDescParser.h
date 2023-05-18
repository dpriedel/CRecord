// =====================================================================================
//
//       Filename:  CRecordDescParser.h
//
//    Description:  Class to parse Record_Desc files using
//                  Antlr4 generated parser.
//                  Implements the Antlr4 visitor interface.
//
//        Version:  1.0
//        Created:  01/04/2023 02:13:28 PM
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

#ifndef  _CRECORDDESCPARSER_INC_
#define  _CRECORDDESCPARSER_INC_

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

#include "CRecord.h"


// =====================================================================================
//        Class:  CRecordDescParser
//  Description:  Maybe just make this a standalone function -- too Java-like, this.  
//
// =====================================================================================
class CRecordDescParser
{
public:
	// ====================  LIFECYCLE     ======================================= 
	CRecordDescParser () = default;                             // constructor 

	// ====================  ACCESSORS     ======================================= 

	// ====================  MUTATORS      ======================================= 

	// ====================  OPERATORS     ======================================= 

    static std::optional<CRecord> ParseRecordDescFile(const fs::path& record_desc_path);

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

}; // -----  end of class CRecordDescParser  ----- 

#endif   // ----- #ifndef _CRECORDDESCPARSER_INC_  ----- 

