/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVLC_TOKENS_H
#define AVLC_TOKENS_H

#include <string>
#include <unordered_set>
#include "types.h"

namespace Token {
enum Token {
	Invalid,     // current_token is not valid.

	Left_Paren,          // (
	Right_Paren,         // )
	Left_Bracket,        // [
	Right_Bracket,       // ]
	Colon,               // :
	Semi_Colon,          // ;
	Comma,               // ,
	Double_Arrow,        // =>
	Tick,                // '
	Double_Star,         // **
	Assign,              // :=
	Bar,                 // |
	Box,                 // <>
	Dot,                 // .

	Equal_Equal,         // == (AMS Vhdl)

	Eof,                 // End of file.
	Newline,
	Comment,
	Character,
	Identifier,
	Integer,
	Real,
	String,

	//  This token corresponds to a base specifier followed by bit_value.
	//  The base specifier is stored in Name_Buffer/Name_Length like an
	//  identifier (in lowercase), the String8_Id contains the expanded bit
	//  value.
	Bit_String,

	//  An integer immediately followed by a letter.  This is used by to
	//  scan vhdl 2008 (and later) bit string with a length.
	Integer_Letter,

	// relational_operator
	Equal,               // =
	Not_Equal,           // /=
	Less,                // <
	Less_Equal,          // <=
	Greater,             // >
	Greater_Equal,       // >=

	Match_Equal,         // ?=
	Match_Not_Equal,     // ?/=
	Match_Less,          // ?<
	Match_Less_Equal,    // ?<=
	Match_Greater,       // ?>
	Match_Greater_Equal, // ?>=

	// sign token
	Plus,                // +
	Minus,               // -
	// and adding_operator
	Ampersand,           // &

	//  VHDL 2008
	Condition,           // ??
	Double_Less,         // <<
	Double_Greater,      // >>
	Caret,               // ^

	//  PSL
	And_And,             // &&
	Bar_Bar,             // ||
	Left_Curly,          // {
	Right_Curly,         // }
	Exclam_Mark,         // !
	Brack_Star,          // [*
	Brack_Plus_Brack,    // [+]
	Brack_Arrow,         // [->
	Brack_Equal,         // [=
	Bar_Arrow,           // |->
	Bar_Double_Arrow,    // |=>
	Minus_Greater,       // ->
	Arobase,             // @

	// multiplying operator
	Star,                // *
	Slash,               // /
	Mod,                 // mod
	Rem,                 // rem

	// relation token:
	And,
	Or,
	Xor,
	Nand,
	Nor,

	//  miscellaneous operator
	Abs,
	Not,

	// Key words
	Access,
	After,
	Alias,
	All,
	Architecture,
	Array,
	Assert,
	Attribute,

	Begin,
	Block,
	Body,
	Buffer,
	Bus,

	Case,
	Component,
	Configuration,
	Constant,

	Disconnect,
	Downto,

	Else,
	Elsif,
	End,
	Entity,
	Exit,

	File,
	For,
	Function,

	Generate,
	Generic,
	Guarded,

	If,
	In,
	Inout,
	Is,

	Label,
	Library,
	Linkage,
	Loop,

	Map,

	New,
	Next,
	Null,

	Of,
	On,
	Open,
	Others,
	Out,

	Package,
	Port,
	Procedure,
	Process,

	Range,
	Record,
	Register,
	Report,
	Return,

	Select,
	Severity,
	Signal,
	Subtype,

	Then,
	To,
	Transport,
	Type,

	Units,
	Until,
	Use,

	Variable,

	Wait,
	When,
	While,
	With,

	// Tokens below this line are key words in vhdl93 but not in vhdl87
	Xnor,
	Group,
	Impure,
	Inertial,
	Literal,
	Postponed,
	Pure,
	Reject,
	Shared,
	Unaffected,

	// shift_operator
	Sll,
	Sla,
	Sra,
	Srl,
	Rol,
	Ror,

	//  Added by Vhdl 2000:
	Protected,

	//  Added by vhdl 2008:
	Context,
	Parameter,

	//  AMS reserved words
	Across,
	Break,
	Limit,
	Nature,
	Noise,
	Procedural,
	Quantity,
	Reference,
	Spectrum,
	Subnature,
	Terminal,
	Through,
	Tolerance,

	// PSL words
	Psl_Default,
	Psl_Clock,
	Psl_Property,
	Psl_Sequence,
	Psl_Endpoint,
	Psl_Cover,

	Psl_Const,
	Psl_Boolean,
	Inf,

	Within,
	Abort,
	Before,
	Always,
	Never,
	Eventually,
	Next_A,
	Next_E,
	Next_Event,
	Next_Event_A,
	Next_Event_E

};

/*
subtype Token_Relational_Operator_Type is Token_Type range
		Tok_Equal .. Tok_Match_Greater_Equal;
subtype Token_Shift_Operator_Type is Token_Type range
		Tok_Sll .. Tok_Ror;
subtype Token_Sign_Type is Token_Type range
		Tok_Plus .. Tok_Minus;
subtype Token_Adding_Operator_Type is Token_Type range
		Tok_Plus .. Tok_Ampersand;
subtype Token_Multiplying_Operator_Type is Token_Type range
		Tok_Star .. Tok_Rem;
*/

//Tok_First_Keyword :  constant Tokens.Token_Type := Tokens.Tok_Mod;

// Return the name of the token.
std::string image(Token token) {
	switch (token) {
	case Token::Invalid :
		return "<invalid>";
	case Token::Left_Paren :
		return "(";
	case Token::Right_Paren :
		return ")";
	case Token::Left_Bracket :
		return "[";
	case Token::Right_Bracket :
		return "]";
	case Token::Colon :
		return ":";
	case Token::Semi_Colon :
		return ";";
	case Token::Comma :
		return ",";
	case Token::Tick :
		return "'";
	case Token::Double_Star :
		return "**";
	case Token::Double_Arrow :
		return ":";
	case Token::Assign :
		return ":=";
	case Token::Bar :
		return "|";
	case Token::Box :
		return "<>";
	case Token::Dot :
		return ".";

	case Token::Eof :
		return "<EOF>";
	case Token::Newline :
		return "<newline>";
	case Token::Comment :
		return "<comment>";
	case Token::Character :
		return "<character>";
	case Token::Identifier :
		return "<identifier>";
	case Token::Integer:
	case Token::Integer_Letter :
		return "<integer>";
	case Token::Real :
		return "<real>";
	case Token::String :
		return "<string>";
	case Token::Bit_String :
		return "<bit string>";

	case Token::Equal_Equal :
		return "==";

		//relational_operator:
	case Token::Equal :
		return "=";
	case Token::Not_Equal :
		return "/=";
	case Token::Less :
		return "<";
	case Token::Less_Equal :
		return "<=";
	case Token::Greater :
		return ">";
	case Token::Greater_Equal :
		return ">=";

	case Token::Match_Equal :
		return "?=";
	case Token::Match_Not_Equal :
		return "?/=";
	case Token::Match_Less :
		return "?<";
	case Token::Match_Less_Equal :
		return "?<=";
	case Token::Match_Greater :
		return "?>";
	case Token::Match_Greater_Equal :
		return "?>=";

		//sign token
	case Token::Plus :
		return "+";
	case Token::Minus :
		return "-";
		// and adding_operator
	case Token::Ampersand :
		return "&";

	case Token::Condition :
		return "??";

	case Token::Double_Less :
		return "<<";
	case Token::Double_Greater :
		return ">>";
	case Token::Caret :
		return "^";

		//multiplying operator
	case Token::Star :
		return "*";
	case Token::Slash :
		return "/";
	case Token::Mod :
		return "mod";
	case Token::Rem :
		return "rem";

		//relation token:
	case Token::And :
		return "and";
	case Token::Or :
		return "or";
	case Token::Xor :
		return "xor";
	case Token::Nand :
		return "nand";
	case Token::Nor :
		return "nor";
	case Token::Xnor :
		return "xnor";

		//Reserved words.
	case Token::Abs :
		return "abs";
	case Token::Access :
		return "access";
	case Token::After :
		return "after";
	case Token::Alias :
		return "alias";
	case Token::All :
		return "all";
	case Token::Architecture :
		return "architecture";
	case Token::Array :
		return "array";
	case Token::Assert :
		return "assert";
	case Token::Attribute :
		return "attribute";

	case Token::Begin :
		return "begin";
	case Token::Block :
		return "block";
	case Token::Body :
		return "body";
	case Token::Buffer :
		return "buffer";
	case Token::Bus :
		return "bus";

	case Token::Case :
		return "case";
	case Token::Component :
		return "component";
	case Token::Configuration :
		return "configuration";
	case Token::Constant :
		return "constant";

	case Token::Disconnect :
		return "disconnect";
	case Token::Downto :
		return "downto";

	case Token::Else :
		return "else";
	case Token::Elsif :
		return "elsif";
	case Token::End :
		return "end";
	case Token::Entity :
		return "entity";
	case Token::Exit :
		return "exit";

	case Token::File :
		return "file";
	case Token::For :
		return "for";
	case Token::Function :
		return "function";

	case Token::Generate :
		return "generate";
	case Token::Generic :
		return "generic";
	case Token::Group :
		return "group";
	case Token::Guarded :
		return "guarded";

	case Token::If :
		return "if";
	case Token::Impure :
		return "impure";
	case Token::In :
		return "in";
	case Token::Inertial :
		return "inertial";
	case Token::Inout :
		return "inout";
	case Token::Is :
		return "is";

	case Token::Label :
		return "label";
	case Token::Library :
		return "library";
	case Token::Linkage :
		return "linkage";
	case Token::Literal :
		return "literal";
	case Token::Loop :
		return "loop";

	case Token::Map :
		return "map";

	case Token::New :
		return "new";
	case Token::Next :
		return "next";
	case Token::Not :
		return "not";
	case Token::Null :
		return "null";

	case Token::Of :
		return "of";
	case Token::On :
		return "on";
	case Token::Open :
		return "open";
	case Token::Out :
		return "out";
	case Token::Others :
		return "others";

	case Token::Package :
		return "package";
	case Token::Port :
		return "port";
	case Token::Postponed :
		return "postponed";
	case Token::Procedure :
		return "procedure";
	case Token::Process :
		return "process";
	case Token::Pure :
		return "pure";

	case Token::Range :
		return "range";
	case Token::Record :
		return "record";
	case Token::Register :
		return "register";
	case Token::Reject :
		return "reject";
	case Token::Report :
		return "report";
	case Token::Return :
		return "return";

	case Token::Select :
		return "select";
	case Token::Severity :
		return "severity";
	case Token::Shared :
		return "shared";
	case Token::Signal :
		return "signal";
	case Token::Subtype :
		return "subtype";

	case Token::Then :
		return "then";
	case Token::To :
		return "to";
	case Token::Transport :
		return "transport";
	case Token::Type :
		return "type";

	case Token::Unaffected :
		return "unaffected";
	case Token::Units :
		return "units";
	case Token::Until :
		return "until";
	case Token::Use :
		return "use";

	case Token::Variable :
		return "variable";

	case Token::Wait :
		return "wait";
	case Token::When :
		return "when";
	case Token::While :
		return "while";
	case Token::With :
		return "with";

		//shift_operator
	case Token::Sll :
		return "sll";
	case Token::Sla :
		return "sla";
	case Token::Sra :
		return "sra";
	case Token::Srl :
		return "srl";
	case Token::Rol :
		return "rol";
	case Token::Ror :
		return "ror";

		//VHDL 00
	case Token::Protected :
		return "protected";

		//VHDL 08
	case Token::Context :
		return "context";
	case Token::Parameter :
		return "parameter";

		//AMS-VHDL
	case Token::Across :
		return "across";
	case Token::Break :
		return "break";
	case Token::Limit :
		return "limit";
	case Token::Nature :
		return "nature";
	case Token::Noise :
		return "noise";
	case Token::Procedural :
		return "procedural";
	case Token::Quantity :
		return "quantity";
	case Token::Reference :
		return "reference";
	case Token::Spectrum :
		return "spectrum";
	case Token::Subnature :
		return "subnature";
	case Token::Terminal :
		return "terminal";
	case Token::Through :
		return "through";
	case Token::Tolerance :
		return "tolerance";

	case Token::And_And :
		return "&&";
	case Token::Bar_Bar :
		return "||";
	case Token::Left_Curly :
		return "{";
	case Token::Right_Curly :
		return "}";
	case Token::Exclam_Mark :
		return "!";
	case Token::Brack_Star :
		return "[*";
	case Token::Brack_Plus_Brack :
		return "[+]";
	case Token::Brack_Arrow :
		return "[->";
	case Token::Brack_Equal :
		return "[=";
	case Token::Bar_Arrow :
		return "|->";
	case Token::Bar_Double_Arrow :
		return "|:";
	case Token::Minus_Greater :
		return "->";
	case Token::Arobase :
		return "@";

	case Token::Psl_Default :
		return "default";
	case Token::Psl_Clock :
		return "clock";
	case Token::Psl_Property :
		return "property";
	case Token::Psl_Sequence :
		return "sequence";
	case Token::Psl_Endpoint :
		return "endpoint";
	case Token::Psl_Cover :
		return "cover";
	case Token::Psl_Const :
		return "const";
	case Token::Psl_Boolean :
		return "boolean";
	case Token::Inf :
		return "inf";
	case Token::Within :
		return "within";
	case Token::Abort :
		return "abort";
	case Token::Before :
		return "before";
	case Token::Always :
		return "always";
	case Token::Never :
		return "never";
	case Token::Eventually :
		return "eventually";
	case Token::Next_A :
		return "next_a";
	case Token::Next_E :
		return "next_e";
	case Token::Next_Event :
		return "next_event";
	case Token::Next_Event_A :
		return "next_event_a";
	case Token::Next_Event_E :
		return "next_event_e";
	}
	throw std::logic_error("Control flow should never reach here!!");
}
	inline bool isSignType(Token token) {
        return token == Plus || token == Minus;
    }

    inline bool isAddingOperator(Token token) {
        return token == Plus || token == Minus || token == Ampersand;
    }

    inline bool isMultiplyingOperator(Token token) {
        return token == Star || token == Slash || token == Mod || token == Rem;
    }

    inline bool isShiftingOperator(Token token) {
        return token == Sll || token == Srl || token == Sla || token == Sra || token == Rol || token == Ror;
    }

    inline bool isRelationalOperator(Token token) {
        return token >= Equal && token <= Match_Greater_Equal;
    }
}
#endif //AVLC_TOKENS_H
