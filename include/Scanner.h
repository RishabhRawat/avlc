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
#ifndef AVLC_SCANNER_H
#define AVLC_SCANNER_H

#include "Token.h"
#include "State.h"

class Scanner {

	State state;

	void invalidateCurrentToken();

	// When CURRENT_TOKEN is an tok_identifier, tok_char or tok_string,
	// its name_id can be got via this function.
	Name_Id currentIdentifier();

	// Get current string identifier and length.
	function Current_String_Id return String8_Id;
	function Current_String_Length return Nat32;

	// Set Current_identifier to null_identifier.
	// Can be used to catch bugs.
	void invalidateCurrentIdentifier();

	// When CURRENT_TOKEN is tok_integer, returns the value.
	// When CURRENT_TOKEN is tok_bit_string, returns the log of the base.
	Iir_Int64 currentIirInt64() ;

	// When CURRENT_TOKEN is tok_real, it returns the value.
	Iir_Fp64 currentIirFp64() ;

	// Advances the lexical analyser.  Put a new token into current_token.
	void scan();

	// Initialize the Scanner with file SOURCE_FILE.
	void setFile (Source_File : Source_File_Entry);

	// This function can be called just after Set_File to detect UTF BOM
	// patterns.  It reports an error if a BOM is present and return True.
	// Silently return False if no error detected.
	function Detect_Encoding_Errors return Boolean;

	procedure Set_Current_Position (Position: Source_Ptr);

// Finalize the Scanner.
	procedure Close_File;

	// If true comments are reported as a token.
	bool Flag_Comment = false;

	// If true newlines are reported as a token.
	bool Flag_Newline = false;

	// If true also scan PSL tokens.
	bool Flag_Psl = false;

	// If true handle PSL embedded in comments: '//  psl' is ignored.
	bool Flag_Psl_Comment = false;

	// If true, ignore '//'.  This is automatically set when Flag_Psl_Comment
	// is true and a starting PSL keyword has been identified.
	// Must be reset to false by the parser.
	bool Flag_Scan_In_Comment = false;

	// If true scan for keywords in comments.  Must be enabled if
	// Flag_Psl_Comment is true.
	bool Flag_Comment_Keyword = false;

	// If the next character is '!', eat it and return True, otherwise return
	// False (used by PSL).
	function Scan_Exclam_Mark return Boolean;

	// If the next character is '_', eat it and return True, otherwise return
	// False (used by PSL).
	function Scan_Underscore return Boolean;

// Get the current location, or the location of the current token.
// Since a token cannot spread over lines, file and line of the current
// token are the same as those of the current position.
	function Get_Current_Source_File return Source_File_Entry;
	function Get_Current_Line return Natural;
	function Get_Current_Column return Natural;
	function Get_Token_Location return Location_Type;
	function Get_Token_Column return Natural;
	function Get_Token_Position return Source_Ptr;
	function Get_Position return Source_Ptr;

	// Convert (canonicalize) an identifier stored in name_buffer/name_length.
	// Upper case letters are converted into lower case.
	// Lexical checks are performed.
	// This procedure is not used by Scan, but should be used for identifiers given in the command line.
	// Errors are directly reported through error_msg_option. Also, Vhdl_Std should be set.
	static bool convertIdentifier(std::string& input, Vhdl_Std standard);

private:
	void scanBitString(unsigned int baseLog);
	void scanDecimalBitString();
	void scanIdentifier();

	// Return TRUE iff C is a whitespace.
	// LRM93 13.2 Lexical elements, separators, and delimiters
	// A space character (SPACE or NBSP) ...
	inline bool isWhitespace (char c);

	//Scans both CR and CRLF
	inline void scanCRLF();

	//Scans both LF and LFCR
	inline void scanLFCR();

	bool scanComment();

	bool scanCommentIdentifier();

	void scanExtendedIdentifier();

	void scanLiteral();

	int scanInteger();

	int scanInteger(int base);

	int scanBasedInteger(int base);
};
namespace Ada_Chars {
inline unsigned char toLower(unsigned char c) const {
	if (c >= 'A' && c <= 'Z')
		return c + ('a' - 'A');
	else if (c >= 192 && c <= 214)
		return static_cast<unsigned char>(c + (224 - 192));
	else if (c >= 216 && c <= 222)
		return static_cast<unsigned char>(c + (248 - 216));
	else
		return c;
}

inline bool isExtendedDigit(unsigned char c) const {
	return (( c <= '9' && c >= '0') || ( c <= 'f' && c >= 'a') || ( c <= 'F' && c > 'A'));
}
}
#endif //AVLC_SCANNER_H
