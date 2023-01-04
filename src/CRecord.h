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


#ifndef  _CRECORD_INC_
#define  _CRECORD_INC_

// #include <vector>
// #include <map>
#include <stdexcept>
#include <string>

#include "CField.h"

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


// 	//	more specializations of stl_hash_fun.h
//
// namespace __gnu_cxx
// {
// 	template<> struct hash<CField::FieldName>
// 	{
// 	  size_t operator()(const CField::FieldName& s) const { return __stl_hash_string(s.c_str()); }
// 	};
// 	
// 	template<> struct hash<const CField::FieldName>
// 	{
// 	  size_t operator()(const CField::FieldName& s) const { return __stl_hash_string(s.c_str()); }
// 	};
// };
//
// class	CUnionRecord;
// 			
class	CRecord
{
// public:
//
// 	enum { EOF_Char	= 26 };
// 	enum	Record_Type
// 	{
// 		e_FixedRecord	=	83,
// 		e_VariableRecord,
// 		e_QuotedRecord,
// 		e_MultiLineVariableRecord,
// 		e_FixedTaggedRecord,
// 		e_UnionRecord,
// 		e_UnknownRecord
// 	};
// 	
// 	//	need this for Union type
//
// 	typedef Proc::RecordName								RecordName;
// 	typedef	std::vector<CField*>							FieldList;
//
// 		virtual	~CRecord(void);
// 	
// 	static	CRecord*	MakeRecord(std::istream& input);
// 	
// 	virtual	void	UseData(const char* record, size_t data_len) = 0;	//	make into ABC
// 	const char*		GetData(void) const			{ return mData; }
//
// 	virtual	CRecord*	Clone(void) const = 0;
// 	
// 			//	Field level access
// 			//	fields are numbered starting from 0
// 			
// 	const CField&	operator[](int i) const;		//	access by field number
// 	const CField&	operator[](const CField::FieldName& name) const;
// 	const CField&	operator[](const char* name) const;
//
// 	//	returns a list of pointers to fields whose names START WITH the provided string
// 	
// 	FieldList		GetFieldNamesLike(const CField::FieldName& name) const;
//
// 	int		GetFieldNumber(const CField::FieldName& name) const;
//
// 	size_t 	GetMaxRecordLen(void) const 			{ return mMaxRecordLen; }
// 	size_t 	GetMinRecordLen(void) const 			{ return mMinRecordLen; }
// 	size_t 	GetRecordLength(void) const 			{ return mRecordLength; }
// 	size_t 	GetNumberFields(void) const 			{ return mFieldNames->size(); }
// 	
// 	//	somethings to look like iterators
//
// 	const char*		RecordBegin(void) const			{ return mData; }
// 	const char*		RecordEnd(void) const			{ return mData + mRecordLength; }
// 	
// 	virtual	const RecordName*	GetRecordName(void) const	{ return NULL; }
// 	
// 	Record_Type		GetRecordType(void) const		{ return mRecord_Type; }
//
// 	void			PrintFieldNames(std::ostream& output) const;
//
// 	const std::string&	GetErrorData(void) const	{ return mErrorData; }
// 	bool				HasErrorData(void) const	{ return ! mErrorData.empty(); }
//
// protected:
//
// 	friend	class	CUnionRecord;
// 	
// 		//	field stuff
// 		
// 	//typedef	std::map<CField::FieldName, int>				FieldNames;
// 	typedef	__gnu_cxx::hash_map<CField::FieldName, int, __gnu_cxx::hash<CField::FieldName> >				FieldNames;
// 	typedef FieldNames::value_type							MapType;
// 	
// 				CRecord(void);
// 				CRecord(std::istream& input);
// 				CRecord(const CRecord& rhs);
// 	CRecord&	operator=(const CRecord& rhs);
// 	
// 	
// 	std::string	mErrorData;			//	place to report parsing problems	
// 	FieldList*	mFields;			//	simple vector of fields.
// 	FieldNames*	mFieldNames;		//	provide access by name
// 	const char*	mData;
// 	size_t		mRecordLength;
// 	size_t 		mMaxRecordLen;		//	from defn maybe
// 	size_t 		mMinRecordLen;
// 	
// 	Record_Type	mRecord_Type;
};

// // -------------------------------------------------------------------------------------
//
// class	CFixedRecord	:	public CRecord
// {
// public:
//
// 		virtual	~CFixedRecord(void);
//
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// 	virtual	CRecord*	Clone(void) const;
//
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
// 	friend	class		CUnionRecord;
//
// 				CFixedRecord(void);
// 				CFixedRecord(const CFixedRecord& rhs);
// 				CFixedRecord(std::istream& input);
// 	CFixedRecord&	operator=(const CFixedRecord& rhs);
//
// //private:
//
// 	enum FixedRecordMode
// 	{
// 		e_Start_Len		=	12,
// 		e_Start_End,
// 		e_Len
// 	};
//
// 		//	special functions only used by CFixedField ctor.
// 		
// 	static	FixedRecordMode	GetFixedRecordMode(void)				{ return mFixedRecordMode; }
// 	static	size_t			NextFieldStart(void)					{ return mNextFieldStart; }
// 	static	void			IncrementFieldStart(size_t incr)		{ mNextFieldStart += incr; }
// 	static	char			FixedTableDelim(void)					{ return mFixedTableDelim; }
//
// 	friend	CFixedField::CFixedField(std::string& tableEntry);
// 	friend	CTaggedField::CTaggedField(std::string& tableEntry);
// 	friend	CSYNTHField::CSYNTHField(CRecord* theRecord, std::string& tableEntry);
// 	
// 	static	FixedRecordMode	mFixedRecordMode;
// 	static	size_t			mNextFieldStart;
// 	static	char			mFixedTableDelim;			
// };
//
// // -------------------------------------------------------------------------------------
//
// class	CVariableRecord	:	public CRecord
// {
// public:
//
// 	enum
// 	{
// 		e_MaxVarRecLen	=	10000
// 	};
// 	
// 		virtual	~CVariableRecord(void);
//
// 	virtual	void		UseData(const char* record, size_t data_len);
// 	CVariableRecord&	operator=(const CVariableRecord& rhs);
//
// 	virtual	CRecord*	Clone(void) const;
//
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
// 	friend	class		CUnionRecord;
//
// 				CVariableRecord(void);
// 				CVariableRecord(const CVariableRecord& rhs);
// 				CVariableRecord(std::istream& input);
// 				
// 	size_t			mFieldCount;		//	how many fields
// 	unsigned char	mDelim;
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	This class will work with records which have fields surrounded by a 'quote' character.
// 	//	NOTE:	Not all fields need be surrounded by the 'quote' character but all
// 	//			fields MUST be separated by the delimiter.
// 	
// class CQuotedRecord	:	public CVariableRecord
// {
// public:
//
// 				
// 		virtual	~CQuotedRecord(void);
//
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// 	virtual	CRecord*	Clone(void) const;
//
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
// 	friend	class		CUnionRecord;
//
// 				CQuotedRecord(void);
// 				CQuotedRecord(const CQuotedRecord& rhs);
// 				CQuotedRecord(std::istream& input);
//
// 	CQuotedRecord&	operator=(const CQuotedRecord& rhs);
//
//
// 	char	mQuoteChar;
// };
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class knows how to deal with 'records' which are actually composed
// 	//	of multiple lines.
// 	//	(it skips over line terminators if you were wondering.)
//
// class CMultiLineVariableRecord	:	public CVariableRecord
// {
// public:
//
// 				
// 		virtual	~CMultiLineVariableRecord(void)					{ };
//
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
// 	friend	class		CUnionRecord;
//
// 				CMultiLineVariableRecord(void);
// 				CMultiLineVariableRecord(const CMultiLineVariableRecord& rhs);
// 				CMultiLineVariableRecord(std::istream& input);
//
// };
//
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class if for records which are made up fixed part and tagged parts.
//
// class CFixedTaggedRecord	:	public CFixedRecord
// {
// public:
//
// 				
// 		virtual	~CFixedTaggedRecord(void)					{ };
// 		
// 	virtual	void	UseData(const char* record, size_t data_len);
//
// 			int		FirstTaggedField(void) const			{ return mFirstTaggedField; }
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
// 	friend	class		CUnionRecord;
//
// 				CFixedTaggedRecord(void);
// 				CFixedTaggedRecord(const CFixedTaggedRecord& rhs);
// 				CFixedTaggedRecord(std::istream& input);
//
// 	CFixedTaggedRecord&	operator=(const CFixedTaggedRecord& rhs);
//
//
// 	size_t		mFieldCount;				//	how many tagged fields
// 	int			mFirstTaggedField;			//	so we know how many fixed and tagged.
//
// };
//
//
// // -------------------------------------------------------------------------------------
//
// 	//	this class allows us to deal with data files which have multiple record types.
// 	//	it will manage a collection for CRecord*s and swicth to the appropriate one
// 	//	when given a record to use.  CRecords*s can be a mix of fixed, variable, etc.
// 	//	Union variants can also be referenced by name
//
// class	CUnionRecord	:	public	CRecord
// {
// public:
// 				
// 		virtual	~CUnionRecord(void);
//
// 	virtual	CRecord*	Clone(void) const;
// 	virtual	void		UseData(const char* record, size_t data_len);
//
// 	virtual	const RecordName*	GetRecordName(void) const;
//
// protected:
//
// 	friend	CRecord*	CRecord::MakeRecord(std::istream& input);
//
// 				CUnionRecord(void);
// 				CUnionRecord(const CUnionRecord& rhs);
// 				CUnionRecord(std::istream& input);
//
// 	typedef flex_string
// 	<
// 		char,
// 		std::char_traits<char>,
// 		std::allocator<char>,
// 		SmallStringOpt<VectorStringStorage<char>, 32>
// 	>														TypeData;
//
// 	//	we need to be able to reference our vairants by either data identifier or variant name
//
// 	struct VariantID
// 	{
// 		TypeData	mVariantType;
// 		RecordName	mVariantName;
//
// 		bool	operator<(const VariantID& rhs) const
// 		{
// 			return mVariantType < rhs.mVariantType;
// 		}
// 	};
// 	
// 	typedef std::map<VariantID, CRecord*>					DataRecords;
//
// 	DataRecords		mUnionRecords;			//	no Confederates here!
//
// 	CRecord*		mCurrentRecord;			//	so i don't have to look all the time
// 	
// 	size_t			mKeyFieldOffset;		//	start of discriminator field
// 	size_t			mKeyFieldLen;			//	how many bytes to we need
// };
//
// -------------------------------------------------------------------------------------

#endif   // ----- #ifndef _CRECORD_INC_  ----- 
