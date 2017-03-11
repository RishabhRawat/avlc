/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Scanner.h"
#include "CompilerExceptions.h"
#include <cassert>
#include "Keywords.h"

// This classification is a simplification of the categories of LRM93 13.1
// LRM93 13.1
// The only characters allowed in the text of a VHDL description are the
// graphic characters and format effector.
enum class CharacterKindType {
	Invalid, // Neither a format effector nor a graphic character.
	Format_Effector,
	Upper_Case_Letter,
	Digit,
	Special_Character,
	Space_Character,
	Lower_Case_Letter,
	Other_Special_Character
};

// LRM93 13.1
// BASIC_GRAPHIC_CHARACTER ::=
//  UPPER_CASE_LETTER | DIGIT | SPECIAL_CHARACTER | SPACE_CHARACTER
//subtype Basic_Graphic_Character is
// CharacterKindType range Upper_Case_Letter .. Space_Character;

// LRM93 13.1
// GRAPHIC_CHARACTER ::=
//  BASIC_GRAPHIC_CHARACTER | LOWER_CASE_LETTER | OTHER_SPECIAL_CHARACTER
// Note: There is 191 graphic character.

using GraphicCharacter = subtype<CharacterKindType,
                                 CharacterKindType::Upper_Case_Letter,
                                 CharacterKindType::Other_Special_Character>;

inline bool isGraphicCharacter(CharacterKindType c) {
	return (c >= CharacterKindType::Upper_Case_Letter && c <= CharacterKindType::Other_Special_Character);
};


// LRM93 13.1
// The characters included in each of the categories of basic graphic
// characters are defined as follows:

// Note: This is Latin-1 encoding (ISO/IEC_8859-1)
CharacterKindType getCharacterKind(unsigned char c) {
	if (c <= 8 || (c >= 14 && c <= 31) || (c >= 127 && c <= 159))
		return CharacterKindType::Invalid;

		// 1. upper case letters
		// 'A' .. 'Z' | UC_A_Grave .. UC_O_Diaeresis | UC_O_Oblique_Stroke .. UC_Icelandic_Thorn
	else if ((c >= 'A' && c <= 'Z') || (c >= 192 && c <= 222 && c != 215))
		return CharacterKindType::Upper_Case_Letter;

// 5. lower case letters
//	'a' .. 'z' | LC_German_Sharp_S .. LC_O_Diaeresis | LC_O_Oblique_Stroke .. LC_Y_Diaeresis
	else if ((c >= 'a' && c <= 'z') || (c >= 224 && c <= 255 && c != 247))
		return CharacterKindType::Lower_Case_Letter;

	else if (c >= '0' && c <= '9')
		return CharacterKindType::Digit;

// 3. special characters
// '"' | '#' | '&' | ''' | '(' | ')' | '+' | ',' | '-' | '.' | '/' | ':'
// ';' | '<' | '=' | '>' | '[' | ']' | '_' | '|' | '*'

	else if (c == 34 || c == 35 || c == 38 || c == 39 || c == 40 || c == 41 || c == 42 || c == 43 || c == 44 || c == 45
			|| c == 46 || c == 47 || c == 58 || c == 59 || c == 60 || c == 61 || c == 62 || c == 91 || c == 93
			|| c == 95 || c == 124)
		return CharacterKindType::Special_Character;

		// Format effectors are the ISO (and ASCII) characters called horizontal
		// tabulation, vertical tabulation, carriage return, line feed, and form
		// feed.
	else if (c >= 9 && c <= 13)
		return CharacterKindType::Format_Effector;
	else if (c == 32 || c == 160)  // SP or NSBP
		return CharacterKindType::Space_Character;
	else if (c <= 214)
		return CharacterKindType::Format_Effector;
// 6. other special characters
//	'!' | '$' | '%' | '@' | '?' | '\' | '^' | '`' | '{' | '}' | '~' and others

	else if (c == 33 || c == 36 || c == 37 || c == 64 || c == 63 || c == 92 || c == 94 || c == 96 || c == 123
			|| c == 125 || c == 126 || (c >= 161 && c <= 191) || c == 215 || c == 247)
		return CharacterKindType::Other_Special_Character;
	else
		throw std::runtime_error("Invalid character");
}
//Iir_Int64 currentIirInt64() ;
//Iir_Fp64 currentIirFp64() ;
//void scan();
/*

function Current_String_Id return String8_Id is
begin
return currentContext.Str_Id;
end Current_String_Id;

function Current_String_Length return Nat32 is
begin
return currentContext.Str_Len;
end Current_String_Length;

function Current_Iir_Int64 return Iir_Int64 is
begin
return currentContext.Int64;
end Current_Iir_Int64;

function Current_Iir_Fp64 return Iir_Fp64 is
begin
return currentContext.Fp64;
end Current_Iir_Fp64;

function Get_Current_Source_File return Source_File_Entry is
begin
return currentContext.Source_File;
end Get_Current_Source_File;

function Get_Current_Line return Natural is
begin
return currentContext.Line_Number;
end Get_Current_Line;
*/
/*

function Get_Current_Column return Natural
		is
Col : Natural;
Name : Name_Id;
begin
		Coord_To_Position
		(currentContext.Source_File,
				currentContext.Line_Pos,
				Integer (currentContext.Pos - currentContext.Line_Pos),
				Name, Col);
return Col;
end Get_Current_Column;

function Get_Token_Column return Natural
		is
Col : Natural;
Name : Name_Id;
begin
		Coord_To_Position
		(currentContext.Source_File,
				currentContext.Line_Pos,
				Integer (currentContext.Token_Pos - currentContext.Line_Pos),
				Name, Col);
return Col;
end Get_Token_Column;
*/

/*
function Get_Token_Position return Source_Ptr is
begin
return currentContext.Token_Pos;
end Get_Token_Position;

function Get_Position return Source_Ptr is
begin
return currentContext.Pos;
end Get_Position;
*/
//
//procedure Set_File (Source_File : Source_File_Entry)
//is
//		N_Source: File_Buffer_Acc;
//begin
//		pragma Assert (currentContext.Source = null);
//pragma Assert (Source_File /= No_Source_File_Entry);
//N_Source := Get_File_Source (Source_File);
//currentContext := (Source => N_Source,
//		Source_File => Source_File,
//Line_Number => 1,
//Line_Pos => 0,
//Pos => N_Source'First,
//Token_Pos => 0, // should be invalid,
//File_Len => Get_File_Length (Source_File),
//		Token => Tok_Invalid,
//Prev_Token => Tok_Invalid,
//Identifier => Null_Identifier,
//Str_Id => Null_String8,
//Str_Len => 0,
//Int64 => -1,
//Fp64 => 0.0);
//currentContext.token := Tok_Invalid;
//end Set_File;
/*

function Detect_Encoding_Errors return Boolean
		is
C : constant Character := Source (Pos);
begin
// No need to check further if first character is plain ASCII-7
if C >= ' ' and C < Character'Val (127) then
return False;
end if;

// UTF-8 BOM is EF BB BF
if Source (Pos + 0) = Character'Val (16#ef#)
and then Source (Pos + 1) = Character'Val (16#bb#)
and then Source (Pos + 2) = Character'Val (16#bf#)
then
		throw std::runtime_error
		("source encoding must be latin-1 (UTF-8 BOM detected)");
return True;
end if;

// UTF-16 BE BOM is FE FF
if Source (Pos + 0) = Character'Val (16#fe#)
and then Source (Pos + 1) = Character'Val (16#ff#)
then
		throw std::runtime_error
		("source encoding must be latin-1 (UTF-16 BE BOM detected)");
return True;
end if;

// UTF-16 LE BOM is FF FE
if Source (Pos + 0) = Character'Val (16#ff#)
and then Source (Pos + 1) = Character'Val (16#fe#)
then
		throw std::runtime_error
		("source encoding must be latin-1 (UTF-16 LE BOM detected)");
return True;
end if;

// Certainly weird, but scanner/parser will catch it.
return False;
end Detect_Encoding_Errors;
*/

/*
procedure Set_Current_Position (Position: Source_Ptr)
is
		Loc : Location_Type;
Offset: Natural;
File_Entry : Source_File_Entry;
begin
// Scanner must have been initialized.
pragma Assert (currentContext.Source /= null);

currentContext.token := Tok_Invalid;
currentContext.Pos := Position;
Loc := File_Pos_To_Location (currentContext.Source_File,
		currentContext.Pos);
Location_To_Coord (Loc,
		File_Entry, currentContext.Line_Pos,
currentContext.Line_Number, Offset);
end Set_Current_Position;

procedure Close_File is
		begin
currentContext.Source := null;
end Close_File;
*/

/*
// Emit an error when a character above 128 was found.
// This must be called only in vhdl87.
procedure std::runtime_error ("extended identifiers not allowed in vhdl87"); is
		begin
throw std::runtime_error ("8 bits characters not allowed in vhdl87");
end std::runtime_error ("extended identifiers not allowed in vhdl87");;
*/

// scan a decimal literal or a based literal.
//
// LRM93 13.4.1
// DECIMAL_LITERAL ::= INTEGER [ . INTEGER ] [ EXPONENT ]
// EXPONENT ::= E [ + ] INTEGER | E - INTEGER
//
// LRM93 13.4.2
// BASED_LITERAL ::= BASE # BASED_INTEGER [ . BASED_INTEGER ] # EXPONENT
// BASE ::= INTEGER

// TODO: procedure Scan_Literal is separate;

// Scan a string literal.
//
// LRM93 13.6 / LRM08 15.7
// A string literal is formed by a sequence of graphic characters
// (possibly none) enclosed between two quotation marks used as string
// brackets.
// STRING_LITERAL ::= " { GRAPHIC_CHARACTER } "
//
// IN: for a string, at the call of this procedure, the current character
// must be either '"' or '%'.

void Scanner::scanString() {
	assert(currentContext.cChar() == '\"' || currentContext.cChar() == '%');
	char mark = currentContext.cChar();
	std::string str = "";
	while (1) {
		unsigned char c = currentContext.incChar();
		if (c == mark) {
			// LRM93 13.6
			// If a quotation mark value is to be represented in the sequence
			// of character values, then a pair of adjacent quoatation
			// characters marks must be written at the corresponding place
			// within the string literal.
			// LRM93 13.10
			// Any pourcent sign within the sequence of characters must then
			// be doubled, and each such doubled percent sign is interpreted
			// as a single percent sign value.
			// The same replacement is allowed for a bit string literal,
			// provieded that both bit string brackets are replaced.
			if (currentContext.cChar() != mark) return;
		}
		switch (getCharacterKind(c)) {
		case CharacterKindType::Format_Effector:
			if (mark == '%')
				// No matching '%' has been found.  Consider '%' was used
				// as the remainder operator, instead of 'rem'.  This will
				// improve the error message.
				throw std::runtime_error("% is not a vhdl operator, use 'rem'");
			throw std::runtime_error("format effector not allowed in a string");
		case CharacterKindType::Invalid:
			throw std::runtime_error("invalid character not allowed, even in a string");
		default:
			if (state.options.standard == Vhdl_Std::Vhdl_87 && c > 127)
				("8 bits characters not allowed in vhdl87");
		}
		if (c == '\"' && mark == '%') {
			// LRM93 13.10
			// The quotation marks (") used as string brackets at both ends of
			// a string literal can be replaced by percent signs (%), provided
			// that the enclosed sequence of characters constains no quotation
			// marks, and provided that both string brackets are replaced.
			std::runtime_error("\" cannot be used in a string delimited with %");
		}
		str.push_back(c);
	}
	currentContext.Identifier = str;
	currentContext.token = Token::String;

}

// Scan a bit string literal.
//
// LRM93 13.7
// A bit string literal is formed by a sequence of extended digits
// (possibly none) enclosed between two quotations used as bit string
// brackets, preceded by a base specifier.
// BIT_STRING_LITERAL ::= BASE_SPECIFIER " [ BIT_VALUE ] "
// BIT_VALUE ::= EXTENDED_DIGIT { [ UNDERLINE ] EXTENDED_DIGIT }
//
// The current character must be a base specifier, followed by '"' or '%'.
// The base must be valid.
// TODO: Implement vhdl 2008 standard
void Scanner::scanBitString(unsigned int baseLog) {
	assert(baseLog == 1 || baseLog == 3 || baseLog == 4);
	assert(currentContext.cChar() == '\"' || currentContext.cChar() == '%');
	auto startPosition = currentContext.Source.tellg();
	unsigned char mark = currentContext.cChar(); // Should be " or %
	int base = 1 << baseLog;
	std::string bitString = "";
	while (currentContext.cChar() != mark) {
		unsigned char c = currentContext.incChar();
		uint8_t val = 0;
		// LRM93 13.7
		// If the base specifier is 'B', the extended digits in the bit
		// value are restricted to 0 and 1.
		// If the base specifier is 'O', the extended digits int the bit
		// value are restricted to legal digits in the octal number
		// system, ie, the digits 0 through 7.
		// If the base specifier is 'X', the extended digits are all digits
		// together with the letters A through F.
		if (c >= '0' && c <= '9')
			val = c - '0';
		else if (c >= 'a' && c <= 'f')
			val = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			val = c - 'A' + 10;
		else if (c == '_') {
			if (currentContext.cChar() == '_')
				throw std::runtime_error("double underscore not allowed in a bit string");
			else if (currentContext.Source.tellg() == startPosition + 2)
				throw std::runtime_error("underscore not allowed at the start of a bit string");
			else if (currentContext.cChar() == mark)
				throw std::runtime_error("underscore not allowed at the end of a bit string");
			else
				continue;
		}
		else if (c == '%')
			throw std::runtime_error("% cannot close a bit string opened by \"");
		else if (c == '\"')
			throw std::runtime_error("\" cannot close a bit string opened by %");
		else {
			if (isGraphicCharacter(getCharacterKind(c))) {
				if (state.options.standard >= Vhdl_Std::Vhdl_08)
					val = 255;
				else
					throw std::runtime_error("invalid character in bit string");
			}
			else {
				if (mark == '%')
					throw std::runtime_error("% is not a vhdl operator, use rem");
				else
					throw std::runtime_error("bit string not terminated");
			}
		}

		// Expand bit value
		if (state.options.standard >= Vhdl_Std::Vhdl_08 && val == 255) {
			// Graphic Character
			for (int i = 0; i < baseLog; ++i)
				bitString.push_back(c);
		}
		else {
			switch (base) {
			case 2:
				if (val > base - 1)
					throw std::runtime_error("invalid character in a binary bit string");
				bitString.push_back(c);
			case 8:
				if (val > base - 1)
					throw std::runtime_error("invalid character in a binary bit string");
				bitString.push_back(val >> 2 ? '1' : '0');
				val &= ~(0x04);
				bitString.push_back(val >> 1 ? '1' : '0');
				val &= ~(0x02);
				bitString.push_back(val ? '1' : '0');
			case 16:
				bitString.push_back(val >> 3 ? '1' : '0');
				val &= ~(0x08);
				bitString.push_back(val >> 2 ? '1' : '0');
				val &= ~(0x04);
				bitString.push_back(val >> 1 ? '1' : '0');
				val &= ~(0x02);
				bitString.push_back(val ? '1' : '0');
			default:
				throw std::runtime_error("invalid base, NOTE: VHDL 2008 not implemented");
			}
		}
	}
	currentContext.token = Token::Bit_String;
	currentContext.Identifier = bitString;
}

// Scan a decimal bit string literal.  For base specifier D the algorithm
// is rather different: all the graphic characters shall be digits, and we
// need to use a (not very efficient) arbitrary precision multiplication.
void Scanner::scanDecimalBitString() {
	// NOTE: I have removed "%" as a marker here because I am not too sure whether it can be used
	// LRM2008 15.8 Bit string literals
	// A bit string literal is formed by a sequence of characters (possibly none) enclosed between two
	// quotation marks used as bit string brackets, preceded by a base specifier.
	assert(currentContext.cChar() == '\"');
	unsigned char mark = currentContext.cChar(); // Should be "
	int decimalString = 0;
	while (currentContext.cChar() != mark) {
		unsigned char c = currentContext.incChar();
		uint8_t val = 0;

		if (currentContext.cChar() >= '0' && currentContext.cChar() <= '9')
			val = currentContext.cChar() - '0';
		else if (currentContext.cChar() == '_') {
			if (currentContext.nextChar() == '_')
				throw std::runtime_error("double underscore not allowed in a bit string");
			else if (currentContext.decChar() == '\"' && currentContext.incChar() == '_')
				throw std::runtime_error("underscore not allowed at the start of a bit string");
			else if (currentContext.nextChar() == mark)
				throw std::runtime_error("underscore not allowed at the end of a bit string");
			else
				continue;
		}
		else if (isGraphicCharacter(getCharacterKind(c)))
			throw std::runtime_error("graphic character not allowed in decimal bit string");
		else
			throw std::runtime_error("bit string not terminated");

		decimalString *= 10;
		decimalString += val;
	}
	std::string revBinary;
	while (decimalString > 0) {
		revBinary.push_back(decimalString & 1 + '0');
		decimalString >>= 1;
	}
	currentContext.Identifier = "";
	for (char c: revBinary)
		currentContext.Identifier.push_back(c);
	currentContext.token = Token::Bit_String;
}

// LRM93 13.3.1
// Basic Identifiers
// A basic identifier consists only of letters, digits, and underlines.
// BASIC_IDENTIFIER ::= LETTER { [ UNDERLINE ] LETTER_OR_DIGIT }
// LETTER_OR_DIGIT ::= LETTER | DIGIT
// LETTER ::= UPPER_CASE_LETTER | LOWER_CASE_LETTER
//
// NB: At the call of this procedure, the current character must be a legal
// character for a basic identifier.
void Scanner::scanIdentifier() {
	// LRM93 13.3.1
	//  All characters if a basic identifier are signifiant, including
	//  any underline character inserted between a letter or digit and
	//  an adjacent letter or digit.
	//  Basic identifiers differing only in the use of the corresponding
	//  upper and lower case letters are considered as the same.
	// This is achieved by converting all upper case letters into
	// equivalent lower case letters.
	// The opposite (converting in lower case letters) is not possible,
	// because two characters have no upper-case equivalent.
	currentContext.Identifier = "";
	while (1) {
		bool exitLoop = false;
		unsigned char c = currentContext.cChar();
		if (c >= 'A' && c <= 'Z')
			c = Ada_Chars::toLower(c);
		else if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
		else if (c == '_') {
			if (currentContext.nextChar() == '_')
				throw std::runtime_error("two underscores can't be consecutive");
		}
		else if (c == ' ' || c == ')' || c == '.' || c == ';' || c == ':')
			break;
		else {
			// Uncommon cases
			switch (getCharacterKind(c)) {
			case CharacterKindType::Upper_Case_Letter:
			case CharacterKindType::Lower_Case_Letter:
				if (state.options.standard == Vhdl_Std::Vhdl_87)
					std::runtime_error("8 bits characters not allowed in vhdl87");
				c = Ada_Chars::toLower(c);
				break;
			case CharacterKindType::Digit:
				throw std::runtime_error("name this error better: this character is not allowed here");
			default:
				exitLoop = true;
			}
			if (exitLoop)
				break;
		}
		currentContext.Identifier += c;
		currentContext.nextChar();
	}

	if (currentContext.Identifier.back() == '_') {
		if (!Flag_Psl)
			// Some PSL reserved words finish with '_'.  This case is handled later.
			throw std::runtime_error("identifier cannot finish with '_'");
		currentContext.decChar();
		currentContext.Identifier.pop_back();
	}


	// LRM93 13.2
	// At least one separator is required between an identifier or an
	// abstract literal and an adjacent identifier or abstract literal.
	switch (getCharacterKind(currentContext.cChar())) {
	case CharacterKindType::Upper_Case_Letter:
	case CharacterKindType::Digit:
	case CharacterKindType::Lower_Case_Letter:
		throw std::logic_error("Should not happen");
	case CharacterKindType::Special_Character:
	case CharacterKindType::Other_Special_Character:
		if ((currentContext.cChar() == '\"' || currentContext.cChar() == '%')
				&& currentContext.Identifier.length() <= 2) {
			if (currentContext.cChar() == '%' && state.options.standard >= Vhdl_Std::Vhdl_08)
				throw std::runtime_error("'%%' not allowed in vhdl 2008 (was replacement character)");
			// Must be a bit string
			// Good candidate for bit string.

			// LRM93 13.7
			// BASE_SPECIFIER ::= B | O | X
			//
			// A letter in a bit string literal (either an extended digit or the base specifier) can be written
			// either in lower case or in upper case, with the same meaning.
			//
			// LRM08 15.8 Bit string literals
			// BASE_SPECICIER ::=
			//    B | O | X | UB | UO | UX | SB | SO | SX | D
			//
			// An extended digit and the base specifier in a bit string literal can be written either in lowercase or
			// in uppercase, with the same meaning.
			int base = 0;
			if (currentContext.Identifier.back() == 'b') base = 1;
			else if (currentContext.Identifier.back() == 'o') base = 3;
			else if (currentContext.Identifier.back() == 'x') base = 4;
			else if (state.options.standard >= Vhdl_Std::Vhdl_87 && currentContext.Identifier == "d") {
				scanDecimalBitString();
				return;
			}

			if (base > 0) {
				if (currentContext.Identifier.length() == 1) {
					scanBitString(base);
					return;
				}
				else if (state.options.standard >= Vhdl_Std::Vhdl_08
						&& (currentContext.Identifier.front() == 's' || currentContext.Identifier.front() == 'u')) {
					// NOTE: this u/s means nothing?
					scanBitString(Base);
					return;
				}
			}
			//NOTE: Is the earlier if ever false?

		}
		else if (state.options.standard >= Vhdl_Std::Vhdl_87 && currentContext.cChar() == '\\')
			throw std::runtime_error("a separator is required here");;
		break;
	case CharacterKindType::Invalid:
		//TODO: Currently I am just using going to be a asshole and crash the compiler here
		throw std::runtime_error("invalid use of UTF8 character, plz use latin-1 encoding, brah you got some  issues");
		/*
		// Improve error message for use of UTF-8 quote marks.
		// It's possible because in the sequence of UTF-8 bytes for the  quote marks, there are invalid character
		// (in the 128-160 range).
		if (currentContext.cChar() == 128 && currentContext.Identifier.back() == 226 &&
				(currentContext.nextChar() == 152 || currentContext.nextChar() == 153)) {
			// UTF-8 left or right single quote mark.
			if (currentContext.Identifier.size() > 1) {
				// The first byte (0xe2) is part of the identifier.  An  error will be detected as the next byte (0x80)
				// is invalid.  Remove the first byte from the identifier, and let's catch the error later.
				currentContext.Identifier.pop_back();
				currentContext.decChar();
			}
			else
				throw std::runtime_error ("invalid use of UTF8 character for '");
			Pos := Pos + 2;

// Distinguish between character literal and tick.  Don't
// care about possible invalid character literal, as in any
// case we have already emitted an error message.
			if currentContext.Prev_Token /= Token::Identifier
						and then currentContext.Prev_Token /= Token::Character
					and then
							(Source (Pos + 1) = '''
					or else
			(Source (Pos + 1) = Character'Val (16#e2#)
					and then Source (Pos + 2) = Character'Val (16#80#)
					and then Source (Pos + 3) = Character'Val (16#99#)))
			then
			currentContext.token := Token::Character;
			currentContext.Identifier :=
			Name_Table.Get_Identifier (Source (Pos));
			if Source (Pos + 1) = ''' then
			Pos := Pos + 2;
			else
			Pos := Pos + 4;
			end if;
			else
				currentContext.token := Token::Tick;
			end if;
			return;
			end if;


		}
		 */
		break;
	default:
		break;
	}

	currentContext.token = Token::Identifier;
	if (Keywords::isKeyword(currentContext.Identifier)) {
		// LRM93 13.9
		// The identifiers listed below are called reserved words and are
		// reserved for signifiances in the language.
		// IN: this is also achieved in packages std_names and tokens.
		currentContext.token =
				Token::Mod + (Keywords::KeywordTable.at(currentContext.Identifier) - Keywords::Name_First_Keyword);

		if (Keywords::isAMSReservedWord(currentContext.Identifier)) {
			if (!state.options.AMS_Vhdl)
				SyntaxWarning("using" + currentContext.Identifier + "AMS-VHDL reserved word as an identifier"
								+ currentContext.Identifier,
						state.options.Warnid_Reserved_Word);
			currentContext.token = Token::Identifier;
		}
		else if (Keywords::isVHDL08ReservedWord(currentContext.Identifier)) {
			if (state.options.standard < Vhdl_Std::Vhdl_08)
				SyntaxWarning("using" + currentContext.Identifier + "vhdl-2008 reserved word as an identifier",
						state.options.Warnid_Reserved_Word);
			currentContext.token = Token::Identifier;
		}
		else if (Keywords::isVHDL00ReservedWord(currentContext.Identifier)) {
			if (state.options.standard < Vhdl_Std::Vhdl_00)
				SyntaxWarning("using" + currentContext.Identifier + "vhdl-2000 reserved word as an identifier",
						state.options.Warnid_Reserved_Word);
			currentContext.token = Token::Identifier;
		}
		else if (Keywords::isVHDL93ReservedWord(currentContext.Identifier)) {
			if (state.options.standard < Vhdl_Std::Vhdl_93)
				SyntaxWarning("using" + currentContext.Identifier
								+ "vhdl93 reserved word as a vhdl87 identifier, (use option --std=93 to compile as vhdl93)",
						state.options.Warnid_Reserved_Word);
			currentContext.token = Token::Identifier;
		}
		else {
			assert(Keywords::isVHDL87ReservedWord(currentContext.Identifier));
		}
	}
	else if(Flag_Psl) {
		if (currentContext.Identifier == Keywords::Name_Clock)
			currentContext.token = Token::Psl_Clock;
		else if (currentContext.Identifier == Keywords::Name_Const)
			currentContext.token = Token::Psl_Const;
		else if (currentContext.Identifier == Keywords::Name_Boolean)
			currentContext.token = Token::Psl_Boolean;
		else if (currentContext.Identifier == Keywords::Name_Sequence)
			currentContext.token = Token::Psl_Sequence;
		else if (currentContext.Identifier == Keywords::Name_Property)
			currentContext.token = Token::Psl_Property;
		else if (currentContext.Identifier == Keywords::Name_Endpoint)
			currentContext.token = Token::Psl_Endpoint;
		else if (currentContext.Identifier == Keywords::Name_Cover)
			currentContext.token = Token::Psl_Cover;
		else if (currentContext.Identifier == Keywords::Name_Default)
			currentContext.token = Token::Psl_Default;
		else if (currentContext.Identifier == Keywords::Name_Inf)
			currentContext.token = Token::Inf;
		else if (currentContext.Identifier == Keywords::Name_Within)
			currentContext.token = Token::Within;
		else if (currentContext.Identifier == Keywords::Name_Abort)
			currentContext.token = Token::Abort;
		else if (currentContext.Identifier == Keywords::Name_Before)
			currentContext.token = Token::Before;
		else if (currentContext.Identifier == Keywords::Name_Always)
			currentContext.token = Token::Always;
		else if (currentContext.Identifier == Keywords::Name_Never)
			currentContext.token = Token::Never;
		else if (currentContext.Identifier == Keywords::Name_Eventually)
			currentContext.token = Token::Eventually;
		else if (currentContext.Identifier == Keywords::Name_Next_A)
			currentContext.token = Token::Next_A;
		else if (currentContext.Identifier == Keywords::Name_Next_E)
			currentContext.token = Token::Next_E;
		else if (currentContext.Identifier == Keywords::Name_Next_Event)
			currentContext.token = Token::Next_Event;
		else if (currentContext.Identifier == Keywords::Name_Next_Event_A)
			currentContext.token = Token::Next_Event_A;
		else if (currentContext.Identifier == Keywords::Name_Next_Event_E)
			currentContext.token = Token::Next_Event_E;
		else if (currentContext.Identifier == Keywords::Name_Until)
			currentContext.token = Token::Until;
		else {
			currentContext.token = Token::Identifier;
			//TODO: test what if this is there in one of the PSL Tokens
			if (currentContext.cChar() == '_')
				throw std::runtime_error("identifiers cannot finish with '_'");
		}
	}
	else
		currentContext.token = Token::Identifier;
}

// LRM93 13.3.2
// EXTENDED_IDENTIFIER ::= \ GRAPHIC_CHARACTER { GRAPHIC_CHARACTER } \
// Create an (extended) indentifier.
// Extended identifiers are stored as they appear (leading and tailing backslashes, doubling backslashes inside).
void Scanner::scanExtendedIdentifier() {
	// LRM93 13.3.2
	// Moreover, every extended identifiers is distinct from any basic
	// identifier. They all begin with a '\' in the name table.
	currentContext.Identifier = "\\";
	assert(currentContext.cChar() == '\\');
	currentContext.incChar();
	while (1) {
		if (currentContext.cChar() == '\\') {
			// LRM93 13.3.2
			// If a backslash is to be used as one of the graphic characters
			// of an extended literal, it must be doubled.
			// LRM93 13.3.2
			// (a doubled backslash couting as one character)
			currentContext.Identifier += '\\';
			if (currentContext.incChar() != '\\')
				break;
			continue;
		}
		else {
			auto kind = getCharacterKind(currentContext.cChar());
			if (kind == CharacterKindType::Format_Effector)
				throw std::runtime_error("format effector in extended identifier");
			else if (kind == CharacterKindType::Invalid)
				throw std::runtime_error("invalid character in extended identifier");
			currentContext.Identifier += currentContext.cChar();
			currentContext.incChar();
		}
	}
	if (currentContext.Identifier.size() == 2)
		throw std::runtime_error("empty extended identifier is not allowed");


	// LRM93 13.2
	// At least one separator is required between an identifier or an
	// abstract literal and an adjacent identifier or abstract literal.
	auto kind = getCharacterKind(currentContext.cChar());
	if (kind == CharacterKindType::Digit || kind == CharacterKindType::Upper_Case_Letter
			|| kind == CharacterKindType::Lower_Case_Letter)
		throw std::runtime_error("a separator is required here");;
	currentContext.token = Token::Identifier;
}

// TODO: Improve error messages
bool Scanner::convertIdentifier(std::string& input, Vhdl_Std standard) {
	if (input[0] == '\\') {
		// Extended Identifier
		if (standard == Vhdl_Std::Vhdl_87)
			throw std::runtime_error("extended identifiers not allowed in vhdl87");

		if (input.size() < 3)
			throw std::runtime_error("extended identifier is too short");

		if (input.back() != '\\')
			throw std::runtime_error("extended identifier must finish with a '\'");

		for (unsigned int i = 1; i < input.size() - 1; ++i) {
			auto kind = getCharacterKind(static_cast<unsigned char>(input[i]));
			if (isGraphicCharacter(kind)) {
				if (input[i] == '\\') {
					if (input[i + 1] != '\\' || i == input.size() - 2)
						throw std::runtime_error("anti-slash must be doubled in extended identifier");
				}
			}
			else if (kind == CharacterKindType::Format_Effector)
				throw std::runtime_error("format effector in extended identifier");
			else
				throw std::runtime_error("bad character in identifier");
		}

	}
	else {
		// Normal Identifier
		for (unsigned int i = 0; i < input.size(); ++i) {
			unsigned char c = input[i];
			switch (getCharacterKind(c)) {
			case CharacterKindType::Upper_Case_Letter:
				if (standard == Vhdl_Std::Vhdl_87 && c > 'Z')
					throw std::runtime_error("8 bits characters not allowed in vhdl87");
				input[i] = std::tolower(c);
				break;
			case CharacterKindType::Lower_Case_Letter:
			case CharacterKindType::Digit:
				if (standard == Vhdl_Std::Vhdl_87 && c > 'z')
					throw std::runtime_error("8 bits characters not allowed in vhdl87");
				break;
			case CharacterKindType::Special_Character:
				if (c != '_') {
					if (i == 0)
						throw std::runtime_error("identifier cannot start with an underscore");
					if (input[i - 1] == '_')
						throw std::runtime_error("two underscores can't be consecutive");
					if (i == input.size() - 1)
						throw std::runtime_error("identifier cannot finish with an underscore");
				}
				else
					throw std::runtime_error("bad character in identifier");
				break;
			default:
				throw std::runtime_error("bad character in identifier");
			}
		}
	}
}

// Scan an identifier within a comment.  Only lower case letters are allowed.
bool Scanner::scanCommentIdentifier() {
	currentContext.Identifier = "";
	// ' ' or HT
	while (currentContext.cChar() == ' ' || currentContext.cChar() == 9)
		currentContext.incChar();
	// The identifier shall start with a lower case letter.
	if (currentContext.cChar() < 'a' || currentContext.cChar() > 'z')
		return false;
	// Scan the identifier (in lower cases).
	while (currentContext.cChar() >= 'a' && currentContext.cChar() <= 'z' && currentContext.cChar() != '_') {
		currentContext.Identifier = currentContext.cChar();
		currentContext.incChar();
	}

	return !(currentContext.cChar() != ' ' || currentContext.cChar() != 9 || currentContext.cChar() != 10 ||
			currentContext.cChar() != 13);
};

// Scan tokens within a comment.  Return TRUE if currentContext.token was set,
// return FALSE to discard the comment (ie treat it like a real comment).
bool Scanner::scanComment() {
	if (!scanCommentIdentifier())
		return false;

	if (currentContext.Identifier == Keywords::Name_Psl) {
		Flag_Psl = true;
		Flag_Scan_In_Comment = true;
		return true;
	}
	return false;

}

inline unsigned int extendedDigit(unsigned char c) {
	if ( c - '0' > 0 && c - '0' < 10)
		return c - '0';
	else if ( Ada_Chars::toLower(c) - 'a' < 6 )
		return static_cast<unsigned int>(Ada_Chars::toLower(c) - 'a' + 10);
	else
		throw std::runtime_error("Extended digit can only be between 0 to f");
}


// LRM 13.4.1
// INTEGER ::= DIGIT { [ UNDERLINE ] DIGIT }
// The first character must be a digit.
int Scanner::scanBasedInteger(int base = 10) {
	assert(extendedDigit(currentContext.cChar()) < base);
	int num = 0;
	while(currentContext.cChar() != '_' && Ada_Chars::isExtendedDigit(currentContext.cChar())){
		if (currentContext.cChar() == '_') {
			if (!Ada_Chars::isExtendedDigit(currentContext.nextChar()))
				throw std::runtime_error("underscore must be followed by a digit");
		}
		else {
			auto val = extendedDigit(currentContext.cChar());
			if (val > base)
				throw std::runtime_error("digit beyond base");
			num = num*base + val;
			currentContext.incChar();
		}
	}
	return num;
}



// scan a decimal literal or a based literal.
//
// LRM93 13.4.1
// DECIMAL_LITERAL ::= INTEGER [ . INTEGER ] [ EXPONENT ]
// EXPONENT ::= E [ + ] INTEGER | E - INTEGER
//
// LRM93 13.4.2
// BASED_LITERAL ::= BASE # BASED_INTEGER [ . BASED_INTEGER ] # EXPONENT
// BASE ::= INTEGER
void Scanner::scanLiteral() {
	// Can be integer or base
	int num_1 = scanBasedInteger(10);

	if (currentContext.cChar() == '.') {
		// Decimal Integer
		currentContext.incChar();
		if (!Ada_Chars::isExtendedDigit(currentContext.cChar()) || extendedDigit(currentContext.cChar()) > 10)
			throw std::runtime_error("a dot must be followed by a digit");
		int num_2 = scanBasedInteger(10);
	}

}


void Scanner::scan() {
	if (currentContext.token != Token::Invalid)
		currentContext.prevToken = currentContext.token;

	while (1) {
		//NOTE: Can possibly skip whitespace earlier to speed stuff up
		currentContext.Identifier = 0;

		switch (currentContext.cChar()) {
		case 32: //Space
		case 9: //HT
		case 11: //VT
		case 12: //FF
			break;
		case 160://NSBP
			if (state.options.standard == Vhdl_Std::Vhdl_87)
				throw std::runtime_error("NBSP character not allowed in vhdl87");
			break;
		case 10: //LF
			scanLFCR();
			currentContext.token = Token::Newline;
			return;
		case 13: //CR
			scanCRLF();
			currentContext.token = Token::Newline;
			return;
		case '-' :
			if (currentContext.incChar() == '-') {
				// This is a comment.
				// LRM93 13.8
				// A comment starts with two adjacent hyphens and extends up
				// to the end of the line.
				// A comment can appear on any line line of a VHDL
				// description.
				// The presence or absence of comments has no influence on
				// wether a description is legal or illegal.
				// Futhermore, comments do not influence the execution of a
				// simulation module; their sole purpose is the enlightenment
				// of the human reader.
				// GHDL note: As a consequence, an obfruscating comment
				// is out of purpose, and a warning could be reported :-)
				currentContext.incChar();

				// Scan inside a comment.  So we just ignore the two dashes.
				if (Flag_Scan_In_Comment)
					break;

					// Handle keywords in comment (PSL).
				else if (state.options.Flag_Comment_Keyword && scanComment())
					break;

				// LRM93 13.2
				// In any case, a sequence of one or more format
				// effectors other than horizontal tabulation must
				// cause at least one end of line.
				// HT -> 9
				while (getCharacterKind(currentContext.incChar()) != CharacterKindType::Format_Effector &&
						currentContext.cChar() != 9) {
					if (!state.options.Mb_Comment
							&& getCharacterKind(currentContext.cChar()) == CharacterKindType::Invalid)
						throw std::runtime_error("invalid character, even in a comment");
				}
				if (Flag_Comment) {
					currentContext.token = Token::Comment;
					return;
				}
				break;
			}
			else if (Flag_Psl && currentContext.cChar() == '>') {
				currentContext.token = Token::Minus_Greater;
				currentContext.incChar();
				return;
			}
			else {
				currentContext.token = Token::Minus;
				return;
			}
		case '+':
			currentContext.token = Token::Plus;
			currentContext.incChar();
			return;
		case '*':
			if (currentContext.incChar() == '*') {
				currentContext.token = Token::Double_Star;
				currentContext.incChar();
			}
			else
				currentContext.token = Token::Star;
			return;
		case '/':
			if (currentContext.incChar() == '=') {
				currentContext.token = Token::Not_Equal;
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '*') {
				// LRM08 15.9 Comments
				// A delimited comment start with a solidus (slash) character
				// immediately followed by an asterisk character and extends up
				// to the first subsequent occurrence of an asterisk character
				// immediately followed by a solidus character.
				if (state.options.standard < Vhdl_Std::Vhdl_08)
					throw std::runtime_error("block comment are not allowed before vhdl 2008");
				currentContext.incChar();
				bool endLoop = false;
				while (!endLoop) {
					switch (currentContext.cChar()) {
					case '/':
						// LRM08 15.9
						// Moreover, an occurrence of a solidus character
						// immediately followed by an asterisk character
						// within a delimited comment is not interpreted as
						// the start of a nested delimited comment.
						if (currentContext.incChar() == '*') {
							SyntaxWarning("'/*' found within a block comment", state.options.Warnid_Nested_Comment);
						}
						break;
					case '*':
						if (currentContext.incChar() == '/') {
							currentContext.incChar();
							endLoop = true;
						}
						break;
					case 10: //CR
						scanCRLF();
						break;
					case 13:
						scanLFCR();
						break;
						// NOTE: add a flag for non terminating block comment at eof
						// Error_Msg_Scan(Get_Token_Location, "block comment not terminated at end of file");
					default:
						currentContext.incChar();
					}
				}
				if (Flag_Comment) {
					currentContext.token = Token::Comment;
					return;
				}
				break;
			}
			else {
				currentContext.token = Token::Slash;
				return;
			}

		case '(':
			currentContext.token = Token::Left_Paren;
			currentContext.incChar();
			return;
		case ')':
			currentContext.token = Token::Right_Paren;
			currentContext.incChar();
			return;
		case '|':
			if (Flag_Psl) {
				if (currentContext.incChar() == '|') {
					currentContext.token = Token::Bar_Bar;
					currentContext.incChar();
				}
				else if (currentContext.cChar() == '-' && currentContext.nextChar() == '>') {
					currentContext.token = Token::Bar_Arrow;
					currentContext.incChar();
					currentContext.incChar();
				}
				else if (currentContext.cChar() == '=' && currentContext.nextChar() == '>') {
					currentContext.token = Token::Bar_Double_Arrow;
					currentContext.incChar();
					currentContext.incChar();
				}
				else {
					currentContext.token = Token::Bar;
				}
				return;
			}
			else {
				currentContext.token = Token::Bar;
			}
			return;
		case '!':
			if (Flag_Psl) {
				currentContext.token = Token::Exclam_Mark;
			}
			else {
				// LRM93 13.10
				// A vertical line (|) can be replaced by an exclamation mark (!)  where used as a delimiter.
				currentContext.token = Token::Bar;
			}
			currentContext.incChar();
			return;
		case ':':
			if (currentContext.incChar() == '=') {
				currentContext.token = Token::Assign;
			}
			else {
				currentContext.token = Token::Colon;
			}
			return;
		case ';':
			currentContext.token = Token::Semi_Colon;
			currentContext.incChar();
			return;
		case ',':
			currentContext.token = Token::Comma;
			currentContext.incChar();
			return;
		case '.':
			if (currentContext.incChar() == '.') {
				// Be Ada friendly...
				throw std::runtime_error("'..' is invalid in vhdl, replaced by 'to'");
				currentContext.token = Token::To;
				currentContext.incChar();
			}
			else {
				currentContext.token = Token::Dot;
			}
			return;
		case '&':
			if (Flag_Psl && currentContext.incChar() == '&') {
				currentContext.token = Token::And_And;
				currentContext.incChar();
			}
			else {
				currentContext.token = Token::Ampersand;
			}
			return;
		case '<':
			switch (currentContext.incChar()) {
			case '=':
				currentContext.token = Token::Less_Equal;
				currentContext.incChar();
				break;
			case '>':
				currentContext.token = Token::Box;
				currentContext.incChar();
				break;
			case '<':
				currentContext.token = Token::Double_Less;
				currentContext.incChar();
				break;
			default:
				currentContext.token = Token::Less;
				break;
			}
			return;
		case '>':
			switch (currentContext.incChar()) {
			case '=':
				currentContext.token = Token::Greater_Equal;
				currentContext.incChar();
				break;
			case '>':
				currentContext.token = Token::Double_Greater;
				currentContext.incChar();
				break;
			default:
				currentContext.token = Token::Greater;
				break;
			}
			return;
		case '=':
			if (currentContext.incChar() == '=') {
				if (state.options.AMS_Vhdl)
					currentContext.token = Token::Equal_Equal;
				else
					throw std::runtime_error("'==' is not the vhdl equality, please replace by '='");
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '>') {
				currentContext.token = Token::Double_Arrow;
				currentContext.incChar();
			}
			else {
				currentContext.token = Token::Equal;
			}
			return;
		case '\'':
			currentContext.incChar();
			// Handle cases such as character'('a')
			// FIXME: what about f ()'length ? or .all'length
			if (currentContext.prevToken != Token::Identifier && currentContext.prevToken != Token::Character
					&& currentContext.nextChar() == '\'') {
				// LRM93 13.5
				// A character literal is formed by enclosing one of the 191
				// graphic character (...) between two apostrophe characters.
				// CHARACTER_LITERAL ::= ' GRAPHIC_CHARACTER '
				if (!isGraphicCharacter(getCharacterKind(currentContext.cChar())))
					throw std::runtime_error("a character literal can only be a graphic character");
				else if (currentContext.cChar() > 127 && state.options.standard == Vhdl_Std::Vhdl_87)
					throw std::runtime_error("extended identifiers not allowed in vhdl87");
				else {
					currentContext.token = Token::Character;
					// TODO: Fix this name table business
					currentContext.Identifier = currentContext.cChar();
				}
				currentContext.nextChar();
				currentContext.nextChar();
				return;
			}
			currentContext.token = Token::Tick;
			return;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			scanLiteral();

			// LRM93 13.2
			// At least one separator is required between an identifier or
			// an abstract literal and an adjacent identifier or abstract
			// literal.
			switch (getCharacterKind(currentContext.cChar())) {
			case CharacterKindType::Digit:
				throw std::runtime_error("Internal error src/vhdl/scanner.adb: 1721");
			case CharacterKindType::Upper_Case_Letter:
			case CharacterKindType::Lower_Case_Letter:
				// Could call throw std::runtime_error ("a separator is required here");, but use a clearer message
				// for this common case.
				// Note: the term "unit name" is not correct here, since
				// it can be any identifier or even a keyword; however it
				// is probably the most common case (eg 10ns).
				if (state.options.standard >= Vhdl_Std::Vhdl_08 && currentContext.token == Token::Integer)
					currentContext.token = Token::Integer_Letter;
				else
					throw std::runtime_error("space is required between number and unit name");
				break;
			case CharacterKindType::Other_Special_Character:
				if (state.options.standard > Vhdl_Std::Vhdl_87 && currentContext.cChar() == '\'')
					// Start of extended identifier.
					throw std::runtime_error("a separator is required here");;
				break;
			default:
				break;
			}
			return;
		case '#':
			throw std::runtime_error("'#' is used for based literals and must be preceded by a base");
		case '"':
			scanString();
			return;
		case '%':
			if (state.options.standard >= Vhdl_Std::Vhdl_08) // Continue as a string.????
				throw std::runtime_error("'%%' not allowed in vhdl 2008 (was replacement character)");
			scanString();
			return;
		case '[':
			if (Flag_Psl) {
				if (currentContext.incChar() == '*') {
					currentContext.token = Token::Brack_Star;
					currentContext.incChar();
				}
				else if (currentContext.cChar() == '+' && currentContext.nextChar() == ']') {
					currentContext.token = Token::Brack_Plus_Brack;
					currentContext.incChar();
					currentContext.incChar();
				}
				else if (currentContext.cChar() == '-' && currentContext.nextChar() == '>') {
					currentContext.token = Token::Brack_Arrow;
					currentContext.incChar();
					currentContext.incChar();
				}
				else if (currentContext.incChar() == '=') {
					currentContext.token = Token::Brack_Equal;
					currentContext.incChar();
				}
				else
					currentContext.token = Token::Left_Bracket;
				return;
			}
			else {
				if (state.options.standard == Vhdl_Std::Vhdl_87)
					throw std::runtime_error("'[' is an invalid character in vhdl87, replaced by '('");
				currentContext.token = Token::Left_Bracket;
				currentContext.incChar();
				return;
			}
		case ']':
			if (state.options.standard == Vhdl_Std::Vhdl_87 && !Flag_Psl)
				throw std::runtime_error("']' is an invalid character in vhdl87, replaced by ')'");
			currentContext.token = Token::Right_Bracket;
			currentContext.incChar();
			return;
		case '{':
			if (!Flag_Psl)
				throw std::runtime_error("'{' is an invalid character, replaced by '('");
			currentContext.token = Token::Left_Curly;
			currentContext.incChar();
			return;
		case '}':
			if (!Flag_Psl)
				throw std::runtime_error("'}' is an invalid character, replaced by ')'");
			currentContext.token = Token::Right_Curly;
			currentContext.incChar();
			return;
		case '\'':
			if (state.options.standard == Vhdl_Std::Vhdl_87)
				throw std::runtime_error("extended identifiers are not allowed in vhdl87");
			scanExtendedIdentifier();
			return;
		case '^':
			if (state.options.standard >= Vhdl_Std::Vhdl_08)
				currentContext.token = Token::Caret;
			else
				throw std::runtime_error("'^' is not a VHDL operator, use 'xor'");
			currentContext.incChar();
			return;
		case '~':
			throw std::runtime_error("'~' is not a VHDL operator, use 'not'");
		case '?':
			if (state.options.standard < Vhdl_Std::Vhdl_08)
				throw std::runtime_error("'?' can only be used in strings or comments");
			if (currentContext.incChar() == '<') {
				if (currentContext.nextChar() == '=') {
					currentContext.token = Token::Match_Less_Equal;
					currentContext.incChar();
				}
				else
					currentContext.token = Token::Match_Less;
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '>') {
				if (currentContext.nextChar() == '=') {
					currentContext.token = Token::Match_Greater_Equal;
					currentContext.incChar();
				}
				else
					currentContext.token = Token::Match_Greater;
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '?') {
				currentContext.token = Token::Condition;
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '=') {
				currentContext.token = Token::Match_Equal;
				currentContext.incChar();
			}
			else if (currentContext.cChar() == '/' && currentContext.nextChar() == '=') {
				currentContext.token = Token::Match_Not_Equal;
				currentContext.incChar();
				currentContext.incChar();
			}
			else
				throw std::runtime_error("unknown matching operator");
			return;
		case '$':
		case '`':
			//Inverted_Exclamation .. Inverted_Question
		case 161:
		case 162:
		case 163:
		case 164:
		case 165:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
		case 179:
		case 180:
		case 181:
		case 182:
		case 183:
		case 184:
		case 185:
		case 186:
		case 187:
		case 188:
		case 189:
		case 190:
		case 191:
		case 215: // Multiplication_Sign
		case 247: // Division_Sign
			throw std::runtime_error("character %c can only be used in strings or comments");
		case '@':
			if (state.options.standard >= Vhdl_Std::Vhdl_08 || Flag_Psl) {
				currentContext.token = Token::Arobase;
				currentContext.incChar();
				return;
			}
			else
				throw std::runtime_error("character %c can only be used in strings or comments");
		case '_':
			throw std::runtime_error("an identifier can't start with '_'");
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			scanIdentifier();
			return;
			// UC_A_Grave(192) .. UC_O_Diaeresis (214)
		case 192:
		case 193:
		case 194:
		case 195:
		case 196:
		case 197:
		case 198:
		case 199:
		case 200:
		case 201:
		case 202:
		case 203:
		case 204:
		case 205:
		case 206:
		case 207:
		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
			//UC_O_Oblique_Stroke(216) .. UC_Icelandic_Thorn(222)
		case 216:
		case 217:
		case 218:
		case 219:
		case 220:
		case 221:
		case 222:
			if (state.options.standard == Vhdl_Std::Vhdl_87)
				throw std::runtime_error("upper case letters above 128 are not allowed in vhdl87");
			//LC_German_Sharp_S(223) .. LC_O_Diaeresis(246)
		case 223:
		case 224:
		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:
		case 233:
		case 234:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
		case 240:
		case 241:
		case 242:
		case 243:
		case 244:
		case 245:
		case 246:
			//LC_O_Oblique_Stroke(248) .. LC_Y_Diaeresis(255) =>
		case 248:
		case 249:
		case 250:
		case 251:
		case 252:
		case 253:
		case 254:
		case 255:
			if (state.options.standard == Vhdl_Std::Vhdl_87)
				throw std::runtime_error("lower case letters above 128 are not allowed in vhdl87");
			// NUL(0) .. ETX(3) | ENQ(5) .. BS(8) | SO(14) .. US(31) | DEL(127) .. APC(159)
		case 0:
		case 1:
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 127:
		case 128:
		case 129:
		case 130:
		case 131:
		case 132:
		case 133:
		case 134:
		case 135:
		case 136:
		case 137:
		case 138:
		case 139:
		case 140:
		case 141:
		case 142:
		case 143:
		case 144:
		case 145:
		case 146:
		case 147:
		case 148:
		case 149:
		case 150:
		case 151:
		case 152:
		case 153:
		case 154:
		case 155:
		case 156:
		case 157:
		case 158:
		case 159:
			throw std::runtime_error("control character that is not CR, LF, FF, HT or VT is not allowed");
		case 4: //EOT
			// FIXME: should conditionnaly emit a warning if the file is not terminated by an end of line.
			// FIXME: should return Eof token? or nextChar should handle it??
			// currentContext.token = Token::Eof;
			throw std::runtime_error("EOT is not allowed inside the file");
		}
		currentContext.incChar();
	}

}

/*
function Get_Token_Location return Location_Type is
begin
return File_Pos_To_Location
(currentContext.Source_File, currentContext.Token_Pos);
end Get_Token_Location;
*/


inline bool Scanner::isWhitespace(char c) {
	// whitespace and nbsp
	return (c == 32 || state.options.standard > Vhdl_Std::Vhdl_87 && c == 160);
}

inline void Scanner::scanLFCR() {
	// Eat both LF and LF+CR
	if (currentContext.incChar() == 13)
		currentContext.incChar();
	currentContext.newLine();
}

inline void Scanner::scanCRLF() {
	// Eat both CR and CR+LF
	if (currentContext.incChar() == 10)
		currentContext.incChar();
	currentContext.newLine();
}