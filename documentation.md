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
