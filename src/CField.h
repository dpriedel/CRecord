// =====================================================================================
//
//       Filename:  CField.h
//
//    Description:  Fields represent the contents of a piece of a record.
//              	Fields can be fixed or variable or virtual.
//	                Their main function is to construct a string_view which
//	                provides access to their data.
//
//        Version:  1.0
//        Created:  12/30/2022 02:31:16 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  David P. Riedel (), driedel@cox.net
//   Organization:  
//
// =====================================================================================


#ifndef  _CFIELD_INC_
#define  _CFIELD_INC_

#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <fmt/format.h>

	
// =====================================================================================
//        Class:  CField
//  Description:  Interface class for fields
//
// =====================================================================================
template < class T >
class BaseField
{
public:
    // use this to make accessing the above variant less opaque.

    enum FieldTypes
    {
        e_FixedField = 1,
        e_VariableField = 2,
        e_VirtualField = 3
    };

    // ====================  LIFECYCLE     ======================================= 

	// ====================  ACCESSORS     ======================================= 
	
	// ====================  MUTATORS      ======================================= 

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:
    BaseField () { };                             // constructor 
    friend T;
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

    size_t offset_ = 0;
    size_t length_ = 0;

}; // ----------  end of template class CField  ---------- 

// =====================================================================================
//        Class:  CFixedField
//  Description:  Interface class for fields
//
// =====================================================================================

class CFixedField : public BaseField<CFixedField>
{
public:
	enum class PositionMode
	{
		e_Start_Len		=	12,
		e_Start_End,
		e_Len,
		e_Unknown
	};

    // ====================  LIFECYCLE     ======================================= 

    CFixedField() = default;
    CFixedField (PositionMode position_mode, size_t a, size_t b = 0);                             // constructor 
		                                   //
	// ====================  ACCESSORS     ======================================= 

	// ====================  MUTATORS      ======================================= 

    std::string_view UseData(std::string_view record_data);

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

    // ====================  DATA MEMBERS  ======================================= 

}; // ----------  end of template class CField  ---------- 


// =====================================================================================
//        Class:  CVariableField
//  Description:  Simple delimited fields -- NOT quoted
//
// =====================================================================================
class CVariableField : public BaseField<CVariableField>
{
public:
	// ====================  LIFECYCLE     ======================================= 
	CVariableField () = default;                             // constructor 

	// ====================  ACCESSORS     ======================================= 

	// ====================  MUTATORS      ======================================= 

    std::string_view UseData(std::string_view record_data) { return {}; }

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

}; // -----  end of class CVariableField  ----- 

// =====================================================================================
//        Class:  CVirtualField
//  Description:  Replaces COMBO and SYNTH fields
//
// =====================================================================================
class CVirtualField
{
public:

    enum class NameOrNumber
    {
        e_UseNames,
        e_UseNumbers,
        e_Unknown
    };

public:
	// ====================  LIFECYCLE     ======================================= 
	CVirtualField () = default;                             // constructor 
	CVirtualField(NameOrNumber reference_type, const std::string& field_sep_char,
	        const std::vector<size_t>& field_numbers);

	// ====================  ACCESSORS     ======================================= 

    // const std::vector<std::string>& GetFieldNames() const { return real_field_names_; }
    const std::vector<size_t>& GetFieldNumbers() const { return real_field_numbers_; }

	// ====================  MUTATORS      ======================================= 

    std::string_view UseData(std::string_view record_data, const std::vector<std::string_view>& fields_data);

	// ====================  OPERATORS     ======================================= 

protected:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

private:
	// ====================  METHODS       ======================================= 

	// ====================  DATA MEMBERS  ======================================= 

    std::string field_data_;
    std::string content_layout_;
    // std::vector<std::string> real_field_names_;
    std::vector<size_t> real_field_numbers_;

    NameOrNumber field_reference_type_ = NameOrNumber::e_Unknown;
    std::string field_sep_char_;

}; // -----  end of class CVirtualField  ----- 

using CField = std::variant<std::monostate, CFixedField, CVariableField, CVirtualField>;

struct FieldData
{
    std::string field_name_;
    CField field_;
    std::string_view field_data_;
};

using FieldList = std::vector<FieldData>;

// a custom formater for fields

template <> struct fmt::formatter<FieldData>: formatter<std::string>
{
    // parse is inherited from formatter<string>.
    auto format(const FieldData& field_info, fmt::format_context& ctx)
    {
        std::string s;
        fmt::format_to(std::back_inserter(s), "\tname: {}, length: {}, content: ->{}<-.",
            field_info.field_name_, field_info.field_data_.size(), field_info.field_data_);

        return formatter<std::string>::format(s, ctx);
    }
};


// //class	CField	:	public	Loki::SmallObject<DEFAULT_THREADING, 128, 8192>
// class	CField
// {
// public:
//
// 	static const size_t npos = (size_t)-1;
// 	
// 	typedef	Proc::FieldName									FieldName;
// 	typedef Proc::ModiferName								ModiferName;
// 	
// 	enum	Modifiers
// 	{
// 		e_NoModifiers		=	0x00,
// 		e_LeadingBlanks		=	0x01,
// 		e_SignedField		=	0x02,
// 		e_RequiredField		=	0x04,
// 		e_NoLeadingBlanks	=	0x08
// 	};
//
// 	struct	ModifierTable
// 	{
// 		const char*		mModifierName;
// 		int				mModifierID;
// 	};
// 	
// 				//	keep containers happy
// 				
// 					CField(void);
// 					CField(const CField& rhs);
// 			
// 			virtual	~CField(void);
// 			
// 		//	Knows how to make various kinds of fields
// 		
// 	//static	CField*	MakeField(std::istream& table);
// 	
// 	CField&	operator=(const CField& rhs);
//
// 	const char&	operator[](int i) const					{ return mFieldStart[i]; }
// 	
// 	virtual	CField*	Clone(void) const = 0;
// 	
// 	virtual	bool	RealField(void) const				{ return true; }
// 	
// 	//	this comparison looks at CField objects to see if they are the same
// 	
// 			bool	operator==(const CField& rhs) const;
//
// 	//	this comparison looks at the contents of CField
// 	
// 	virtual	bool	operator==(const char* rhs) const;
// 	
// 	virtual	bool	is_equal_to(const CField* rhs) const;
// 	virtual	bool	is_equal_to(const char* rhs) const;
// 	virtual	bool	is_less_than(const char* rhs) const;
//
// 	//	do it again with a max length parm to compare just part of a field
// 	
// 	virtual	bool	is_equal_to(const CField* rhs, size_t n) const;
// 	virtual	bool	is_equal_to(const char* rhs, size_t n) const;
// 	virtual	bool	is_less_than(const char* rhs, size_t n) const;
// 		//	does case insensitive compare
//
// 	virtual	bool	ci_Equivalent(const char* rhs) const;
// 	virtual	bool	ci_Equivalent(const CField* rhs) const;
// 	virtual	bool	ci_Less(const char* rhs) const;
//
// 		//	returns true if start position is less.
// 		
// 	bool	operator<(const CField& rhs) const;
//
// 		//	this looks at the contents of the fields
//
// 	virtual	bool	is_less_than(const CField& rhs) const;
//
// 		//	output operations
// 		//	copy at most n characters to output
// 		
// 	virtual	void	WriteTo(std::ostream& output, size_t n = npos) const ;
// 	virtual	void	CopyTo(char* output, size_t n = npos) const ;
// 	virtual	void	AppendTo(std::string& output, size_t n = npos) const;
// 	virtual	void	AppendWithDelim(std::string& output, unsigned char delim = '>', size_t n = npos) const;
// 	virtual	void	AppendWithLongDelim(std::string& output, const char* delim = "", size_t n = npos) const;
// 	
// 	//	these next 3 routines allow character substitution when writing to the output
// 	//	only 1 character at a time can be substituted.
// 	//	NOTE:	using an 'xlated' value of '\0' means to remove the 'orig' character
// 	//			when doing the copy.
// 	
// 	virtual	void	AppendToXlate(std::string&	output,
// 									char		orig,
// 									char		xlated,
// 									size_t		n = npos) const;
// 									
// 	virtual	void	AppendWithDelimXlate(std::string&		output,
// 											char			orig,
// 											char			xlated,
// 											unsigned char	delim = '>',
// 											size_t			n = npos) const;
// 											
// 	virtual	void	AppendWithLongDelimXlate(std::string&	output,
// 												char		orig,
// 												char		xlated,
// 												const char*	delim = "",
// 												size_t		n = npos) const;
//
// 		//	if no delimiter needed, pass NULL, not ""
// 		
// 	virtual	void	WriteTo(std::ostream& output, const char* delim, size_t n = npos) const ;
// 	
// 	const FieldName&	GetFieldName(void) const	{ return mFieldName; }
// 	const char*			DataBegin(void) const		{ return mFieldStart; }
// 	const char*			DataEnd(void) const			{ return mDataEnd; }
// 	size_t				Capacity(void) const		{ return mFieldLen; }
// 	size_t				Size(void) const			{ return mDataLen; }
// 	
// 	size_t				GetOffset(void) const		{ return mBeginPos; }
// 	int					GetModifiers(void) const	{ return mModifiers; }
// 	
// 	bool				Empty(void) const			{ return (mDataLen == 0); }
//
// 	//	this next routine is primarily for variable type records.
//
// 	virtual	void	Clear(void);
//
// protected:
//
//     //  need this for variable records
//
//     virtual void        SetOffset(size_t offset)    { /* do nothing here. */ }
//
// private:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 64>
// 	>														TempData;
//
// public:
//
// 	//	this creates a useful temp object
//
// 	const TempData		MakeTemp(void) const		{ return TempData(mFieldStart, mDataEnd); }
//
// 	//	for python
//
// 	std::string			ToStr(void) const			{ return std::string(mFieldStart, mDataLen); }
// 	
// 		//	lastly, we need to actually have some reference to our data
// 		
// 	virtual	void	UseData(const char* record, size_t data_len) = 0;		//	ABC
//     
// protected:
//
// 				//	field name, start, end
// 				
// 			CField(std::string& tableEntry);
//
// 	FieldName		mFieldName;				//	optional
// 	const char*		mFieldStart;
// 	const char*		mFieldEnd;				//	1-beyond as in iterators
// 	const char*		mDataEnd;				//	1-beyond as in iterators
// 	size_t			mBeginPos;
// 	size_t			mFieldLen;
// 	size_t			mDataLen;
//
// 	int				mModifiers;
//
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	We (eventually) can have many field types (including structs and arrays)
// 	//	For now, we have just Fixed and Variable Fields
// 	
// class	CFixedField	:	public CField
// {
// public:
//
// 					CFixedField(void);
// 					CFixedField(const CFixedField& rhs);
// 					CFixedField(std::string& tableEntry);
// 			
// 			virtual	~CFixedField(void);
//
// 	virtual	CFixedField*	Clone(void) const;
//
// 		//	point to start of record
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// };
//
// // -------------------------------------------------------------------------------------
//
// class	CVariableField	:	public CField
// {
// public:
//
// 					CVariableField(void);
// 					CVariableField(const CVariableField& rhs);
// 					CVariableField(std::string& tableEntry, char delim, short count = 1);
// 					CVariableField(char delim, short count = 1);
// 			
// 			virtual	~CVariableField(void);
//
// 	virtual	CVariableField*	Clone(void) const;
//
// 		//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// 	virtual	void	CopyTo(char* output, size_t n = npos) const;
// 	virtual	void	AppendTo(std::string& output, size_t n = npos) const;
// 	virtual	void	AppendWithDelim(std::string& output, unsigned char delim = '>', size_t n = npos) const;
// 	
// private:
//
//     friend  class CVariableRecord;
//
//     virtual void        SetOffset(size_t offset)    { mBeginPos =   offset; }
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 32>
// 	>														Terminators;
//
// 	short		mHowManyDelims;			//	number of fields to include
// 	char		mDelim;
// 	Terminators	mTerminators;			//	all chars which can end a field
// };
//
// // -------------------------------------------------------------------------------------
//
//
// 	//	This class is for fields surrounded by a 'quote' character
// 	
// 	//	The quote character is specified on the same line as the variable field field
// 	//	separator.  Use the word 'Comma' for comma otherwise just put the character
// 	
// class CQuotedField	:	public	CVariableField
// {
// public:
//
// 					CQuotedField(void);
// 					CQuotedField(const CQuotedField& rhs);
// 					CQuotedField(std::string& tableEntry, char quote, char delim, short count = 1);
// 					CQuotedField(char quote, char delim, short count = 1);
// 			
// 			virtual	~CQuotedField(void);
//
// 	virtual	CQuotedField*	Clone(void) const;
//
// 	static	void	SetupDelims(char quote = '\0', char delim = '\0');
//
// 		//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
//     friend  class CQuotedRecord;
//
//     virtual void        SetOffset(size_t offset)    { mBeginPos =   offset; }
//
// 	static	char	mEndPair[2];					//	quote char, delim char
// 			char	mQuoteChar;						//	surrounding char
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class is a 'synthetic' field which means it is created from
// 	//	the contents of other fields or selected parts of the input
// 	//	record.
// 	//	If it is composed of other fields, all must have been previously
// 	//	defined before used by this field.
//
// 	//	we make our copy of the base fields and set our
// 	//	data pointers to point to our local data
// 	
// class	CSYNTHField	:	public CField
// {
// public:
//
// 					CSYNTHField(void);
// 					CSYNTHField(const CSYNTHField& rhs);
// 					CSYNTHField(CRecord* theRecord, std::string& tableEntry);
// 			
// 			virtual	~CSYNTHField(void);
//
// 	virtual	CSYNTHField*	Clone(void) const;
//
// 	virtual	bool	RealField(void) const				{ return false; }
//
// 	CSYNTHField&	operator=(const CSYNTHField& rhs);
//
// 	//	special comparison functions to take into account the component delimiter
// 	//	and to allow GENERIC comparisons using '*' for a component.
// 	//	'*' can be on either lhs or rhs of comparison.
// 	
// 	virtual	bool	operator==(const char* key) const;
//
// 	virtual	bool	is_equal_to(const char* rhs) const;
// 	virtual	bool	is_less_than(const char* rhs) const;
//
// 	//	does case insensitive compare
//
// 	virtual	bool	ci_Equivalent(const char* rhs) const;
// 	virtual	bool	ci_Less(const char* rhs) const;
//
// 		//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 64>
// 	>														SYNTHData;
//
// 	SYNTHData			mFieldData;			//	collect from our parts
// 	std::vector<int>	mFieldNumbers;		//	our base fields
// 	CRecord*			mTheRecord;
// 	char				mComponentDelim;	//	component separator
// 	
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class is a 'combination' field which means it is created from
// 	//	the contents of other fields or selected parts of the input
// 	//	record.
// 	//	If it is composed of other fields, all must have been previously
// 	//	defined before used by this field.
//
// 	//	we make our copy of the base fields and set our
// 	//	data pointers to point to our local data
// 	
// class	CComboField	:	public CField
// {
// public:
//
// 					CComboField(void);
// 					CComboField(const CComboField& rhs);
// 					CComboField(CRecord* theRecord, std::string& tableEntry);
// 			
// 			virtual	~CComboField(void);
//
// 	virtual	CComboField*	Clone(void) const;
//
// 	virtual	bool	RealField(void) const				{ return false; }
//
// 	CComboField&	operator=(const CComboField& rhs);
//
// 	//	unlike SYNTH field, there are no special comparison ooperations here
// 	
// 	//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 64>
// 	>														COMBOData;
//
// 	COMBOData			mFieldData;			//	collect from our parts
// 	std::vector<int>	mFieldNumbers;		//	our base fields
// 	CRecord*			mTheRecord;
// 	char				mComponentDelim;	//	component separator	
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class is a derived field which means it is created from
// 	//	the contents of another field.
//
// 	//	we make our copy of the base fields and set our
// 	//	data pointers to point to our local data
// 	
// class	CSkip2DelimField	:	public CField
// {
// public:
//
// 					CSkip2DelimField(void);
// 					CSkip2DelimField(const CSkip2DelimField& rhs);
// 					CSkip2DelimField(CRecord* theRecord, std::string& tableEntry);
// 			
// 			virtual	~CSkip2DelimField(void);
//
// 	virtual	CSkip2DelimField*	Clone(void) const;
//
// 	virtual	bool	RealField(void) const				{ return false; }
//
// 	CSkip2DelimField&	operator=(const CSkip2DelimField& rhs);
//
// 	//	unlike SYNTH field, there are no special comparison ooperations here
// 	
// 	//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 64>
// 	>														DerivedData;
//
// 	DerivedData			mFieldData;			//	collect from our parts
// 	CRecord*			mTheRecord;
// 	int					mBaseFieldNumber;	//	our base field
// 	char				mDelim;				//	what we skip to
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class is a derived field which means it is created from
// 	//	the contents of another field.
//
// 	//	we make our copy of the base fields and set our
// 	//	data pointers to point to our local data
// 	//
// 	//	This class looks for the first field in the list of
// 	//	fields that is not empty and uses that for its data.
// 	
// class	CEitherOrField	:	public CField
// {
// public:
//
// 					CEitherOrField(void);
// 					CEitherOrField(const CEitherOrField& rhs);
// 					CEitherOrField(CRecord* theRecord, std::string& tableEntry);
// 			
// 			virtual	~CEitherOrField(void);
//
// 	virtual	CEitherOrField*	Clone(void) const;
//
// 	virtual	bool	RealField(void) const				{ return false; }
//
// 	CEitherOrField&	operator=(const CEitherOrField& rhs);
//
// 	//	unlike SYNTH field, there are no special comparison ooperations here
// 	
// 	//	point to start of field
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 64>
// 	>														DerivedData;
//
// 	DerivedData			mFieldData;			//	collect from our parts
// 	std::vector<int>	mFieldNumbers;		//	our base fields
// 	CRecord*			mTheRecord;
// };
//
// // -------------------------------------------------------------------------------------
// 	//	the Decimal field type provides support for implied decimal points in data.
// 	//	NOTE: maximum length of 31 digits supported.
// /*
// class	CDecimalField	:	public CField
// {
// public:
//
// 					CDecimalField(void);
// 					CDecimalField(const CDecimalField& rhs);
// 					CDecimalField(CRecord* theRecord, std::string& tableEntry);
// 			
// 			virtual	~CDecimalField(void);
//
// 	virtual	CDecimalField*	Clone(void) const;
//
// 	CDecimalField&	operator=(const CDecimalField& rhs);
//
// 	//	this routine will take care of inserting the decimal point in the specified
// 	//	place.
// 	
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<char, 32, VectorStringStorage<char, std::allocator<char> > >
// 	>														DecimalData;
//
// 	DecimalData			mFieldData;			//	collect from our parts
// 	short				mDecimalPlaces;
// };
// */
// // -------------------------------------------------------------------------------------
//
// 	//	Signed Field if for numeric fields which have sign character encoded as the rightmost
// 	//	character (for negative fields).
// 	
// class	CFixedSignedField	:	public CFixedField
// {
// public:
//
// 					CFixedSignedField(void);
// 					CFixedSignedField(const CFixedSignedField& rhs);
// 					CFixedSignedField(std::string& tableEntry);
// 			
// 			virtual	~CFixedSignedField(void);
//
// 	CFixedSignedField&	operator=(const CFixedSignedField& rhs);
//
// 	virtual	CFixedSignedField*	Clone(void) const;
//
// 		//	point to start of record
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 32>
// 	>														SignedData;
//
//
// 	SignedData	mFieldData;					//	will have translated value
// 	size_t		mSaveBeginPos;
// 	size_t		mSaveFieldLen;				//	1-beyond as in iterators
// };
//
// // -------------------------------------------------------------------------------------
//
// class	CVariableSignedField	:	public CVariableField
// {
// public:
//
// 					CVariableSignedField(void);
// 					CVariableSignedField(const CVariableSignedField& rhs);
// 					CVariableSignedField(std::string& tableEntry);
// 			
// 			virtual	~CVariableSignedField(void);
//
// 	virtual	CVariableSignedField*	Clone(void) const;
//
// 		//	point to start of record
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 32>
// 	>														SignedData;
//
// 	SignedData			mFieldData;					//	will have translated value
// };
//
// // -------------------------------------------------------------------------------------
//
//
// //	This class supports HTML type fields which have name tags and a value.
//
// class	CTaggedField	:	public CField
// {
// public:
//
// 					CTaggedField(void);
// 					CTaggedField(const CTaggedField& rhs);
// 					CTaggedField(std::string& tableEntry);
// 			
// 			virtual	~CTaggedField(void);
//
// 	virtual	CTaggedField*	Clone(void) const;
//
// 		//	point to start of record
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// 	virtual	void	Clear(void);
//
// 	//	NOTE:	We need class specific versions of all these operations
// 	//			because these fields are optional. If there is no content
// 	//			for the field we will have a bunch of NULL pointers which
// 	//			could cause problems.
// 		
// 	//	this comparison looks at the contents of CField
// 	
// 	virtual	bool	operator==(const char* rhs) const;
// 	
// 	virtual	bool	is_equal_to(const CField* rhs) const;
// 	virtual	bool	is_equal_to(const char* rhs) const;
// 	virtual	bool	is_less_than(const char* rhs) const;
//
// 	//	do it again with a max length parm to compare just part of a field
// 	
// 	virtual	bool	is_equal_to(const CField* rhs, size_t n) const;
// 	virtual	bool	is_equal_to(const char* rhs, size_t n) const;
// 	virtual	bool	is_less_than(const char* rhs, size_t n) const;
// 		//	does case insensitive compare
//
// 	virtual	bool	ci_Equivalent(const char* rhs) const;
// 	virtual	bool	ci_Equivalent(const CField* rhs) const;
// 	virtual	bool	ci_Less(const char* rhs) const;
//
// 		//	this looks at the contents of the fields
//
// 	virtual	bool	is_less_than(const CField& rhs) const;
// 		
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	A template function to copy data from a Field to a cp_string.
// 	
// template<size_t Size>
// cp_string<Size>&	Assign(cp_string<Size>& dest, const CField& source)
// {
// 	dest.assign(source.DataBegin(), source.Size());
// 	return dest;
// }
//
// // -------------------------------------------------------------------------------------
//
// 	//	for case-insensitive strings
// 	
// // template<size_t Size>
// // ci_cp_string(Size)& Assign(ci_cp_string(Size)& dest, const CField& source)
// // {
// //     dest.assign(source.DataBegin(), source.Size());
// //     return dest;
// // }
//
// template<size_t Size>
// cp_string<Size, cp_stack_allocator<Size>, ci_char_traits>&	Assign(cp_string<Size, cp_stack_allocator<Size>, ci_char_traits>& dest, const CField& source)
// {
// 	dest.assign(source.DataBegin(), source.Size());
// 	return dest;
// }
//
// // -------------------------------------------------------------------------------------
//
// template <typename E, class T, class A, class S>
// flex_string<E, T, A, S>&	Assign(flex_string<E, T, A, S>& dest, const CField& source)
// {
// 	dest.assign(source.DataBegin(), source.Size());
// 	return dest;
// }
//
// // -------------------------------------------------------------------------------------
//
// 	//	A template function to append data from a Field to a cp_string.
// 	
// template<size_t Size>
// cp_string<Size>&	Append(cp_string<Size>& dest, const CField& source)
// {
// 	dest.append(source.DataBegin(), source.Size());
// 	return dest;
// }
//
// // -------------------------------------------------------------------------------------
//
// template <typename E, class T, class A, class S>
// flex_string<E, T, A, S>&	Append(flex_string<E, T, A, S>& dest, const CField& source)
// {
// 	dest.append(source.DataBegin(), source.Size());
// 	return dest;
// }
//
// // -------------------------------------------------------------------------------------
//
// 	//	A template function to compare data in a Field to a cp_string.
// 	
// template<size_t Size>
// bool	IsEquivalent(cp_string<Size>& lhs, const CField& rhs)
// {
// 	return (lhs.compare(0, lhs.size(), rhs.DataBegin(), rhs.Size()) == 0);
// }
//
// // -------------------------------------------------------------------------------------
//
// 	//	convert data to an int
//
// inline
// int		ToInt(const CField& source)
// {
// 	cp_string<32> temp;
// 	Assign(temp, source);
// 	return atoi(temp.c_str());
// }
//
// // -------------------------------------------------------------------------------------
//
#endif   // ----- #ifndef _CFIELD_INC_  ----- 
