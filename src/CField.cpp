// =====================================================================================
//
//       Filename:  CField.cpp
//
//    Description:  Implementation of basic CField functions 
//
//        Version:  1.0
//        Created:  12/30/2022 02:34:22 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================

#include <stdexcept>

#include "CField.h"



//--------------------------------------------------------------------------------------
//       Class:  CFixedField
//      Method:  CFixedField
// Description:  constructor
//--------------------------------------------------------------------------------------
CFixedField::CFixedField (PositionMode position_mode, size_t a, size_t b)
{
    if (position_mode == PositionMode::e_Start_End)
    {
        offset_ = a - 1;
        length_ = b - a;
    }
    else if (position_mode == PositionMode::e_Start_Len)
    {
        offset_ = a - 1;
        length_ = b;
    }
    else
    {
        throw std::runtime_error("Length only not implmented yet.");
    }
}  // -----  end of method CFixedField::CFixedField  (constructor)  ----- 


std::string_view CFixedField::UseData (std::string_view record)
{
	return std::string_view{record.data() + offset_, length_};
}		// -----  end of method CFixedField::UseData  ----- 


