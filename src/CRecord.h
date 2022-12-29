/*
	File:		CRecord.h

	Contains:	Header file with field offsets for each field in input record
				and output record.
				

	Written by:	Riedel

	Copyright:	© 1998 Printing For Systems, Inc. all rights reserved.

	Change History (most recent first):

				11/15/2002	DPR 	version 12?		added Union record type.
				 8/26/98	DPR		version 1

	To Do:
*/

#ifndef _CRECORD_
#define _CRECORD_

#include <iosfwd>
//#include <size_t.h>
#include <vector>
#include <map>
#include <stdexcept>
//#include <ext/hash_fun.h>
#include <ext/hash_map>

//using size_t;

#include "cp_string.h"
#include "string_utils.h"

//	we want to take advantage of alexandrescu's custom allocators.

//#include "SmallObj.h"
#include "Definitions.h"

	//	this is taking the easy way out for now.

#define LINE_END_CHAR	'\n'			//	for unix description
//#define LINE_END_CHAR	'\r'			//	for Macintosh description


	//	for error reporting
	
class Short_Record	:	public	fatal_error
{
	public:
		explicit Short_Record(const char* what_arg = NULL) : fatal_error(what_arg) {}
		explicit Short_Record(const std::string& what_arg) : fatal_error(what_arg) {}
};
class Long_Record	:	public	fatal_error
{
	public:
		explicit Long_Record(const char* what_arg = NULL) : fatal_error(what_arg) {}
		explicit Long_Record(const std::string& what_arg) : fatal_error(what_arg) {}
};

	//	Fields represent the contents of a piece of a record.
	//	Fields can be fixed or variable.
	//	Variable Fields know how to find the end of their data.
	
class	CRecord;
class   CVariableRecord;
class   CQuotedRecord;

//class	CField	:	public	Loki::SmallObject<DEFAULT_THREADING, 128, 8192>
class	CField
{
public:

	static const size_t npos = (size_t)-1;
	
	typedef	Proc::FieldName									FieldName;
	typedef Proc::ModiferName								ModiferName;
	
	enum	Modifiers
	{
		e_NoModifiers		=	0x00,
		e_LeadingBlanks		=	0x01,
		e_SignedField		=	0x02,
		e_RequiredField		=	0x04,
		e_NoLeadingBlanks	=	0x08
	};

	struct	ModifierTable
	{
		const char*		mModifierName;
		int				mModifierID;
	};
	
				//	keep containers happy
				
					CField(void);
					CField(const CField& rhs);
			
			virtual	~CField(void);
			
		//	Knows how to make various kinds of fields
		
	//static	CField*	MakeField(std::istream& table);
	
	CField&	operator=(const CField& rhs);

	const char&	operator[](int i) const					{ return mFieldStart[i]; }
	
	virtual	CField*	Clone(void) const = 0;
	
	virtual	bool	RealField(void) const				{ return true; }
	
	//	this comparison looks at CField objects to see if they are the same
	
			bool	operator==(const CField& rhs) const;

	//	this comparison looks at the contents of CField
	
	virtual	bool	operator==(const char* rhs) const;
	
	virtual	bool	is_equal_to(const CField* rhs) const;
	virtual	bool	is_equal_to(const char* rhs) const;
	virtual	bool	is_less_than(const char* rhs) const;

	//	do it again with a max length parm to compare just part of a field
	
	virtual	bool	is_equal_to(const CField* rhs, size_t n) const;
	virtual	bool	is_equal_to(const char* rhs, size_t n) const;
	virtual	bool	is_less_than(const char* rhs, size_t n) const;
		//	does case insensitive compare

	virtual	bool	ci_Equivalent(const char* rhs) const;
	virtual	bool	ci_Equivalent(const CField* rhs) const;
	virtual	bool	ci_Less(const char* rhs) const;

		//	returns true if start position is less.
		
	bool	operator<(const CField& rhs) const;

		//	this looks at the contents of the fields

	virtual	bool	is_less_than(const CField& rhs) const;

		//	output operations
		//	copy at most n characters to output
		
	virtual	void	WriteTo(std::ostream& output, size_t n = npos) const ;
	virtual	void	CopyTo(char* output, size_t n = npos) const ;
	virtual	void	AppendTo(std::string& output, size_t n = npos) const;
	virtual	void	AppendWithDelim(std::string& output, unsigned char delim = '>', size_t n = npos) const;
	virtual	void	AppendWithLongDelim(std::string& output, const char* delim = "", size_t n = npos) const;
	
	//	these next 3 routines allow character substitution when writing to the output
	//	only 1 character at a time can be substituted.
	//	NOTE:	using an 'xlated' value of '\0' means to remove the 'orig' character
	//			when doing the copy.
	
	virtual	void	AppendToXlate(std::string&	output,
									char		orig,
									char		xlated,
									size_t		n = npos) const;
									
	virtual	void	AppendWithDelimXlate(std::string&		output,
											char			orig,
											char			xlated,
											unsigned char	delim = '>',
											size_t			n = npos) const;
											
	virtual	void	AppendWithLongDelimXlate(std::string&	output,
												char		orig,
												char		xlated,
												const char*	delim = "",
												size_t		n = npos) const;

		//	if no delimiter needed, pass NULL, not ""
		
	virtual	void	WriteTo(std::ostream& output, const char* delim, size_t n = npos) const ;
	
	const FieldName&	GetFieldName(void) const	{ return mFieldName; }
	const char*			DataBegin(void) const		{ return mFieldStart; }
	const char*			DataEnd(void) const			{ return mDataEnd; }
	size_t				Capacity(void) const		{ return mFieldLen; }
	size_t				Size(void) const			{ return mDataLen; }
	
	size_t				GetOffset(void) const		{ return mBeginPos; }
	int					GetModifiers(void) const	{ return mModifiers; }
	
	bool				Empty(void) const			{ return (mDataLen == 0); }

	//	this next routine is primarily for variable type records.

	virtual	void	Clear(void);

protected:

    //  need this for variable records

    virtual void        SetOffset(size_t offset)    { /* do nothing here. */ }

private:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 64>
	>														TempData;

public:

	//	this creates a useful temp object

	const TempData		MakeTemp(void) const		{ return TempData(mFieldStart, mDataEnd); }

	//	for python

	std::string			ToStr(void) const			{ return std::string(mFieldStart, mDataLen); }
	
		//	lastly, we need to actually have some reference to our data
		
	virtual	void	UseData(const char* record, size_t data_len) = 0;		//	ABC
    
protected:

				//	field name, start, end
				
			CField(std::string& tableEntry);

	FieldName		mFieldName;				//	optional
	const char*		mFieldStart;
	const char*		mFieldEnd;				//	1-beyond as in iterators
	const char*		mDataEnd;				//	1-beyond as in iterators
	size_t			mBeginPos;
	size_t			mFieldLen;
	size_t			mDataLen;

	int				mModifiers;

};

// -------------------------------------------------------------------------------------

	//	We (eventually) can have many field types (including structs and arrays)
	//	For now, we have just Fixed and Variable Fields
	
class	CFixedField	:	public CField
{
public:

					CFixedField(void);
					CFixedField(const CFixedField& rhs);
					CFixedField(std::string& tableEntry);
			
			virtual	~CFixedField(void);

	virtual	CFixedField*	Clone(void) const;

		//	point to start of record
		
	virtual	void	UseData(const char* record, size_t data_len);

};

// -------------------------------------------------------------------------------------

class	CVariableField	:	public CField
{
public:

					CVariableField(void);
					CVariableField(const CVariableField& rhs);
					CVariableField(std::string& tableEntry, char delim, short count = 1);
					CVariableField(char delim, short count = 1);
			
			virtual	~CVariableField(void);

	virtual	CVariableField*	Clone(void) const;

		//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

	virtual	void	CopyTo(char* output, size_t n = npos) const;
	virtual	void	AppendTo(std::string& output, size_t n = npos) const;
	virtual	void	AppendWithDelim(std::string& output, unsigned char delim = '>', size_t n = npos) const;
	
private:

    friend  class CVariableRecord;

    virtual void        SetOffset(size_t offset)    { mBeginPos =   offset; }

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 32>
	>														Terminators;

	short		mHowManyDelims;			//	number of fields to include
	char		mDelim;
	Terminators	mTerminators;			//	all chars which can end a field
};

// -------------------------------------------------------------------------------------


	//	This class is for fields surrounded by a 'quote' character
	
	//	The quote character is specified on the same line as the variable field field
	//	separator.  Use the word 'Comma' for comma otherwise just put the character
	
class CQuotedField	:	public	CVariableField
{
public:

					CQuotedField(void);
					CQuotedField(const CQuotedField& rhs);
					CQuotedField(std::string& tableEntry, char quote, char delim, short count = 1);
					CQuotedField(char quote, char delim, short count = 1);
			
			virtual	~CQuotedField(void);

	virtual	CQuotedField*	Clone(void) const;

	static	void	SetupDelims(char quote = '\0', char delim = '\0');

		//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

    friend  class CQuotedRecord;

    virtual void        SetOffset(size_t offset)    { mBeginPos =   offset; }

	static	char	mEndPair[2];					//	quote char, delim char
			char	mQuoteChar;						//	surrounding char
};

// -------------------------------------------------------------------------------------

	//	this class is a 'synthetic' field which means it is created from
	//	the contents of other fields or selected parts of the input
	//	record.
	//	If it is composed of other fields, all must have been previously
	//	defined before used by this field.

	//	we make our copy of the base fields and set our
	//	data pointers to point to our local data
	
class	CSYNTHField	:	public CField
{
public:

					CSYNTHField(void);
					CSYNTHField(const CSYNTHField& rhs);
					CSYNTHField(CRecord* theRecord, std::string& tableEntry);
			
			virtual	~CSYNTHField(void);

	virtual	CSYNTHField*	Clone(void) const;

	virtual	bool	RealField(void) const				{ return false; }

	CSYNTHField&	operator=(const CSYNTHField& rhs);

	//	special comparison functions to take into account the component delimiter
	//	and to allow GENERIC comparisons using '*' for a component.
	//	'*' can be on either lhs or rhs of comparison.
	
	virtual	bool	operator==(const char* key) const;

	virtual	bool	is_equal_to(const char* rhs) const;
	virtual	bool	is_less_than(const char* rhs) const;

	//	does case insensitive compare

	virtual	bool	ci_Equivalent(const char* rhs) const;
	virtual	bool	ci_Less(const char* rhs) const;

		//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 64>
	>														SYNTHData;

	SYNTHData			mFieldData;			//	collect from our parts
	std::vector<int>	mFieldNumbers;		//	our base fields
	CRecord*			mTheRecord;
	char				mComponentDelim;	//	component separator
	
};

// -------------------------------------------------------------------------------------

	//	this class is a 'combination' field which means it is created from
	//	the contents of other fields or selected parts of the input
	//	record.
	//	If it is composed of other fields, all must have been previously
	//	defined before used by this field.

	//	we make our copy of the base fields and set our
	//	data pointers to point to our local data
	
class	CComboField	:	public CField
{
public:

					CComboField(void);
					CComboField(const CComboField& rhs);
					CComboField(CRecord* theRecord, std::string& tableEntry);
			
			virtual	~CComboField(void);

	virtual	CComboField*	Clone(void) const;

	virtual	bool	RealField(void) const				{ return false; }

	CComboField&	operator=(const CComboField& rhs);

	//	unlike SYNTH field, there are no special comparison ooperations here
	
	//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 64>
	>														COMBOData;

	COMBOData			mFieldData;			//	collect from our parts
	std::vector<int>	mFieldNumbers;		//	our base fields
	CRecord*			mTheRecord;
	char				mComponentDelim;	//	component separator	
};

// -------------------------------------------------------------------------------------

	//	this class is a derived field which means it is created from
	//	the contents of another field.

	//	we make our copy of the base fields and set our
	//	data pointers to point to our local data
	
class	CSkip2DelimField	:	public CField
{
public:

					CSkip2DelimField(void);
					CSkip2DelimField(const CSkip2DelimField& rhs);
					CSkip2DelimField(CRecord* theRecord, std::string& tableEntry);
			
			virtual	~CSkip2DelimField(void);

	virtual	CSkip2DelimField*	Clone(void) const;

	virtual	bool	RealField(void) const				{ return false; }

	CSkip2DelimField&	operator=(const CSkip2DelimField& rhs);

	//	unlike SYNTH field, there are no special comparison ooperations here
	
	//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 64>
	>														DerivedData;

	DerivedData			mFieldData;			//	collect from our parts
	CRecord*			mTheRecord;
	int					mBaseFieldNumber;	//	our base field
	char				mDelim;				//	what we skip to
};

// -------------------------------------------------------------------------------------

	//	this class is a derived field which means it is created from
	//	the contents of another field.

	//	we make our copy of the base fields and set our
	//	data pointers to point to our local data
	//
	//	This class looks for the first field in the list of
	//	fields that is not empty and uses that for its data.
	
class	CEitherOrField	:	public CField
{
public:

					CEitherOrField(void);
					CEitherOrField(const CEitherOrField& rhs);
					CEitherOrField(CRecord* theRecord, std::string& tableEntry);
			
			virtual	~CEitherOrField(void);

	virtual	CEitherOrField*	Clone(void) const;

	virtual	bool	RealField(void) const				{ return false; }

	CEitherOrField&	operator=(const CEitherOrField& rhs);

	//	unlike SYNTH field, there are no special comparison ooperations here
	
	//	point to start of field
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 64>
	>														DerivedData;

	DerivedData			mFieldData;			//	collect from our parts
	std::vector<int>	mFieldNumbers;		//	our base fields
	CRecord*			mTheRecord;
};

// -------------------------------------------------------------------------------------
	//	the Decimal field type provides support for implied decimal points in data.
	//	NOTE: maximum length of 31 digits supported.
/*
class	CDecimalField	:	public CField
{
public:

					CDecimalField(void);
					CDecimalField(const CDecimalField& rhs);
					CDecimalField(CRecord* theRecord, std::string& tableEntry);
			
			virtual	~CDecimalField(void);

	virtual	CDecimalField*	Clone(void) const;

	CDecimalField&	operator=(const CDecimalField& rhs);

	//	this routine will take care of inserting the decimal point in the specified
	//	place.
	
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<char, 32, VectorStringStorage<char, std::allocator<char> > >
	>														DecimalData;

	DecimalData			mFieldData;			//	collect from our parts
	short				mDecimalPlaces;
};
*/
// -------------------------------------------------------------------------------------

	//	Signed Field if for numeric fields which have sign character encoded as the rightmost
	//	character (for negative fields).
	
class	CFixedSignedField	:	public CFixedField
{
public:

					CFixedSignedField(void);
					CFixedSignedField(const CFixedSignedField& rhs);
					CFixedSignedField(std::string& tableEntry);
			
			virtual	~CFixedSignedField(void);

	CFixedSignedField&	operator=(const CFixedSignedField& rhs);

	virtual	CFixedSignedField*	Clone(void) const;

		//	point to start of record
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 32>
	>														SignedData;


	SignedData	mFieldData;					//	will have translated value
	size_t		mSaveBeginPos;
	size_t		mSaveFieldLen;				//	1-beyond as in iterators
};

// -------------------------------------------------------------------------------------

class	CVariableSignedField	:	public CVariableField
{
public:

					CVariableSignedField(void);
					CVariableSignedField(const CVariableSignedField& rhs);
					CVariableSignedField(std::string& tableEntry);
			
			virtual	~CVariableSignedField(void);

	virtual	CVariableSignedField*	Clone(void) const;

		//	point to start of record
		
	virtual	void	UseData(const char* record, size_t data_len);

protected:

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 32>
	>														SignedData;

	SignedData			mFieldData;					//	will have translated value
};

// -------------------------------------------------------------------------------------


//	This class supports HTML type fields which have name tags and a value.

class	CTaggedField	:	public CField
{
public:

					CTaggedField(void);
					CTaggedField(const CTaggedField& rhs);
					CTaggedField(std::string& tableEntry);
			
			virtual	~CTaggedField(void);

	virtual	CTaggedField*	Clone(void) const;

		//	point to start of record
		
	virtual	void	UseData(const char* record, size_t data_len);

	virtual	void	Clear(void);

	//	NOTE:	We need class specific versions of all these operations
	//			because these fields are optional. If there is no content
	//			for the field we will have a bunch of NULL pointers which
	//			could cause problems.
		
	//	this comparison looks at the contents of CField
	
	virtual	bool	operator==(const char* rhs) const;
	
	virtual	bool	is_equal_to(const CField* rhs) const;
	virtual	bool	is_equal_to(const char* rhs) const;
	virtual	bool	is_less_than(const char* rhs) const;

	//	do it again with a max length parm to compare just part of a field
	
	virtual	bool	is_equal_to(const CField* rhs, size_t n) const;
	virtual	bool	is_equal_to(const char* rhs, size_t n) const;
	virtual	bool	is_less_than(const char* rhs, size_t n) const;
		//	does case insensitive compare

	virtual	bool	ci_Equivalent(const char* rhs) const;
	virtual	bool	ci_Equivalent(const CField* rhs) const;
	virtual	bool	ci_Less(const char* rhs) const;

		//	this looks at the contents of the fields

	virtual	bool	is_less_than(const CField& rhs) const;
		
};

// -------------------------------------------------------------------------------------

	//	A template function to copy data from a Field to a cp_string.
	
template<size_t Size>
cp_string<Size>&	Assign(cp_string<Size>& dest, const CField& source)
{
	dest.assign(source.DataBegin(), source.Size());
	return dest;
}

// -------------------------------------------------------------------------------------

	//	for case-insensitive strings
	
// template<size_t Size>
// ci_cp_string(Size)& Assign(ci_cp_string(Size)& dest, const CField& source)
// {
//     dest.assign(source.DataBegin(), source.Size());
//     return dest;
// }

template<size_t Size>
cp_string<Size, cp_stack_allocator<Size>, ci_char_traits>&	Assign(cp_string<Size, cp_stack_allocator<Size>, ci_char_traits>& dest, const CField& source)
{
	dest.assign(source.DataBegin(), source.Size());
	return dest;
}

// -------------------------------------------------------------------------------------

template <typename E, class T, class A, class S>
flex_string<E, T, A, S>&	Assign(flex_string<E, T, A, S>& dest, const CField& source)
{
	dest.assign(source.DataBegin(), source.Size());
	return dest;
}

// -------------------------------------------------------------------------------------

	//	A template function to append data from a Field to a cp_string.
	
template<size_t Size>
cp_string<Size>&	Append(cp_string<Size>& dest, const CField& source)
{
	dest.append(source.DataBegin(), source.Size());
	return dest;
}

// -------------------------------------------------------------------------------------

template <typename E, class T, class A, class S>
flex_string<E, T, A, S>&	Append(flex_string<E, T, A, S>& dest, const CField& source)
{
	dest.append(source.DataBegin(), source.Size());
	return dest;
}

// -------------------------------------------------------------------------------------

	//	A template function to compare data in a Field to a cp_string.
	
template<size_t Size>
bool	IsEquivalent(cp_string<Size>& lhs, const CField& rhs)
{
	return (lhs.compare(0, lhs.size(), rhs.DataBegin(), rhs.Size()) == 0);
}

// -------------------------------------------------------------------------------------

	//	convert data to an int

inline
int		ToInt(const CField& source)
{
	cp_string<32> temp;
	Assign(temp, source);
	return atoi(temp.c_str());
}

// -------------------------------------------------------------------------------------

	//	This class hierarchy supports both fixed and variable records.
	//	Record descriptons are read from a stream.
	//	Records are parsed on the fly to generate fields.

	/*	The Format of the Record Definition follows:
	
		The First non-comment line MUST BE:
			Fixed or Variable or Quoted or MultiLineVariable or Union.
		The NEXT LINE MUST BE:
			for Union:				<starting position of type id field>\t<length of type id field>.
			for other record type:	Yes or No for field names

		For Union record types, the existing record descriptions are bracketed with
		:BEGIN and :END.  everthing in between is as described below.
		the :BEGIN line has the following form
			:BEGIN\t<value that identifies this flavor of record>\t<name to reference this flavor by>
			value must be found in the field boundaries specified on the Union record.
			
		The NEXT LINE MUST BE:
			for Variable fields the delimiter character
			(use the word Tab for the tab key)
			for fixed fields, an empty line means:
				-	Start_Len, comma delimited table.
			for fixed fields, indicator for kind of length info provided and table delimiter.
				-	Start_End: beginning and ending column for field
				- 	Start_Len: beginning column and field length
				-	Len: field length (field start is computed so ALL fields must be specified)
				-	specify table delimiter if not comma.
			NOTE:	Above fields MUST be separted by a comma.
			for Quoted fields the variable field delimiter is followd by a comma
				followed by the quote character.  Use the word 'Comma' for a comma
			
		The NEXT LINE MUST BE:
			for Variable fields the number of fields in the record
			for fixed fields, the record length.
			
		For Fixed records, each field is defined as follows:
			Field Name (if Names == Yes in line 2) <delim> start position <delim> field length
			eg;	(for Start_End)
				City,100,119
				State,120,121
				Zip,122,130
				
			eg;	(for Start_Len)
				City,100,20
				State,120,2
				Zip,122,9
				
			eg;	(for Len)
				...	Earlier fields MUST be defined although you can group contiguous fields into 1 entry
				City,20
				State,2
				Zip,9
			
		NOTE:	commas in examples above could be replaced by the delimiter char specifed
				after the Start_Len or whatever.
		
		NOTE:	for Fixed fields specified by length only, the field start
				position can be reset using the following:
				
			ORG * fieldName or
			ORG * nn where nn is a field number
			
				use name if using field names otherwise, use number
			
			Fields can be redefined:
			eg;	Place,100,31
			
			Only fields of interest need be specified.  There
			are no edits regarding coverage of the entire record.

		Field entries can have modifiers.  This must be the first entry
		in the line followed by field delimiter specified earlier (comma, tab, etc)
			eg;	LB	PlanInfo1	300	320
		
		Currently support modifiers are:
		-	LB	keep leading blanks.
		-

		for SYNTH or synthetic fields, so-called because they are composed
		of other real fields:
		(fields are separated by table separator character identified above.)

			- SYNTH flag
			- field name if using names
			- Name or Number to specify how to identify component fields
			- field separator character,
			- comma delimited list of component field identifiers.

			eg;	SNYTH	key 	Name	|	flda, fldb, fldz
			
			
		Further field definitions are optional for Variable Records.
		For Variable records, each field is defined as follows
			field name if Names = Yes an optional comma followed
				by the number of delimiters this name applies to
				or blank line for place holder.
			eg;	First Name
				
				Last Name,2	(Smith plus JR, SR etc)
			Fields can be redefined:
				City
				State
				Zip
				-3,Place,3 means back up 3 fields and use the
				next 3 delimiters.
				
	*/

	//	more specializations of stl_hash_fun.h

namespace __gnu_cxx
{
	template<> struct hash<CField::FieldName>
	{
	  size_t operator()(const CField::FieldName& s) const { return __stl_hash_string(s.c_str()); }
	};
	
	template<> struct hash<const CField::FieldName>
	{
	  size_t operator()(const CField::FieldName& s) const { return __stl_hash_string(s.c_str()); }
	};
};

class	CUnionRecord;
			
class	CRecord
{
public:

	enum { EOF_Char	= 26 };
	enum	Record_Type
	{
		e_FixedRecord	=	83,
		e_VariableRecord,
		e_QuotedRecord,
		e_MultiLineVariableRecord,
		e_FixedTaggedRecord,
		e_UnionRecord,
		e_UnknownRecord
	};
	
	//	need this for Union type

	typedef Proc::RecordName								RecordName;
	typedef	std::vector<CField*>							FieldList;

		virtual	~CRecord(void);
	
	static	CRecord*	MakeRecord(std::istream& input);
	
	virtual	void	UseData(const char* record, size_t data_len) = 0;	//	make into ABC
	const char*		GetData(void) const			{ return mData; }

	virtual	CRecord*	Clone(void) const = 0;
	
			//	Field level access
			//	fields are numbered starting from 0
			
	const CField&	operator[](int i) const;		//	access by field number
	const CField&	operator[](const CField::FieldName& name) const;
	const CField&	operator[](const char* name) const;

	//	returns a list of pointers to fields whose names START WITH the provided string
	
	FieldList		GetFieldNamesLike(const CField::FieldName& name) const;

	int		GetFieldNumber(const CField::FieldName& name) const;

	size_t 	GetMaxRecordLen(void) const 			{ return mMaxRecordLen; }
	size_t 	GetMinRecordLen(void) const 			{ return mMinRecordLen; }
	size_t 	GetRecordLength(void) const 			{ return mRecordLength; }
	size_t 	GetNumberFields(void) const 			{ return mFieldNames->size(); }
	
	//	somethings to look like iterators

	const char*		RecordBegin(void) const			{ return mData; }
	const char*		RecordEnd(void) const			{ return mData + mRecordLength; }
	
	virtual	const RecordName*	GetRecordName(void) const	{ return NULL; }
	
	Record_Type		GetRecordType(void) const		{ return mRecord_Type; }

	void			PrintFieldNames(std::ostream& output) const;

	const std::string&	GetErrorData(void) const	{ return mErrorData; }
	bool				HasErrorData(void) const	{ return ! mErrorData.empty(); }

protected:

	friend	class	CUnionRecord;
	
		//	field stuff
		
	//typedef	std::map<CField::FieldName, int>				FieldNames;
	typedef	__gnu_cxx::hash_map<CField::FieldName, int, __gnu_cxx::hash<CField::FieldName> >				FieldNames;
	typedef FieldNames::value_type							MapType;
	
				CRecord(void);
				CRecord(std::istream& input);
				CRecord(const CRecord& rhs);
	CRecord&	operator=(const CRecord& rhs);
	
	
	std::string	mErrorData;			//	place to report parsing problems	
	FieldList*	mFields;			//	simple vector of fields.
	FieldNames*	mFieldNames;		//	provide access by name
	const char*	mData;
	size_t		mRecordLength;
	size_t 		mMaxRecordLen;		//	from defn maybe
	size_t 		mMinRecordLen;
	
	Record_Type	mRecord_Type;
};

// -------------------------------------------------------------------------------------

class	CFixedRecord	:	public CRecord
{
public:

		virtual	~CFixedRecord(void);

	virtual	void	UseData(const char* record, size_t data_len);

	virtual	CRecord*	Clone(void) const;

protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
	friend	class		CUnionRecord;

				CFixedRecord(void);
				CFixedRecord(const CFixedRecord& rhs);
				CFixedRecord(std::istream& input);
	CFixedRecord&	operator=(const CFixedRecord& rhs);

//private:

	enum FixedRecordMode
	{
		e_Start_Len		=	12,
		e_Start_End,
		e_Len
	};

		//	special functions only used by CFixedField ctor.
		
	static	FixedRecordMode	GetFixedRecordMode(void)				{ return mFixedRecordMode; }
	static	size_t			NextFieldStart(void)					{ return mNextFieldStart; }
	static	void			IncrementFieldStart(size_t incr)		{ mNextFieldStart += incr; }
	static	char			FixedTableDelim(void)					{ return mFixedTableDelim; }

	friend	CFixedField::CFixedField(std::string& tableEntry);
	friend	CTaggedField::CTaggedField(std::string& tableEntry);
	friend	CSYNTHField::CSYNTHField(CRecord* theRecord, std::string& tableEntry);
	
	static	FixedRecordMode	mFixedRecordMode;
	static	size_t			mNextFieldStart;
	static	char			mFixedTableDelim;			
};

// -------------------------------------------------------------------------------------

class	CVariableRecord	:	public CRecord
{
public:

	enum
	{
		e_MaxVarRecLen	=	10000
	};
	
		virtual	~CVariableRecord(void);

	virtual	void		UseData(const char* record, size_t data_len);
	CVariableRecord&	operator=(const CVariableRecord& rhs);

	virtual	CRecord*	Clone(void) const;

protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
	friend	class		CUnionRecord;

				CVariableRecord(void);
				CVariableRecord(const CVariableRecord& rhs);
				CVariableRecord(std::istream& input);
				
	size_t			mFieldCount;		//	how many fields
	unsigned char	mDelim;
};

// -------------------------------------------------------------------------------------

	//	This class will work with records which have fields surrounded by a 'quote' character.
	//	NOTE:	Not all fields need be surrounded by the 'quote' character but all
	//			fields MUST be separated by the delimiter.
	
class CQuotedRecord	:	public CVariableRecord
{
public:

				
		virtual	~CQuotedRecord(void);

	virtual	void	UseData(const char* record, size_t data_len);

	virtual	CRecord*	Clone(void) const;

protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
	friend	class		CUnionRecord;

				CQuotedRecord(void);
				CQuotedRecord(const CQuotedRecord& rhs);
				CQuotedRecord(std::istream& input);

	CQuotedRecord&	operator=(const CQuotedRecord& rhs);


	char	mQuoteChar;
};

// -------------------------------------------------------------------------------------

	//	this class knows how to deal with 'records' which are actually composed
	//	of multiple lines.
	//	(it skips over line terminators if you were wondering.)

class CMultiLineVariableRecord	:	public CVariableRecord
{
public:

				
		virtual	~CMultiLineVariableRecord(void)					{ };

	virtual	void	UseData(const char* record, size_t data_len);

protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
	friend	class		CUnionRecord;

				CMultiLineVariableRecord(void);
				CMultiLineVariableRecord(const CMultiLineVariableRecord& rhs);
				CMultiLineVariableRecord(std::istream& input);

};


// -------------------------------------------------------------------------------------

	//	this class if for records which are made up fixed part and tagged parts.

class CFixedTaggedRecord	:	public CFixedRecord
{
public:

				
		virtual	~CFixedTaggedRecord(void)					{ };
		
	virtual	void	UseData(const char* record, size_t data_len);

			int		FirstTaggedField(void) const			{ return mFirstTaggedField; }
protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
	friend	class		CUnionRecord;

				CFixedTaggedRecord(void);
				CFixedTaggedRecord(const CFixedTaggedRecord& rhs);
				CFixedTaggedRecord(std::istream& input);

	CFixedTaggedRecord&	operator=(const CFixedTaggedRecord& rhs);


	size_t		mFieldCount;				//	how many tagged fields
	int			mFirstTaggedField;			//	so we know how many fixed and tagged.

};


// -------------------------------------------------------------------------------------

	//	this class allows us to deal with data files which have multiple record types.
	//	it will manage a collection for CRecord*s and swicth to the appropriate one
	//	when given a record to use.  CRecords*s can be a mix of fixed, variable, etc.
	//	Union variants can also be referenced by name

class	CUnionRecord	:	public	CRecord
{
public:
				
		virtual	~CUnionRecord(void);

	virtual	CRecord*	Clone(void) const;
	virtual	void		UseData(const char* record, size_t data_len);

	virtual	const RecordName*	GetRecordName(void) const;

protected:

	friend	CRecord*	CRecord::MakeRecord(std::istream& input);

				CUnionRecord(void);
				CUnionRecord(const CUnionRecord& rhs);
				CUnionRecord(std::istream& input);

	typedef flex_string
	<
		char,
		std::char_traits<char>,
		std::allocator<char>,
		SmallStringOpt<VectorStringStorage<char>, 32>
	>														TypeData;

	//	we need to be able to reference our vairants by either data identifier or variant name

	struct VariantID
	{
		TypeData	mVariantType;
		RecordName	mVariantName;

		bool	operator<(const VariantID& rhs) const
		{
			return mVariantType < rhs.mVariantType;
		}
	};
	
	typedef std::map<VariantID, CRecord*>					DataRecords;

	DataRecords		mUnionRecords;			//	no Confederates here!

	CRecord*		mCurrentRecord;			//	so i don't have to look all the time
	
	size_t			mKeyFieldOffset;		//	start of discriminator field
	size_t			mKeyFieldLen;			//	how many bytes to we need
};

// -------------------------------------------------------------------------------------

#endif
