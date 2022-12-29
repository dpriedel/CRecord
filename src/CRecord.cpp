/*
	File:		CRecord.cp

	Contains:	Implentation file with field offsets for each field in input record
				and output record.
				

	Written by:	Riedel

	Copyright:	© 1998 Printing For Systems, Inc. all rights reserved.

	Change History (most recent first):

				 8/26/98	DPR		version 1

	To Do:
*/

//----------------------------------------------------------------------------------------
// Includes: 
//----------------------------------------------------------------------------------------

#include <istream>
#include <ostream>
#include <limits>
#include <algorithm>

#include "boost/regex.hpp"

#include "CRecord.h"
#include "TException.h"
#include "string_utils.h"

//	available modifiers

CField::ModifierTable	gAvailableModifiers[]	=
	{
		{ "",		CField::e_NoModifiers },
		{ "LB",		CField::e_LeadingBlanks },
		{ "Signed",	CField::e_SignedField },			//	just to keep things happy
		{ "REQ",	CField::e_RequiredField },
		{ "NLB",	CField::e_NoLeadingBlanks }		//	mainly for Variable records which keep them by default
	};


CFixedRecord::FixedRecordMode	CFixedRecord::mFixedRecordMode;
size_t							CFixedRecord::mNextFieldStart;
char							CFixedRecord::mFixedTableDelim;			
char							CQuotedField::mEndPair[2];

//----------------------------------------------------------------------------------------


struct compare_modifiers	:	public	std::unary_function<const CField::ModifierTable&,  bool>
{
	CField::ModiferName mName;
	
		compare_modifiers(const CField::ModiferName& x)	:	mName(x)				{ }
	
	bool	operator()(const CField::ModifierTable& lhs)
	{
		return mName == lhs.mModifierName;
	}
};





/*****************************************************************************************
	CField
*****************************************************************************************/

CField::CField(void)
	:	mFieldStart(NULL), mFieldEnd(NULL), mDataEnd(NULL), mBeginPos(npos),
		mFieldLen(0), mDataLen(0), mModifiers(e_NoModifiers)

{
}

//----------------------------------------------------------------------------------------

CField::CField(const CField& rhs)
	:	mFieldName(rhs.mFieldName),
		mFieldStart(rhs.mFieldStart), mFieldEnd(rhs.mFieldEnd),
		mDataEnd(rhs.mDataEnd), 
		mBeginPos(rhs.mBeginPos), mFieldLen(rhs.mFieldLen),
		mDataLen(rhs.mDataLen), mModifiers(rhs.mModifiers)

{
}

//----------------------------------------------------------------------------------------

CField::CField(std::string& tableEntry)
	:	mFieldStart(NULL), mFieldEnd(NULL), mDataEnd(NULL), mBeginPos(npos),
		mFieldLen(0), mDataLen(0), mModifiers(e_NoModifiers)

{
	//	we assume that the only thing on the line is the field name
	//	this way we can have spaces and punctuation in the names
	
	mFieldName = tableEntry.c_str();
}

//----------------------------------------------------------------------------------------

CField::~CField(void)

{
	//	nothing to do
}

//----------------------------------------------------------------------------------------

/*
CField*	CField::MakeField(std::istream& table)

{
	//	need to recognize different options
}
*/
//----------------------------------------------------------------------------------------

CField&	CField::operator=(const CField& rhs)

{
	if (this != &rhs)
	{
		mFieldName	=	rhs.mFieldName;
		mFieldStart	=	rhs.mFieldStart;
		mFieldEnd	=	rhs.mFieldEnd;
		mDataEnd	=	rhs.mDataEnd;
		mBeginPos	=	rhs.mBeginPos;
		mFieldLen	=	rhs.mFieldLen;
		mDataLen	=	rhs.mDataLen;
		mModifiers	=	rhs.mModifiers;
	}
	return *this;
}

//----------------------------------------------------------------------------------------

bool	CField::operator==(const CField& rhs) const

{
	return (mFieldLen == rhs.mFieldLen
		&& mBeginPos == rhs.mBeginPos
		&& mFieldName == rhs.mFieldName);
}

//----------------------------------------------------------------------------------------

bool	CField::operator==(const char* rhs) const

{
	if (mDataLen	!=	strlen(rhs))
		return false;
	return std::memcmp(mFieldStart, rhs, mDataLen) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::is_equal_to(const CField* rhs) const

{
	if (mDataLen	!=	rhs->mDataLen)
		return false;
	return std::memcmp(mFieldStart, rhs->mFieldStart, mDataLen) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::is_equal_to(const char* rhs) const

{
	if (mDataLen	!=	strlen(rhs))
		return false;
	return std::memcmp(mFieldStart, rhs, mDataLen) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::is_less_than(const char* rhs) const

{
	size_t rhsLen	=	strlen(rhs);
	if (mDataLen	==	rhsLen)
		return std::memcmp(mFieldStart, rhs, mDataLen) < 0;
	else
	{
		int result	=	std::memcmp(mFieldStart, rhs, std::min(mDataLen, rhsLen));
		
		if (result	<	0)
			return true;
		else if (result	>	0)
			return false;
		else
			return (mDataLen < rhsLen)	?	true	:	false;
	}
}

//----------------------------------------------------------------------------------------

bool	CField::is_equal_to(const CField* rhs, size_t n) const

{
	return std::memcmp(mFieldStart, rhs->mFieldStart, n) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::is_equal_to(const char* rhs, size_t n) const

{
	return std::memcmp(mFieldStart, rhs, n) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::is_less_than(const char* rhs, size_t n) const

{
	return std::memcmp(mFieldStart, rhs, n) < 0;
}

//----------------------------------------------------------------------------------------

bool	CField::ci_Equivalent(const char* rhs) const

{
	if (mDataLen	!=	strlen(rhs))
		return false;
	return strnicmp(mFieldStart, rhs, mDataLen) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::ci_Equivalent(const CField* rhs) const

{
	if (mDataLen	!=	rhs->mDataLen)
		return false;
	return strnicmp(mFieldStart, rhs->mFieldStart, mDataLen) == 0;
}

//----------------------------------------------------------------------------------------

bool	CField::ci_Less(const char* rhs) const

{
	return strnicmp(mFieldStart, rhs, std::max(mDataLen, std::strlen(rhs))) < 0;

	size_t rhsLen	=	strlen(rhs);
	if (mDataLen	==	rhsLen)
		return strnicmp(mFieldStart, rhs, mDataLen) < 0;
	else
	{
		int result	=	strnicmp(mFieldStart, rhs, std::min(mDataLen, rhsLen));
		
		if (result	<	0)
			return true;
		else if (result	>	0)
			return false;
		else
			return (mDataLen < rhsLen)	?	true	:	false;
	}


}

//----------------------------------------------------------------------------------------

bool	CField::operator<(const CField& rhs) const

{
	return (mBeginPos < rhs.mBeginPos);
}

//----------------------------------------------------------------------------------------

bool	CField::is_less_than(const CField& rhs) const

{
	return std::lexicographical_compare(mFieldStart, mDataEnd, rhs.mFieldStart, rhs.mDataEnd);
}

//----------------------------------------------------------------------------------------

void	CField::WriteTo(std::ostream& output, const char* delim, size_t n) const

{
	if (mFieldStart)
		output.write(mFieldStart, std::min(n, mDataLen));
	
	if (delim)
		output << delim;
}

//----------------------------------------------------------------------------------------

void	CField::WriteTo(std::ostream& output, size_t n) const

{
	if (mFieldStart)
		output.write(mFieldStart, std::min(n, mDataLen));
	
}

//----------------------------------------------------------------------------------------

void	CField::CopyTo(char* output, size_t n) const

{
	//	better be room....
	
	if (mFieldStart)
		std::memcpy(output, mFieldStart, std::min(n, mDataLen));
}

//----------------------------------------------------------------------------------------

void	CField::AppendTo(std::string& output, size_t n) const

{
	if (mFieldStart)
		output.append(mFieldStart, std::min(n, mDataLen));
}

//----------------------------------------------------------------------------------------

void	CField::AppendWithDelim(std::string& output, unsigned char delim, size_t n) const

{
	if (mFieldStart)
		output.append(mFieldStart, std::min(n, mDataLen));
	
	output += delim;
}

//----------------------------------------------------------------------------------------

void	CField::AppendWithLongDelim(std::string& output, const char* delim, size_t n) const

{
	if (mFieldStart)
		output.append(mFieldStart, std::min(n, mDataLen));
	
	output += delim;
}

//----------------------------------------------------------------------------------------

void	CField::AppendToXlate(std::string&	output,
								char		orig,
								char		xlated,
								size_t		n) const

{
	if (mFieldStart)
	{
		//	first, compute our end positon

		const char* copyEnd = this->DataEnd();
		
		if (n != std::string::npos)
			copyEnd = std::min(this->DataEnd(), this->DataBegin() + n);
			
		if (xlated == '\0')
			std::remove_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig);
		else
			std::replace_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig, xlated);
	}
}

//----------------------------------------------------------------------------------------

void	CField::AppendWithDelimXlate(std::string&		output,
										char			orig,
										char			xlated,
										unsigned char	delim,
										size_t			n) const

{
	if (mFieldStart)
	{
		//	first, compute our end positon

		const char* copyEnd = this->DataEnd();
		
		if (n != std::string::npos)
			copyEnd = std::min(this->DataEnd(), this->DataBegin() + n);
			
		if (xlated == '\0')
			std::remove_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig);
		else
			std::replace_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig, xlated);
	}
	output += delim;
}

//----------------------------------------------------------------------------------------

void	CField::AppendWithLongDelimXlate(std::string&	output,
											char		orig,
											char		xlated,
											const char*	delim,
											size_t		n) const

{
	if (mFieldStart)
	{
		//	first, compute our end positon

		const char* copyEnd = this->DataEnd();
		
		if (n != std::string::npos)
			copyEnd = std::min(this->DataEnd(), this->DataBegin() + n);
			
		if (xlated == '\0')
			std::remove_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig);
		else
			std::replace_copy(this->DataBegin(), copyEnd, std::back_inserter(output), orig, xlated);
	}
	output += delim;
}

//----------------------------------------------------------------------------------------

void	CField::Clear(void)

{
	//	our default is to do nothing.
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CFixedField
*****************************************************************************************/

CFixedField::CFixedField(void)
	:	CField()

{
}

//----------------------------------------------------------------------------------------

CFixedField::CFixedField(const CFixedField& rhs)
	:	CField(rhs)

{
}

//----------------------------------------------------------------------------------------

CFixedField::~CFixedField(void)

{
}

//----------------------------------------------------------------------------------------

CFixedField*	CFixedField::Clone(void) const

{
	return new CFixedField(*this);
}

//----------------------------------------------------------------------------------------

void	CFixedField::UseData(const char* record, size_t data_len)

{
	if (mBeginPos > data_len)
		throw Short_Record("Short Record");
		
	//	we just need to know where we start and end
	
	mFieldStart = record + mBeginPos;
	mFieldEnd = mFieldStart + mFieldLen;
	
	//	need to see if we have any data in the field.
	
	//	start by skipping for leading blanks, but only if modifier not set
	
	if ((mModifiers & CField::e_LeadingBlanks) == 0)
	{
		for (; *mFieldStart == ' ' && mFieldStart < mFieldEnd; ++mFieldStart)
		{
			//	just loop
		}
	}
	
	//	now look for trailing blanks
	
	const char* i = mFieldEnd - 1;
	for (; *i == ' ' && i >= mFieldStart; --i)
	{
		//	just loop
	}
	
	//	ok remember what we saw
	
	if (i != mFieldStart)
	{
		mDataEnd = i + 1;
		mDataLen = mDataEnd - mFieldStart;
	}
	else
	{
		if (*i != ' ')
		{
			mDataEnd = i + 1;
			mDataLen = 1;
		}
		else
		{
			mDataEnd = mFieldStart;
			mDataLen = 0;
		}
	}
}

//----------------------------------------------------------------------------------------

CFixedField::CFixedField(std::string& tableEntry)

{
	//	need to grab our data from the input table
	
	//	we have optional modifier followed by optional field name followed 
	//	by starting position and field length.
	//	fields are comma separated.

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;

	//	look for our modifier

	pos = tableEntry.find_first_not_of(' ', 0);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	if (tableEntry[pos] == CFixedRecord::FixedTableDelim())
	{
		//	no modifier present

		mModifiers = CField::e_NoModifiers;
		pos = prev_pos = 0;
	}
	else
	{
		prev_pos = pos;
		pos = tableEntry.find(CFixedRecord::FixedTableDelim(), prev_pos);

		CField::ModiferName mName;
		mName.assign(&tableEntry[prev_pos], pos - prev_pos);

		static ModifierTable* tableEnd = &gAvailableModifiers[0] + sizeof(gAvailableModifiers)
			/ sizeof(gAvailableModifiers[0]);
			
		ModifierTable* theEntry = std::find_if(gAvailableModifiers, tableEnd, compare_modifiers(mName));
		if (theEntry != tableEnd)
		{
			//	we got a hit

			mModifiers |= theEntry->mModifierID;
			prev_pos = ++pos;
		}
		else
		{
			//	pretend we weren't here

			pos = prev_pos = 0;
		}
	}

	//	start by looking for field name.
	
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	if (tableEntry[pos] == CFixedRecord::FixedTableDelim())
	{
		//	no name present
	}
	else
	{
		prev_pos = pos;
		pos = tableEntry.find(CFixedRecord::FixedTableDelim(), prev_pos);
		mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);
	}

	prev_pos = ++pos;
	
	//	now, look for a number.
	
	size_t temp = std::atoi(&tableEntry[pos]);
	if (CFixedRecord::GetFixedRecordMode() != CFixedRecord::e_Len)
	{
		//	if we're not length only, then the number we have must be starting position
		
		mBeginPos = temp;
		dfail_if_(mBeginPos == 0, "Bad Field Begin in Table entry ==> ", tableEntry.c_str());
		--mBeginPos;					//	need to make zero-based
	}
	else
	{
		//	we're length only so we need to ask the record for field start
		
		mBeginPos = CFixedRecord::NextFieldStart();
	}
	
	if (CFixedRecord::GetFixedRecordMode() == CFixedRecord::e_Start_Len)
	{
		pos = tableEntry.find(CFixedRecord::FixedTableDelim(), prev_pos);
		dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());
	
		++pos;							//	bump past delimiter
	
		mFieldLen = std::atoi(&tableEntry[pos]);
	}
	else if (CFixedRecord::GetFixedRecordMode() == CFixedRecord::e_Start_End)
	{
		pos = tableEntry.find(CFixedRecord::FixedTableDelim(), prev_pos);
		dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());
	
		++pos;							//	bump past delimiter
	
		int temp = std::atoi(&tableEntry[pos]);
		mFieldLen = temp - mBeginPos;	//	beginpos already decremented so don't add 1
	}
	else
	{
		mFieldLen = temp;
		CFixedRecord::IncrementFieldStart(mFieldLen);
	}

	dfail_if_(mFieldLen == 0, "Bad Field End in Table entry ==> ", tableEntry.c_str());
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CVariableField
*****************************************************************************************/

CVariableField::CVariableField(void)
	:	CField(), mHowManyDelims(1), mDelim('\t'), mTerminators(1, mDelim)

{
}

//----------------------------------------------------------------------------------------

CVariableField::CVariableField(const CVariableField& rhs)
	:	CField(rhs), mHowManyDelims(rhs.mHowManyDelims), mDelim(rhs.mDelim),
		mTerminators(rhs.mTerminators)

{
}

//----------------------------------------------------------------------------------------

CVariableField::~CVariableField(void)

{
}

//----------------------------------------------------------------------------------------

CVariableField*	CVariableField::Clone(void) const

{
	return new CVariableField(*this);
}

//----------------------------------------------------------------------------------------

void	CVariableField::UseData(const char* record, size_t data_len)

{
	//	We start looking from the pointer we get and go for mHowManyDelims
	//	fields to get to the end
	//	(this supports redefines)

     mBeginPos  =   reinterpret_cast<size_t>(const_cast<char*>(record));        //  stays constant while we use this record

    mFieldStart = record;
	size_t len = data_len;
	
	const char* x = mFieldStart;
	const char* dataEnd = record + len;					//	this stays fixed
	
	for (int i = 0; i < mHowManyDelims; ++i)
	{
		/*
		mFieldEnd = reinterpret_cast<const char*>(std::memchr(x, mDelim, len));
		//mFieldEnd = x + std::strcspn(x, mFieldDelims);

		if (! mFieldEnd)
		{
			//	we may be at the end of the record, so check for that
			
			if (*(x + data_len) == LINE_END_CHAR || *(x + len) == '\0')
			{
				//	we're ok
				
				mFieldEnd = x + len;
			}
			else
				dfail_msg_("No field delimiter in record.");
		}
		x = mFieldEnd + 1;
		*/
		mFieldEnd = std::find_first_of(x, dataEnd, mTerminators.begin(), mTerminators.end());
		if (mFieldEnd == dataEnd)
		{
			if (mTerminators.find(*mFieldEnd) != Terminators::npos)
			{
				//	we're ok -- end of record
			}
			else
				throw Short_Record("Short Record. No more field delimiters in record.");
		}
		else
			x = mFieldEnd + 1;
	}
	
	len -= mFieldEnd - mFieldStart;
	mFieldLen = mFieldEnd - mFieldStart;
	
	//	compute data length
	//	need to see if we have any data in the field.
	
	//	start by skipping for leading blanks, but only if modifier set
	
	if ((mModifiers & CField::e_NoLeadingBlanks) != 0)
	{
		for (; *mFieldStart == ' ' && mFieldStart < mFieldEnd; ++mFieldStart)
		{
			//	just loop
		}
	}
	
	//	now look for trailing blanks
	
	const char* i = mFieldEnd - 1;
	for (; (*i == ' ' || *i == mDelim) && i > mFieldStart; --i)
	{
		//	just loop
	}
	
	//	ok remember what we saw
	
	mDataLen = mDataEnd - mFieldStart;
	
	if (i != mFieldStart)
	{
		mDataEnd = i + 1;
		mDataLen = mDataEnd - mFieldStart;
	}
	else
	{
		if (*i != ' ')
		{
			mDataEnd = i + 1;
			mDataLen = 1;
		}
		else
		{
			mDataEnd = mFieldStart;
			mDataLen = 0;
		}
	}
	
}

//----------------------------------------------------------------------------------------

CVariableField::CVariableField(std::string& tableEntry, char delim, short count)
	:	CField(tableEntry),mHowManyDelims(count),  mDelim(delim), mTerminators(1, mDelim)

{
	mTerminators += "\n\r";
	mTerminators += '\0';					//	force NULL char to be added

	//	look for our modifier
	
	std::string::size_type pos, prev_pos	=	0;

	pos = tableEntry.find_first_not_of(' ', 0);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	if (tableEntry.compare(0, 4, "NLB\t")	==	0)
	{
		CField::ModiferName mName;
		mName.assign(&tableEntry[0], 3);

		static ModifierTable* tableEnd = &gAvailableModifiers[0] + sizeof(gAvailableModifiers)
			/ sizeof(gAvailableModifiers[0]);
			
		ModifierTable* theEntry = std::find_if(gAvailableModifiers, tableEnd, compare_modifiers(mName));
		if (theEntry != tableEnd)
		{
			//	we got a hit

			mModifiers |= theEntry->mModifierID;
			prev_pos = 4;						//	skip over modifier
		}
		else
		{
			//	pretend we weren't here

			pos = prev_pos = 0;
		}
	}

	//	start by looking for field name.
	
	pos			=	tableEntry.find_first_not_of(' ', prev_pos);		//	point to start of field name
	mFieldName	=	&tableEntry[pos];
}

//----------------------------------------------------------------------------------------

CVariableField::CVariableField(char delim, short count)
	:	 CField(), mHowManyDelims(count), mDelim(delim), mTerminators(1, mDelim)
	
{
	mTerminators += "\n\r";
	mTerminators += '\0';					//	force NULL char to be added
}

//----------------------------------------------------------------------------------------

void	CVariableField::CopyTo(char* output, size_t n) const
	
{
	//	need to exclude any embedded delimiters
	
	if (mHowManyDelims == 1)
		CField::CopyTo(output, n);
	else
	{
		dthrow_msg_("CRecord: Unimplemented feature -- multi-delim Copy"); 
	}
}

//----------------------------------------------------------------------------------------

void	CVariableField::AppendTo(std::string& output, size_t n) const
	
{
	//	need to exclude any embedded delimiters

	if (mHowManyDelims == 1)
		CField::AppendTo(output, n);
	else
	{
		dthrow_msg_("CRecord: Unimplemented feature -- multi-delim Append"); 
	}
}

//----------------------------------------------------------------------------------------

void	CVariableField::AppendWithDelim(std::string& output, unsigned char delim, size_t n) const
	
{
	//	need to exclude any embedded delimiters

	if (mHowManyDelims == 1)
		CField::AppendWithDelim(output, delim, n);
	else
	{
		dthrow_msg_("CRecord: Unimplemented feature -- multi-delim Append"); 
	}
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CSYNTHField
*****************************************************************************************/

CSYNTHField::CSYNTHField(void)
	:	CField(), mTheRecord(NULL), mComponentDelim('|')

{
}

//----------------------------------------------------------------------------------------

CSYNTHField::CSYNTHField(const CSYNTHField& rhs)
	:	CField(rhs), mFieldData(rhs.mFieldData), mFieldNumbers(rhs.mFieldNumbers),
		mTheRecord(rhs.mTheRecord), mComponentDelim(rhs.mComponentDelim)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CSYNTHField*	CSYNTHField::Clone(void) const

{
	return new CSYNTHField(*this);
}

//----------------------------------------------------------------------------------------

CSYNTHField::CSYNTHField(CRecord* theRecord, std::string& tableEntry)
	:	mTheRecord(theRecord)

{
	//	we can have either Names or numbers

	//	first, let's skip over our modifier tag

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	//	start by skipping our field type
	//	we know it's 'SYNTH' so we work with that

	char tableDelim = tableEntry[5];
	
	pos = tableEntry.find(tableDelim, prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	next, look for our name

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);
	mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);		//	may be empty

	//	next, see how we are to refer to component fields
	//	choice are 'Name' or 'Number'

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);

	bool useNames = false;
	if (strnicmp(&tableEntry[prev_pos], "Name", pos - prev_pos)  == 0)
		useNames = true;
	else if (strnicmp(&tableEntry[prev_pos], "Number", pos - prev_pos)  == 0)
		useNames = false;
	else
		dfail_msg_("Unknown SNTH component type ==> ", tableEntry.c_str());
	
	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	now we should be looking at a single character component delimiter
	//	use the word 'Tab' for tab delimiter
	
	if (memcmp(&tableEntry[pos], "Tab", 3) == 0)
	{
		mComponentDelim	=	'\t';
		pos	+= 3;
		prev_pos += 3;
	}
	else
	{
		mComponentDelim	=	tableEntry[pos];
		prev_pos = ++pos;
	}
	
	//	should be looking at table delim char
	
	dfail_if_(tableEntry[pos] != tableDelim, "Bad Table entry ==> ", tableEntry.c_str());

	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	we should be looking at a comma-delimited list our component fields
	//	so we count how many we have then loop over the table

	int componentCount = std::count(tableEntry.begin() + prev_pos, tableEntry.end(), ',');
		
	for (int i = 0; i < componentCount; ++i)
	{
		pos = tableEntry.find(',', prev_pos);
		CField::FieldName temp(&tableEntry[prev_pos], pos - prev_pos);
		
		int fldNumber = 0;
		if (useNames)
			fldNumber = mTheRecord->GetFieldNumber(temp);
		else
			fldNumber = atoi(temp.c_str());

		mFieldNumbers.push_back(fldNumber);

		prev_pos = ++pos;
		pos = tableEntry.find_first_not_of(' ', prev_pos);
		prev_pos = pos;
	}

	//	now, grab our last field

	CField::FieldName temp(&tableEntry[prev_pos]);
	int fldNumber = 0;
	if (useNames)
		fldNumber = mTheRecord->GetFieldNumber(temp);
	else
		fldNumber = atoi(temp.c_str());

	mFieldNumbers.push_back(fldNumber);
}

//----------------------------------------------------------------------------------------

CSYNTHField::~CSYNTHField(void)

{
	//	nothing special to do since vectors manage their own storage
}

//----------------------------------------------------------------------------------------

void	CSYNTHField::UseData(const char* record, size_t data_len)

{
	//	we need to loop over our components and grab their contents

	mFieldData.clear();

	for (std::vector<int>::iterator i = mFieldNumbers.begin();
		i != mFieldNumbers.end(); ++i)
	{
		const CField& baseFld = (*mTheRecord)[*i];
		//std::copy(baseFld.DataBegin(), baseFld.DataEnd(), std::back_inserter(mFieldData));
		mFieldData.append(baseFld.DataBegin(), baseFld.Size());
		mFieldData.append(1, mComponentDelim);
	}

	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();

}

//----------------------------------------------------------------------------------------

CSYNTHField&	CSYNTHField::operator=(const CSYNTHField& rhs)

{
	if (this != &rhs)
	{
		CField::operator=(rhs);
		
		mFieldData		=	rhs.mFieldData;
		mFieldNumbers	=	rhs.mFieldNumbers;
		mTheRecord		=	rhs.mTheRecord;
		mComponentDelim	=	rhs.mComponentDelim;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------

/*
bool	CSYNTHField::operator==(const char* rhs) const

{
	//	main thing here is support for '*' in a component to support generic matches
	//	first, see if we need to do a generic search
	//	
	//	NOTE:	if there are embedded '*' then the generic search will be forced.

	//if (std::find(mFieldData.begin(), mFieldData.end(), '*') == mFieldData.end()
	//	&& strchr(rhs, '*') == NULL)
	//{
		//	ok, simple comparison

	//	return CField::operator==(rhs);
	//}

	//	ok, now we're in for it
	//	first, let's split our search string into parts on the delimiter

	size_t searchStringParts = std::count(rhs, rhs + strlen(rhs), mComponentDelim) + 1;
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", rhs);

	std::vector<cp_string<150> > searchParts;

	const char* x = rhs;
	const char* y = rhs;
	for (size_t i = 0; i < searchStringParts; ++i)
	{
		y = strchr(x, mComponentDelim);
		if (y == NULL)
		{
			//	last 1

			cp_string<150> newEntry(x);
			searchParts.push_back(newEntry);
			break;											//	should be redundant
		}

		cp_string<150> newEntry(x, y - x);
		searchParts.push_back(newEntry);
		x = ++y;
	}

	//	next we split our own data
	
	std::vector<cp_string<150> > ourParts;

	x = mFieldData.begin();
	y = rhs;
	for (size_t i = 0; i < searchStringParts; ++i)
	{
		y = strchr(x, mComponentDelim);
		if (y == NULL)
		{
			//	last 1

			cp_string<150> newEntry(x);
			ourParts.push_back(newEntry);
			break;											//	should be redundant
		}

		cp_string<150> newEntry(x, y - x);
		ourParts.push_back(newEntry);
		x = ++y;
	}
	//	ok, now we compare each part to our component field.
	//	if either == '*' then we consider it a match

	bool aMatch = true;
	for (size_t i = 0; i < searchStringParts; ++i)
	{
		if (searchParts[i] == "*" || (*mTheRecord)[mFieldNumbers[i]] == "*")
			continue;

		if ((*mTheRecord)[mFieldNumbers[i]] == searchParts[i].c_str())
			continue;

		aMatch = false;
		break;
	}

	return aMatch;
	
}
*/
//----------------------------------------------------------------------------------------

bool	CSYNTHField::operator==(const char* key) const

{
	//	main thing here is support for '*' in a component to support generic matches

	size_t searchStringParts = std::count(key, key + strlen(key), mComponentDelim);
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", key);

	const char* lhs		=	mFieldData.c_str();
	const char* rhs		=	key;
	const char* lhs_end	=	lhs + mFieldData.size();
	const char* rhs_end	=	key + strlen(key);

	bool aMatch = true;
	while (lhs < lhs_end && rhs < rhs_end)
	{
		//	first look for generic comparison

		if (*lhs == '*')
		{
			while(*rhs != mComponentDelim && rhs < rhs_end)
				++rhs;

			if (*rhs == mComponentDelim)
				++rhs;
				
			++lhs;										//	skip delm
			++lhs;

			continue;									//	next component
		}
		else if (*rhs == '*')
		{
			while(*lhs != mComponentDelim && lhs < lhs_end)
				++lhs;

			if (*lhs == mComponentDelim)
				++lhs;
				
			++rhs;										//	skip delm
			++rhs;

			continue;									//	next component
		}
		
		//	neither part is generic compare

		while (lhs < lhs_end && rhs < rhs_end && *lhs != mComponentDelim && *rhs != mComponentDelim)
		{
			if (*lhs != *rhs)
				break;
				
			++lhs;
			++rhs;
		}

		if (*lhs == mComponentDelim && *rhs == mComponentDelim)
		{
			++lhs;
			++rhs;
			continue;									//	next component
		}

		if (lhs == lhs_end && rhs == rhs_end)
			break;										//	we're done, it's a match

		//	both pointers are not pointing to delimiters and both are not
		//	at the end so there is a difference.

		aMatch = false;
		break;
	}
	return aMatch;
	
}

//----------------------------------------------------------------------------------------

bool	CSYNTHField::is_equal_to(const char* rhs) const
{
	return this->operator==(rhs);
}

//----------------------------------------------------------------------------------------


/*
bool	CSYNTHField::ci_Equivalent(const char* rhs) const

{
	//	main thing here is support for '*' in a component to support generic matches
	//	first, see if we need to do a generic search

	//if (std::find(mFieldData.begin(), mFieldData.end(), '*') == mFieldData.end()
	//	&& strchr(rhs, '*') == NULL)
	//{
		//	ok, simple comparison

	//	return CField::operator==(rhs);
	//}

	//	ok, now we're in for it
	//	first, let's split our string into parts on the delimiter

	size_t searchStringParts = std::count(rhs, rhs + strlen(rhs), mComponentDelim) + 1;
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", rhs);

	std::vector<cp_string<150> > parts;

	const char* x = rhs;
	const char* y = rhs;
	for (size_t i = 0; i < searchStringParts; ++i)
	{
		y = strchr(x, mComponentDelim);
		if (y == NULL)
		{
			//	last 1

			cp_string<150> newEntry(x);
			parts.push_back(newEntry);
			break;											//	should be redundant
		}

		cp_string<150> newEntry(x, y - x);
		parts.push_back(newEntry);
		x = ++y;
	}

	//	ok, now we compare each part to our component field.
	//	if either == '*' then we consider it a match

	bool aMatch = true;
	for (size_t i = 0; i < searchStringParts; ++i)
	{
		if (parts[i] == "*" || (*mTheRecord)[mFieldNumbers[i]] == "*")
			continue;

		if ((*mTheRecord)[mFieldNumbers[i]].ci_Equivalent(parts[i].c_str()))
			continue;

		aMatch = false;
		break;
	}

	return aMatch;
	
}
*/
//----------------------------------------------------------------------------------------

bool	CSYNTHField::ci_Equivalent(const char* key) const

{
	//	main thing here is support for '*' in a component to support generic matches

	size_t searchStringParts = std::count(key, key + strlen(key), mComponentDelim);
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", key);

	const char* lhs		=	mFieldData.c_str();
	const char* rhs		=	key;
	const char* lhs_end	=	lhs + mFieldData.size();
	const char* rhs_end	=	key + strlen(key);

	bool aMatch = true;
	while (lhs < lhs_end && rhs < rhs_end)
	{
		//	first look for generic comparison

		if (*lhs == '*')
		{
			while(*rhs != mComponentDelim && rhs < rhs_end)
				++rhs;

			if (*rhs == mComponentDelim)
				++rhs;
				
			++lhs;										//	skip delm
			++lhs;

			continue;									//	next component
		}
		else if (*rhs == '*')
		{
			while(*lhs != mComponentDelim && lhs < lhs_end)
				++lhs;

			if (*lhs == mComponentDelim)
				++lhs;
				
			++rhs;										//	skip delm
			++rhs;

			continue;									//	next component
		}
		
		//	neither part is generic compare

		while (lhs < lhs_end && rhs < rhs_end && *lhs != mComponentDelim && *rhs != mComponentDelim)
		{
			if (toupper(*lhs) != toupper(*rhs))
				break;
			++lhs;
			++rhs;
		}

		if (*lhs == mComponentDelim && *rhs == mComponentDelim)
		{
			++lhs;
			++rhs;
			continue;									//	next component
		}

		if (lhs == lhs_end && rhs == rhs_end)
			break;										//	we're done, it's a match

		//	both pointers are not pointing to delimiters and both are not
		//	at the end so there is a difference.

		aMatch = false;
		break;
	}
	return aMatch;
	
}

//----------------------------------------------------------------------------------------

bool	CSYNTHField::is_less_than(const char* key) const

{
	//	main thing here is support for '*' in a component to support generic matches

	size_t searchStringParts = std::count(key, key + strlen(key), mComponentDelim);
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", key);

	const char* lhs		=	mFieldData.c_str();
	const char* rhs		=	key;
	const char* lhs_end	=	lhs + mFieldData.size();
	const char* rhs_end	=	key + strlen(key);

	bool lessThan = true;
	while (lhs < lhs_end && rhs < rhs_end)
	{
		//	first look for generic comparison

		if (*lhs == '*')
		{
			while(*rhs != mComponentDelim && rhs < rhs_end)
				++rhs;

			if (*rhs == mComponentDelim)
				++rhs;
				
			++lhs;										//	skip delm
			++lhs;

			continue;									//	next component
		}
		else if (*rhs == '*')
		{
			while(*lhs != mComponentDelim && lhs < lhs_end)
				++lhs;

			if (*lhs == mComponentDelim)
				++lhs;
				
			++rhs;										//	skip delm
			++rhs;

			continue;									//	next component
		}
		
		//	neither part is generic compare

		while (lhs < lhs_end && rhs < rhs_end && *lhs != mComponentDelim && *rhs != mComponentDelim)
		{
			if (*lhs >= *rhs)
			{
				lessThan = false;
				break;
			}
			++lhs;
			++rhs;
		}

		if (lessThan && *lhs == mComponentDelim && *rhs == mComponentDelim)
		{
			++lhs;
			++rhs;
			continue;									//	next component
		}

		if (lhs == lhs_end && rhs == rhs_end)
			break;										//	we're done, it's a match

		//	both pointers are not pointing to delimiters and both are not
		//	at the end so there is a difference.

		lessThan = false;
		break;
	}
	return lessThan;
	
}

//----------------------------------------------------------------------------------------

bool	CSYNTHField::ci_Less(const char* key) const

{
	//	main thing here is support for '*' in a component to support generic matches

	size_t searchStringParts = std::count(key, key + strlen(key), mComponentDelim);
	dfail_if_(searchStringParts != mFieldNumbers.size(), "Wrong number of components ==> ", key);

	const char* lhs		=	mFieldData.c_str();
	const char* rhs		=	key;
	const char* lhs_end	=	lhs + mFieldData.size();
	const char* rhs_end	=	key + strlen(key);

	bool lessThan = true;
	while (lhs < lhs_end && rhs < rhs_end)
	{
		//	first look for generic comparison

		if (*lhs == '*')
		{
			while(*rhs != mComponentDelim && rhs < rhs_end)
				++rhs;

			if (*rhs == mComponentDelim)
				++rhs;
				
			++lhs;										//	skip delm
			++lhs;

			continue;									//	next component
		}
		else if (*rhs == '*')
		{
			while(*lhs != mComponentDelim && lhs < lhs_end)
				++lhs;

			if (*lhs == mComponentDelim)
				++lhs;
				
			++rhs;										//	skip delm
			++rhs;

			continue;									//	next component
		}
		
		//	neither part is generic compare

		while (lhs < lhs_end && rhs < rhs_end && *lhs != mComponentDelim && *rhs != mComponentDelim)
		{
			if (tolower(*lhs) >= tolower(*rhs))
			{
				lessThan = false;
				break;
			}
			++lhs;
			++rhs;
		}

		if (lessThan && *lhs == mComponentDelim && *rhs == mComponentDelim)
		{
			++lhs;
			++rhs;
			continue;									//	next component
		}

		if (lhs == lhs_end && rhs == rhs_end)
			break;										//	we're done, it's a match

		//	both pointers are not pointing to delimiters and both are not
		//	at the end so there is a difference.

		lessThan = false;
		break;
	}
	return lessThan;
	
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CComboField
*****************************************************************************************/

CComboField::CComboField(void)
	:	CField(), mTheRecord(NULL), mComponentDelim('\0')

	//	If this null terminator is not overlayed, 
	//	it will tell us (in UseData) that
	//	we have no (optional) component delimter
{
}

//----------------------------------------------------------------------------------------

CComboField::CComboField(const CComboField& rhs)
	:	CField(rhs), mFieldData(rhs.mFieldData), mFieldNumbers(rhs.mFieldNumbers),
		mTheRecord(rhs.mTheRecord), mComponentDelim(rhs.mComponentDelim)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CComboField*	CComboField::Clone(void) const

{
	return new CComboField(*this);
}

//----------------------------------------------------------------------------------------

CComboField::CComboField(CRecord* theRecord, std::string& tableEntry)
	:	mTheRecord(theRecord), mComponentDelim('\0')

{
	//	we can have either Names or numbers

	//	first, let's skip over our modifier tag

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	//	start by skipping our field type
	//	we know it's 'Combo' so we work with that

	char tableDelim = tableEntry[5];
	
	pos = tableEntry.find(tableDelim, prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	next, look for our name

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);
	mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);		//	may be empty

	//	next, see how we are to refer to component fields
	//	choice are 'Name' or 'Number'

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);

	bool useNames = false;
	if (strnicmp(&tableEntry[prev_pos], "Name", pos - prev_pos)  == 0)
		useNames = true;
	else if (strnicmp(&tableEntry[prev_pos], "Number", pos - prev_pos)  == 0)
		useNames = false;
	else
		dfail_msg_("Unknown Combo component type ==> ", tableEntry.c_str());
	
	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	now we should be looking at a single character component delimiter
	//	use the word 'Tab' for tab delimiter
	//

	if (memcmp(&tableEntry[pos], "Tab", 3) == 0)
	{
		mComponentDelim	=	'\t';
		pos	+= 3;
		prev_pos += 3;
	}
	else if (memcmp(&tableEntry[pos], "Spc", 3) == 0)
	{
		mComponentDelim	=	' ';
		pos	+= 3;
		prev_pos += 3;
	}
	else if (tableEntry[pos] != tableDelim)
	{
		mComponentDelim	= tableEntry[pos];	// if we have one, save it
		prev_pos = ++pos;
	}

	//	should be looking at table delim char
	
	dfail_if_(tableEntry[pos] != tableDelim, "Bad Table entry ==> ", tableEntry.c_str());

	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	we should be looking at a comma-delimited list our component fields
	//	so we count how many we have then loop over the table

	int componentCount = std::count(tableEntry.begin() + prev_pos, tableEntry.end(), ',');
		
	for (int i = 0; i < componentCount; ++i)
	{
		pos = tableEntry.find(',', prev_pos);
		CField::FieldName temp(&tableEntry[prev_pos], pos - prev_pos);
		
		int fldNumber = 0;
		if (useNames)
			fldNumber = mTheRecord->GetFieldNumber(temp);
		else
			fldNumber = atoi(temp.c_str());

		mFieldNumbers.push_back(fldNumber);

		prev_pos = ++pos;
		pos = tableEntry.find_first_not_of(' ', prev_pos);
		prev_pos = pos;
	}

	//	now, grab our last field

	CField::FieldName temp(&tableEntry[prev_pos]);
	int fldNumber = 0;
	if (useNames)
		fldNumber = mTheRecord->GetFieldNumber(temp);
	else
		fldNumber = atoi(temp.c_str());

	mFieldNumbers.push_back(fldNumber);
}

//----------------------------------------------------------------------------------------

CComboField::~CComboField(void)

{
	//	nothing special to do since vectors manage their own storage
}

//----------------------------------------------------------------------------------------

void	CComboField::UseData(const char* record, size_t data_len)

{
	//	we need to loop over our components and grab their contents

	mFieldData.clear();

	for (std::vector<int>::iterator i = mFieldNumbers.begin();
		i != mFieldNumbers.end(); ++i)
	{
		const CField& baseFld = (*mTheRecord)[*i];
		//std::copy(baseFld.DataBegin(), baseFld.DataEnd(), std::back_inserter(mFieldData));
		mFieldData.append(baseFld.DataBegin(), baseFld.Size());
		//NOTE: Unlike SYNTH, a Combo optional delimiter will only appear between components:
		if ((mComponentDelim != '\0') && (i + 1 < mFieldNumbers.end()))
		{
			mFieldData.append(1, mComponentDelim);
		}
	}

	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();

}

//----------------------------------------------------------------------------------------

CComboField&	CComboField::operator=(const CComboField& rhs)

{
	if (this != &rhs)
	{
		CField::operator=(rhs);
		
		mFieldData		=	rhs.mFieldData;
		mFieldNumbers	=	rhs.mFieldNumbers;
		mTheRecord		=	rhs.mTheRecord;
		mComponentDelim	=	rhs.mComponentDelim;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CSkip2DelimField
*****************************************************************************************/

CSkip2DelimField::CSkip2DelimField(void)
	:	CField(), mTheRecord(NULL), mBaseFieldNumber(0), mDelim('\0')

	//	If this null terminator is not overlayed, 
	//	it will tell us (in UseData) that
	//	we have no (optional) component delimter
{
}

//----------------------------------------------------------------------------------------

CSkip2DelimField::CSkip2DelimField(const CSkip2DelimField& rhs)
	:	CField(rhs), mFieldData(rhs.mFieldData), mTheRecord(rhs.mTheRecord),
		mBaseFieldNumber(rhs.mBaseFieldNumber), mDelim(rhs.mDelim)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CSkip2DelimField*	CSkip2DelimField::Clone(void) const

{
	return new CSkip2DelimField(*this);
}

//----------------------------------------------------------------------------------------

CSkip2DelimField::CSkip2DelimField(CRecord* theRecord, std::string& tableEntry)
	:	mTheRecord(theRecord), mBaseFieldNumber(0), mDelim('\0')

{
	//	we can have either Names or numbers

	//	first, let's skip over our modifier tag

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	//	start by skipping our field type
	//	we know it's 'Skip2Delim' so we work with that

	char tableDelim = tableEntry[10];
	
	pos = tableEntry.find(tableDelim, prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	next, look for our name

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);
	mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);		//	may be empty

	//	next, see how we are to refer to component fields
	//	choice are 'Name' or 'Number'

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);

	bool useNames = false;
	if (strnicmp(&tableEntry[prev_pos], "Name", pos - prev_pos)  == 0)
		useNames = true;
	else if (strnicmp(&tableEntry[prev_pos], "Number", pos - prev_pos)  == 0)
		useNames = false;
	else
		dfail_msg_("Unknown Skip2Delim component type ==> ", tableEntry.c_str());
	
	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	now we should be looking at a single character delimiter
	//	use the word 'Tab' for tab delimiter
	//

	if (memcmp(&tableEntry[pos], "Tab", 3) == 0)
	{
		mDelim	=	'\t';
		pos	+= 3;
		prev_pos += 3;
	}
	else if (memcmp(&tableEntry[pos], "Spc", 3) == 0)
	{
		mDelim	=	' ';
		pos	+= 3;
		prev_pos += 3;
	}
	else if (tableEntry[pos] != tableDelim)
	{
		mDelim	= tableEntry[pos];	// if we have one, save it
		prev_pos = ++pos;
	}

	//	should be looking at table delim char
	
	dfail_if_(tableEntry[pos] != tableDelim, "Bad Table entry ==> ", tableEntry.c_str());

	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	we should be looking at our base field name or number so grab it.

	CField::FieldName temp(&tableEntry[prev_pos]);
	int fldNumber = 0;
	if (useNames)
		fldNumber = mTheRecord->GetFieldNumber(temp);
	else
		fldNumber = atoi(temp.c_str());

	mBaseFieldNumber	=	fldNumber;
}

//----------------------------------------------------------------------------------------

CSkip2DelimField::~CSkip2DelimField(void)

{
	//	nothing special to do since vectors manage their own storage
}

//----------------------------------------------------------------------------------------

void	CSkip2DelimField::UseData(const char* record, size_t data_len)

{
	//	we grab our base field and skip till the delimiter.

	mFieldData.clear();

	const CField& baseFld = (*mTheRecord)[mBaseFieldNumber];
	if (! baseFld.Empty())
	{
		mFieldData.append(baseFld.DataBegin(), baseFld.Size());
		if (mDelim != '\0')
		{
			std::string::size_type pos	=	mFieldData.find(mDelim);
			if (pos	!=	std::string::npos)
				mFieldData.erase(0, pos + 1);
		}
	}

	//	get rid of leading blanks
	
	while (! mFieldData.empty()	&&	mFieldData[0] == ' ')
		mFieldData.erase(0, 1);

	//	trailing blanks should be gone from the base field already
	
	//	need to update these based upon our current contents
	
	if (! baseFld.Empty())
	{	
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	else
	{
		mFieldStart	=	0;
		mFieldEnd	=	0;
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	0;
	}
}

//----------------------------------------------------------------------------------------

CSkip2DelimField&	CSkip2DelimField::operator=(const CSkip2DelimField& rhs)

{
	if (this != &rhs)
	{
		CField::operator=(rhs);
		
		mFieldData			=	rhs.mFieldData;
		mTheRecord			=	rhs.mTheRecord;
		mBaseFieldNumber	=	rhs.mBaseFieldNumber;
		mDelim				=	rhs.mDelim;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CEitherOrField
*****************************************************************************************/

CEitherOrField::CEitherOrField(void)
	:	CField(), mTheRecord(NULL)

{
}

//----------------------------------------------------------------------------------------

CEitherOrField::CEitherOrField(const CEitherOrField& rhs)
	:	CField(rhs), mFieldData(rhs.mFieldData), mFieldNumbers(rhs.mFieldNumbers),
		mTheRecord(rhs.mTheRecord)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CEitherOrField*	CEitherOrField::Clone(void) const

{
	return new CEitherOrField(*this);
}

//----------------------------------------------------------------------------------------

CEitherOrField::CEitherOrField(CRecord* theRecord, std::string& tableEntry)
	:	mTheRecord(theRecord)

{
	//	we can have either Names or numbers

	//	first, let's skip over our modifier tag

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	//	start by skipping our field type
	//	we know it's 'EITHER' so we work with that

	char tableDelim = tableEntry[6];
	
	pos = tableEntry.find(tableDelim, prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	next, look for our name

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);
	mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);		//	may be empty

	//	next, see how we are to refer to component fields
	//	choice are 'Name' or 'Number'

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);

	bool useNames = false;
	if (strnicmp(&tableEntry[prev_pos], "Name", pos - prev_pos)  == 0)
		useNames = true;
	else if (strnicmp(&tableEntry[prev_pos], "Number", pos - prev_pos)  == 0)
		useNames = false;
	else
		dfail_msg_("Unknown EITHER component type ==> ", tableEntry.c_str());
	
	//prev_pos = ++pos;
//	pos = tableEntry.find_first_not_of(' ', prev_pos);
//	prev_pos = pos;

	//	should be looking at table delim char
	
	dfail_if_(tableEntry[pos] != tableDelim, "Bad Table entry ==> ", tableEntry.c_str());

	prev_pos = ++pos;
	pos = tableEntry.find_first_not_of(' ', prev_pos);
	prev_pos = pos;

	//	we should be looking at a comma-delimited list our component fields
	//	so we count how many we have then loop over the table

	int componentCount = std::count(tableEntry.begin() + prev_pos, tableEntry.end(), ',');
		
	for (int i = 0; i < componentCount; ++i)
	{
		pos = tableEntry.find(',', prev_pos);
		CField::FieldName temp(&tableEntry[prev_pos], pos - prev_pos);
		
		int fldNumber = 0;
		if (useNames)
			fldNumber = mTheRecord->GetFieldNumber(temp);
		else
			fldNumber = atoi(temp.c_str());

		mFieldNumbers.push_back(fldNumber);

		prev_pos = ++pos;
		pos = tableEntry.find_first_not_of(' ', prev_pos);
		prev_pos = pos;
	}

	//	now, grab our last field

	CField::FieldName temp(&tableEntry[prev_pos]);
	int fldNumber = 0;
	if (useNames)
		fldNumber = mTheRecord->GetFieldNumber(temp);
	else
		fldNumber = atoi(temp.c_str());

	mFieldNumbers.push_back(fldNumber);
}

//----------------------------------------------------------------------------------------

CEitherOrField::~CEitherOrField(void)

{
	//	nothing special to do since vectors manage their own storage
}

//----------------------------------------------------------------------------------------

void	CEitherOrField::UseData(const char* record, size_t data_len)

{
	//	we need to loop over our components and find the first one which is not empty

	mFieldData.clear();

	for (std::vector<int>::iterator i = mFieldNumbers.begin();
		i != mFieldNumbers.end(); ++i)
	{
		const CField& baseFld = (*mTheRecord)[*i];

		if (! baseFld.Empty())
		{
			mFieldData.append(baseFld.DataBegin(), baseFld.Size());
			break;
		}
	}

	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();

}

//----------------------------------------------------------------------------------------

CEitherOrField&	CEitherOrField::operator=(const CEitherOrField& rhs)

{
	if (this != &rhs)
	{
		CField::operator=(rhs);
		
		mFieldData		=	rhs.mFieldData;
		mFieldNumbers	=	rhs.mFieldNumbers;
		mTheRecord		=	rhs.mTheRecord;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CFixedSignedField
*****************************************************************************************/

CFixedSignedField::CFixedSignedField(void)
	:	CFixedField(), mSaveBeginPos(0), mSaveFieldLen(0)

{
}

//----------------------------------------------------------------------------------------

CFixedSignedField::CFixedSignedField(const CFixedSignedField& rhs)
	:	CFixedField(rhs), mFieldData(rhs.mFieldData), mSaveBeginPos(rhs.mSaveBeginPos),
		mSaveFieldLen(rhs.mSaveFieldLen)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CFixedSignedField*	CFixedSignedField::Clone(void) const

{
	return new CFixedSignedField(*this);
}

//----------------------------------------------------------------------------------------

CFixedSignedField::CFixedSignedField(std::string& tableEntry)
	:	CFixedField(tableEntry)

{	
	mSaveBeginPos	=	mBeginPos;
	mSaveFieldLen	=	mFieldLen;
}

//----------------------------------------------------------------------------------------

CFixedSignedField::~CFixedSignedField(void)

{
	//	nothing special to do since flex strings manage their own storage
}

//----------------------------------------------------------------------------------------

void	CFixedSignedField::UseData(const char* record, size_t data_len)

{
	//	let's let the base class do its thing

	mBeginPos	=	mSaveBeginPos;
	mFieldLen	=	mSaveFieldLen;
	
	CFixedField::UseData(record, data_len);
	
	//	now we need to make a local copy of our data then update our pointers etc.

	mFieldData.clear();

	mFieldData.assign(mFieldStart, mDataLen);

	//	now, let's translate our sign digit

	char newChar 	=	mFieldData[mDataLen - 1];
	bool isNegative	=	false;
	
	switch(newChar)
	{
		case	'A':
			newChar = '1';
			break;
			
		case	'B':
			newChar = '2';
			break;
			
		case	'C':
			newChar = '3';
			break;
			
		case	'D':
			newChar = '4';
			break;
			
		case	'E':
			newChar = '5';
			break;
			
		case	'F':
			newChar = '6';
			break;
			
		case	'G':
			newChar = '7';
			break;
			
		case	'H':
			newChar = '8';
			break;
			
		case	'I':
			newChar = '9';
			break;

		case	'J':
			newChar		=	'1';
			isNegative	=	true;
			break;
			
		case	'K':
			newChar		=	'2';
			isNegative	=	true;
			break;
			
		case	'L':
			newChar		=	'3';
			isNegative	=	true;
			break;
			
		case	'M':
			newChar		=	'4';
			isNegative	=	true;
			break;
			
		case	'N':
			newChar		=	'5';
			isNegative	=	true;
			break;
			
		case	'O':
			newChar		=	'6';
			isNegative	=	true;
			break;
			
		case	'P':
			newChar		=	'7';
			isNegative	=	true;
			break;
			
		case	'Q':
			newChar		=	'8';
			isNegative	=	true;
			break;
			
		case	'R':
			newChar		=	'9';
			isNegative	=	true;
			break;

		case	'{':
			newChar = '0';
			break;
			
		case	'}':
			newChar = '0';
			isNegative	=	true;
			break;
			
	}

	mFieldData[mDataLen - 1] = newChar;

	if (isNegative)
		mFieldData.insert(static_cast<size_t>(0), static_cast<size_t>(1), static_cast<char>('-'));
	
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();

}

//----------------------------------------------------------------------------------------

CFixedSignedField&	CFixedSignedField::operator=(const CFixedSignedField& rhs)

{
	if (this != &rhs)
	{
		mBeginPos	=	mSaveBeginPos;
		mFieldLen	=	mSaveFieldLen;
		CFixedField::operator=(rhs);
		
		mFieldData		=	rhs.mFieldData;
		mSaveBeginPos	=	rhs.mSaveBeginPos;
		mSaveFieldLen	=	rhs.mSaveFieldLen;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CTaggedField
*****************************************************************************************/

CTaggedField::CTaggedField(void)
	:	CField()

{
}

//----------------------------------------------------------------------------------------

CTaggedField::CTaggedField(const CTaggedField& rhs)
	:	CField(rhs)

{
}

//----------------------------------------------------------------------------------------

CTaggedField::CTaggedField(std::string& tableEntry)
	:	CField(tableEntry)

{
	//	we just have field names and possibly a modifier

	std::string::size_type pos, prev_pos;
	pos	=	prev_pos	=	0;

	//	look for our modifier

	pos = tableEntry.find_first_not_of(' ', 0);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	prev_pos	=	pos;
	pos			=	tableEntry.find(CFixedRecord::FixedTableDelim(), prev_pos);

	if (pos	!=	std::string::npos)
	{
		//	OK, we have a modifier

		CField::ModiferName mName;
		mName.assign(&tableEntry[prev_pos], pos - prev_pos);

		static ModifierTable* tableEnd = &gAvailableModifiers[0] + sizeof(gAvailableModifiers)
			/ sizeof(gAvailableModifiers[0]);

		ModifierTable* theEntry = std::find_if(gAvailableModifiers, tableEnd, compare_modifiers(mName));
		if (theEntry != tableEnd)
		{
			//	we got a hit

			mModifiers	|=	theEntry->mModifierID;
			prev_pos	=	++pos;
		}
		else
		{
			dfail_msg_("Unknown 'Modifier' ==> ", mName.c_str());
		}
	}
	else
	{
		//	no modifiers

		pos	=	0;
	}

	//	start by looking for field name. It's all that's left on the line

	mFieldName	=	&tableEntry[pos];

}

//----------------------------------------------------------------------------------------

CTaggedField::~CTaggedField(void)

{
}

//----------------------------------------------------------------------------------------

CTaggedField*	CTaggedField::Clone(void) const

{
	return new CTaggedField(*this);
}

//----------------------------------------------------------------------------------------

void	CTaggedField::Clear(void)

{
	//	need to reset our pointers.

	//mFieldName.clear();				//	can't do this -- we need our names
	mFieldStart	=	0;
	mFieldEnd	=	0;
	mDataEnd	=	0;
	mBeginPos	=	0;
	mFieldLen	=	0;
	mDataLen	=	0;

}

//----------------------------------------------------------------------------------------

void	CTaggedField::UseData(const char* record, size_t data_len)

{	
	//	The calling routine has already found and verified our start/end tags.
	//	we have pointers to just our data value.
	
	mFieldStart	=	record;
	mFieldEnd	=	mFieldStart	+	data_len;

	//	need to see if we have any data in the field.
	
	//	start by skipping for leading blanks, but only if modifier not set
	
	if ((mModifiers & CField::e_LeadingBlanks) == 0)
	{
		for (; *mFieldStart == ' ' && mFieldStart < mFieldEnd; ++mFieldStart)
		{
			//	just loop
		}
	}
	
	//	now look for trailing blanks
	
	const char* i = mFieldEnd - 1;
	for (; *i == ' ' && i >= mFieldStart; --i)
	{
		//	just loop
	}
	
	//	ok remember what we saw
	
	if (i != mFieldStart)
	{
		mDataEnd	=	i + 1;
		mDataLen	=	mDataEnd - mFieldStart;
	}
	else
	{
		if (*i != ' ')
		{
			mDataEnd	=	i + 1;
			mDataLen	=	1;
		}
		else
		{
			mDataEnd	=	mFieldStart;
			mDataLen	=	0;
		}
	}

}

//----------------------------------------------------------------------------------------

bool	CTaggedField::operator==(const char* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::operator==(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_equal_to(const CField* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_equal_to(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_equal_to(const char* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_equal_to(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_less_than(const char* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_less_than(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_equal_to(const CField* rhs, size_t n) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_equal_to(rhs, n);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_equal_to(const char* rhs, size_t n) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_equal_to(rhs, n);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_less_than(const char* rhs, size_t n) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_less_than(rhs, n);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::ci_Equivalent(const char* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::ci_Equivalent(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::ci_Equivalent(const CField* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::ci_Equivalent(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::ci_Less(const char* rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::ci_Less(rhs);
}

//----------------------------------------------------------------------------------------

bool	CTaggedField::is_less_than(const CField& rhs) const

{
	if (mFieldStart	==	0)
		return false;
	else
		return CField::is_less_than(rhs);
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CDecimalField
*****************************************************************************************/
/*
CDecimalField::CDecimalField(void)
	:	CField(), mDecimalPlaces(0)

{
}

//----------------------------------------------------------------------------------------

CDecimalField::CDecimalField(const CDecimalField& rhs)
	:	CField(rhs), mFieldData(rhs.mFieldData), mDecimalPlaces(rhs.mDecimalPlaces)

{
	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();
}

//----------------------------------------------------------------------------------------

CDecimalField*	CDecimalField::Clone(void) const

{
	return new CDecimalField(*this);
}

//----------------------------------------------------------------------------------------

CDecimalField::CDecimalField(CRecord* theRecord, std::string& tableEntry)
	:	mTheRecord(theRecord)

{
	//	our entry is of the form:
	//	Decimal<delim><Field Name><delim>m.n where 'm' is the total field length and 'n' is the
	//	number of digits to the right of the (implied) decimal point.

	//	first, let's skip over our modifier tag

	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	//	start by skipping our field type
	//	we know it's 'Decimal' so we work with that

	char tableDelim = tableEntry[7];
	
	pos = tableEntry.find(tableDelim, prev_pos);
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	next, look for our name

	prev_pos = ++pos;
	pos = tableEntry.find(tableDelim, prev_pos);
	mFieldName.assign(&tableEntry[prev_pos], pos - prev_pos);		//	may be empty

	//	next, get field dimensions

	prev_pos = ++pos;

	pos = tableEntry.find('.', prev_pos);	
	dfail_if_(pos == std::string::npos, "Bad Table entry ==> ", tableEntry.c_str());

	//	i (currently) don't care about total field length so just grab the number of decimals

	mDecimalPlaces = std::atoi(tableEntry.c_str() + ++pos);

}

//----------------------------------------------------------------------------------------

CDecimalField::~CDecimalField(void)

{
	//	nothing special to do since flex strings manage their own storage
}

//----------------------------------------------------------------------------------------

void	CDecimalField::UseData(const char* record, size_t data_len)

{
	//	we need to make a local copy of our data then update our pointers etc.

	mFieldData.clear();

	for (std::vector<int>::iterator i = mFieldNumbers.begin();
		i != mFieldNumbers.end(); ++i)
	{
		const CField& baseFld = (*mTheRecord)[*i];
		//std::copy(baseFld.DataBegin(), baseFld.DataEnd(), std::back_inserter(mFieldData));
		mFieldData.append(baseFld.DataBegin(), baseFld.DataEnd());
	}

	//	need to update these based upon our current contents
	
	mFieldStart	=	&(*mFieldData.begin());
	mFieldEnd	=	mFieldStart + mFieldData.size();
	mDataEnd	=	mFieldEnd;
	mBeginPos	=	0;
	mFieldLen	=	mFieldData.capacity();
	mDataLen	=	mFieldData.size();

}

//----------------------------------------------------------------------------------------

CDecimalField&	CDecimalField::operator=(const CDecimalField& rhs)

{
	if (this != &rhs)
	{
		CField::operator=(rhs);
		
		mFieldData		=	rhs.mFieldData;
		mFieldNumbers	=	rhs.mFieldNumbers;
		mTheRecord		=	rhs.mTheRecord;

		//	need to update these based upon our current contents
		
		mFieldStart	=	&(*mFieldData.begin());
		mFieldEnd	=	mFieldStart + mFieldData.size();
		mDataEnd	=	mFieldEnd;
		mBeginPos	=	0;
		mFieldLen	=	mFieldData.capacity();
		mDataLen	=	mFieldData.size();
	}
	return *this;
}

//----------------------------------------------------------------------------------------

*/
/*****************************************************************************************
	CRecord
*****************************************************************************************/

CRecord::CRecord(void)
	:	mFields(NULL), mFieldNames(NULL),
		mData(NULL), mRecordLength(0),
		mMaxRecordLen(0), mMinRecordLen(0),
		mRecord_Type(e_UnknownRecord)
	
{
}

//----------------------------------------------------------------------------------------

CRecord::CRecord(const CRecord& rhs)
	:	mErrorData(rhs.mErrorData),
		mFields(NULL), mFieldNames(NULL), mData(rhs.mData),
		mRecordLength(rhs.mRecordLength), mMaxRecordLen(rhs.mMaxRecordLen),
		mMinRecordLen(rhs.mMinRecordLen), mRecord_Type(rhs.mRecord_Type)
	
{
	//	fields are dynamically allocated so we need to clone them too
	
	//	Fields and FieldNames may be NULL if we are cloning a Union type record.
	
	if (rhs.mFields)
	{
		mFields	=	new FieldList;
		
		for (FieldList::const_iterator i = rhs.mFields->begin();
			i != rhs.mFields->end(); ++i)
		{
			CField* newField = (*i)->Clone();
			mFields->push_back(newField);
		}
	}
	//	now to copy field names

	if (rhs.mFieldNames)
	{
		mFieldNames		=	new FieldNames;
		*mFieldNames	=	*(rhs.mFieldNames);
	}
}

//----------------------------------------------------------------------------------------

CRecord::~CRecord(void)

{
	//	we do NOT own mData, but we do own our fields.
	//	first, make sure we have some.  some records don't have their own, they use other's fields.
	
	if (mFields)
	{
		for (FieldList::iterator i = mFields->begin();
			i != mFields->end(); ++i)
		{
			delete (*i);
		}
	}
	delete mFields;
	delete mFieldNames;
}

//----------------------------------------------------------------------------------------

void	CRecord::PrintFieldNames(std::ostream& output) const

{
	for (FieldNames::const_iterator i = mFieldNames->begin();
		i != mFieldNames->end(); ++i)
	{
		output << i->first << std::endl;
	}
}

//----------------------------------------------------------------------------------------

CRecord::FieldList	CRecord::GetFieldNamesLike(const CField::FieldName& name) const

{
	FieldList result;

	for (FieldList::const_iterator i = mFields->begin();
			i != mFields->end(); ++i)
	{
		if (strncmp(name.c_str(), (*i)->GetFieldName().c_str(), name.size())	==	0)
			result.push_back(*i);
	}

	return result;
}

//----------------------------------------------------------------------------------------

CRecord*	CRecord::MakeRecord(std::istream& input)

{
	//	We read enough of the file to determine which kind of record to
	//	make, then we create the record and let it finish constructing itself
	
	CRecord* theRecord = NULL;
	std::string buffer;
	
	while(input.good())
	{
		//	Need to see if at eof
		//	There should be no extra characters after last return
		//	so we force eof if necessary.
		//	need to do this so iterator loop works properly.
	
		std::istream::int_type nextChar = input.peek();
		if (nextChar == std::istream::traits_type::eof())
		{
			nextChar = input.get();		//	force eof
			continue;
		}
		
		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad Record Description");
		nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();
		
		if (buffer.empty())
			continue;						//	skip empty lines
		if (buffer[0] == '/' && buffer[1] == '/')
			continue;						//	skip comments
		else
			break;
	}
	
		//	OK, we should be pointing at the first line of the actual
		//	description
		
	std::string::size_type pos = 0;
	
	pos = buffer.find_first_not_of(' ', pos);
	
	char recordType = std::toupper(buffer[pos]);
	
	if (recordType == 'F')
	{
		//	now see if it is just Fixed or FixedTagged

		if (std::toupper(buffer[pos + 5])	==	'T')		//	the 'T' is the sixth character in FixedTagged
			theRecord	=	new CFixedTaggedRecord(input);
		else
            theRecord = new CFixedRecord(input);
	}
	else if (recordType == 'V')
		theRecord = new CVariableRecord(input);
	else if (recordType == 'Q')
		theRecord = new CQuotedRecord(input);
	else if (recordType == 'M')
		theRecord = new CMultiLineVariableRecord(input);
	else if (recordType == 'U')
		theRecord = new CUnionRecord(input);
	else
		dfail_msg_ ("Invalid Record type ==> ", buffer.c_str());
		
	return theRecord;
}

//----------------------------------------------------------------------------------------

CRecord&	CRecord::operator=(const CRecord& rhs)

{
	if (this != &rhs)
	{
		//	fields are dynamically allocated so we need to clone them too
		//	first, delete our existing fields

		for (FieldList::iterator i = mFields->begin();
			i != mFields->end(); ++i)
		{
			delete (*i);
		}
		
		for (FieldList::const_iterator i = rhs.mFields->begin();
			i != rhs.mFields->end(); ++i)
		{
			CField* newField = (*i)->Clone();
			mFields->push_back(newField);
		}
		mErrorData		=	rhs.mErrorData;
		*mFieldNames	=	*(rhs.mFieldNames);
		mData			=	rhs.mData;
		mRecordLength	=	rhs.mRecordLength;
		mMaxRecordLen	=	rhs.mMaxRecordLen;
		mMinRecordLen	=	rhs.mMinRecordLen;
		mRecord_Type	=	rhs.mRecord_Type;
	}
	return *this;
}

//----------------------------------------------------------------------------------------


const CField&	CRecord::operator[](int i) const

{
	//	we'll use the range checked version
	
	return *(mFields->at(i));
}

//----------------------------------------------------------------------------------------

const CField&	CRecord::operator[](const CField::FieldName& name) const

{
	//	first, make sure we can find the name
	
	FieldNames::const_iterator i = mFieldNames->find(name);
	dfail_if_(i == mFieldNames->end(), name.c_str(), " not found in record description");
	
	return this->operator[]((*i).second);
}

//----------------------------------------------------------------------------------------

const CField&	CRecord::operator[](const char* name) const

{
	CField::FieldName temp(name);
	
	//	first, make sure we can find the name
	
	FieldNames::const_iterator i = mFieldNames->find(temp);
	dfail_if_(i == mFieldNames->end(), temp.c_str(), " not found in record description");
	
	return this->operator[]((*i).second);
}

//----------------------------------------------------------------------------------------

int	CRecord::GetFieldNumber(const CField::FieldName& name) const

{
	//	first, make sure we can find the name
	
	FieldNames::const_iterator i = mFieldNames->find(name);
	dfail_if_(i == mFieldNames->end(), name.c_str(), " not found in record description");
	
	return i->second;
}

//----------------------------------------------------------------------------------------

CRecord::CRecord(std::istream& input)
	:	mRecord_Type(e_UnknownRecord)

{
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CFixedRecord
*****************************************************************************************/

CFixedRecord::CFixedRecord(void)
	:	CRecord()

{
	mFixedRecordMode	=	e_Start_Len;
	mNextFieldStart		=	0;
	mFixedTableDelim	=	',';
	mRecord_Type		=	e_FixedRecord;

	mFields		=	new FieldList;
	mFieldNames	=	new FieldNames;
	
}

//----------------------------------------------------------------------------------------

CFixedRecord::CFixedRecord(const CFixedRecord& rhs)
	:	CRecord(rhs)

{
}

//----------------------------------------------------------------------------------------

CFixedRecord::CFixedRecord(std::istream& input)
	:	CRecord(input)

{
	mRecord_Type	=	e_FixedRecord;
	mFields			=	new FieldList;
	mFieldNames		=	new FieldNames;

	//	We should be looking at the line for field names indicator
	
	std::string buffer;
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	std::istream::int_type nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	pos = buffer.find_first_not_of(' ', pos);
	
	char names = std::toupper(buffer[pos]);
	dfail_if_(names != 'Y' && names != 'N',
		"Bad Name indicator ==> ", buffer.c_str());
	
	//	next line is unused
	//	8/11/99	added some more options to this line.  see header notes

	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	if (buffer.empty())
	{
		//	set our defaults to match old usage
		
		mFixedRecordMode = e_Start_Len;
		mNextFieldStart = 0;
		mFixedTableDelim = ',';
	}
	else
	{
		//	let's see what the user wants.
		
		pos = 0;
		prev_pos = buffer.find_first_not_of(' ', pos);
		pos = buffer.find(',', prev_pos);
		if (buffer.compare(prev_pos, pos - prev_pos, "Start_End") == 0)
			mFixedRecordMode = e_Start_End;
		else if (buffer.compare(prev_pos, pos - prev_pos, "Start_Len") == 0)
			mFixedRecordMode = e_Start_Len;
		else if (buffer.compare(prev_pos, pos - prev_pos, "Len") == 0)
			mFixedRecordMode = e_Len;
		else
			dfail_msg_("Invalid Control Info ==> ", buffer.c_str());
		
		//	let's see what the delimiter is.
		
		prev_pos = ++pos;						//	bump past comma
		prev_pos = buffer.find_first_not_of(' ', pos);
		if (std::toupper(buffer[prev_pos]) == 'T')
			mFixedTableDelim = '\t';
		else
			mFixedTableDelim = buffer[prev_pos];
			
		mNextFieldStart = 0;
	}
	//	next is record length
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Fixed Record Description");
	mRecordLength = std::atoi(buffer.c_str());
	dfail_if_(mRecordLength == 0, "Missing Fixed Record record length");
	mMaxRecordLen = mRecordLength;
	mMinRecordLen = mRecordLength;
	
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	//	now, we scan and pick up field info

	while(input.good())
	{
		//	Need to see if at eof
		//	There should be no extra characters after last return
		//	so we force eof if necessary.
		//	need to do this so iterator loop works properly.
	
		std::istream::int_type nextChar = input.peek();
		if (nextChar == std::istream::traits_type::eof())
		{
			nextChar = input.get();		//	force eof
			continue;
		}
		
		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad Fixed Record Field Name list");
		nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();
	
		if (buffer.empty())
			continue;						//	skip empty lines
		
		if (buffer[0] == '/' && buffer[1] == '/')
			continue;						//	skip comments

		if (buffer.compare(0, 4, ":END") == 0)
			break;
			
		//	first, check for resetting position pointer
		//	for redefines
		//	ONLY for Length only mode
		
		if (mFixedRecordMode == e_Len && buffer.compare(0, 6, "ORG * ") == 0)
		{
			//	we need to reset our current location to
			//	the start of the specified field
			//	can be either a field name or field number
			
			size_t fldStart = 0;
			if (names == 'Y')
			{
				CField::FieldName fldName(buffer.c_str() + 6);
				fldStart = this->operator[](fldName).GetOffset();
			}
			else
			{
				int whichFld = atoi(buffer.c_str() + 6);
				fldStart = this->operator[](whichFld).GetOffset();
			}
			
			//	now, just reset our pointer and we're on our way
			
			mNextFieldStart = fldStart;
			continue;
		}
		
		//	Let the field figure itself out
		//	we can't do that any more.  we may have multiple field types per record

		CField* newField = NULL;
		
		if (buffer.compare(0, 5, "SYNTH") == 0)
			newField = new CSYNTHField(this, buffer);
		else if (buffer.compare(0, 5, "Combo") == 0)
			newField = new CComboField(this, buffer);
		else if (buffer.compare(0, 6, "EITHER") == 0)
			newField = new CEitherOrField(this, buffer);
		else if (buffer.compare(0, 10, "Skip2Delim") == 0)
			newField = new CSkip2DelimField(this, buffer);
		else if (buffer.compare(0, 6, "Signed") == 0)
			newField = new CFixedSignedField(buffer);
		//else if (buffer.compare(0, 7, "Decimal") == 0)
		//	newField = new CDecimalField(this, buffer);
		else
			newField = new CFixedField(buffer);
			
		mFields->push_back(newField);
		
		//	need to build our field name list too.
		//	make sure there are no duplicate field names
		if (names == 'Y')
		{
			std::pair<FieldNames::iterator, bool> result = mFieldNames->insert(
				MapType(newField->GetFieldName(), mFields->size() - 1));
			dfail_if_(result.second == false, "Duplicate Field Name ==> ",
					newField->GetFieldName().c_str());
		}
	}
}

//----------------------------------------------------------------------------------------

CFixedRecord::~CFixedRecord(void)

{
}

//----------------------------------------------------------------------------------------

CFixedRecord&	CFixedRecord::operator=(const CFixedRecord& rhs)

{
	if (this != &rhs)
	{
		CRecord::operator=(rhs);
	}
	return *this;
}

//----------------------------------------------------------------------------------------

void	CFixedRecord::UseData(const char* record, size_t data_len)

{
	if (data_len > mMaxRecordLen)
		throw Long_Record("Fixed Record data too long.");

	mData 			=	record;
	mRecordLength	=	data_len;
	
	//	each field knows where it starts.
	
	for (FieldList::iterator i = mFields->begin();
		i != mFields->end(); ++i)
	{
		(*i)->UseData(record, data_len);
	}
}

//----------------------------------------------------------------------------------------

CRecord*	CFixedRecord::Clone(void) const

{
	return new CFixedRecord(*this);
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CVariableRecord
*****************************************************************************************/

CVariableRecord::CVariableRecord(void)
	:	CRecord(), mFieldCount(0), mDelim('\t')

{
	mRecord_Type	=	e_VariableRecord;
	mFields			=	new FieldList;
	mFieldNames		=	new FieldNames;
}

//----------------------------------------------------------------------------------------

CVariableRecord::CVariableRecord(const CVariableRecord& rhs)
	:	CRecord(rhs), mFieldCount(rhs.mFieldCount), mDelim(rhs.mDelim)

{
}

//----------------------------------------------------------------------------------------

CVariableRecord::CVariableRecord(std::istream& input)
	:	CRecord(input)

{
	mRecord_Type	=	e_VariableRecord;
	mFields			=	new FieldList;
	mFieldNames		=	new FieldNames;

	//	We should be looking at the line for field names indicator
	
	std::string buffer;
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	std::istream::int_type nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	std::string::size_type pos = 0;
	
	pos = buffer.find_first_not_of(' ', pos);
	
	char names = std::toupper(buffer[pos]);
	dfail_if_(names != 'Y' && names != 'N',
		"Bad Name indicator ==> ", buffer.c_str());
	
	//	next is the delimiter character
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	pos = buffer.find_first_not_of(' ', pos);
	dfail_if_(pos == std::string::npos, "Missing variable field delimiter");

	ci_string temp(buffer.c_str(), pos, std::string::npos - pos);
	if (temp == "tab")
		mDelim = '\t';
	else if (temp == "comma")
		mDelim = ',';
	else if (temp == "return")
		mDelim = '\r';
	else if (temp == "linefeed")
		mDelim = '\n';
	else
		mDelim = buffer[0];
		
	//	next if field count
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	mFieldCount = std::atol(buffer.c_str());
	dfail_if_(mFieldCount == 0, "Missing variable field field count");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	//	now, if fields are unnamed, we're done
	//	otherwise, need to scan and pick up field names
	
	if (names == 'Y')
	{
		while(input.good())
		{
			//	Need to see if at eof
			//	There should be no extra characters after last return
			//	so we force eof if necessary.
			//	need to do this so iterator loop works properly.
		
			std::istream::int_type nextChar = input.peek();
			if (nextChar == std::istream::traits_type::eof())
			{
				nextChar = input.get();		//	force eof
				continue;
			}
			
			std::getline(input, buffer, LINE_END_CHAR);
			dfail_if_(! input.good(), "Bad Variable Record Field Name list");
			nextChar = input.peek();
			if (nextChar == '\n') nextChar = input.get();

			if (buffer.empty())
				continue;						//	skip empty lines

			if (buffer[0] == '/' && buffer[1] == '/')
				continue;						//	skip comments
			
			if (buffer.compare(0, 4, ":END") == 0)
				break;
			
			//	Let the field figure itself out
			//	we can't do that any more.  we may have multiple field types per record

			CField* newField = NULL;
	
			if (buffer.compare(0, 5, "SYNTH") == 0)
				newField = new CSYNTHField(this, buffer);
			else if (buffer.compare(0, 5, "Combo") == 0)
				newField = new CComboField(this, buffer);
			else if (buffer.compare(0, 6, "EITHER") == 0)
				newField = new CEitherOrField(this, buffer);
			else if (buffer.compare(0, 10, "Skip2Delim") == 0)
				newField = new CSkip2DelimField(this, buffer);
			else
			{
				std::string::size_type pos = buffer.find(',');
				short delimCount = 1;
			
				if (pos != std::string::npos)
				{
					delimCount = atoi(buffer.c_str() + pos + 1);

					//	need to remove trailing delimiter count

					buffer.resize(pos);
				}
				
				newField = new CVariableField(buffer, mDelim, delimCount);
			}
			
			mFields->push_back(newField);
			
			//	need to build our field name list too.
			//	make sure there are no duplicate field names

			std::pair<FieldNames::iterator, bool> result = mFieldNames->insert(
				MapType(newField->GetFieldName(), mFields->size() - 1));
			dfail_if_(result.second == false, "Duplicate Field Name ==> ",
					newField->GetFieldName().c_str());
		}
	}
	else
	{
		//	just allocate our Field objects.
		
		for(size_t i = 0; i < mFieldCount; ++i)
		{
			mFields->push_back(new CVariableField(mDelim));
		}
	}

	//	default to max variable length

	mRecordLength = e_MaxVarRecLen;
	mMaxRecordLen = e_MaxVarRecLen;

	mMinRecordLen = mFields->size();				//	all fields are empty
	
}

//----------------------------------------------------------------------------------------

CVariableRecord::~CVariableRecord(void)

{
}

//----------------------------------------------------------------------------------------

CVariableRecord&	CVariableRecord::operator=(const CVariableRecord& rhs)

{
	if (this != &rhs)
	{
		CRecord::operator=(rhs);
		mFieldCount	=	rhs.mFieldCount;
		mDelim		=	rhs.mDelim;
	}
	return *this;
}

//----------------------------------------------------------------------------------------

void	CVariableRecord::UseData(const char* record, size_t data_len)

{
	//if (data_len > mMaxRecordLen)
	//	throw Long_Record("Variable Record data too long.");

	mData 			=	record;
	mRecordLength	=	data_len;
	
	//	let each field find and report its endpoint
	
	//	NOTE:  redefines  NOT supported yet.
	
	int length = data_len;
	const char* data_start = record;
	
	for (FieldList::iterator i = mFields->begin();
		i != mFields->end(); ++i)
	{
		(*i)->UseData(data_start, length);
		if ((*i)->RealField())
		{
			data_start += (*i)->Capacity() + 1;
			length -= (*i)->Capacity() + 1;
			//if (data_len > std::numeric_limits<int>::max())
			if (length < -1)					//	allow unterminated data
			{
				cp_string<100> temp("Variable record too short ==> ");
				temp += cp_string<32>(record, 32);
				throw Short_Record(temp.c_str());
			}

            //  let's remember this...

            dynamic_cast<CVariableField*>((*i))->SetOffset(reinterpret_cast<const char*>((*i)->GetOffset()) - mData);
		}
	}
	
	if (length > 0)
	{
		cp_string<100> temp("Variable record too long ==> ");
		temp += cp_string<32>(record, 32);
		throw Long_Record(temp.c_str());
	}
}	

//----------------------------------------------------------------------------------------

CRecord*	CVariableRecord::Clone(void) const

{
	return new CVariableRecord(*this);
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CQuotedField
*****************************************************************************************/
CQuotedField::CQuotedField(void)
	:	CVariableField()

{
}

//----------------------------------------------------------------------------------------

CQuotedField::CQuotedField(const CQuotedField& rhs)
	:	CVariableField(rhs), mQuoteChar(rhs.mQuoteChar)

{
}

//----------------------------------------------------------------------------------------

CQuotedField::CQuotedField(std::string& tableEntry, char quote, char delim, short count)
	:	CVariableField(tableEntry, delim, count), mQuoteChar(quote)

{
}

//----------------------------------------------------------------------------------------

CQuotedField::CQuotedField(char quote, char delim, short count)
	:	CVariableField(delim, count), mQuoteChar(quote)

{
}

//----------------------------------------------------------------------------------------

CQuotedField::~CQuotedField(void)

{
	//	nothing special to do
}

//----------------------------------------------------------------------------------------

CQuotedField*	CQuotedField::Clone(void) const

{
	return new CQuotedField(*this);
}

//----------------------------------------------------------------------------------------

void	CQuotedField::SetupDelims(char quote, char delim)

{
	mEndPair[0] = quote;
	mEndPair[1] = delim;
}

//----------------------------------------------------------------------------------------


void	CQuotedField::UseData(const char* record, size_t data_len)

{
	//	Each field MAY BE surrounded by the specified quote character.
	//	if not, then just look for delimiters
	
	//	"abc","cde",...
	//	There may or may not be chars between the ending and beginning quotes.
	
	//	There may be no data between enclosing quotes -- an empty field.
	
	//	fields are separated by a delimiter character which may occur between quotes too.
	
	//	This algorithm is based on code from Lippman's C++ Primer, 3rd Edition
	//	page 280 except that the strings are preallocated to avoid a lot of calls
	//	to string ctors.

	if (*record != mQuoteChar)
	{
		//	this field is not quoted so it's just like a variable field
		
		CVariableField::UseData(record, data_len);
	}
	else
	{
         mBeginPos  =   reinterpret_cast<size_t>(const_cast<char*>(record));        //  stays constant while we use this record

		//	expect to be looking at the beginning of the field
		
		mFieldStart = ++record;					//	skip the quote
		
		//mFieldEnd = std::find(record, record + data_len, mQuoteChar);
		mFieldEnd = std::search(record, record + data_len, mEndPair, mEndPair + sizeof(mEndPair));
		
		//	last field probably won't have a trailing delimiter so...
		
		if (mFieldEnd == record + data_len)
		{
			//	let's just look to see if the last char is a quote char
			
			mFieldEnd = record + data_len - 2;			//	-2 because we bumped record
			dthrow_if_(*mFieldEnd != mQuoteChar, "Mis-matched 'Quote' characters.");
		}
		
		mFieldLen = mFieldEnd - mFieldStart;
		
		//	compute data length
		//	need to see if we have any data in the field.
		
		const char* i = mFieldEnd - 1;
		for (; *i == ' ' && i > mFieldStart; --i)
		{
			//	just loop
		}
		
		//	ok remember what we saw

		mDataLen = mDataEnd - mFieldStart;
		
		if (i != mFieldStart)
		{
			mDataEnd = i + 1;
			mDataLen = mDataEnd - mFieldStart;
		}
		else
		{
			if (*i != ' ')
			{
				mDataEnd = i + 1;
				mDataLen = 1;
			}
			else
			{
				mDataEnd = mFieldStart;
				mDataLen = 0;
			}
		}
	}
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CQuotedRecord
*****************************************************************************************/

CQuotedRecord::CQuotedRecord(void)
	:	CVariableRecord()

{
	CQuotedField::SetupDelims();
	mRecord_Type	=	e_QuotedRecord;
}

//----------------------------------------------------------------------------------------

CQuotedRecord::CQuotedRecord(const CQuotedRecord& rhs)
	:	CVariableRecord(rhs), mQuoteChar(rhs.mQuoteChar)

{
}

//----------------------------------------------------------------------------------------

CQuotedRecord::CQuotedRecord(std::istream& input)
	:	CVariableRecord()

{
	//	we handle the stream based stuff ourselves
	
	//	We should be looking at the line for field names indicator
	
	mRecord_Type	=	e_QuotedRecord;
	
	std::string buffer;
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	std::istream::int_type nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	std::string::size_type pos = 0;
	
	pos = buffer.find_first_not_of(' ', pos);
	
	char names = std::toupper(buffer[pos]);
	dfail_if_(names != 'Y' && names != 'N',
		"Bad Name indicator ==> ", buffer.c_str());
	
	//	next is the delimiter character/quote char
	
	std::getline(input, buffer, ',');
	dfail_if_(! input.good(), "Bad Variable Record Description");
	pos = buffer.find_first_not_of(' ');
	dfail_if_(pos == std::string::npos, "Missing variable field delimiter");

	ci_string temp(buffer.c_str(), pos, std::string::npos - pos);
	if (temp == "tab")
		mDelim = '\t';
	else if (temp == "comma")
		mDelim = ',';
	else
		mDelim = buffer[0];
		
	//	next we get our quote character
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	pos = buffer.find_first_not_of(' ');
	dfail_if_(pos == std::string::npos, "Missing 'Quote' character");
	temp.assign(buffer.c_str(), pos, std::string::npos - pos);
	if (temp == "comma")
		mQuoteChar = ',';
	else
		mQuoteChar = buffer[0];
		
	//	next if field count
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Variable Record Description");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	mFieldCount = std::atoi(buffer.c_str());
	dfail_if_(mFieldCount == 0, "Missing variable field field count");
	
	CQuotedField::SetupDelims(mQuoteChar, mDelim);
	
	//	now, if fields are unnamed, we're done
	//	otherwise, need to scan and pick up field names
	
	if (names == 'Y')
	{
		while(input.good())
		{
			//	Need to see if at eof
			//	There should be no extra characters after last return
			//	so we force eof if necessary.
			//	need to do this so iterator loop works properly.
		
			std::istream::int_type nextChar = input.peek();
			if (nextChar == std::istream::traits_type::eof())
			{
				nextChar = input.get();		//	force eof
				continue;
			}
			
			std::getline(input, buffer, LINE_END_CHAR);
			dfail_if_(! input.good(), "Bad Variable Record Field Name list");
			nextChar = input.peek();
			if (nextChar == '\n') nextChar = input.get();

			if (buffer.empty())
				continue;						//	skip empty lines

			if (buffer[0] == '/' && buffer[1] == '/')
				continue;						//	skip comments
			
			if (buffer.compare(0, 4, ":END") == 0)
				break;
			
			//	Let the field figure itself out
			
			CField* newField = NULL;
			
			if (buffer.compare(0, 5, "SYNTH") == 0)
				newField = new CSYNTHField(this, buffer);
			else if (buffer.compare(0, 5, "Combo") == 0)
				newField = new CComboField(this, buffer);
			else if (buffer.compare(0, 6, "EITHER") == 0)
				newField = new CEitherOrField(this, buffer);
			else if (buffer.compare(0, 10, "Skip2Delim") == 0)
				newField = new CSkip2DelimField(this, buffer);
			else			
				newField = new CQuotedField(buffer, mQuoteChar, mDelim);

			mFields->push_back(newField);
			
			//	need to build our field name list too.
			//	make sure there are no duplicate field names
			
			std::pair<FieldNames::iterator, bool> result = mFieldNames->insert(
				MapType(newField->GetFieldName(), mFields->size() - 1));
			dfail_if_(result.second == false, "Duplicate Field Name ==> ",
					newField->GetFieldName().c_str());
		}
	}
	else
	{
		//	just allocate our Field objects.
		
		for(size_t i = 0; i < mFieldCount; ++i)
		{
			mFields->push_back(new CQuotedField(mQuoteChar, mDelim));
		}
	}
	//	default to max variable length

	mRecordLength = e_MaxVarRecLen;
	mMaxRecordLen = e_MaxVarRecLen;

	mMinRecordLen = mFields->size();				//	all fields are empty
	
}

//----------------------------------------------------------------------------------------

CQuotedRecord::~CQuotedRecord(void)

{
}

//----------------------------------------------------------------------------------------

void	CQuotedRecord::UseData(const char* record, size_t data_len)

{
	//	let each field find and report its endpoint
	
	//	NOTE:  redefines  NOT supported yet.
	
	mData 			=	record;
	mRecordLength	=	data_len;

	int length = data_len;
	const char* data_start = record;

	for (FieldList::iterator i = mFields->begin();
		i != mFields->end(); ++i)
	{
		bool haveQuote = *data_start == mQuoteChar;	
		(*i)->UseData(data_start, length);
		if ((*i)->RealField())
		{
			data_start += (*i)->Capacity();
			length -= (*i)->Capacity();
		
			//	should be pointing to either trailing quote char or delimiter if non-quoted field
			//	so we skip over them
		
			if (haveQuote)
			{
				data_start += 2;
				length	-= 2;
			}
			if (*data_start == mDelim)
			{
				++data_start;
				--length;
			}
		

            //  let's remember this...

            dynamic_cast<CQuotedField*>((*i))->SetOffset(reinterpret_cast<const char*>((*i)->GetOffset()) - mData);

			//if (data_len > std::numeric_limits<int>::max())
			if (length < -1)					//	allow unterminated data
			{
				cp_string<100> temp("Quoted record too short ==> ");
				temp += cp_string<32>(record, 32);
				throw Short_Record(temp.c_str());
			}
		}
	}
	
	if (length > 0)
	{
		cp_string<100> temp("Variable record too long ==> ");
		temp += cp_string<32>(record, 32);
		throw Long_Record(temp.c_str());
	}
}

//----------------------------------------------------------------------------------------

CQuotedRecord&	CQuotedRecord::operator=(const CQuotedRecord& rhs)

{
	if (this != &rhs)
	{
		CVariableRecord::operator=(rhs);
		mQuoteChar	=	rhs.mQuoteChar;
	}
	return *this;
}

//----------------------------------------------------------------------------------------

CRecord*	CQuotedRecord::Clone(void) const

{
	return new CQuotedRecord(*this);
}

//----------------------------------------------------------------------------------------



/*****************************************************************************************
	CMultiLineVariableRecord
*****************************************************************************************/

CMultiLineVariableRecord::CMultiLineVariableRecord(void)
	:	CVariableRecord()

{
	mRecord_Type	=	e_MultiLineVariableRecord;
}

//----------------------------------------------------------------------------------------

CMultiLineVariableRecord::CMultiLineVariableRecord(const CMultiLineVariableRecord& rhs)
	:	CVariableRecord(rhs)

{
}

//----------------------------------------------------------------------------------------

CMultiLineVariableRecord::CMultiLineVariableRecord(std::istream& input)
	:	CVariableRecord(input)

{
	mRecord_Type	=	e_MultiLineVariableRecord;
}

//----------------------------------------------------------------------------------------

void	CMultiLineVariableRecord::UseData(const char* record, size_t data_len)

{
	//if (data_len > mMaxRecordLen)
	//	throw Long_Record("Variable Record data too long.");

	mData 			=	record;
	mRecordLength	=	data_len;
	
	//	let each field find and report its endpoint
	
	//	NOTE:  redefines  NOT supported yet.
	
	int length = data_len;
	const char* data_start = record;
	
	for (FieldList::iterator i = mFields->begin();
		i != mFields->end(); ++i)
	{
		//	here's our multiline part...

		//if (*data_start == '\n' && length > 0)
		//{
		//	++data_start;
		//	--length;
		//}
		(*i)->UseData(data_start, length);
		if ((*i)->RealField())
		{
			data_start += (*i)->Capacity() + 1;
			length -= (*i)->Capacity() + 1;
			//if (data_len > std::numeric_limits<int>::max())
			if (length < -1)					//	allow unterminated data
			{
				cp_string<100> temp("Variable record too short ==> ");
				temp += cp_string<32>(record, 32);
				throw Short_Record(temp.c_str());
			}
		}
	}
	
	if (length > 0)
	{
		cp_string<100> temp("Variable record too long ==> ");
		temp += cp_string<32>(record, 32);
		throw Long_Record(temp.c_str());
	}
}

//----------------------------------------------------------------------------------------

/*****************************************************************************************
	CFixedTaggedRecord
*****************************************************************************************/

void	CFixedTaggedRecord::UseData(const char* record, size_t data_len)

{
	//	The first record type which uses the ability to store error information and
	//	move on.
	
	mErrorData.clear();

	//	the first part of our record is fixed so we can use that code

	if (data_len > mMaxRecordLen)
		throw Long_Record("Record data too long.");

	mData 			=	record;
	mRecordLength	=	data_len;
	
	//	each field knows where it starts.
	
	for (int i = 0;	i < mFirstTaggedField; ++i)
	{
		(*mFields)[i]->UseData(record, data_len);
	}

	//	We don't know which fields are present in the tagged part nor what order they might be in
	//	so we need to drive the process from here.
	//	we will pull off each tagged field here, find the field object that it belongs too and
	//	pass the information to it.

	//	but first, we need to reset our tagged fields.

	for (size_t i = mFirstTaggedField; i < this->GetNumberFields(); ++i)
	{
		(*mFields)[i]->Clear();
	}

	//	now, load the tagged portion.

	//	let's find our field name inside the angle brackets.
	//	we'll use a regex to do the heavy lifting.

	static boost::regex	taggedFieldEx("<([^>/]+)>([^<]*)</([^>]+)>");

	//	we will know iterate over the remainder of the input record and identify all tagged fields
	//	and associate each with the proper field object.

    boost::cregex_iterator m1(record, record + data_len, taggedFieldEx);
    boost::cregex_iterator m2;

	while (m1 != m2)
	{
		//	now check that the field tags match

		dthrow_if_(! ((*m1)[1].length() == (*m1)[3].length()), "Start/End tags do not match ==> ", cp_string<64>(record, record+62).c_str());
		dthrow_if_(! ((*m1)[1].str() == (*m1)[3].str()), "Start/End tags do not match ==> ", cp_string<64>(record, record+62).c_str());

		//	now, let's grab our values.

		CField::FieldName newFieldName((*m1)[1].first, (*m1)[1].second);

		//	find our field object.

		bool foundField	=	false;
		for (size_t i = mFirstTaggedField;	i < this->GetNumberFields();	++i)
		{
			if ((*mFields)[i]->GetFieldName()	==	newFieldName)
			{
				//dthrow_if_(! (*mFields)[i]->Empty(), "Already have data for field ==> ", newFieldName.c_str());
				//	we'll log this too instead of failing.  The field will end up with the first
				//	instance of data in it.
				foundField	=	true;

				if (! (*mFields)[i]->Empty())
				{
					mErrorData	+=	newFieldName.c_str();
					mErrorData	+=	':';
					mErrorData	+=	" Already have data for field.\n";
				}
				else
					(*mFields)[i]->UseData(record + (*m1).position(2), (*m1)[2].length());
				break;
			}
		}

		//dthrow_if_(! foundField, "Unknown Tag Name ==> ", newFieldName.c_str());
		//	instead, we will log this information for later retrieval by the application
		if (! foundField)
		{
			mErrorData	+=	newFieldName.c_str();
			mErrorData	+=	':';
			mErrorData	+=	" Unknown Tag Name.\n";
		}

		++m1;
	}

	//	since derived fields will never show up in the tagged portion of the file
	//	we need to make a pass over the variable portion of the fields list and
	//	manually set up these fields.
	
	for (size_t i = mFirstTaggedField;	i < this->GetNumberFields();	++i)
	{
		if (! ((*mFields)[i]->RealField()))
			(*mFields)[i]->UseData(record, data_len);
	}

	//	now make sure all the required tagged fields are present -- must have data.

	for (size_t i = mFirstTaggedField;	i < this->GetNumberFields();	++i)
	{
		if ((*mFields)[i]->GetModifiers() & CField::e_RequiredField)
			dthrow_if_((*mFields)[i]->Size() == 0, "Missing data in Required field ==> ", (*mFields)[i]->GetFieldName().c_str());
	}

}

//----------------------------------------------------------------------------------------

CFixedTaggedRecord::CFixedTaggedRecord(void)
	:	CFixedRecord(), mFieldCount(0), mFirstTaggedField(0)

{
	mRecord_Type		=	e_FixedTaggedRecord;
}

//----------------------------------------------------------------------------------------

CFixedTaggedRecord::CFixedTaggedRecord(const CFixedTaggedRecord& rhs)
	:	CFixedRecord(rhs), mFieldCount(rhs.mFieldCount),
		mFirstTaggedField(rhs.mFirstTaggedField)

{
}

//----------------------------------------------------------------------------------------

CFixedTaggedRecord::CFixedTaggedRecord(std::istream& input)
	:	CFixedRecord(), mFieldCount(0), mFirstTaggedField(0)

{
	//	we steal the code from Fixed Record for the first part and
	//	add our code for the tagged field definitions.

	//	We should be looking at the line for field names indicator
	
	std::string buffer;
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad FixedTagged Record Description");
	std::istream::int_type nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	std::string::size_type pos, prev_pos;
	pos = prev_pos = 0;
	
	pos = buffer.find_first_not_of(' ', pos);
	
	char names = std::toupper(buffer[pos]);
	dfail_if_(names != 'Y' && names != 'N',
		"Bad Name indicator ==> ", buffer.c_str());
	
	//	next line is unused
	//	8/11/99	added some more options to this line.  see header notes

	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad FixedTagged Record Description");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	if (buffer.empty())
	{
		//	set our defaults to match old usage
		
		mFixedRecordMode = e_Start_Len;
		mNextFieldStart = 0;
		mFixedTableDelim = ',';
	}
	else
	{
		//	let's see what the user wants.
		
		pos = 0;
		prev_pos = buffer.find_first_not_of(' ', pos);
		pos = buffer.find(',', prev_pos);
		if (buffer.compare(prev_pos, pos - prev_pos, "Start_End") == 0)
			mFixedRecordMode = e_Start_End;
		else if (buffer.compare(prev_pos, pos - prev_pos, "Start_Len") == 0)
			mFixedRecordMode = e_Start_Len;
		else if (buffer.compare(prev_pos, pos - prev_pos, "Len") == 0)
			mFixedRecordMode = e_Len;
		else
			dfail_msg_("Invalid Control Info ==> ", buffer.c_str());
		
		//	let's see what the delimiter is.
		
		prev_pos = ++pos;						//	bump past comma
		prev_pos = buffer.find_first_not_of(' ', pos);
		if (std::toupper(buffer[prev_pos]) == 'T')
			mFixedTableDelim = '\t';
		else
			mFixedTableDelim = buffer[prev_pos];
			
		mNextFieldStart = 0;
	}
	//	next is record length
	
	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad FixedTagged Record Description");
	mRecordLength = std::atoi(buffer.c_str());
	dfail_if_(mRecordLength == 0, "Missing FixedTagged Record record length");
	mMaxRecordLen = mRecordLength;
	mMinRecordLen = mRecordLength;
	
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();
	
	//	now, we scan and pick up field info

	while(input.good())
	{
		//	Need to see if at eof
		//	There should be no extra characters after last return
		//	so we force eof if necessary.
		//	need to do this so iterator loop works properly.
	
		std::istream::int_type nextChar = input.peek();
		if (nextChar == std::istream::traits_type::eof())
		{
			nextChar = input.get();		//	force eof
			continue;
		}
		
		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad FixedTagged Record Field Name list");
		nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();
	
		if (buffer.empty())
			continue;						//	skip empty lines
		
		if (buffer[0] == '/' && buffer[1] == '/')
			continue;						//	skip comments

		if (buffer.compare(0, 4, ":END") == 0)
			break;
			
		//	see if we need to switch to our tagged field code

		if (buffer.compare(0, 8, "||TAGS||")	==	0)
			break;

		//	first, check for resetting position pointer
		//	for redefines
		//	ONLY for Length only mode
		
		if (mFixedRecordMode == e_Len && buffer.compare(0, 6, "ORG * ") == 0)
		{
			//	we need to reset our current location to
			//	the start of the specified field
			//	can be either a field name or field number
			
			size_t fldStart = 0;
			if (names == 'Y')
			{
				CField::FieldName fldName(buffer.c_str() + 6);
				fldStart = this->operator[](fldName).GetOffset();
			}
			else
			{
				int whichFld = atoi(buffer.c_str() + 6);
				fldStart = this->operator[](whichFld).GetOffset();
			}
			
			//	now, just reset our pointer and we're on our way
			
			mNextFieldStart = fldStart;
			continue;
		}
		
		//	Let the field figure itself out
		//	we can't do that any more.  we may have multiple field types per record

		CField* newField = NULL;
		
		if (buffer.compare(0, 5, "SYNTH") == 0)
			newField = new CSYNTHField(this, buffer);
		else if (buffer.compare(0, 5, "Combo") == 0)
			newField = new CComboField(this, buffer);
		else if (buffer.compare(0, 6, "EITHER") == 0)
			newField = new CEitherOrField(this, buffer);
		else if (buffer.compare(0, 10, "Skip2Delim") == 0)
			newField = new CSkip2DelimField(this, buffer);
		else if (buffer.compare(0, 6, "Signed") == 0)
			newField = new CFixedSignedField(buffer);
		//else if (buffer.compare(0, 7, "Decimal") == 0)
		//	newField = new CDecimalField(this, buffer);
		else
			newField = new CFixedField(buffer);
			
		mFields->push_back(newField);
		
		//	need to build our field name list too.
		//	make sure there are no duplicate field names
		if (names == 'Y')
		{
			std::pair<FieldNames::iterator, bool> result = mFieldNames->insert(
				MapType(newField->GetFieldName(), mFields->size() - 1));
			dfail_if_(result.second == false, "Duplicate Field Name ==> ",
					newField->GetFieldName().c_str());
		}
	}

	if (buffer.compare(0, 8, "||TAGS||")	==	0)
	{
		//	need to know where the tagged fields start.

		mFirstTaggedField	=	this->GetNumberFields();

		//	now rip off our variable record processing.

		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad FixedTagged Record Description");
		std::istream::int_type nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();

		std::string::size_type pos = 0;

		pos = buffer.find_first_not_of(' ', pos);

		char names = std::toupper(buffer[pos]);
		dfail_if_(names != 'Y' ,
			"Bad Name indicator ==> ", buffer.c_str());			//	we MUST have field names for UseData routine to work.

		//	next if field count

		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad FixedTagged Record Description");
		mFieldCount = std::atol(buffer.c_str());
		dfail_if_(mFieldCount == 0, "Missing Tagged Field field count");
		nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();

		while(input.good())
		{
			//	Need to see if at eof
			//	There should be no extra characters after last return
			//	so we force eof if necessary.
			//	need to do this so iterator loop works properly.

			std::istream::int_type nextChar = input.peek();
			if (nextChar == std::istream::traits_type::eof())
			{
				nextChar = input.get();		//	force eof
				continue;
			}

			std::getline(input, buffer, LINE_END_CHAR);
			dfail_if_(! input.good(), "Bad Variable Record Field Name list");
			nextChar = input.peek();
			if (nextChar == '\n') nextChar = input.get();

			if (buffer.empty())
				continue;						//	skip empty lines

			if (buffer[0] == '/' && buffer[1] == '/')
				continue;						//	skip comments

			if (buffer.compare(0, 4, ":END") == 0)
				break;

			//	Let the field figure itself out
			//	we can't do that any more.  we may have multiple field types per record

			CField* newField = NULL;

			if (buffer.compare(0, 5, "SYNTH") == 0)
				newField = new CSYNTHField(this, buffer);
			else if (buffer.compare(0, 5, "Combo") == 0)
				newField = new CComboField(this, buffer);
			else if (buffer.compare(0, 6, "EITHER") == 0)
				newField = new CEitherOrField(this, buffer);
			else if (buffer.compare(0, 10, "Skip2Delim") == 0)
				newField = new CSkip2DelimField(this, buffer);
			else
			{
				newField = new CTaggedField(buffer);
			}

			mFields->push_back(newField);

			//	need to build our field name list too.
			//	make sure there are no duplicate field names

			std::pair<FieldNames::iterator, bool> result = mFieldNames->insert(
				MapType(newField->GetFieldName(), mFields->size() - 1));
			dfail_if_(result.second == false, "Duplicate Field Name ==> ",
					newField->GetFieldName().c_str());
		}
	}

	//	1 last thing -- make sure we got all the tagged fields we were told to expect

	dfail_if_(mFieldCount != (this->GetNumberFields() - mFirstTaggedField), "Wrong number of Tagged Field field names.");

}

//----------------------------------------------------------------------------------------


CFixedTaggedRecord&	CFixedTaggedRecord::operator=(const CFixedTaggedRecord& rhs)

{
	if (this != &rhs)
	{
		CRecord::operator=(rhs);
		mFieldCount			=	rhs.mFieldCount;
		mFirstTaggedField	=	rhs.mFirstTaggedField;
	}
	return *this;
}

//----------------------------------------------------------------------------------------


/*****************************************************************************************
	CUnionRecord
*****************************************************************************************/

CUnionRecord::CUnionRecord(void)
	:	CRecord(),
		mCurrentRecord(NULL), mKeyFieldOffset(0), mKeyFieldLen(0)

{
	mRecord_Type	=	e_UnionRecord;
}

//----------------------------------------------------------------------------------------

CUnionRecord::CUnionRecord(const CUnionRecord& rhs)
	:	CRecord(rhs), mCurrentRecord(NULL), mKeyFieldOffset(0), mKeyFieldLen(0)

{
	//	need to clone our components

	for (DataRecords::const_iterator i = rhs.mUnionRecords.begin();
		i != rhs.mUnionRecords.end(); ++i)
	{
		DataRecords::value_type newEntry(i->first, i->second->Clone());
		mUnionRecords.insert(newEntry);

		if (i->second == rhs.mCurrentRecord)
			mCurrentRecord	=	newEntry.second;
	}
}

//----------------------------------------------------------------------------------------

CUnionRecord::CUnionRecord(std::istream& input)
	:	CRecord()

{
	//	we need to scan for the begin blocks in our record definition

	mRecord_Type	=	e_UnionRecord;
	
	std::string buffer;	
	std::istream::int_type nextChar;

	//	we should be looking at our key field info
	//	tab delimited fields so the following works
	//	DOESN'T WORK on web2 @!#$!$@!#@!

	//input >> mKeyFieldOffset >> mKeyFieldLen;
	//dfail_if_(mKeyFieldOffset == 0 && mKeyFieldLen == 0, "Bad Union Record Description");

	std::getline(input, buffer, LINE_END_CHAR);
	dfail_if_(! input.good(), "Bad Fixed Record Field Name list");
	nextChar = input.peek();
	if (nextChar == '\n') nextChar = input.get();

	std::string::size_type p;

	p	=	buffer.find('\t');
	dfail_if_(p == std::string::npos, "Bad Fixed Record Field Name list");

	mKeyFieldOffset	=	std::atoi(buffer.c_str());
	mKeyFieldLen	=	std::atoi(buffer.c_str() + ++p);
	
	//	skip to end of line

	//input.ignore(std::numeric_limits<std::streamsize>::max(), LINE_END_CHAR);
	//dfail_if_(! input.good(), "Bad Union Record Description");
	//nextChar = input.peek();
	//if (nextChar == '\n') nextChar = input.get();

	//	now just loop till we get our first :BEGIN

	while(input.good())
	{
		//	Need to see if at eof
		//	There should be no extra characters after last return
		//	so we force eof if necessary.
		//	need to do this so iterator loop works properly.
	
		std::istream::int_type nextChar = input.peek();
		if (nextChar == std::istream::traits_type::eof())
		{
			nextChar = input.get();		//	force eof
			continue;
		}
		
		std::getline(input, buffer, LINE_END_CHAR);
		dfail_if_(! input.good(), "Bad Fixed Record Field Name list");
		nextChar = input.peek();
		if (nextChar == '\n') nextChar = input.get();
	
		if (buffer.empty())
			continue;						//	skip empty lines
		
		if (buffer[0] == '/' && buffer[1] == '/')
			continue;						//	skip comments
		
		if (buffer.compare(0, 6, ":BEGIN") == 0)
		{
			//	need to pull off the record identifier field.
			//	we expect this to start in col. 7 and go to end of line 
			//	OR be followed by a tab followed by a record type name

			VariantID newRecordID;

			std::string::size_type pos = buffer.find('\t', 7);

			newRecordID.mVariantType.assign(buffer.c_str() + 7, pos - 7);
			if (pos != std::string::npos)
				newRecordID.mVariantName = buffer.c_str() + pos + 1;
			
			//	if our buffer is empty then we consider this a default value

			if (newRecordID.mVariantType.empty())
			{
				//	make sure we don't already have a default value

				VariantID defaultID;
				defaultID.mVariantType = ":^Default^:";
				
				DataRecords::iterator i = mUnionRecords.find(defaultID);
				dfail_if_(i != mUnionRecords.end(), "Multiple 'Default' values.");

				newRecordID.mVariantType = ":^Default^:";
			}

			//	ok, our next line is the CRecord type.  any type but a Union
			//	we don't do nesting!

			std::getline(input, buffer, LINE_END_CHAR);
			dfail_if_(! input.good(), "Bad Fixed Record Field Name list");
			nextChar = input.peek();
			if (nextChar == '\n') nextChar = input.get();
			
			pos = 0;
			
			pos = buffer.find_first_not_of(' ', pos);
			
			CRecord* theRecord	=	NULL;
			
			char recordType = std::toupper(buffer[pos]);
			
			if (recordType == 'F')
				theRecord = new CFixedRecord(input);
			else if (recordType == 'V')
				theRecord = new CVariableRecord(input);
			else if (recordType == 'Q')
				theRecord = new CQuotedRecord(input);
			else if (recordType == 'M')
				theRecord = new CMultiLineVariableRecord(input);
			else if (recordType == 'U')
				dfail_msg_ ("Illegal nested 'Union' record types.");
			else
				dfail_msg_ ("Invalid Record type ==> ", buffer.c_str());

			//	ok, save it and get ready for next one

			DataRecords::value_type newEntry(newRecordID, theRecord);
			mUnionRecords.insert(newEntry);
		}
		else
		{
			//	we have a non-empty line which is not part of our known record description

			dfail_msg_("Unexpected data in 'Union' description ==> ", buffer.c_str());
		}
	}

	//	our max is the largest max and our min will be the largest min.

	for (DataRecords::iterator i = mUnionRecords.begin();
		i != mUnionRecords.end(); ++i)
	{
		if (i->second->GetMaxRecordLen() > mMaxRecordLen)
			mMaxRecordLen	=	i->second->GetMaxRecordLen();
		if (i->second->GetMinRecordLen() > mMinRecordLen)
			mMinRecordLen	=	i->second->GetMinRecordLen();
	}
}

//----------------------------------------------------------------------------------------

CUnionRecord::~CUnionRecord(void)

{
	//	need to clear these because they only point to values which belong to our
	//	variants.

	mFields		=	NULL;
	mFieldNames	=	NULL;

	//	need to delete our components

	for (DataRecords::iterator i = mUnionRecords.begin();
		i != mUnionRecords.end(); ++i)
	{
		delete (i->second);
	}
}

//----------------------------------------------------------------------------------------

CRecord*	CUnionRecord::Clone(void) const

{
	return new CUnionRecord(*this);
}

//----------------------------------------------------------------------------------------

void	CUnionRecord::UseData(const char* record, size_t data_len)

{
	//	we need to look in the specified location on the data record and grab the type
	//	identifier.  then we look thru our various record descriptions and select the
	//	appropriate one.  if we don't get a match we look for a default value.

	dfail_if_(mKeyFieldOffset > data_len, "Record too short for 'Union'");
	
	VariantID theRecordType;
	theRecordType.mVariantType.assign(record + mKeyFieldOffset, mKeyFieldLen);

	DataRecords::iterator i = mUnionRecords.find(theRecordType);
	if (i == mUnionRecords.end())
	{
		VariantID defaultID;
		defaultID.mVariantType = ":^Default^:";
		
		i = mUnionRecords.find(defaultID);
		dfail_if_(i == mUnionRecords.end(), "Unknown Union record type ==> ", theRecordType.mVariantType.c_str());
	}

	//	point to our description info

	mFields			=	i->second->mFields;
	mFieldNames		=	i->second->mFieldNames;

	//	need to set these fields so UseData will work correctly
	
	mRecordLength	=	i->second->mRecordLength;
	mMaxRecordLen	=	i->second->mMaxRecordLen;
	mMinRecordLen	=	i->second->mMinRecordLen;
	
	//	ok, pass it on

	i->second->UseData(record, data_len);

	mCurrentRecord	=	i->second;
	mData			=	i->second->mData;
}

//----------------------------------------------------------------------------------------

const CRecord::RecordName*	CUnionRecord::GetRecordName(void) const

{
	//	just need to loop thru our variants and match on record pointer

	for (DataRecords::const_iterator i = mUnionRecords.begin();
		i != mUnionRecords.end(); ++i)
	{
		if (i->second == mCurrentRecord)
			return &(i->first.mVariantName);
	}
	
	return NULL;
}

//----------------------------------------------------------------------------------------





