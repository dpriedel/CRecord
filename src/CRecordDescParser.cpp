// =====================================================================================
//
//       Filename:  CRecordDescParser.cpp
//
//    Description:  Implements Record_Desc file parser using
//                  Antlr4 generated parser.
//
//        Version:  1.0
//        Created:  01/04/2023 02:17:17 PM
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

// #include <format>

#include "antlr4-runtime.h"

#include "CRecordDescParser.h"
#include "CPP_Record_DescLexer.h"
#include "CPP_Record_DescParser.h"
#include "CRecordDescVisitor.h"

#include "utilities.h"

using namespace antlr4;


std::optional<CRecord> CRecordDescParser::ParseRecordDescFile (const fs::path& record_desc_path)
{
	BOOST_ASSERT_MSG(fs::exists(record_desc_path), std::format("Can't find record description file: {}", record_desc_path).c_str());

    const std::string record_desc_data = LoadDataFileForUse(record_desc_path);

    ANTLRInputStream input(record_desc_data);
    CPP_Record_DescLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    // tokens.fill();
    // for (auto token : tokens.getTokens())
    // {
    //     std::cout << token->toString() << std::endl;
    // }

    CPP_Record_DescParser parser(&tokens);
    tree::ParseTree* parsed_tree = parser.record_desc();

    // std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

    CRecord_DescVisitor visitor;
    visitor.visit(parsed_tree);
	return {visitor.GetCRecord()};
}		// -----  end of method CRecordDescParser::ParseRecordDescFile  ----- 


