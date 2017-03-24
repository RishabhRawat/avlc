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

#include "Parser.h"
#include <cassert>
#include <Keywords.h>

/*
Expect_Error: exception;

// Copy the current location into an iir.
 Set_Location (Node : Iir) is
        begin
Set_Location (Node, Get_Token_Location);
end Set_Location;

 Set_End_Location (Node : Iir) is
        begin
if Get_Kind (Node) = Iir_Design_Unit {
        Set_End_Location (Node, Get_Token_Location);
end if;
end Set_End_Location;

 Unexpected (Where: String) is
        begin
Error_Msg_Parse
("unexpected token %t in a " & Where, +scanner.currentContext.token);
end Unexpected;
*/
//   procedure Unexpected_Eof is
//   begin
//      throw std::runtime_error("PARSE ERROR: unexpected end of file");
//   end Unexpected_Eof;

//  Emit an error if the current_token if different from TOKEN.
//  Otherwise, accept the current_token (ie set it to Token::invalid, unless
//  TOKEN is Token::Identifier).
void Parser::Expect(Token::Token token, std::string message = "") {
    if (scanner.currentContext.token != token) {
        if (message.length() > 0)
            throw std::runtime_error(
                    "PARSE ERROR:" + message + " (found: " + Token::image(scanner.currentContext.token) + " )");
        else if (scanner.currentContext.token == Token::Identifier)
            throw std::runtime_error("PARSE ERROR:" + Token::image(token) + " is expected instead of "
                    + scanner.currentContext.Identifier);
        else
            throw std::runtime_error("PARSE ERROR:" + Token::image(token) + " is expected instead of "
                    + Token::image(scanner.currentContext.token));

    }

    if (scanner.currentContext.token != Token::Identifier)
        scanner.currentContext.invalidateToken();
}


//  Scan a token and expect it.
inline void Parser::Scan_Expect(Token::Token token, std::string message = "") {
    scanner.scan();
    Expect(token, message);
}

//  If the current_token is an identifier, it must be equal to name.
//  In this case, a token is eaten.
//  If the current_token is not an identifier, this is a noop.
void Parser::Check_End_Name(std::string Name, Iir* Decl) {
    if (scanner.currentContext.token != Token::Identifier)
        return;

    if (Name.empty())
        throw std::runtime_error("PARSE ERROR: end label for an unlabeled declaration or statement");
    else {
        if (scanner.currentContext.Identifier != Name)
            throw std::runtime_error("PARSE ERROR: misspelling, %i expected" + Name);
        else {
            static_cast<Iir_End_Has_Identifier_Abs*>(Decl)->End_Has_Identifier = true;
            //TODO: Take care of Xrefs
            //Xrefs.Xref_End(Get_Token_Location, Decl);
        }
    }


//  Skip identifier.
    scanner.scan();

}

inline void Parser::Check_End_Name(Iir* Decl) {
    Check_End_Name(static_cast<Iir_Identifier_Abs*>(Decl)->Identifier, Decl);
}


//  Expect ' END tok [ name ] ; '
void Parser::Check_End_Name (Token::Token token, Iir* Decl){
    if (scanner.currentContext.token != Token::End)
        throw std::runtime_error("PARSE ERROR: \"end " + Token::image(token) + ";\" expected");
    else {
        scanner.scan();
        if (scanner.currentContext.token != token)
            throw std::runtime_error("PARSE ERROR: ""end"" must be followed by %t" + Token::image(token));
        else {
            static_cast<Iir_End_Has_Reserved_Id_Abs*>(Decl)->End_Has_Reserved_Id = true;
            scanner.scan();
        }
        Check_End_Name(Decl);
        Expect(Token::Semi_Colon);
    }
}

inline void Parser::Eat_Tokens_Until_Semi_Colon() {
        while(scanner.currentContext.token != Token::Semi_Colon)
            scanner.scan();
}

//  Expect and scan ';' emit an error message using MSG if not present.
inline void Parser::Scan_Semi_Colon (std::string Msg) {
    if (scanner.currentContext.token != Token::Semi_Colon)
        throw std::runtime_error("PARSE ERROR: missing \";\" at end of " + Msg);
    else
        scanner.scan();
}

//  precond : next token
//  postcond: next token.
//
//  [ LRM93 4.3.2 ]
//  mode ::= IN | OUT | INOUT | BUFFER | LINKAGE
//
//  If there is no mode, DEFAULT is returned.
Iir_Mode Parser::Parse_Mode() {
    switch (scanner.currentContext.token) {
    case Token::In:
        scanner.scan();
        if (scanner.currentContext.token == Token::Out)
            throw std::runtime_error("PARSE ERROR: typo error, in out must be 'inout' in vhdl");
        return Iir_In_Mode;
    case Token::Out:
        scanner.scan();
        return Iir_Out_Mode;
    case Token::Inout:
        scanner.scan();
        return Iir_Inout_Mode;
    case Token::Linkage:
        scanner.scan();
        return Iir_Linkage_Mode;
    case Token::Buffer:
        scanner.scan();
        return Iir_Buffer_Mode;
    default:
        throw std::logic_error("This cannot be a mode, parsing logic error");
    }
}

//  precond : next token
//  postcond: next token
//
//  [ LRM 4.3.1.2 ]
//  signal_kind ::= REGISTER | BUS
//
//  If there is no signal_kind, then no_signal_kind is returned.

std::tuple<bool, Iir_Signal_Kind> Parser::Parse_Signal_Kind() {

    if (scanner.currentContext.token == Token::Bus) {
    //  Eat 'bus'
        scanner.scan();
        return {true, Iir_Bus_Kind};
    }
    else if (scanner.currentContext.token == Token::Register) {
    //  Eat 'register'
        scanner.scan();
        return {true, Iir_Register_Kind};
    }
    else {
    //  Avoid uninitialized variable.
        return {false, Iir_Bus_Kind};
    }
}

//  precond : TO, DOWNTO
//  postcond: next token
//
// Parse a range.
// If LEFT is not null_iir, then it must be an expression corresponding to
// the left limit of the range, and the current_token must be either
// Token::to or Token::downto.
// If left is null_iir, the current token is used to create the left limit
// expression.
//
//  [ 3.1 ]
//  range_constraint ::= RANGE range
//
//  [ 3.1 ]
//  range ::= RANGE_attribute_name | simple_expression direction simple_expression
//
//  direction ::= TO | DOWNTO
Iir* Parser::Parse_Range_Expression(Iir* Left) {
    auto result = new Iir_Range_Expression;
    result->Left_Limit_Expr = Left;
    //FIXME: this
    // Location_Copy (result, Left);
    switch (scanner.currentContext.token) {
    case Token::To:
        result->Direction = Iir_To;
        break;
    case Token::Downto:
        result->Direction = Iir_Downto;
        break;
    default:
        throw std::logic_error("This cannot be a mode, parsing logic error");
    }

//  Skip TO or DOWNTO.
    scanner.scan();
    result->Right_Limit_Expr = Parse_Simple_Expression();
    return result;
}

//  precond:  next token
//  postcond: next token
Iir* Parser::Parse_Range() {
    auto Left = Parse_Simple_Expression();
    if (scanner.currentContext.token == Token::To || scanner.currentContext.token == Token::Downto)
        return Parse_Range_Expression(Left);
    else {
        //FIXME: this check 
        //if Is_Range_Attribute_Name (Left) else
        //throw std::runtime_error("PARSE ERROR: 'to' or 'downto' expected");
        return Left;
    }
}


//  precond:  next token (after RANGE)
//  postcond: next token
Iir* Parser::Parse_Range_Constraint() {
    if (scanner.currentContext.token == Token::Box)
        throw std::runtime_error("PARSE ERROR: range constraint required");

    return Parse_Range();
}

//  precond:  next token (after RANGE)
//  postcond: next token
Iir* Parser::Parse_Range_Constraint_Of_Subtype_Indication(Iir* Type_Mark, Iir* Resolution_Indication = nullptr) {
    auto Def = new Iir_Subtype_Definition;
    // FIXME: Location_Copy (Def, Type_Mark);
    Def->Subtype_Type_Mark = Type_Mark;
    Def->Range_Constraint = Parse_Range_Constraint();
    Def->Resolution_Indication = Resolution_Indication;
    Def->Tolerance = Parse_Tolerance_Aspect_Opt();

    return Def;
}

//  precond:  next token
//  postcond: next token
//
//  [ 3.2.1 ]
//  discrete_range ::= discrete_subtype_indication | range
Iir* Parser::Parse_Discrete_Range() {
    auto Left = Parse_Simple_Expression();
    if (scanner.currentContext.token == Token::To || scanner.currentContext.token == Token::Downto)
        return Parse_Range_Expression(Left);

    else if (scanner.currentContext.token == Token::Range)
        return Parse_Subtype_Indication(Left);
    else {
//  Either a /range/_attribute_name or a type_mark.
        return Left;
    }
}


//  Convert the STR (0 .. LEN - 1) into a operator symbol identifier.
//  Emit an error message if the name is not an operator name.
unsigned int Parser::Str_To_Operator_Name (std::string str/*FIXME: , Loc : Location_Type*/) {
    //  LRM93 2.1
    //  Extra spaces are not allowed in an operator symbol, and the
    //  case of letters is not signifiant.

    //  LRM93 2.1
    //  The sequence of characters represented by an operator symbol
    //  must be an operator belonging to one of classes of operators
    //  defined in section 7.2.
    for (int i = 0; i < str.length(); ++i) {
        str[i] = Ada_Chars::toLower(str[i]);
    }

    auto it = Keywords::KeywordTable.find(str);

    if (auto it = Keywords::KeywordTable.find(str); it != Keywords::KeywordTable.end()) {
        return *it;
    }
    else
        throw std::runtime_error("PARSE ERROR:" /*+ Loc +*/ "\"" + str + "\" is not an operator symbol");

}


inline unsigned int Parser::Scan_To_Operator_Name (/*FIXME: Loc : Location_Type*/) {
    return Str_To_Operator_Name(scanner.currentContext.Identifier/*FIXME:, Loc*/);
}


//  Convert string literal STR to an operator symbol.
//  Emit an error message if the string is not an operator name.
Iir* Parser::String_To_Operator_Symbol (Iir* str) {
    auto Id = Str_To_Operator_Name(static_cast<Iir_String_Literal8*>(str)->str);
    auto result = new Iir_Operator_Symbol;
    //FIXME: Location_Copy(result, Str);
    result->Identifier = Id;
    //FIXME: delete str;
    return result;
}

// precond : next token
// postcond: next token
//
// [ 6.1 ]
// name ::= simple_name | operator_symbol | selected_name | indexed_name | slice_name | attribute_name
//
// [ 6.2 ]
// simple_name ::= identifier
//
// [ 6.5 ]
// slice_name ::= prefix ( discrete_range )
//
// [ 6.3 ]
// selected_name ::= prefix . suffix
//
// [ 6.1 ]
// prefix ::= name | function_call
//
// [ 6.3 ]
// suffix ::= simple_name | character_literal | operator_symbol | ALL
//
// [ 3.2.1 ]
// discrete_range ::= DISCRETE_subtype_indication | range
//
// [ 3.1 ]
// range ::= RANGE_attribute_name | simple_expression direction simple_expression
//
// [ 3.1 ]
// direction ::= TO | DOWNTO
//
// [ 6.6 ]
// attribute_name ::=
//     prefix [ signature ] ' attribute_designator [ ( expression ) ]
//
// [ 6.6 ]
// attribute_designator ::= ATTRIBUTE_simple_name
//
// Note: in order to simplify the parsing, this function may return a signature without attribute designator.
// Signatures may appear at 3 places:
// - in attribute name
// - in alias declaration
// - in entity designator
Iir* Parser::Parse_Name_Suffix(Iir* Pfx, bool Allow_Indexes = true) {
    auto result = Pfx;
    while (1) {
        auto Prefix = result;

        switch (scanner.currentContext.token) {
        case Token::Left_Bracket:
            if (Get_Kind(Prefix) == Iir_String_Literal8) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

//  There is a signature. They are normally followed by an
//  attribute.
            result = Parse_Signature();
            Set_Signature_Prefix(result, Prefix);
            break;

        case Token::Tick:
// There is an attribute.
            if (Get_Kind(Prefix) == Iir_String_Literal8) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            scanner.scan();
            if (scanner.currentContext.token == Token::Left_Paren) {
                // A qualified expression.
                result = new Iir_Qualified_Expression;
                Set_Type_Mark(result, Prefix);
                Location_Copy(result, Prefix);
                Set_Expression(result, Parse_Aggregate);
                return result;
            }
            else if (scanner.currentContext.token != Token::Range && scanner.currentContext.token != Token::Identifier
                    && not(Vhdl_Std >= Vhdl_08 && scanner.currentContext.token = Token::Subtype)) {
                Expect(Token::Identifier, "attribute identifier expected after '");
                return nullptr;
            }
            result = new Iir_Attribute_Name;
            Set_Identifier(result, scanner.currentContext.Identifier);
            Set_Location(result);
            if (Get_Kind(Prefix) == Iir_Signature) {
                Set_Attribute_Signature(result, Prefix);

//  Transfer the prefix from the signature to the attribute.
                Set_Prefix(result, Get_Signature_Prefix(Prefix));
                Set_Signature_Prefix(Prefix, nullptr);
            }
            else
                Set_Prefix(result, Prefix);


// accept the identifier.
            scanner.scan();
            break;

        case Token::Left_Paren:
            if (!Allow_Indexes) {
                return result;
            }

            if (Get_Kind(Prefix) = Iir_String_Literal8) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            result = new Iir_Parenthesis_Name;
            Set_Location(result);
            Set_Prefix(result, Prefix);
            Set_Association_Chain(result, Parse_Association_List_In_Parenthesis);
            break;

        case Token::Dot:
            if (Get_Kind(Prefix) == Iir_String_Literal8) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            scanner.scan();
            switch (scanner.currentContext.token) {
            case Token::All:
                result = new Iir_Selected_By_All_Name;
                Set_Location(result);
                Set_Prefix(result, Prefix);
                break;
            case Token::Identifier:
            case Token::Character:
                result = new Iir_Selected_Name;
                Set_Location(result);
                Set_Prefix(result, Prefix);
                Set_Identifier(result, scanner.currentContext.Identifier);
                break;
            case Token::String:
                result = new Iir_Selected_Name;
                Set_Location(result);
                Set_Prefix(result, Prefix);
                Set_Identifier(result, Scan_To_Operator_Name(Get_Token_Location));
                break;
            default:
                throw std::runtime_error("PARSE ERROR: an identifier or all is expected");
            }
            scanner.scan();
            break;
        default:
            return result;
        }
    }
}


//  Precond:  next token
//  Postcond: next token
//
//  LRM08 8.7 External names
//
//  external_pathname ::=
//      package_pathname | absolute_pathname | relative_pathname
//
//  package_pathname ::=
//    @ library_logical_name . package_simple_name .
//      { package_simple_name . } object_simple_name
//
//  absolute_pathname ::=
//    . partial_pathname
//
//  relative_pathname ::=
//    { ^ . } partial_pathname
//
//  partial_pathname ::= { pathname_element . } object_simple_name
//
//  pathname_element ::=
//      entity_simple_name | component_instantiation_label | block_label
//      | generate_statement_label [ ( static_expression ) ] | package_simple_name
Iir* Parser::Parse_External_Pathname() {
    Iir* result;
    Iir* Last;
    Iir* El;
    switch (scanner.currentContext.token) {
    case Token::Arobase:
        result = new Iir_Package_Pathname;
        Set_Location(result);
        Last = result;

//  Skip '@'
        scanner.scan();

        if (scanner.currentContext.token != Token::Identifier)
            throw std::runtime_error("PARSE ERROR: library name expected after '@'");
        else {
            Set_Identifier(result, scanner.currentContext.Identifier);

//  Skip ident
            scanner.scan();
        }

        if (scanner.currentContext.token != Token::Dot)
            throw std::runtime_error("PARSE ERROR: '.' expected after library name");
        else {
//  Skip '.'
            scanner.scan();
        }
        break;
    case Token::Dot:
        result = new Iir_Absolute_Pathname;
        Set_Location(result);
        Last = result;

//  Skip '.'
        scanner.scan();
        break;
    case Token::Caret:
        Last = nullptr;
        do {
            El = new Iir_Relative_Pathname;
            Set_Location(El);

//  Skip '^'
            scanner.scan();

            if (scanner.currentContext.token != Token::Dot)
                throw std::runtime_error("PARSE ERROR: '.' expected after '^'");
            else {
//  Skip '.'
                scanner.scan();
            }

            if (!Last)
                result = El;
            else
                Set_Pathname_Suffix(Last, El);

            Last = El;

        }
        while (scanner.currentContext.token != Token::Caret);
        break;

    case Token::Identifier:
        Last = nullptr;

    default:
        Last = nullptr;
//  Error is handled just below.
    }

//  Parse pathname elements.
    while (1) {
        if (scanner.currentContext.token != Token::Identifier)
            throw std::runtime_error("PARSE ERROR: pathname element expected");

        El = new Iir_Pathname_Element;
        Set_Location(El);
        Set_Identifier(El, scanner.currentContext.Identifier);
        if (!Last)
            result = El;
        else
            Set_Pathname_Suffix(Last, El);

        Last = El;

//  Skip identifier
        scanner.scan();
        if (scanner.currentContext.token == Token::Caret)
            break;

//  Skip '.'
        scanner.scan();
    }

    return result;
}

//  Precond:  '<<'
//  Postcond: next token
//
//  LRM08 8.7 External names
//  external_name ::= external_constant_name | external_signal_name | external_variable_name
//
//  external_constant_name ::= << CONSTANT external_pathname : subtype_indication >>
//
//  external_signal_name ::=  << SIGNAL external_pathname : subtype_indication >>
//
//  external_variable_name ::= << VARIABLE external_pathname : subtype_indication >>
Iir* Parser::Parse_External_Name() {

//Loc : Location_Type;
//result : Iir;
//Kind : Iir_Kind;
//begin
    Loc = Get_Token_Location();

//  Skip '<<'
    scanner.scan();

    switch (scanner.currentContext.token) {
    case Token::Constant:
        Kind = Iir_External_Constant_Name;
//  Skip 'constant'
        scanner.scan();
        break;
    case Token::Signal:
        Kind = Iir_External_Signal_Name;
//  Skip 'signal'
        scanner.scan();
        break;
    case Token::Variable:
        Kind = Iir_External_Variable_Name;
//  Skip 'variable'
        scanner.scan();
        break;
    default:
        throw std::runtime_error("PARSE ERROR: constant, signal or variable expected after '<<'");
    }

    auto result = new Kind;
    Set_Location(result, Loc);

    Set_External_Pathname(result, Parse_External_Pathname);

    if (scanner.currentContext.token != Token::Colon)
        throw std::runtime_error("PARSE ERROR: ':' expected after external pathname");
    else
//  Skip ':'
        scanner.scan();

    Set_Subtype_Indication(result, Parse_Subtype_Indication);

    if (scanner.currentContext.token != Token::Double_Greater)
        throw std::runtime_error("PARSE ERROR: '>>' expected at end of external name");
    else
//  Skip '>>'
        scanner.scan();

    return result;
}


/*
//  Precond: next token (identifier, string or '<<')
//  Postcond: next token
//
//  LRM08 8. Names
//  name ::=
//     simple_name
        --   | operator_symbol
        --   | character_literal    --  FIXME: not handled.
//   | selected_name
//   | indexed_name
//   | slice_name
//   | attribute_name
//   | external_name
        Iir function Parse_Name (Allow_Indexes: Boolean := true) 
        is
Res: Iir;
begin
switch (scanner.currentContext.token) {
case Token::Identifier:
Res := Create_Iir (Iir_Simple_Name);
Set_Identifier (Res, scanner.currentContext.Identifier);
Set_Location (Res);

//  Skip identifier
scanner.scan();

case Token::String:
//  For operator symbol, such as: "+" (A, B).
Res := Create_Iir (Iir_String_Literal8);
Set_String8_Id (Res, Current_String_Id);
Set_String_Length (Res, Current_String_Length);
Set_Location (Res);

//  Skip string
scanner.scan();
case Token::Double_Less:
if Vhdl_Std < Vhdl_08 {
        throw std::runtime_error("PARSE ERROR: external name not allowed before vhdl 08");
end if;
Res := Parse_External_Name;
default:
throw std::runtime_error("PARSE ERROR: identifier expected here");
raise Parse_Error;
}

return Parse_Name_Suffix (Res, Allow_Indexes);
end Parse_Name;

//  Emit an error message if MARK doesn't have the form of a type mark.
 Check_Type_Mark (Mark : Iir) is
        begin
switch (Get_Kind (Mark) is
        when Iir_Simple_Name
| Iir_Selected_Name:
null;
case Iir_Attribute_Name:
//  For O'Subtype.
null;
default:
throw std::runtime_error("PARSE ERROR:" + +Mark, "type mark must be a name of a type");
}
end Check_Type_Mark;

//  precond : next token
//  postcond: next token
//
//  [ 4.2 ]
//  type_mark ::= type_name
        --              | subtype_name
Iir Parse_Type_Mark (Check_Paren : Boolean := false) 
        is
Res : Iir;
Old : Iir;
pragma Unreferenced (Old);
begin
        Res := Parse_Name (Allow_Indexes: false);
Check_Type_Mark (Res);
if Check_Paren && scanner.currentContext.token = Token::Left_Paren {
        throw std::runtime_error("PARSE ERROR: index constraint not allowed here");
Old := Parse_Name_Suffix (Res, true);
end if;
return Res;
end Parse_Type_Mark;

//  precond : CONSTANT, SIGNAL, VARIABLE. FILE or identifier
//  postcond: next token (';' or ')')
//
//  [ LRM93 4.3.2 ] [ LRM08 6.5.2 ]
//  interface_declaration ::= interface_constant_declaration
        --                          | interface_signal_declaration
        --                          | interface_variable_declaration
        --                          | interface_file_declaration
        --
        --
        --  [ LRM93 3.2.2 ]
//  identifier_list ::= identifier { , identifier }
        --
        --  [ LRM93 4.3.2 ]
//  interface_constant_declaration ::=
//      [ CONSTANT ] identifier_list : [ IN ] subtype_indication
//          [ := STATIC_expression ]
//
//  [ LRM93 4.3.2 ]
//  interface_file_declaration ::= FILE identifier_list : subtype_indication
//
//  [ LRM93 4.3.2 ]
//  interface_signal_declaration ::=
//      [ SIGNAL ] identifier_list : [ mode ] subtype_indication [ BUS ]
//          [ := STATIC_expression ]
//
//  [ LRM93 4.3.2 ]
//  interface_variable_declaration ::=
//      [ VARIABLE ] identifier_list : [ mode ] subtype_indication
//          [ := STATIC_expression ]
//
//  The default kind of interface declaration is DEFAULT.
Parse_Interface_Object_Declaration (Ctxt : Interface_Kind_Type)
return Iir
        is
Kind : Iir_Kind;
Res, Last : Iir;
First, Prev_First : Iir;
Inter: Iir;
Is_Default : Boolean;
Interface_Mode: Iir_Mode;
Interface_Type: Iir;
Is_Guarded : Boolean;
Signal_Kind: Iir_Signal_Kind;
Default_Value: Iir;
Has_Mode : Boolean;
Has_Class : Boolean;
begin
        Res := nullptr;
Last := nullptr;

//  LRM08 6.5.2 Interface object declarations
//  Interface obejcts include interface constants that appear as
//  generics of a design entity, a component, a block, a package or
//  a subprogram, or as constant parameter of subprograms; interface
//  signals that appear as ports of a design entity, component or
//  block, or as signal parameters of subprograms; interface variables
//  that appear as variable parameter subprograms; interface files
//  that appear as file parameters of subrograms.
switch (scanner.currentContext.token) {
case Token::Identifier:
//  The class of the object is unknown.  Select default
//  according to the above rule, assuming the mode is IN.  If
//  the mode is not IN, Parse_Interface_Object_Declaration will
//  change the class.
switch (Ctxt is
case Generic_Interface_List
| Parameter_Interface_List:
Kind := Iir_Interface_Constant_Declaration;
case Port_Interface_List:
Kind := Iir_Interface_Signal_Declaration;
}
case Token::Constant:
Kind := Iir_Interface_Constant_Declaration;
case Token::Signal:
if Ctxt = Generic_Interface_List {
        Error_Msg_Parse
        ("signal interface not allowed in generic clause");
end if;
Kind := Iir_Interface_Signal_Declaration;
case Token::Variable:
if Ctxt not in Parameter_Interface_List {
        Error_Msg_Parse
        ("variable interface not allowed in generic or port clause");
end if;
Kind := Iir_Interface_Variable_Declaration;
case Token::File:
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: file interface not allowed in vhdl 87");
end if;
if Ctxt not in Parameter_Interface_List {
        Error_Msg_Parse
        ("variable interface not allowed in generic or port clause");
end if;
Kind := Iir_Interface_File_Declaration;
default:
//  Fall back in case of parse error.
Kind := Iir_Interface_Variable_Declaration;
}

Inter := Create_Iir (Kind);

if (scanner.currentContext.token = Token::Identifier {
        Is_Default := true;
Has_Class := false;
else
Is_Default := false;
Has_Class := true;

//  Skip 'signal', 'variable', 'constant' or 'file'.
scanner.scan();
end if;

Prev_First := Last;
First := Inter;
loop
if (scanner.currentContext.token != Token::Identifier {
Expect (Token::Identifier);
end if;
Set_Identifier (Inter, scanner.currentContext.Identifier);
Set_Location (Inter);

if Res = nullptr {
        Res := Inter;
else
Set_Chain (Last, Inter);
end if;
Last := Inter;

//  Skip identifier
scanner.scan();

exit when scanner.currentContext.token = Token::Colon;
Expect (Token::Comma, "',' or ':' expected after identifier");

//  Skip ','
scanner.scan();

Inter := Create_Iir (Kind);
end loop;

Expect (Token::Colon, "':' must follow the interface element identifier");

//  Skip ':'
scanner.scan();

//  Parse mode.
switch (scanner.currentContext.token) {
case Token::In
| Token::Out
| Token::Inout
| Token::Linkage
| Token::Buffer:
Interface_Mode := Parse_Mode;
Has_Mode := true;
default:
Interface_Mode := Iir_Unknown_Mode;
Has_Mode := false;
}

//  LRM93 2.1.1  LRM08 4.2.2.1
//  If the mode is INOUT or OUT, and no object class is explicitly
//  specified, variable is assumed.
if Is_Default
        && Ctxt in Parameter_Interface_List
        && Interface_Mode in Iir_Out_Modes
        {
//  Convert into variable.
declare
        O_Interface : Iir_Interface_Constant_Declaration;
N_Interface : Iir_Interface_Variable_Declaration;
begin
        O_Interface := First;
while O_Interface != nullptr loop
N_Interface :=
Create_Iir (Iir_Interface_Variable_Declaration);
Location_Copy (N_Interface, O_Interface);
Set_Identifier (N_Interface,
        Get_Identifier (O_Interface));
if Prev_First = nullptr {
        Res := N_Interface;
else
Set_Chain (Prev_First, N_Interface);
end if;
Prev_First := N_Interface;
if O_Interface = First {
        First := N_Interface;
end if;
Last := N_Interface;
Inter := Get_Chain (O_Interface);
Free_Iir (O_Interface);
O_Interface := Inter;
end loop;
Inter := First;
end;
end if;

//  Parse mode (and handle default mode).
switch (Iir_Kinds_Interface_Object_Declaration (Get_Kind (Inter)) is
        when Iir_Interface_File_Declaration:
if Interface_Mode != Iir_Unknown_Mode {
Error_Msg_Parse
("mode can't be specified for a file interface");
end if;
Interface_Mode := Iir_Inout_Mode;
case Iir_Interface_Signal_Declaration
| Iir_Interface_Variable_Declaration:
//  LRM93 4.3.2
//  If no mode is explicitly given in an interface declaration
//  other than an interface file declaration, mode IN is
//  assumed.
if Interface_Mode = Iir_Unknown_Mode {
        Interface_Mode := Iir_In_Mode;
end if;
case Iir_Interface_Constant_Declaration:
if Interface_Mode = Iir_Unknown_Mode {
        Interface_Mode := Iir_In_Mode;
elsif Interface_Mode != Iir_In_Mode {
throw std::runtime_error("PARSE ERROR: mode must be 'in' for a constant");
Interface_Mode := Iir_In_Mode;
end if;
}

Interface_Type := Parse_Subtype_Indication;

//  Signal kind (but only for signal).
if Get_Kind (Inter) = Iir_Interface_Signal_Declaration {
        Parse_Signal_Kind (Is_Guarded, Signal_Kind);
else
Is_Guarded := false;
Signal_Kind := Iir_Register_Kind;
end if;

if (scanner.currentContext.token = Token::Assign {
if Get_Kind (Inter) = Iir_Interface_File_Declaration {
        Error_Msg_Parse
        ("default expression not allowed for an interface file");
end if;

//  Skip ':='
scanner.scan();

Default_Value := Parse_Expression;
else
Default_Value := nullptr;
end if;

//  Subtype_Indication and Default_Value are set only on the first
//  interface.
Set_Subtype_Indication (First, Interface_Type);
if Get_Kind (First) != Iir_Interface_File_Declaration {
Set_Default_Value (First, Default_Value);
end if;

Inter := First;
while Inter != nullptr loop
Set_Mode (Inter, Interface_Mode);
Set_Is_Ref (Inter, Inter != First);
Set_Has_Mode (Inter, Has_Mode);
Set_Has_Class (Inter, Has_Class);
Set_Has_Identifier_List (Inter, Inter != Last);
if Get_Kind (Inter) = Iir_Interface_Signal_Declaration {
        Set_Guarded_Signal_Flag (Inter, Is_Guarded);
Set_Signal_Kind (Inter, Signal_Kind);
end if;
Inter := Get_Chain (Inter);
end loop;

return Res;
end Parse_Interface_Object_Declaration;

//  Precond : 'package'
//  Postcond: next token
//
//  LRM08 6.5.5 Interface package declarations
//  interface_package_declaration ::=
//    PACKAGE identifier IS NEW uninstantiated_package name
//      interface_package_generic_map_aspect
//
//  interface_package_generic_map_aspect ::=
//       generic_map_aspect
        --     | GENERIC MAP ( <> )
//     | GENERIC MAP ( DEFAULT )
Parse_Interface_Package_Declaration return Iir
        is
Inter : Iir;
Map : Iir;
begin
        Inter := Create_Iir (Iir_Interface_Package_Declaration);

//  Skip 'package'
Scan_Expect (Token::Identifier,
"an identifier is expected after ""package""");
Set_Identifier (Inter, scanner.currentContext.Identifier);
Set_Location (Inter);

//  Skip identifier
Scan_Expect (Token::Is);

//  Skip 'is'
Scan_Expect (Token::New);

//  Skip 'new'
scanner.scan();

Set_Uninstantiated_Package_Name (Inter, Parse_Name (false));

Expect (Token::Generic);

//  Skip 'generic'
Scan_Expect (Token::Map);

//  Skip 'map'
Scan_Expect (Token::Left_Paren);

//  Skip '('
scanner.scan();

switch (scanner.currentContext.token) {
case Token::Box:
Map := nullptr;
//  Skip '<>'
scanner.scan();
default:
Map := Parse_Association_List;
}
Set_Generic_Map_Aspect_Chain (Inter, Map);

Expect (Token::Right_Paren);

//  Skip ')'
scanner.scan();

return Inter;
end Parse_Interface_Package_Declaration;

//  Precond:  identifier or string
//  Postcond: next token
//
//  [ 2.1 ]
//  designator ::= identifier | operator_symbol
 Parse_Subprogram_Designator (Subprg : Iir) is
        begin
if (scanner.currentContext.token = Token::Identifier {
        Set_Identifier (Subprg, scanner.currentContext.Identifier);
Set_Location (Subprg);
elsif (scanner.currentContext.token = Token::String {
if Kind_In (Subprg, Iir_Procedure_Declaration,
        Iir_Interface_Procedure_Declaration)
{
//  LRM93 2.1
//  A procedure designator is always an identifier.
throw std::runtime_error("PARSE ERROR: a procedure name must be an identifier");
end if;
//  LRM93 2.1
//  A function designator is either an identifier or an operator
        --  symbol.
Set_Identifier (Subprg, Scan_To_Operator_Name (Get_Token_Location));
Set_Location (Subprg);
else
//  Just to display a parse error.
Expect (Token::Identifier);
end if;

//  Eat designator (identifier or string).
scanner.scan();
end Parse_Subprogram_Designator;

//  Precond: '(' or return or any
//  Postcond: next token
        procedure Parse_Subprogram_Parameters_And_Return
(Subprg : Iir; Is_Func : Boolean)
is
        Old : Iir;
pragma Unreferenced (Old);
Inters : Iir;
begin
if (scanner.currentContext.token = Token::Parameter {
        Set_Has_Parameter (Subprg, true);

//  Eat 'parameter'
scanner.scan();

if (scanner.currentContext.token != Token::Left_Paren {
Error_Msg_Parse
("'parameter' must be followed by a list of parameters");
end if;
end if;

if (scanner.currentContext.token = Token::Left_Paren {
//  Parse the interface declaration.
if Is_Func {
Inters := Parse_Interface_List
        (Function_Parameter_Interface_List, Subprg);
else
Inters := Parse_Interface_List
        (Procedure_Parameter_Interface_List, Subprg);
end if;
Set_Interface_Declaration_Chain (Subprg, Inters);
end if;

if (scanner.currentContext.token = Token::Return {
if not Is_Func {
Error_Msg_Parse
("'return' not allowed for a procedure", Cont: true);
Error_Msg_Parse
("(remove return part or declare a function)");

//  Skip 'return'
scanner.scan();

Old := Parse_Type_Mark;
else
//  Skip 'return'
scanner.scan();

Set_Return_Type_Mark
(Subprg, Parse_Type_Mark (Check_Paren: true));
end if;
else
if Is_Func {
throw std::runtime_error("PARSE ERROR: 'return' expected");
end if;
end if;
end Parse_Subprogram_Parameters_And_Return;

//  Precond:  PROCEDURE, FUNCTION, PURE, IMPURE
//  Postcond: next token
//
//  LRM08 6.5.4 Interface subrpogram declarations
//  interface_subprogram_declaration ::=
//     interface_subprogram_specification
        --        [ IS interface_subprogram_default ]
//
//  interface_subrpogram_specification ::=
//     interface_procedure_specification | interface_function_specification
        --
        --  interface_procedure_specification ::=
//     PROCEDURE designator
//     [ [ PARAMETER ] ( formal_parameter_list ) ]
//
//  interface_function_specification ::=
//     [ PURE | IMPURE ] FUNCTION designator
//       [ [ PARAMETER ] ( formal_parameter_list ) ] RETURN type_mark
//
//  interface_subprogram_default ::=
//     /subprogram/_name | <>
        function Parse_Interface_Subprogram_Declaration return Iir
        is
Kind : Iir_Kind;
Subprg: Iir;
Old : Iir;
pragma Unreferenced (Old);
begin
//  Create the node.
switch (scanner.currentContext.token) {
case Token::Procedure:
Kind := Iir_Interface_Procedure_Declaration;
case Token::Function
| Token::Pure
| Token::Impure:
Kind := Iir_Interface_Function_Declaration;
default:
raise Internal_Error;
}
Subprg := Create_Iir (Kind);
Set_Location (Subprg);

switch (scanner.currentContext.token) {
case Token::Procedure:
null;
case Token::Function:
//  LRM93 2.1
//  A function is impure if its specification contains the
//  reserved word IMPURE; otherwise it is said to be pure.
Set_Pure_Flag (Subprg, true);
case Token::Pure
| Token::Impure:
Set_Pure_Flag (Subprg, scanner.currentContext.token = Token::Pure);
Set_Has_Pure (Subprg, true);
//  FIXME: what to do in case of error ??

//  Eat 'pure' or 'impure'.
scanner.scan();

Expect (Token::Function, "'function' must follow 'pure' or 'impure'");
default:
raise Internal_Error;
}

//  Eat 'procedure' or 'function'.
scanner.scan();

//  Designator.
Parse_Subprogram_Designator (Subprg);

Parse_Subprogram_Parameters_And_Return
(Subprg, Kind = Iir_Interface_Function_Declaration);

//  TODO: interface_subprogram_default

return Subprg;
end Parse_Interface_Subprogram_Declaration;

//  Precond : '('
//  Postcond: next token
//
//  LRM08 6.5.6 Interface lists
//  interface_list ::= interface_element { ';' interface_element }
        --
        --  interface_element ::= interface_declaration
Parse_Interface_List (Ctxt : Interface_Kind_Type; Parent : Iir)
return Iir
        is
Res, Last : Iir;
Inters : Iir;
Next : Iir;
Prev_Loc : Location_Type;
begin
        Expect (Token::Left_Paren);

Res := nullptr;
Last := nullptr;
loop
        Prev_Loc := Get_Token_Location;

//  Skip '(' or ';'
scanner.scan();

switch (scanner.currentContext.token) {
case Token::Identifier
| Token::Signal
| Token::Variable
| Token::Constant
| Token::File:
//  An inteface object.
Inters := Parse_Interface_Object_Declaration (Ctxt);
case Token::Package:
if Ctxt != Generic_Interface_List {
Error_Msg_Parse
("package interface only allowed in generic interface");
elsif Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("package interface not allowed before vhdl 08");
end if;
Inters := Parse_Interface_Package_Declaration;
case Token::Type:
if Ctxt != Generic_Interface_List {
Error_Msg_Parse
("type interface only allowed in generic interface");
elsif Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("type interface not allowed before vhdl 08");
end if;
Inters := Create_Iir (Iir_Interface_Type_Declaration);
Scan_Expect (Token::Identifier,
"an identifier is expected after 'type'");
Set_Identifier (Inters, scanner.currentContext.Identifier);
Set_Location (Inters);

//  Skip identifier
scanner.scan();
case Token::Procedure
| Token::Pure
| Token::Impure
| Token::Function:
if Ctxt != Generic_Interface_List {
Error_Msg_Parse
("subprogram interface only allowed in generic interface");
elsif Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("subprogram interface not allowed before vhdl 08");
end if;
Inters := Parse_Interface_Subprogram_Declaration;
case Token::Right_Paren:
if Res = nullptr {
        Error_Msg_Parse
        (Prev_Loc, "empty interface list not allowed");
else
Error_Msg_Parse
(Prev_Loc, "extra ';' at end of interface list");
end if;
exit;
default:
throw std::runtime_error("PARSE ERROR: interface declaration expected");
//  Use a variable interface as a fall-back.
Inters := Parse_Interface_Object_Declaration (Ctxt);
}

//  Chain
if Last = nullptr {
        Res := Inters;
else
Set_Chain (Last, Inters);
end if;

//  Set parent and set Last to the last interface.
Last := Inters;
loop
        Set_Parent (Last, Parent);
Next := Get_Chain (Last);
exit when Next = nullptr;
Last := Next;
end loop;

switch (scanner.currentContext.token) {
case Token::Comma:
Error_Msg_Parse
("interfaces must be separated by ';' (found ',')");
case Token::Semi_Colon:
null;
default:
exit;
}
end loop;

if (scanner.currentContext.token != Token::Right_Paren {
throw std::runtime_error("PARSE ERROR: ')' expected at end of interface list");
end if;

//  Skip ')'
scanner.scan();

return Res;
end Parse_Interface_List;

//  precond : PORT
//  postcond: next token
//
//  [ 1.1.1 ]
//  port_clause ::= PORT ( port_list ) ;
//
//  [ 1.1.1.2 ]
//  port_list ::= PORT_interface_list
 Parse_Port_Clause (Parent : Iir)
is
        Res: Iir;
El : Iir;
begin
//  Skip 'port'
pragma Assert (scanner.currentContext.token = Token::Port);
scanner.scan();

Res := Parse_Interface_List (Port_Interface_List, Parent);

//  Check the interface are signal interfaces.
El := Res;
while El != nullptr loop
if Get_Kind (El) != Iir_Interface_Signal_Declaration {
throw std::runtime_error("PARSE ERROR:" + +El, "port must be a signal");
end if;
El := Get_Chain (El);
end loop;

Scan_Semi_Colon ("port clause");
Set_Port_Chain (Parent, Res);
end Parse_Port_Clause;

//  precond : GENERIC
//  postcond: next token
//
//  [ LRM93 1.1.1, LRM08 6.5.6.2 ]
//  generic_clause ::= GENERIC ( generic_list ) ;
//
//  [ LRM93 1.1.1.1, LRM08 6.5.6.2]
//  generic_list ::= GENERIC_interface_list
 Parse_Generic_Clause (Parent : Iir)
is
        Res: Iir;
begin
//  Skip 'generic'
pragma Assert (scanner.currentContext.token = Token::Generic);
scanner.scan();

Res := Parse_Interface_List (Generic_Interface_List, Parent);
Set_Generic_Chain (Parent, Res);

Scan_Semi_Colon ("generic clause");
end Parse_Generic_Clause;

//  precond : a token.
//  postcond: next token
//
//  [ 1.1.1 ]
//  entity_header ::=
//      [ FORMAL_generic_clause ]
//      [ FORMAL_port_clause ]
//
//  [ 4.5 ]
//          [ LOCAL_generic_clause ]
//          [ LOCAL_port_clause ]
 Parse_Generic_Port_Clauses (Parent : Iir)
is
        Has_Port, Has_Generic : Boolean;
begin
        Has_Port := false;
Has_Generic := false;
loop
if (scanner.currentContext.token = Token::Generic {
if Has_Generic {
throw std::runtime_error("PARSE ERROR: at most one generic clause is allowed");
end if;
if Has_Port {
throw std::runtime_error("PARSE ERROR: generic clause must precede port clause");
end if;
Has_Generic := true;
Parse_Generic_Clause (Parent);
elsif (scanner.currentContext.token = Token::Port {
if Has_Port {
throw std::runtime_error("PARSE ERROR: at most one port clause is allowed");
end if;
Has_Port := true;
Parse_Port_Clause (Parent);
else
exit;
end if;
end loop;
end Parse_Generic_Port_Clauses;

//  precond : a token
//  postcond: next token
//
//  [ LRM93 3.1.1 ]
//  enumeration_type_definition ::=
//      ( enumeration_literal { , enumeration_literal } )
        --
        --  [ LRM93 3.1.1 ]
//  enumeration_literal ::= identifier | character_literal
Parse_Enumeration_Type_Definition (Parent : Iir)
return Iir_Enumeration_Type_Definition
        is
Pos: Iir_Int32;
Enum_Lit: Iir_Enumeration_Literal;
Enum_Type: Iir_Enumeration_Type_Definition;
Enum_List : Iir_List;
begin
//  This is an enumeration.
Enum_Type := Create_Iir (Iir_Enumeration_Type_Definition);
Set_Location (Enum_Type);
Enum_List := Create_Iir_List;
Set_Enumeration_Literal_List (Enum_Type, Enum_List);

//  LRM93 3.1.1
//  The position number of the first listed enumeration literal is zero.
Pos := 0;

//  Eat '('.
scanner.scan();

if (scanner.currentContext.token = Token::Right_Paren {
        throw std::runtime_error("PARSE ERROR: at least one literal must be declared");
scanner.scan();
return Enum_Type;
end if;
loop
if (scanner.currentContext.token != Token::Identifier
        && scanner.currentContext.token != Token::Character
        {
if (scanner.currentContext.token = Token::Eof {
        throw std::runtime_error("PARSE ERROR: unexpected end of file");
return Enum_Type;
end if;
throw std::runtime_error("PARSE ERROR: identifier or character expected");
end if;

Enum_Lit := Create_Iir (Iir_Enumeration_Literal);
Set_Identifier (Enum_Lit, scanner.currentContext.Identifier);
Set_Parent (Enum_Lit, Parent);
Set_Location (Enum_Lit);
Set_Enum_Pos (Enum_Lit, Pos);

// LRM93 3.1.1
// the position number for each additional enumeration literal is
// one more than that if its predecessor in the list.
Pos := Pos + 1;

Append_Element (Enum_List, Enum_Lit);

//  Skip identifier or character.
scanner.scan();

exit when scanner.currentContext.token = Token::Right_Paren;
if (scanner.currentContext.token != Token::Comma {
throw std::runtime_error("PARSE ERROR: ')' or ',' is expected after an enum literal");
end if;

//  Skip ','.
scanner.scan();

if (scanner.currentContext.token = Token::Right_Paren {
        throw std::runtime_error("PARSE ERROR: extra ',' ignored");
exit;
end if;
end loop;

//  Skip ')'.
scanner.scan();

return Enum_Type;
end Parse_Enumeration_Type_Definition;

//  precond : ARRAY
//  postcond: ??
//
//  [ LRM93 3.2.1 ]
//  array_type_definition ::= unconstrained_array_definition
        --                          | constrained_array_definition
        --
        --   unconstrained_array_definition ::=
//      ARRAY ( index_subtype_definition { , index_subtype_definition } )
        --      OF element_subtype_indication
//
//   constrained_array_definition ::=
//      ARRAY index_constraint OF element_subtype_indication
//
//   index_subtype_definition ::= type_mark RANGE <>
//
//   index_constraint ::= ( discrete_range { , discrete_range } )
        --
        --   discrete_range ::= discrete_subtype_indication | range
        --
        --  [ LRM08 5.3.2.1 ]
//  array_type_definition ::= unbounded_array_definition
        --                          | constrained_array_definition
        --
        --   unbounded_array_definition ::=
//      ARRAY ( index_subtype_definition { , index_subtype_definition } )
        --      OF element_subtype_indication
Parse_Array_Definition return Iir
        is
Index_Constrained : Boolean;
Array_Constrained : Boolean;
First : Boolean;
Res_Type: Iir;
Index_List : Iir_List;

Loc : Location_Type;
Def : Iir;
Type_Mark : Iir;
Element_Subtype : Iir;
begin
        Loc := Get_Token_Location;

//  Skip 'array', scan '('
Scan_Expect (Token::Left_Paren);
scanner.scan();

First := true;
Index_List := Create_Iir_List;

loop
//  The accepted syntax can be one of:
//  * index_subtype_definition, which is:
//    * type_mark RANGE <>
//  * discrete_range, which is either:
//    * /discrete/_subtype_indication
//      * [ resolution_indication ] type_mark [ range_constraint ]
//        * range_constraint ::= RANGE range
//    * range
//      * /range/_attribute_name
//      * simple_expression direction simple_expression

//  Parse a simple expression (for the range), which can also parse a
//  name.
Type_Mark := Parse_Simple_Expression;

switch (scanner.currentContext.token) {
case Token::Range:
//  Skip 'range'
scanner.scan();

if (scanner.currentContext.token = Token::Box {
//  Parsed 'RANGE <>': this is an index_subtype_definition.
Index_Constrained := false;
scanner.scan();
Def := Type_Mark;
else
//  This is a /discrete/_subtype_indication
        Index_Constrained := true;
Def :=
Parse_Range_Constraint_Of_Subtype_Indication (Type_Mark);
end if;
case Token::To
| Token::Downto:
//  A range
Index_Constrained := true;
Def := Parse_Range_Expression (Type_Mark);
default:
//  For a /range/_attribute_name
        Index_Constrained := true;
Def := Type_Mark;
}

Append_Element (Index_List, Def);

if First {
Array_Constrained := Index_Constrained;
First := false;
else
if Array_Constrained != Index_Constrained {
Error_Msg_Parse
("cannot mix constrained and unconstrained index");
end if;
end if;
exit when scanner.currentContext.token != Token::Comma;
scanner.scan();
end loop;

//  Skip ')' and 'of'
Expect (Token::Right_Paren);
Scan_Expect (Token::Of);
scanner.scan();

Element_Subtype := Parse_Subtype_Indication;

if Array_Constrained {
//  Sem_Type will create the array type.
Res_Type := Create_Iir (Iir_Array_Subtype_Definition);
Set_Array_Element_Constraint (Res_Type, Element_Subtype);
Set_Index_Constraint_List (Res_Type, Index_List);
else
Res_Type := Create_Iir (Iir_Array_Type_Definition);
Set_Element_Subtype_Indication (Res_Type, Element_Subtype);
Set_Index_Subtype_Definition_List (Res_Type, Index_List);
end if;
Set_Location (Res_Type, Loc);

return Res_Type;
end Parse_Array_Definition;

//  precond : UNITS
//  postcond: next token
//
//  [ LRM93 3.1.3 ]
//  physical_type_definition ::=
//      range_constraint
        --          UNITS
//              base_unit_declaration
//              { secondary_unit_declaration }
//          END UNITS [ PHYSICAL_TYPE_simple_name ]
//
//  [ LRM93 3.1.3 ]
//  base_unit_declaration ::= identifier ;
//
//  [ LRM93 3.1.3 ]
//  secondary_unit_declaration ::= identifier = physical_literal ;
Parse_Physical_Type_Definition (Parent : Iir)
return Iir_Physical_Type_Definition
        is
use Iir_Chains.Unit_Chain_Handling;
Res: Iir_Physical_Type_Definition;
Unit: Iir_Unit_Declaration;
Last : Iir_Unit_Declaration;
Multiplier : Iir;
begin
        Res := Create_Iir (Iir_Physical_Type_Definition);
Set_Location (Res);

//  Skip 'units'
Expect (Token::Units);
scanner.scan();

//  Parse primary unit.
Expect (Token::Identifier);
Unit := Create_Iir (Iir_Unit_Declaration);
Set_Location (Unit);
Set_Parent (Unit, Parent);
Set_Identifier (Unit, scanner.currentContext.Identifier);

//  Skip identifier
scanner.scan();

Scan_Semi_Colon ("primary unit");

Build_Init (Last);
Append (Last, Res, Unit);

//  Parse secondary units.
while scanner.currentContext.token != Token::End loop
Unit := Create_Iir (Iir_Unit_Declaration);
Set_Location (Unit);
Set_Parent (Unit, Parent);
Set_Identifier (Unit, scanner.currentContext.Identifier);

//  Skip identifier.
Scan_Expect (Token::Equal);

//  Skip '='.
scanner.scan();

Multiplier := Parse_Primary;
Set_Physical_Literal (Unit, Multiplier);
switch (Get_Kind (Multiplier) is
        when Iir_Simple_Name
| Iir_Selected_Name
| Iir_Physical_Int_Literal:
null;
case Iir_Physical_Fp_Literal:
Error_Msg_Parse
("secondary units may only be defined with integer literals");
default:
throw std::runtime_error("PARSE ERROR: a physical literal is expected here");
}
Append (Last, Res, Unit);
Scan_Semi_Colon ("secondary unit");
end loop;

//  Skip 'end'.
scanner.scan();

Expect (Token::Units);
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'units'.
scanner.scan();
return Res;
end Parse_Physical_Type_Definition;

//  precond : RECORD
//  postcond: next token
//
//  [ LRM93 3.2.2 ]
//  record_type_definition ::=
//      RECORD
        --          element_declaration
//          { element_declaration }
//      END RECORD [ RECORD_TYPE_simple_name ]
//
//  element_declaration ::=
//      identifier_list : element_subtype_definition
//
//  element_subtype_definition ::= subtype_indication
Parse_Record_Type_Definition return Iir_Record_Type_Definition
        is
Res: Iir_Record_Type_Definition;
El_List : Iir_List;
El: Iir_Element_Declaration;
First : Iir;
Pos: Iir_Index32;
Subtype_Indication: Iir;
begin
        Res := Create_Iir (Iir_Record_Type_Definition);
Set_Location (Res);
El_List := Create_Iir_List;
Set_Elements_Declaration_List (Res, El_List);

//  Skip 'record'
scanner.scan();

Pos := 0;
First := nullptr;
loop
        pragma Assert (First = nullptr);
//  Parse identifier_list
loop
        El := Create_Iir (Iir_Element_Declaration);
Set_Location (El);
Set_Parent (El, Res);
if First = nullptr {
        First := El;
end if;
Expect (Token::Identifier);
Set_Identifier (El, scanner.currentContext.Identifier);
Append_Element (El_List, El);
Set_Element_Position (El, Pos);
Pos := Pos + 1;

//  Skip identifier
scanner.scan();

exit when scanner.currentContext.token != Token::Comma;

Set_Has_Identifier_List (El, true);

//  Skip ','
scanner.scan();
end loop;

//  Scan ':'.
Expect (Token::Colon);
scanner.scan();

//  Parse element subtype indication.
Subtype_Indication := Parse_Subtype_Indication;
Set_Subtype_Indication (First, Subtype_Indication);

First := nullptr;
Scan_Semi_Colon ("element declaration");
exit when scanner.currentContext.token = Token::End;
end loop;

//  Skip 'end'
Scan_Expect (Token::Record);
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'record'
scanner.scan();

return Res;
end Parse_Record_Type_Definition;

//  precond : ACCESS
//  postcond: ?
//
//  [ LRM93 3.3]
//  access_type_definition ::= ACCESS subtype_indication.
Parse_Access_Type_Definition return Iir_Access_Type_Definition
        is
Res : Iir_Access_Type_Definition;
begin
        Res := Create_Iir (Iir_Access_Type_Definition);
Set_Location (Res);

//  Skip 'access'
Expect (Token::Access);
scanner.scan();

Set_Designated_Subtype_Indication (Res, Parse_Subtype_Indication);

return Res;
end Parse_Access_Type_Definition;

//  precond : FILE
//  postcond: next token
//
//  [ LRM93 3.4 ]
//  file_type_definition ::= FILE OF type_mark
Parse_File_Type_Definition return Iir_File_Type_Definition
        is
Res : Iir_File_Type_Definition;
Type_Mark: Iir;
begin
        Res := Create_Iir (Iir_File_Type_Definition);
Set_Location (Res);
// Accept token 'file'.
Scan_Expect (Token::Of);
scanner.scan();
Type_Mark := Parse_Type_Mark (Check_Paren: true);
if Get_Kind (Type_Mark) not in Iir_Kinds_Denoting_Name {
        throw std::runtime_error("PARSE ERROR: type mark expected");
else
Set_File_Type_Mark (Res, Type_Mark);
end if;
return Res;
end Parse_File_Type_Definition;

//  precond : PROTECTED
//  postcond: ';'
//
//  [ 3.5 ]
//  protected_type_definition ::= protected_type_declaration
        --                              | protected_type_body
        --
        --  [ 3.5.1 ]
        --  protected_type_declaration ::= PROTECTED
        --                                     protected_type_declarative_part
//                                 END PROTECTED [ simple_name ]
//
//  protected_type_declarative_part ::=
//     { protected_type_declarative_item }
        --
        --  protected_type_declarative_item ::=
//       subprogram_declaration
        --     | attribute_specification
        --     | use_clause
        --
        --  [ 3.5.2 ]
        --  protected_type_body ::= PROTECTED BODY
//                              protected_type_body_declarative_part
//                          END PROTECTED BODY [ simple_name ]
//
//  protected_type_body_declarative_part ::=
//      { protected_type_body_declarative_item }
Parse_Protected_Type_Definition
        Iir (Ident : Name_Id; Loc : Location_Type) 
        is
Res : Iir;
Decl : Iir;
begin
        scanner.scan();
if (scanner.currentContext.token = Token::Body {
        Res := Create_Iir (Iir_Protected_Type_Body);
scanner.scan();
Decl := Res;
else
Decl := Create_Iir (Iir_Type_Declaration);
Res := Create_Iir (Iir_Protected_Type_Declaration);
Set_Location (Res, Loc);
Set_Type_Definition (Decl, Res);
end if;
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);

Parse_Declarative_Part (Res);

Expect (Token::End);
//  Eat 'end'.
scanner.scan();
if Flags.Vhdl_Std >= Vhdl_00 {
Expect (Token::Protected);
else
//  Avoid weird message: 'protected' expected instead of 'protected'.
Expect (Token::Identifier);
end if;
Set_End_Has_Reserved_Id (Res, true);
if Get_Kind (Res) = Iir_Protected_Type_Body {
        Scan_Expect (Token::Body);
end if;
scanner.scan();
Check_End_Name (Ident, Res);
return Decl;
end Parse_Protected_Type_Definition;

//  precond : TYPE
//  postcond: a token
//
//  [ LRM93 4.1 ]
//  type_definition ::= scalar_type_definition
        --                    | composite_type_definition
        --                    | access_type_definition
        --                    | file_type_definition
        --                    | protected_type_definition
        --
        --  [ LRM93 3.1 ]
//  scalar_type_definition ::= enumeration_type_definition
        --                           | integer_type_definition
        --                           | floating_type_definition
        --                           | physical_type_definition
        --
        --  [ LRM93 3.2 ]
//  composite_type_definition ::= array_type_definition
        --                              | record_type_definition
        --
        --  [ LRM93 3.1.2 ]
//  integer_type_definition ::= range_constraint
        --
        --  [ LRM93 3.1.4 ]
//  floating_type_definition ::= range_constraint
Iir Parse_Type_Declaration (Parent : Iir) 
        is
Def : Iir;
Loc : Location_Type;
Ident : Name_Id;
Decl : Iir;
begin
// The current token must be type.
pragma Assert (scanner.currentContext.token = Token::Type);

// Get the identifier
        Scan_Expect (Token::Identifier,
        "an identifier is expected after 'type' keyword");
Loc := Get_Token_Location;
Ident := scanner.currentContext.Identifier;

//  Skip identifier
scanner.scan();

if (scanner.currentContext.token = Token::Semi_Colon {
//  If there is a ';', this is an imcomplete type declaration.
Invalidate_scanner.currentContext.token;
Decl := Create_Iir (Iir_Type_Declaration);
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);
return Decl;
end if;

if (scanner.currentContext.token != Token::Is {
throw std::runtime_error("PARSE ERROR: 'is' expected here");
//  Act as if IS token was forgotten.
else
//  Eat IS token.
scanner.scan();
end if;

switch (scanner.currentContext.token) {
case Token::Left_Paren:
//  This is an enumeration.
Def := Parse_Enumeration_Type_Definition (Parent);
Decl := nullptr;

case Token::Range:
//  This is a range definition.
Decl := Create_Iir (Iir_Anonymous_Type_Declaration);
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);

//  Skip 'range'
scanner.scan();

Def := Parse_Range_Constraint;
Set_Type_Definition (Decl, Def);

if (scanner.currentContext.token = Token::Units {
//  A physical type definition.
declare
        Phys_Def : Iir;
begin
        Phys_Def := Parse_Physical_Type_Definition (Parent);
if (scanner.currentContext.token = Token::Identifier {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("simple_name not allowed here in vhdl87");
end if;
Check_End_Name (Get_Identifier (Decl), Phys_Def);
end if;
Set_Range_Constraint (Phys_Def, Def);
Set_Type_Definition (Decl, Phys_Def);
end;
end if;

case Token::Array:
Def := Parse_Array_Definition;
Decl := nullptr;

case Token::Record:
Decl := Create_Iir (Iir_Type_Declaration);
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);
Def := Parse_Record_Type_Definition;
Set_Type_Definition (Decl, Def);
if (scanner.currentContext.token = Token::Identifier {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: simple_name not allowed here in vhdl87");
end if;
Check_End_Name (Get_Identifier (Decl), Def);
end if;

case Token::Access:
Def := Parse_Access_Type_Definition;
Decl := nullptr;

case Token::File:
Def := Parse_File_Type_Definition;
Decl := nullptr;

case Token::Identifier:
if (scanner.currentContext.Identifier = Name_Protected {
        throw std::runtime_error("PARSE ERROR: protected type not allowed in vhdl87/93");
Decl := Parse_Protected_Type_Definition (Ident, Loc);
else
throw std::runtime_error("PARSE ERROR: type %i cannot be defined from another type",
(1: +Ident), Cont: true);
throw std::runtime_error("PARSE ERROR: (you should declare a subtype)");
Decl := Create_Iir (Iir_Type_Declaration);
Eat_Tokens_Until_Semi_Colon;
end if;

case Token::Protected:
if Flags.Vhdl_Std < Vhdl_00 {
        throw std::runtime_error("PARSE ERROR: protected type not allowed in vhdl87/93");
end if;
Decl := Parse_Protected_Type_Definition (Ident, Loc);

default:
Error_Msg_Parse
("type definition starting with a keyword such as RANGE, ARRAY");
Error_Msg_Parse
(" FILE, RECORD or '(' is expected here");
Eat_Tokens_Until_Semi_Colon;
Decl := Create_Iir (Iir_Type_Declaration);
}

if Decl = nullptr {
switch (Get_Kind (Def) is
        when Iir_Enumeration_Type_Definition
| Iir_Access_Type_Definition
| Iir_Array_Type_Definition
| Iir_File_Type_Definition:
Decl := Create_Iir (Iir_Type_Declaration);
case Iir_Array_Subtype_Definition:
Decl := Create_Iir (Iir_Anonymous_Type_Declaration);
default:
Error_Kind ("parse_type_declaration", Def);
}
Set_Type_Definition (Decl, Def);
end if;
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);

// ';' is expected after end of type declaration
        Expect (Token::Semi_Colon);
Invalidate_scanner.currentContext.token;
return Decl;
end Parse_Type_Declaration;

//  precond: '(' or identifier
//  postcond: next token
//
//  [ LRM08 6.3 ]
//
//  resolution_indication ::=
//      resolution_function_name | ( element_resolution )
        --
        --  element_resolution ::=
//      array_element_resolution | record_resolution
        --
        --  array_element_resolution ::= resolution_indication
        --
        --  record_resolution ::=
//      record_element_resolution { , record_element_resolution }
        --
        --  record_element_resolution ::=
//      record_element_simple_name resolution_indication
        function Parse_Resolution_Indication return Iir
        is
Ind : Iir;
Def : Iir;
Loc : Location_Type;
begin
if (scanner.currentContext.token = Token::Identifier {
//  Resolution function name.
return Parse_Name (Allow_Indexes: false);
elsif (scanner.currentContext.token = Token::Left_Paren {
//  Element resolution.
Loc := Get_Token_Location;

//  Eat '('
scanner.scan();

Ind := Parse_Resolution_Indication;
if (scanner.currentContext.token = Token::Identifier
        or else scanner.currentContext.token = Token::Left_Paren
{
        declare
Id : Name_Id;
El : Iir;
First, Last : Iir;
begin
//  This was in fact a record_resolution.
if Get_Kind (Ind) = Iir_Simple_Name {
        Id := Get_Identifier (Ind);
else
throw std::runtime_error("PARSE ERROR:" + +Ind, "element name expected");
Id := Null_Identifier;
end if;
Free_Iir (Ind);

Def := Create_Iir (Iir_Record_Resolution);
Set_Location (Def, Loc);
Sub_Chain_Init (First, Last);
loop
        El := Create_Iir (Iir_Record_Element_Resolution);
Set_Location (El, Loc);
Set_Identifier (El, Id);
Set_Resolution_Indication (El, Parse_Resolution_Indication);
Sub_Chain_Append (First, Last, El);
exit when scanner.currentContext.token = Token::Right_Paren;

//  Eat ','
Expect (Token::Comma);
scanner.scan();

if (scanner.currentContext.token != Token::Identifier {
throw std::runtime_error("PARSE ERROR: record element identifier expected");
exit;
end if;
Id := scanner.currentContext.Identifier;
Loc := Get_Token_Location;

//  Eat identifier
scanner.scan();
end loop;
Set_Record_Element_Resolution_Chain (Def, First);
end;
else
Def := Create_Iir (Iir_Array_Element_Resolution);
Set_Location (Def, Loc);
Set_Resolution_Indication (Def, Ind);
end if;

//  Eat ')'
Expect (Token::Right_Paren);
scanner.scan();

return Def;
else
throw std::runtime_error("PARSE ERROR: resolution indication expected");
raise Parse_Error;
end if;
end Parse_Resolution_Indication;

//  precond : '('
//  postcond: next token
//
//  [ LRM08 6.3 Subtype declarations ]
//  element_constraint ::=
//      array_constraint | record_constraint
        --
        --  [ LRM08 5.3.2.1 Array types ]
//  array_constraint ::=
//      index_constraint [ array_element_constraint ]
        --      | ( open ) [ array_element_constraint ]
        --
        --  array_element_constraint ::= element_constraint
        --
        --  RES is the resolution_indication of the subtype indication.
Parse_Element_Constraint return Iir
        is
Def : Iir;
El : Iir;
Index_List : Iir_List;
begin
//  Index_constraint.
Def := Create_Iir (Iir_Array_Subtype_Definition);
Set_Location (Def);

//  Eat '('.
scanner.scan();

if (scanner.currentContext.token = Token::Open {
//  Eat 'open'.
scanner.scan();
else
Index_List := Create_Iir_List;
Set_Index_Constraint_List (Def, Index_List);
//  index_constraint ::= (discrete_range {, discrete_range} )
loop
        El := Parse_Discrete_Range;
Append_Element (Index_List, El);

exit when scanner.currentContext.token = Token::Right_Paren;

//  Eat ','
Expect (Token::Comma);
scanner.scan();
end loop;
end if;

//  Eat ')'
Expect (Token::Right_Paren);
scanner.scan();

if (scanner.currentContext.token = Token::Left_Paren {
        Set_Array_Element_Constraint (Def, Parse_Element_Constraint);
end if;
return Def;
end Parse_Element_Constraint;

//  precond : tolerance
//  postcond: next token
//
//  [ LRM93 4.2 ]
//  tolerance_aspect ::= TOLERANCE string_expression
        function Parse_Tolerance_Aspect_Opt return Iir is
begin
if AMS_Vhdl
        && scanner.currentContext.token = Token::Tolerance
{
        scanner.scan();
return Parse_Expression;
else
return nullptr;
end if;
end Parse_Tolerance_Aspect_Opt;

//  precond : identifier or '('
//  postcond: next token
//
//  [ LRM93 4.2 ]
//  subtype_indication ::=
//      [ RESOLUTION_FUNCTION_name ] type_mark [ constraint ]
//
//  constraint ::= range_constraint | index_constraint
        --
        --  [ LRM08 6.3 ]
//  subtype_indication ::=
//      [ resolution_indication ] type_mark [ constraint ]
//
//  constraint ::=
//      range_constraint | array_constraint | record_constraint
        --
        --  NAME is the type_mark when already parsed (in range expression or
//   allocator by type).
Parse_Subtype_Indication (Name : Iir := nullptr)
return Iir
        is
Type_Mark : Iir;
Def: Iir;
Resolution_Indication: Iir;
Tolerance : Iir;
begin
// FIXME: location.
Resolution_Indication := nullptr;
Def := nullptr;

if Name != nullptr {
//  The type_mark was already parsed.
Type_Mark := Name;
Check_Type_Mark (Name);
else
if (scanner.currentContext.token = Token::Left_Paren {
if Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("resolution_indication not allowed before vhdl08");
end if;
Resolution_Indication := Parse_Resolution_Indication;
end if;
if (scanner.currentContext.token != Token::Identifier {
throw std::runtime_error("PARSE ERROR: type mark expected in a subtype indication");
raise Parse_Error;
end if;
Type_Mark := Parse_Type_Mark (Check_Paren: false);
end if;

if (scanner.currentContext.token = Token::Identifier {
if Resolution_Indication != nullptr {
throw std::runtime_error("PARSE ERROR: resolution function already indicated");
end if;
Resolution_Indication := Type_Mark;
Type_Mark := Parse_Type_Mark (Check_Paren: false);
end if;

switch (scanner.currentContext.token) {
case Token::Left_Paren:
//  element_constraint.
Def := Parse_Element_Constraint;
Set_Subtype_Type_Mark (Def, Type_Mark);
Set_Resolution_Indication (Def, Resolution_Indication);
Set_Tolerance (Def, Parse_Tolerance_Aspect_Opt);

case Token::Range:
//  range_constraint.
//  Skip 'range'
scanner.scan();

Def := Parse_Range_Constraint_Of_Subtype_Indication
        (Type_Mark, Resolution_Indication);

default:
Tolerance := Parse_Tolerance_Aspect_Opt;
if Resolution_Indication != nullptr
or else Tolerance != nullptr
        {
//  A subtype needs to be created.
Def := Create_Iir (Iir_Subtype_Definition);
Location_Copy (Def, Type_Mark);
Set_Subtype_Type_Mark (Def, Type_Mark);
Set_Resolution_Indication (Def, Resolution_Indication);
Set_Tolerance (Def, Tolerance);
else
//  This is just an alias.
Def := Type_Mark;
end if;
}
return Def;
end Parse_Subtype_Indication;

//  precond : SUBTYPE
//  postcond: ';'
//
//  [ 4.2 ]
//  subtype_declaration ::= SUBTYPE identifier IS subtype_indication ;
Parse_Subtype_Declaration (Parent : Iir)
return Iir_Subtype_Declaration
        is
Decl: Iir_Subtype_Declaration;
Def: Iir;
begin
        Decl := Create_Iir (Iir_Subtype_Declaration);

Scan_Expect (Token::Identifier);
Set_Identifier (Decl, scanner.currentContext.Identifier);
Set_Parent (Decl, Parent);
Set_Location (Decl);

//  Skip identifier.
Scan_Expect (Token::Is);

//  Skip 'is'.
scanner.scan();

Def := Parse_Subtype_Indication;
Set_Subtype_Indication (Decl, Def);

Expect (Token::Semi_Colon);
return Decl;
end Parse_Subtype_Declaration;

//  precond : NATURE
//  postcond: a token
//
//  [ 4.8 ]
//  nature_definition ::= scalar_nature_definition
        --                    | composite_nature_definition
        --
        --  [ 3.5.1 ]
//  scalar_nature_definition ::= type_mark ACROSS
//                               type_mark THROUGH
//                               identifier REFERENCE
//
//  [ 3.5.2 ]
//  composite_nature_definition ::= array_nature_definition
        --                              | record_nature_definition
Parse_Nature_Declaration return Iir
        is
Def : Iir;
Ref : Iir;
Loc : Location_Type;
Ident : Name_Id;
Decl : Iir;
begin
// The current token must be type.
if (scanner.currentContext.token != Token::Nature {
raise Program_Error;
end if;

// Get the identifier
        Scan_Expect (Token::Identifier,
        "an identifier is expected after 'nature'");
Loc := Get_Token_Location;
Ident := scanner.currentContext.Identifier;

scanner.scan();

if (scanner.currentContext.token != Token::Is {
throw std::runtime_error("PARSE ERROR: 'is' expected here");
//  Act as if IS token was forgotten.
else
//  Eat IS token.
scanner.scan();
end if;

switch (scanner.currentContext.token) {
case Token::Array:
//  TODO
        throw std::runtime_error("PARSE ERROR: array nature definition not supported");
Def := nullptr;
Eat_Tokens_Until_Semi_Colon;
case Token::Record:
//  TODO
        throw std::runtime_error("PARSE ERROR: record nature definition not supported");
Def := nullptr;
Eat_Tokens_Until_Semi_Colon;
case Token::Identifier:
Def := Create_Iir (Iir_Scalar_Nature_Definition);
Set_Location (Def, Loc);
Set_Across_Type (Def, Parse_Type_Mark);
if (scanner.currentContext.token = Token::Across {
        scanner.scan();
else
Expect (Token::Across, "'across' expected after type mark");
end if;
Set_Through_Type (Def, Parse_Type_Mark);
if (scanner.currentContext.token = Token::Through {
        scanner.scan();
else
Expect (Token::Across, "'through' expected after type mark");
end if;
if (scanner.currentContext.token = Token::Identifier {
        Ref := Create_Iir (Iir_Terminal_Declaration);
Set_Identifier (Ref, scanner.currentContext.Identifier);
Set_Location (Ref);
Set_Reference (Def, Ref);
scanner.scan();
if (scanner.currentContext.token = Token::Reference {
        scanner.scan();
else
Expect (Token::Reference, "'reference' expected");
Eat_Tokens_Until_Semi_Colon;
end if;
else
throw std::runtime_error("PARSE ERROR: reference identifier expected");
Eat_Tokens_Until_Semi_Colon;
end if;
default:
throw std::runtime_error("PARSE ERROR: nature definition expected here");
Eat_Tokens_Until_Semi_Colon;
}

Decl := Create_Iir (Iir_Nature_Declaration);
Set_Nature (Decl, Def);
Set_Identifier (Decl, Ident);
Set_Location (Decl, Loc);

// ';' is expected after end of type declaration
        Expect (Token::Semi_Colon);
Invalidate_scanner.currentContext.token;
return Decl;
end Parse_Nature_Declaration;

//  precond : identifier
//  postcond: next token
//
//  LRM 4.8 Nature declaration
//
//  subnature_indication ::=
//      nature_mark [ index_constraint ]
        --      [ TOLERANCE string_expression ACROSS string_expression THROUGH ]
//
//  nature_mark ::=
//      nature_name | subnature_name
Parse_Subnature_Indication return Iir is
Nature_Mark : Iir;
begin
if (scanner.currentContext.token != Token::Identifier {
throw std::runtime_error("PARSE ERROR: nature mark expected in a subnature indication");
raise Parse_Error;
end if;
Nature_Mark := Parse_Name (Allow_Indexes: false);

if (scanner.currentContext.token = Token::Left_Paren {
//  TODO
        Error_Msg_Parse
        ("index constraint not supported for subnature indication");
raise Parse_Error;
end if;

if (scanner.currentContext.token = Token::Tolerance {
        Error_Msg_Parse
        ("tolerance not supported for subnature indication");
raise Parse_Error;
end if;
return Nature_Mark;
end Parse_Subnature_Indication;

//  precond : TERMINAL
//  postcond: ;
//
//  [ 4.3.1.5 Terminal declarations ]
//  terminal_declaration ::=
//      TERMINAL identifier_list : subnature_indication
Iir Parse_Terminal_Declaration (Parent : Iir) 
        is
//  First and last element of the chain to be returned.
First, Last : Iir;
Terminal : Iir;
Subnature : Iir;
begin
        Sub_Chain_Init (First, Last);

loop
// 'terminal' or "," was just scanned.
Terminal := Create_Iir (Iir_Terminal_Declaration);
Scan_Expect (Token::Identifier);
Set_Identifier (Terminal, scanner.currentContext.Identifier);
Set_Location (Terminal);
Set_Parent (Terminal, Parent);

Sub_Chain_Append (First, Last, Terminal);

scanner.scan();
exit when scanner.currentContext.token = Token::Colon;
if (scanner.currentContext.token != Token::Comma {
Error_Msg_Parse
("',' or ':' is expected after "
& "identifier in terminal declaration");
raise Expect_Error;
end if;
end loop;

// The colon was parsed.
scanner.scan();
Subnature := Parse_Subnature_Indication;

Terminal := First;
while Terminal != nullptr loop
// Type definitions are factorized.  This is OK, but not done by
// sem.
if Terminal = First {
        Set_Nature (Terminal, Subnature);
else
Set_Nature (Terminal, nullptr);
end if;
Terminal := Get_Chain (Terminal);
end loop;
Expect (Token::Semi_Colon);
return First;
end Parse_Terminal_Declaration;

//  precond : QUANTITY
//  postcond: ;
//
//  [ 4.3.1.6 Quantity declarations ]
//  quantity_declaration ::=
//      free_quantity_declaration
        --      | branch_quantity_declaration
        --      | source_quantity_declaration
        --
        --  free_quantity_declaration ::=
//      QUANTITY identifier_list : subtype_indication [ := expression ] ;
//
//  branch_quantity_declaration ::=
//      QUANTITY [ across_aspect ] [ through_aspect ] terminal_aspect ;
//
//  source_quantity_declaration ::=
//      QUANTITY identifier_list : subtype_indication source_aspect ;
//
//  across_aspect ::=
//      identifier_list [ tolerance_aspect ] [ := expression ] ACROSS
//
//  through_aspect ::=
//      identifier_list [ tolerance_aspect ] [ := expression ] THROUGH
//
//  terminal_aspect ::=
//      plus_terminal_name [ TO minus_terminal_name ]
Iir Parse_Quantity_Declaration (Parent : Iir) 
        is
//  First and last element of the chain to be returned.
First, Last : Iir;
Object : Iir;
New_Object : Iir;
Tolerance : Iir;
Default_Value : Iir;
Kind : Iir_Kind;
Plus_Terminal : Iir;
begin
        Sub_Chain_Init (First, Last);

//  Eat 'quantity'
scanner.scan();

loop
//  Quantity or "," was just scanned.  We assume a free quantity
//  declaration and will change to branch or source quantity if
//  necessary.
Object := Create_Iir (Iir_Free_Quantity_Declaration);
Expect (Token::Identifier);
Set_Identifier (Object, scanner.currentContext.Identifier);
Set_Location (Object);
Set_Parent (Object, Parent);

Sub_Chain_Append (First, Last, Object);

//  Eat identifier
scanner.scan();
exit when scanner.currentContext.token != Token::Comma;

//  Eat ','
scanner.scan();
end loop;

switch (scanner.currentContext.token) {
case Token::Colon:
//  Either a free quantity (or a source quantity)
//  TODO
        raise Program_Error;
case Token::Tolerance
| Token::Assign
| Token::Across
| Token::Through:
//  A branch quantity

//  Parse tolerance aspect
        Tolerance := Parse_Tolerance_Aspect_Opt;

//  Parse default value
if (scanner.currentContext.token = Token::Assign {
        scanner.scan();
Default_Value := Parse_Expression;
else
Default_Value := nullptr;
end if;

switch (scanner.currentContext.token) {
case Token::Across:
Kind := Iir_Across_Quantity_Declaration;
case Token::Through:
Kind := Iir_Through_Quantity_Declaration;
default:
throw std::runtime_error("PARSE ERROR: 'across' or 'through' expected here");
Eat_Tokens_Until_Semi_Colon;
raise Expect_Error;
}

//  Eat across/through
        scanner.scan();

//  Change declarations
Object := First;
Sub_Chain_Init (First, Last);
while Object != nullptr loop
New_Object := Create_Iir (Kind);
Location_Copy (New_Object, Object);
Set_Identifier (New_Object, Get_Identifier (Object));
Set_Parent (New_Object, Parent);
Set_Tolerance (New_Object, Tolerance);
Set_Default_Value (New_Object, Default_Value);

Sub_Chain_Append (First, Last, New_Object);

if Object != First {
Set_Plus_Terminal (New_Object, nullptr);
end if;
New_Object := Get_Chain (Object);
Free_Iir (Object);
Object := New_Object;
end loop;

//  Parse terminal (or first identifier of through declarations)
Plus_Terminal := Parse_Name;

switch (scanner.currentContext.token) {
case Token::Comma
| Token::Tolerance
| Token::Assign
| Token::Through
| Token::Across:
//  Through quantity declaration.  Convert the Plus_Terminal
//  to a declaration.
Object := Create_Iir (Iir_Through_Quantity_Declaration);
New_Object := Object;
Location_Copy (Object, Plus_Terminal);
if Get_Kind (Plus_Terminal) != Iir_Simple_Name {
Error_Msg_Parse
("identifier for quantity declaration expected");
else
Set_Identifier (Object, Get_Identifier (Plus_Terminal));
end if;
Set_Plus_Terminal (Object, nullptr);
Free_Iir (Plus_Terminal);

loop
        Set_Parent (Object, Parent);
Sub_Chain_Append (First, Last, Object);
exit when scanner.currentContext.token != Token::Comma;
scanner.scan();

Object := Create_Iir
        (Iir_Through_Quantity_Declaration);
Set_Location (Object);
if (scanner.currentContext.token != Token::Identifier {
Error_Msg_Parse
("identifier for quantity declaration expected");
else
Set_Identifier (Object, scanner.currentContext.Identifier);
scanner.scan();
end if;
Set_Plus_Terminal (Object, nullptr);

end loop;

//  Parse tolerance aspect
        Set_Tolerance (Object, Parse_Tolerance_Aspect_Opt);

//  Parse default value
if (scanner.currentContext.token = Token::Assign {
        scanner.scan();
Set_Default_Value (Object, Parse_Expression);
end if;

//  Scan 'through'
if (scanner.currentContext.token = Token::Through {
        scanner.scan();
elsif (scanner.currentContext.token = Token::Across {
        throw std::runtime_error("PARSE ERROR: across quantity declaration must appear"
        & " before though declaration");
scanner.scan();
else
throw std::runtime_error("PARSE ERROR: 'through' expected");
end if;

//  Parse plus terminal
        Plus_Terminal := Parse_Name;
default:
null;
}

Set_Plus_Terminal (First, Plus_Terminal);

//  Parse minus terminal (if present)
if (scanner.currentContext.token = Token::To {
        scanner.scan();
Set_Minus_Terminal (First, Parse_Name);
end if;
default:
throw std::runtime_error("PARSE ERROR: missing type or across/throught aspect "
& "in quantity declaration");
Eat_Tokens_Until_Semi_Colon;
raise Expect_Error;
}
Expect (Token::Semi_Colon);
return First;
end Parse_Quantity_Declaration;

//  precond : token (CONSTANT, SIGNAL, VARIABLE, FILE)
//  postcond: ;
//
//  KIND can be iir_kind_constant_declaration, iir_kind_file_declaration
//   or iir_kind_variable_declaration
//
//  [ LRM93 4.3.1 ]
//  object_declaration ::= constant_declaration
        --                       | signal_declaration
        --                       | variable_declaration
        --                       | file_declaration
        --
        --  [ LRM93 4.3.1.1 ]
//  constant_declaration ::=
//      CONSTANT identifier_list : subtype_indication [ := expression ]
//
//  [ LRM87 4.3.2 ]
//  file_declaration ::=
//      FILE identifier : subtype_indication IS [ mode ] file_logical_name
//
//  [ LRM93 4.3.1.4 ]
//  file_declaration ::=
//      FILE identifier_list : subtype_indication [ file_open_information ]
//
//  [ LRM93 4.3.1.4 ]
//  file_open_information ::=
//      [ OPEN FILE_OPEN_KIND_expression ] IS file_logical_name
//
//  [ LRM93 4.3.1.4 ]
//  file_logical_name ::= STRING_expression
        --
        --  [ LRM93 4.3.1.3 ]
//  variable_declaration ::=
//      [ SHARED ] VARIABLE identifier_list : subtype_indication
//          [ := expression ]
//
//  [ LRM93 4.3.1.2 ]
//  signal_declaration ::=
//      SIGNAL identifier_list : subtype_information [ signal_kind ]
//          [ := expression ]
//
//  [ LRM93 4.3.1.2 ]
//  signal_kind ::= REGISTER | BUS
        --
        --  FIXME: file_open_information.
Iir Parse_Object_Declaration (Parent : Iir) 
        is
//  First and last element of the chain to be returned.
First, Last : Iir;
Object: Iir;
Object_Type: Iir;
Default_Value : Iir;
Mode: Iir_Mode;
Signal_Kind : Iir_Signal_Kind;
Is_Guarded : Boolean;
Open_Kind : Iir;
Logical_Name : Iir;
Kind: Iir_Kind;
Shared : Boolean;
Has_Mode : Boolean;
begin
        Sub_Chain_Init (First, Last);

// object keyword was just scanned.
switch (scanner.currentContext.token) {
case Token::Signal:
Kind := Iir_Signal_Declaration;
case Token::Constant:
Kind := Iir_Constant_Declaration;
case Token::File:
Kind := Iir_File_Declaration;
case Token::Variable:
Kind := Iir_Variable_Declaration;
Shared := false;
case Token::Shared:
Kind := Iir_Variable_Declaration;
Shared := true;
Scan_Expect (Token::Variable);
default:
raise Internal_Error;
}

loop
// object or "," was just scanned.
Object := Create_Iir (Kind);
if Kind = Iir_Variable_Declaration {
        Set_Shared_Flag (Object, Shared);
end if;
Scan_Expect (Token::Identifier);
Set_Identifier (Object, scanner.currentContext.Identifier);
Set_Location (Object);
Set_Parent (Object, Parent);

Sub_Chain_Append (First, Last, Object);

scanner.scan();
exit when scanner.currentContext.token = Token::Colon;
if (scanner.currentContext.token != Token::Comma {
switch (scanner.currentContext.token) {
case Token::Assign:
throw std::runtime_error("PARSE ERROR: missing type in " & Disp_Name (Kind));
exit;
default:
Error_Msg_Parse
("',' or ':' is expected after identifier in "
& Disp_Name (Kind));
raise Expect_Error;
}
end if;
Set_Has_Identifier_List (Object, true);
end loop;

// Eat ':'
scanner.scan();

Object_Type := Parse_Subtype_Indication;

if Kind = Iir_Signal_Declaration {
        Parse_Signal_Kind (Is_Guarded, Signal_Kind);
end if;

if (scanner.currentContext.token = Token::Assign {
if Kind = Iir_File_Declaration {
        Error_Msg_Parse
        ("default expression not allowed for a file declaration");
end if;

//  Skip ':='.
scanner.scan();

Default_Value := Parse_Expression;
elsif (scanner.currentContext.token = Token::Equal {
        throw std::runtime_error("PARSE ERROR: = should be := for initial value");

//  Skip '='
scanner.scan();

Default_Value := Parse_Expression;
else
Default_Value := nullptr;
end if;

if Kind = Iir_File_Declaration {
if (scanner.currentContext.token = Token::Open {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'open' and open kind expression not allowed in vhdl 87");
end if;
scanner.scan();
Open_Kind := Parse_Expression;
else
Open_Kind := nullptr;
end if;

//  LRM 4.3.1.4
//  The default mode is IN, if no mode is specified.
Mode := Iir_In_Mode;

Logical_Name := nullptr;
Has_Mode := false;
if (scanner.currentContext.token = Token::Is {
//  Skip 'is'.
scanner.scan();

switch (scanner.currentContext.token) {
case Token::In | Token::Out | Token::Inout:
if Flags.Vhdl_Std >= Vhdl_93 {
throw std::runtime_error("PARSE ERROR: mode allowed only in vhdl 87");
end if;
Mode := Parse_Mode;
if Mode = Iir_Inout_Mode {
        throw std::runtime_error("PARSE ERROR: inout mode not allowed for file");
end if;
Has_Mode := true;
default:
null;
}
Logical_Name := Parse_Expression;
elsif Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: file name expected (vhdl 87)");
end if;
end if;

Set_Subtype_Indication (First, Object_Type);
if Kind != Iir_File_Declaration {
Set_Default_Value (First, Default_Value);
end if;

Object := First;
while Object != nullptr loop
switch (Kind is
case Iir_File_Declaration:
Set_Mode (Object, Mode);
Set_File_Open_Kind (Object, Open_Kind);
Set_File_Logical_Name (Object, Logical_Name);
Set_Has_Mode (Object, Has_Mode);
case Iir_Signal_Declaration:
Set_Guarded_Signal_Flag (Object, Is_Guarded);
Set_Signal_Kind (Object, Signal_Kind);
default:
null;
}
Object := Get_Chain (Object);
end loop;

//  ';' is not eaten.
Expect (Token::Semi_Colon);

return First;
end Parse_Object_Declaration;

//  precond : COMPONENT
//  postcond: ';'
//
//  [ LRM93 4.5 ]
//  component_declaration ::=
//      COMPONENT identifier [ IS ]
//          [ LOCAL_generic_clause ]
//          [ LOCAL_port_clause ]
//      END COMPONENT [ COMPONENT_simple_name ] ;
Parse_Component_Declaration
return Iir_Component_Declaration
        is
Component: Iir_Component_Declaration;
begin
        Component := Create_Iir (Iir_Component_Declaration);
Scan_Expect (Token::Identifier,
"an identifier is expected after 'component'");
Set_Identifier (Component, scanner.currentContext.Identifier);
Set_Location (Component);
scanner.scan();
if (scanner.currentContext.token = Token::Is {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: ""is"" keyword is not allowed here by vhdl 87");
end if;
Set_Has_Is (Component, true);
scanner.scan();
end if;
Parse_Generic_Port_Clauses (Component);
Check_End_Name (Token::Component, Component);
return Component;
end Parse_Component_Declaration;

//  precond : '['
//  postcond: next token after ']'
//
//  [ 2.3.2 ]
//  signature ::= [ [ type_mark { , type_mark } ] [ RETURN type_mark ] ]
Parse_Signature return Iir_Signature
        is
Res : Iir_Signature;
List : Iir_List;
begin
        Expect (Token::Left_Bracket);
Res := Create_Iir (Iir_Signature);
Set_Location (Res);

//  Skip '['
scanner.scan();

//  List of type_marks.
if (scanner.currentContext.token = Token::Identifier {
        List := Create_Iir_List;
Set_Type_Marks_List (Res, List);
loop
        Append_Element (List, Parse_Type_Mark (Check_Paren: true));
exit when scanner.currentContext.token != Token::Comma;
scanner.scan();
end loop;
end if;

if (scanner.currentContext.token = Token::Return {
//  Skip 'return'
scanner.scan();

Set_Return_Type_Mark (Res, Parse_Name);
end if;

//  Skip ']'
Expect (Token::Right_Bracket);
scanner.scan();

return Res;
end Parse_Signature;

//  precond : ALIAS
//  postcond: a token
//
//  [ LRM93 4.3.3 ]
//  alias_declaration ::=
//      ALIAS alias_designator [ : subtype_indication ]
//          IS name [ signature ] ;
//
//  [ LRM93 4.3.3 ]
//  alias_designator ::= identifier | character_literal | operator_symbol
        --
        --  FIXME: signature is not part of the node.
Parse_Alias_Declaration return Iir
        is
Res: Iir;
Ident : Name_Id;
begin
//  Eat 'alias'.
scanner.scan();

Res := Create_Iir (Iir_Object_Alias_Declaration);
Set_Location (Res);

switch (scanner.currentContext.token) {
case Token::Identifier:
Ident := scanner.currentContext.Identifier;
case Token::Character:
Ident := scanner.currentContext.Identifier;
case Token::String:
Ident := Scan_To_Operator_Name (Get_Token_Location);
//  FIXME: vhdl87
//  FIXME: operator symbol.
default:
throw std::runtime_error("PARSE ERROR: alias designator expected");
}

//  Eat identifier.
Set_Identifier (Res, Ident);
scanner.scan();

if (scanner.currentContext.token = Token::Colon {
        scanner.scan();
Set_Subtype_Indication (Res, Parse_Subtype_Indication);
end if;

//  FIXME: nice message if token is ':=' ?
Expect (Token::Is);
scanner.scan();
Set_Name (Res, Parse_Name);

return Res;
end Parse_Alias_Declaration;

//  precond : FOR
//  postcond: ';'
//
//  [ 5.2 ]
//  configuration_specification ::=
//      FOR component_specification binding_indication ;
Parse_Configuration_Specification
return Iir_Configuration_Specification
        is
Res : Iir_Configuration_Specification;
begin
        Res := Create_Iir (Iir_Configuration_Specification);
Set_Location (Res);
Expect (Token::For);
scanner.scan();
Parse_Component_Specification (Res);
Set_Binding_Indication (Res, Parse_Binding_Indication);
Expect (Token::Semi_Colon);
return Res;
end Parse_Configuration_Specification;

//  precond : next token
//  postcond: next token
//
//  [  5.2 ]
//  entity_class := ENTITY | ARCHITECTURE | CONFIGURATION | PROCEDURE
        --                | FUNCTION | PACKAGE | TYPE | SUBTYPE | CONSTANT
        --                | SIGNAL | VARIABLE | COMPONENT | LABEL | LITERAL
        --                | UNITS | GROUP | FILE
Parse_Entity_Class return Token_Type
        is
Res : Token_Type;
begin
switch (scanner.currentContext.token) {
case Token::Entity
| Token::Architecture
| Token::Configuration
| Token::Procedure
| Token::Function
| Token::Package
| Token::Type
| Token::Subtype
| Token::Constant
| Token::Signal
| Token::Variable
| Token::Component
| Token::Label:
null;
case Token::Literal
| Token::Units
| Token::Group
| Token::File:
null;
default:
throw std::runtime_error("PARSE ERROR: %t is not a entity class", +scanner.currentContext.token);
}
Res := scanner.currentContext.token;
scanner.scan();
return Res;
end Parse_Entity_Class;

Parse_Entity_Class_Entry return Iir_Entity_Class
        is
Res : Iir_Entity_Class;
begin
        Res := Create_Iir (Iir_Entity_Class);
Set_Location (Res);
Set_Entity_Class (Res, Parse_Entity_Class);
return Res;
end Parse_Entity_Class_Entry;

//  precond : next token
//  postcond: next token
//
//  [ 5.1 ]
//  entity_designator ::= entity_tag [ signature ]
        --
        --  entity_tag ::= simple_name | character_literal | operator_symbol
Parse_Entity_Designator return Iir
        is
Res : Iir;
Name : Iir;
begin
switch (scanner.currentContext.token) {
case Token::Identifier:
Res := Create_Iir (Iir_Simple_Name);
Set_Location (Res);
Set_Identifier (Res, scanner.currentContext.Identifier);
case Token::Character:
Res := Create_Iir (Iir_Character_Literal);
Set_Location (Res);
Set_Identifier (Res, scanner.currentContext.Identifier);
case Token::String:
Res := Create_Iir (Iir_Operator_Symbol);
Set_Location (Res);
Set_Identifier (Res, Scan_To_Operator_Name (Get_Token_Location));
default:
throw std::runtime_error("PARSE ERROR: identifier, character or string expected");
raise Expect_Error;
}
scanner.scan();
if (scanner.currentContext.token = Token::Left_Bracket {
        Name := Res;
Res := Parse_Signature;
Set_Signature_Prefix (Res, Name);
end if;
return Res;
end Parse_Entity_Designator;

//  precond : next token
//  postcond: IS
//
//  [ 5.1 ]
//  entity_name_list ::= entity_designator { , entity_designator }
        --                     | OTHERS
        --                     | ALL
 Parse_Entity_Name_List
        (Attribute : Iir_Attribute_Specification)
is
        List : Iir_List;
El : Iir;
begin
switch (scanner.currentContext.token) {
case Token::All:
List := Iir_List_All;
scanner.scan();
case Token::Others:
List := Iir_List_Others;
scanner.scan();
default:
List := Create_Iir_List;
loop
        El := Parse_Entity_Designator;
Append_Element (List, El);
exit when scanner.currentContext.token != Token::Comma;
scanner.scan();
end loop;
}
Set_Entity_Name_List (Attribute, List);
if (scanner.currentContext.token = Token::Colon {
        scanner.scan();
Set_Entity_Class (Attribute, Parse_Entity_Class);
else
Error_Msg_Parse
("missing ':' and entity kind in attribute specification");
end if;
end Parse_Entity_Name_List;

//  precond : ATTRIBUTE
//  postcond: ';'
//
//  [ 4.4 ]
//  attribute_declaration ::= ATTRIBUTE identifier : type_mark ;
//
//  [ 5.1 ]
//  attribute_specification ::=
//     ATTRIBUTE attribute_designator OF entity_specification
//       IS expression ;
//
//  entity_specification ::= entity_name_list : entity_class
//
Parse_Attribute return Iir
        is
Loc : Location_Type;
Ident : Name_Id;
begin
        Expect (Token::Attribute);
Scan_Expect (Token::Identifier);
Loc := Get_Token_Location;
Ident := scanner.currentContext.Identifier;

//  Skip identifier.
scanner.scan();

switch (scanner.currentContext.token) {
case Token::Colon:
declare
        Res : Iir_Attribute_Declaration;
begin
        Res := Create_Iir (Iir_Attribute_Declaration);
Set_Location (Res, Loc);
Set_Identifier (Res, Ident);

//  Skip ':'.
scanner.scan();

Set_Type_Mark (Res, Parse_Type_Mark (Check_Paren: true));
Expect (Token::Semi_Colon);
return Res;
end;

case Token::Of:
declare
        Res : Iir_Attribute_Specification;
Designator : Iir_Simple_Name;
begin
        Res := Create_Iir (Iir_Attribute_Specification);
Set_Location (Res, Loc);
Designator := Create_Iir (Iir_Simple_Name);
Set_Location (Designator, Loc);
Set_Identifier (Designator, Ident);
Set_Attribute_Designator (Res, Designator);

//  Skip 'of'.
scanner.scan();

Parse_Entity_Name_List (Res);
Expect (Token::Is);

//  Skip 'is'.
scanner.scan();

Set_Expression (Res, Parse_Expression);
Expect (Token::Semi_Colon);
return Res;
end;
default:
throw std::runtime_error("PARSE ERROR: ':' or 'of' expected after identifier");
return nullptr;
}
end Parse_Attribute;

//  precond : GROUP
//  postcond: ';'
//
//  [ 4.6 ]
//  group_template_declaration ::=
//     GROUP identifier IS (entity_class_entry_list) ;
//
//  entity_class_entry_list ::= entity_class_entry { , entity_class_entry }
        --
        --  entity_class_entry ::= entity_class [ <> ]
Parse_Group return Iir is
Loc : Location_Type;
Ident : Name_Id;
begin
        Expect (Token::Group);
Scan_Expect (Token::Identifier);
Loc := Get_Token_Location;
Ident := scanner.currentContext.Identifier;
scanner.scan();
switch (scanner.currentContext.token) {
case Token::Is:
declare
        use Iir_Chains.Entity_Class_Entry_Chain_Handling;
Res : Iir_Group_Template_Declaration;
El : Iir_Entity_Class;
Last : Iir_Entity_Class;
begin
        Res := Create_Iir (Iir_Group_Template_Declaration);
Set_Location (Res, Loc);
Set_Identifier (Res, Ident);
Scan_Expect (Token::Left_Paren);
scanner.scan();
Build_Init (Last);
loop
        Append (Last, Res, Parse_Entity_Class_Entry);
if (scanner.currentContext.token = Token::Box {
        El := Create_Iir (Iir_Entity_Class);
Set_Location (El);
Set_Entity_Class (El, Token::Box);
Append (Last, Res, El);
scanner.scan();
if (scanner.currentContext.token = Token::Comma {
        Error_Msg_Parse
        ("'<>' is allowed only for the last "
                & "entity class entry");
end if;
end if;
exit when scanner.currentContext.token = Token::Right_Paren;
Expect (Token::Comma);
scanner.scan();
end loop;
Scan_Expect (Token::Semi_Colon);
return Res;
end;
case Token::Colon:
declare
        Res : Iir_Group_Declaration;
List : Iir_Group_Constituent_List;
begin
        Res := Create_Iir (Iir_Group_Declaration);
Set_Location (Res, Loc);
Set_Identifier (Res, Ident);
scanner.scan();
Set_Group_Template_Name
(Res, Parse_Name (Allow_Indexes: false));
Expect (Token::Left_Paren);
scanner.scan();
List := Create_Iir_List;
Set_Group_Constituent_List (Res, List);
loop
        Append_Element (List, Parse_Name (Allow_Indexes: false));
exit when scanner.currentContext.token = Token::Right_Paren;
Expect (Token::Comma);
scanner.scan();
end loop;
Scan_Expect (Token::Semi_Colon);
return Res;
end;
default:
throw std::runtime_error("PARSE ERROR: ':' or 'is' expected here");
return nullptr;
}
end Parse_Group;

//  precond : next token
//  postcond: ':'
//
//  [ 5.4 ]
//  signal_list ::= signal_name { , signal_name }
        --                | OTHERS
        --                | ALL
Parse_Signal_List return Iir_List
        is
Res : Iir_List;
begin
switch (scanner.currentContext.token) {
case Token::Others:
scanner.scan();
return Iir_List_Others;
case Token::All:
scanner.scan();
return Iir_List_All;
default:
Res := Create_Iir_List;
loop
        Append_Element (Res, Parse_Name);
exit when scanner.currentContext.token = Token::Colon;
Expect (Token::Comma);
scanner.scan();
end loop;
return Res;
}
end Parse_Signal_List;

//  precond : DISCONNECT
//  postcond: ';'
//
//  [ 5.4 ]
//  disconnection_specification ::=
//      DISCONNECT guarded_signal_specification AFTER time_expression ;
Parse_Disconnection_Specification
return Iir_Disconnection_Specification
        is
Res : Iir_Disconnection_Specification;
begin
        Res := Create_Iir (Iir_Disconnection_Specification);
Set_Location (Res);

//  Skip 'disconnect'
Expect (Token::Disconnect);
scanner.scan();

Set_Signal_List (Res, Parse_Signal_List);

//  Skip ':'
Expect (Token::Colon);
scanner.scan();

Set_Type_Mark (Res, Parse_Type_Mark (Check_Paren: true));

//  Skip 'after'
Expect (Token::After);
scanner.scan();

Set_Expression (Res, Parse_Expression);
return Res;
end Parse_Disconnection_Specification;

Parse_Psl_Default_Clock return Iir
        is
Res : Iir;
begin
        Res := Create_Iir (Iir_Psl_Default_Clock);
Set_Location (Res);

//  Recognize PSL keywords.
Scanner.Flag_Psl := true;

//  Skip 'default'.
Scan_Expect (Token::Psl_Clock);
Xrefs.Xref_Keyword (Get_Token_Location);

//  Skip 'clock'.
Scan_Expect (Token::Is);

//  Skip 'is'.
scanner.scan();

Set_Psl_Boolean (Res, Parse_Psl.Parse_Psl_Boolean);
Expect (Token::Semi_Colon);

Scanner.Flag_Scan_In_Comment := false;
Scanner.Flag_Psl := false;

return Res;
end Parse_Psl_Default_Clock;

Parse_Psl_Declaration return Iir
        is
Tok : constant Token_Type := scanner.currentContext.token;
Loc : constant Location_Type := Get_Token_Location;
Res : Iir;
Decl : PSL_Node;
Id : Name_Id;
begin
//  Skip 'property', 'sequence' or 'endpoint'.
scanner.scan();

if (scanner.currentContext.token != Token::Identifier {
throw std::runtime_error("PARSE ERROR: declaration name expected here");
Id := Null_Identifier;
else
Id := scanner.currentContext.Identifier;
end if;

//  Parse PSL declaration.
Scanner.Flag_Psl := true;
Decl := Parse_Psl.Parse_Psl_Declaration (Tok);
Expect (Token::Semi_Colon);
Scanner.Flag_Scan_In_Comment := false;
Scanner.Flag_Psl := false;

if Tok = Token::Psl_Endpoint
        && Parse_Psl.Is_Instantiated_Declaration (Decl)
{
//  Instantiated endpoint: make it visible from VHDL.
Res := Create_Iir (Iir_Psl_Endpoint_Declaration);
else
//  Otherwise, it will be visible only from PSL.
Res := Create_Iir (Iir_Psl_Declaration);
end if;
Set_Location (Res, Loc);
Set_Identifier (Res, Id);
Set_Psl_Declaration (Res, Decl);

return Res;
end Parse_Psl_Declaration;

//  Return the parent of a nested package.  Used to check if some
//  declarations are allowed in a package.
Iir Get_Package_Parent (Decl : Iir) 
        is
Res : Iir;
Parent : Iir;
begin
        Res := Decl;
loop
switch (Get_Kind (Res) is
        when Iir_Package_Declaration
| Iir_Package_Body:
Parent := Get_Parent (Res);
if Get_Kind (Parent) = Iir_Design_Unit {
return Res;
else
Res := Parent;
end if;
default:
return Res;
}
end loop;
end Get_Package_Parent;

//  precond : next token
//  postcond: next token
//
//  [ LRM08 3.2.3 Entity declarative part ]
//  entity_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | signal_declaration
        --     | shared_variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | disconnection_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --     | PSL_property_declaration
        --     | PSL_sequence_declaration
        --     | PSL_clock_declaration
        --
        --  [ LRM08 3.3.2 Architecture declarative part ]
//  block_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | signal_declaration
        --     | shared_variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | component_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | configuration_specification
        --     | disconnection_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --     | PSL_property_declaration
        --     | PSL_sequence_declaration
        --     | PSL_clock_declaration
        --
        --  [ LRM08 4.3 Subprogram bodies ]
//  subprogram_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --
        --  [ LRM08 4.7 Package declarations ]
//  package_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | signal_declaration
        --     | variable_declaration
        --     | file_declaraton
        --     | alias_declaration
        --     | component_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | disconnection_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --     | PSL_property_declaration
        --     | PSL_sequence_declaration
        --
        --  [ LRM08 4.8 Package bodies ]
//  package_body_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --
        --  [ LRM08 5.6.2 Protected type declarations ]
//  protected_type_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_instantiation_declaration
        --     | attribute_specification
        --     | use_clause
        --
        --  [ LRM08 5.6.3 Protected type bodies ]
//  protected_type_body_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --
        --  [ LRM08 11.3 Process statement ]
//  process_declarative_item ::=
//       subprogram_declaration
        --     | subprogram_body
        --     | subprogram_instantiation_declaration
        --     | package_declaration
        --     | package_body
        --     | package_instantiation_declaration
        --     | type_declaration
        --     | subtype_declaration
        --     | constant_declaration
        --     | variable_declaration
        --     | file_declaration
        --     | alias_declaration
        --     | attribute_declaration
        --     | attribute_specification
        --     | use_clause
        --     | group_template_declaration
        --     | group_declaration
        --
        --  Some declarations are not allowed in every declarative part:
//  - subprogram_body, package_body:
//    not in package_declaration
//  - signal_declaration, disconnection_specification:
//    not in process, protected_type_body, package_body, subprogram
//  - variable_declaration:
//    shared in entity, block (*)
//    not shared in subprogram, protected_type_body, process
//    depends on parent for package and package_body
//  - component_declaration:
//    not in entity, subprogram, package_body, protected_type_body,
//       process
//    depends on parent for package
//  - configuration_specification:
//    not in entity, subprogram, package, package_body, protected_type_body,
//       process
//  - PSL_property_declaration, PSL_sequence_declaration:
//    in entity and block (*)
//    depends on parent for package
//  - PSL_clock_declaration:
//    in block (*)
//
//  Declarations for protected_type_declaration are handled in sem.
//
//  (*): block means block_declarative_item, ie: block_statement,
//       architecture_body and generate_statement)
 Parse_Declarative_Part (Parent : Iir)
is
        use Declaration_Chain_Handling;
Last_Decl : Iir;
Decl : Iir;
Package_Parent_Cache : Iir;

Package_Parent return Iir is
begin
if Package_Parent_Cache = nullptr {
        Package_Parent_Cache := Get_Package_Parent (Parent);
end if;
return Package_Parent_Cache;
end Package_Parent;
begin
        Package_Parent_Cache := nullptr;
Build_Init (Last_Decl);
loop
        Decl := nullptr;
switch (scanner.currentContext.token) {
case Token::Invalid:
raise Internal_Error;
case Token::Type:
Decl := Parse_Type_Declaration (Parent);

//  LRM 2.5  Package declarations
//  If a package declarative item is a type declaration that is
//  a full type declaration whose type definition is a
//  protected_type definition, then that protected type
//  definition must not be a protected type body.
if Decl != nullptr
        && Get_Kind (Decl) = Iir_Protected_Type_Body
{
switch (Get_Kind (Parent) is
        when Iir_Package_Declaration:
throw std::runtime_error("PARSE ERROR:" + +Decl, "protected type body not "
& "allowed in package declaration");
default:
null;
}
end if;
case Token::Subtype:
Decl := Parse_Subtype_Declaration (Parent);
case Token::Nature:
Decl := Parse_Nature_Declaration;
case Token::Terminal:
Decl := Parse_Terminal_Declaration (Parent);
case Token::Quantity:
Decl := Parse_Quantity_Declaration (Parent);
case Token::Signal:
//  LRM08 4.7 Package declarations
//  For package declaration that appears in a subprogram body,
//  a process statement, or a protected type body, [...]
//  Moreover, it is an eror if [...] a signal declaration [...]
//  appears as a package declarative item of such a package
//  declaration.
switch (Get_Kind (Package_Parent) is
        when Iir_Function_Body
| Iir_Procedure_Body:
Error_Msg_Parse
("signal declaration not allowed in subprogram body");
case Iir_Kinds_Process_Statement:
Error_Msg_Parse
("signal declaration not allowed in process");
case Iir_Protected_Type_Body
| Iir_Protected_Type_Declaration:
Error_Msg_Parse
("signal declaration not allowed in protected type");
case Iir_Package_Body:
Error_Msg_Parse
("signal declaration not allowed in package body");
case Iir_Entity_Declaration
| Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body
| Iir_Package_Declaration:
null;
default:
Error_Kind ("parse_declarative_part", Package_Parent);
}
Decl := Parse_Object_Declaration (Parent);
case Token::Constant:
Decl := Parse_Object_Declaration (Parent);
case Token::Variable:
//  LRM93 4.3.1.3  Variable declarations
//  Variable declared immediatly within entity declarations,
//  architectures bodies, packages, packages bodies, and blocks
//  must be shared variable.
//  Variables declared immediatly within subprograms and
//  processes must not be shared variables.
//  Variables may appear in protected type bodies; such
//  variables, which must not be shared variables, represent
//  shared data.
switch (Get_Kind (Package_Parent) is
        when Iir_Entity_Declaration
| Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body
| Iir_Package_Declaration
| Iir_Package_Body
| Iir_Protected_Type_Declaration:
//  FIXME: replace HERE with the kind of declaration
//  ie: "not allowed in a package" rather than "here".
Error_Msg_Parse
("non-shared variable declaration not allowed here");
case Iir_Function_Body
| Iir_Procedure_Body
| Iir_Kinds_Process_Statement
| Iir_Protected_Type_Body:
null;
default:
Error_Kind ("parse_declarative_part", Package_Parent);
}
Decl := Parse_Object_Declaration (Parent);
case Token::Shared:
if Flags.Vhdl_Std <= Vhdl_87 {
throw std::runtime_error("PARSE ERROR: shared variable not allowed in vhdl 87");
end if;
//  LRM08 4.7 Package declarations
//  For package declaration that appears in a subprogram body,
//  a process statement, or a protected type body, it is an
//  error if a variable declaration in the package declaratie
//  part of the package declaration declares a shared variable.

//  LRM08 4.8 Package bodies
//  For a package body that appears in a subprogram body, a
//  process statement or a protected type body, it is an error
//  if a variable declaration in the package body declarative
//  part of the package body declares a shared variable.

//  LRM93 4.3.1.3  Variable declarations
//  Variable declared immediatly within entity declarations,
//  architectures bodies, packages, packages bodies, and blocks
//  must be shared variable.
//  Variables declared immediatly within subprograms and
//  processes must not be shared variables.
//  Variables may appear in proteted type bodies; such
//  variables, which must not be shared variables, represent
//  shared data.
switch (Get_Kind (Package_Parent) is
        when Iir_Entity_Declaration
| Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body
| Iir_Package_Declaration
| Iir_Package_Body
| Iir_Protected_Type_Declaration:
null;
case Iir_Function_Body
| Iir_Procedure_Body
| Iir_Kinds_Process_Statement
| Iir_Protected_Type_Body:
Error_Msg_Parse
("shared variable declaration not allowed here");
default:
Error_Kind ("parse_declarative_part", Package_Parent);
}
Decl := Parse_Object_Declaration (Parent);
case Token::File:
Decl := Parse_Object_Declaration (Parent);
case Token::Function
| Token::Procedure
| Token::Pure
| Token::Impure:
Decl := Parse_Subprogram_Declaration;
if Decl != nullptr
        && Get_Subprogram_Body (Decl) != nullptr
        {
if Get_Kind (Parent) = Iir_Package_Declaration {
        Error_Msg_Parse
        (+Decl, "subprogram body not allowed in a package");
end if;
end if;
case Token::Alias:
Decl := Parse_Alias_Declaration;
case Token::Component:
switch (Get_Kind (Parent) is
        when Iir_Entity_Declaration
| Iir_Procedure_Body
| Iir_Function_Body
| Iir_Kinds_Process_Statement
| Iir_Package_Body
| Iir_Protected_Type_Body
| Iir_Protected_Type_Declaration:
Error_Msg_Parse
("component declaration are not allowed here");
case Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body
| Iir_Package_Declaration:
null;
default:
Error_Kind ("parse_declarative_part", Package_Parent);
}
Decl := Parse_Component_Declaration;
case Token::For:
switch (Get_Kind (Parent) is
        when Iir_Entity_Declaration
| Iir_Function_Body
| Iir_Procedure_Body
| Iir_Kinds_Process_Statement
| Iir_Package_Declaration
| Iir_Package_Body
| Iir_Protected_Type_Body
| Iir_Protected_Type_Declaration:
Error_Msg_Parse
("configuration specification not allowed here");
case Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body:
null;
default:
Error_Kind ("parse_declarative_part", Package_Parent);
}
Decl := Parse_Configuration_Specification;
case Token::Attribute:
Decl := Parse_Attribute;
case Token::Disconnect:
//  LRM08 4.7 Package declarations
//  For package declaration that appears in a subprogram body,
//  a process statement, or a protected type body, [...]
//  Moreover, it is an eror if [...] a disconnection
//  specification [...] appears as a package declarative item
//  of such a package declaration.
switch (Get_Kind (Parent) is
        when Iir_Function_Body
| Iir_Procedure_Body
| Iir_Kinds_Process_Statement
| Iir_Protected_Type_Body
| Iir_Package_Body
| Iir_Protected_Type_Declaration:
Error_Msg_Parse
("disconnect specification not allowed here");
case Iir_Entity_Declaration
| Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body
| Iir_Package_Declaration:
null;
default:
Error_Kind ("parse_declarative_part", Parent);
}
Decl := Parse_Disconnection_Specification;
case Token::Use:
Decl := Parse_Use_Clause;
case Token::Group:
Decl := Parse_Group;
case Token::Package:
if Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("nested package not allowed before vhdl 2008");
end if;
Decl := Parse_Package (Parent);
if Decl != nullptr
        && Get_Kind (Decl) = Iir_Package_Body
{
if Get_Kind (Parent) = Iir_Package_Declaration {
        Error_Msg_Parse
        (+Decl, "package body not allowed in a package");
end if;
end if;
case Token::Identifier:
if Vhdl_Std >= Vhdl_08
        && scanner.currentContext.Identifier = Name_Default
{
//  This identifier is a PSL keyword.
Xrefs.Xref_Keyword (Get_Token_Location);

//  Check whether default clock are allowed in this region.
switch (Get_Kind (Parent) is
        when Iir_Function_Body
| Iir_Procedure_Body
| Iir_Kinds_Process_Statement
| Iir_Protected_Type_Body
| Iir_Package_Declaration
| Iir_Package_Body
| Iir_Protected_Type_Declaration:
Error_Msg_Parse
("PSL default clock declaration not allowed here");
case Iir_Entity_Declaration
| Iir_Architecture_Body
| Iir_Block_Statement
| Iir_Generate_Statement_Body:
null;
default:
Error_Kind ("parse_declarative_part", Parent);
}
Decl := Parse_Psl_Default_Clock;
else
Error_Msg_Parse
("object class keyword such as 'variable' is expected");
Eat_Tokens_Until_Semi_Colon;
end if;
case Token::Semi_Colon:
throw std::runtime_error("PARSE ERROR: ';' (semi colon) not allowed alone");
scanner.scan();
default:
exit;
}
if Decl != nullptr {
Append_Subchain (Last_Decl, Parent, Decl);
end if;

if (scanner.currentContext.token = Token::Semi_Colon or scanner.currentContext.token = Token::Invalid {
        scanner.scan();
end if;
end loop;
end Parse_Declarative_Part;

//  precond : ENTITY
//  postcond: ';'
//
//  [ 1.1 ]
//  entity_declaration ::=
//      ENTITY identifier IS
//          entiy_header
//          entity_declarative_part
//      [ BEGIN
//          entity_statement_part ]
//      END [ ENTITY ] [ ENTITY_simple_name ]
//
//  [ 1.1.1 ]
//  entity_header ::=
//      [ FORMAL_generic_clause ]
//      [ FORMAL_port_clause ]
 Parse_Entity_Declaration (Unit : Iir_Design_Unit)
is
        Res: Iir_Entity_Declaration;
begin
        Expect (Token::Entity);
Res := Create_Iir (Iir_Entity_Declaration);

//  Get identifier.
Scan_Expect (Token::Identifier,
"an identifier is expected after ""entity""");
Set_Identifier (Res, scanner.currentContext.Identifier);
Set_Location (Res);

Scan_Expect (Token::Is, "missing ""is"" after identifier");
scanner.scan();

Parse_Generic_Port_Clauses (Res);

Parse_Declarative_Part (Res);

if (scanner.currentContext.token = Token::Begin {
        Set_Has_Begin (Res, true);
scanner.scan();
Parse_Concurrent_Statements (Res);
end if;

//   end keyword is expected to finish an entity declaration
        Expect (Token::End);
Set_End_Location (Unit);

scanner.scan();
if (scanner.currentContext.token = Token::Entity {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'entity' keyword not allowed here by vhdl 87");
end if;
Set_End_Has_Reserved_Id (Res, true);
scanner.scan();
end if;
Check_End_Name (Res);
Expect (Token::Semi_Colon);
Invalidate_scanner.currentContext.token;
Set_Library_Unit (Unit, Res);
end Parse_Entity_Declaration;

//  [ LRM93 7.3.2 ]
//  choice ::= simple_expression
        --           | discrete_range
        --           | ELEMENT_simple_name
        --           | OTHERS
Iir Parse_A_Choice (Expr: Iir) 
        is
A_Choice: Iir;
Expr1: Iir;
begin
if Expr = nullptr {
if (scanner.currentContext.token = Token::Others {
        A_Choice := Create_Iir (Iir_Choice_By_Others);
Set_Location (A_Choice);

//  Skip 'others'
scanner.scan();

return A_Choice;
else
Expr1 := Parse_Expression;

if Expr1 = nullptr {
//  Handle parse error now.
//  FIXME: skip until '=>'.
A_Choice := Create_Iir (Iir_Choice_By_Expression);
Set_Location (A_Choice);
return A_Choice;
end if;
end if;
else
Expr1 := Expr;
end if;
if Is_Range_Attribute_Name (Expr1) {
        A_Choice := Create_Iir (Iir_Choice_By_Range);
Location_Copy (A_Choice, Expr1);
Set_Choice_Range (A_Choice, Expr1);
return A_Choice;
elsif (scanner.currentContext.token = Token::To or else scanner.currentContext.token = Token::Downto {
        A_Choice := Create_Iir (Iir_Choice_By_Range);
Location_Copy (A_Choice, Expr1);
Set_Choice_Range (A_Choice, Parse_Range_Expression (Expr1));
return A_Choice;
else
A_Choice := Create_Iir (Iir_Choice_By_Expression);
Location_Copy (A_Choice, Expr1);
Set_Choice_Expression (A_Choice, Expr1);
return A_Choice;
end if;
end Parse_A_Choice;

//  [ LRM93 7.3.2 ]
//  choices ::= choice { | choice }
        --
        -- Leave Token::double_arrow as current token.
Iir Parse_Choices (Expr: Iir) 
        is
First, Last : Iir;
A_Choice: Iir;
Expr1 : Iir;
begin
        Sub_Chain_Init (First, Last);
Expr1 := Expr;
loop
        A_Choice := Parse_A_Choice (Expr1);
if First != nullptr {
Set_Same_Alternative_Flag (A_Choice, true);
if Get_Kind (A_Choice) = Iir_Choice_By_Others {
        throw std::runtime_error("PARSE ERROR: 'others' choice must be alone");
end if;
end if;
Sub_Chain_Append (First, Last, A_Choice);
if (scanner.currentContext.token != Token::Bar {
return First;
end if;
scanner.scan();
Expr1 := nullptr;
end loop;
end Parse_Choices;

//  precond : '('
//  postcond: next token
//
//  This can be an expression or an aggregate.
//
//  [ LRM93 7.3.2 ]
//  aggregate ::= ( element_association { , element_association } )
        --
        --  [ LRM93 7.3.2 ]
//  element_association ::= [ choices: ] expression
        function Parse_Aggregate return Iir
        is
use Iir_Chains.Association_Choices_Chain_Handling;
Expr: Iir;
Res: Iir;
Last : Iir;
Assoc: Iir;
Loc : Location_Type;
begin
        Loc := Get_Token_Location;

//  Skip '('
scanner.scan();

if (scanner.currentContext.token != Token::Others {
Expr := Parse_Expression;
switch (scanner.currentContext.token) {
case Token::Comma
| Token::Double_Arrow
| Token::Bar:
//  This is really an aggregate
        null;
case Token::Right_Paren:
// This was just a braced expression.

// Eat ')'.
scanner.scan();

if Get_Kind (Expr) = Iir_Aggregate {
//  Parenthesis around aggregate is useless and change the
//  context for array aggregate.
Warning_Msg_Sem
(Warnid_Parenthesis, +Expr,
"suspicious parenthesis around aggregate");
elsif not Flag_Parse_Parenthesis {
return Expr;
end if;

//  Create a node for the parenthesis.
Res := Create_Iir (Iir_Parenthesis_Expression);
Set_Location (Res, Loc);
Set_Expression (Res, Expr);
return Res;

case Token::Semi_Colon:
//  Surely a missing parenthesis.
//  FIXME: in case of multiple missing parenthesises, several
//    messages will be displayed
Error_Msg_Parse
("missing ')' for opening parenthesis at %l", +Loc);
return Expr;
default:
//  Surely a parse error...
null;
}
else
Expr := nullptr;
end if;
Res := Create_Iir (Iir_Aggregate);
Set_Location (Res, Loc);
Build_Init (Last);
loop
if (scanner.currentContext.token = Token::Others {
        Assoc := Parse_A_Choice (nullptr);
Expect (Token::Double_Arrow);
scanner.scan();
Expr := Parse_Expression;
else
if Expr = nullptr {
        Expr := Parse_Expression;
end if;
if Expr = nullptr {
return nullptr;
end if;
switch (scanner.currentContext.token) {
case Token::Comma
| Token::Right_Paren:
Assoc := Create_Iir (Iir_Choice_By_None);
Location_Copy (Assoc, Expr);
default:
Assoc := Parse_Choices (Expr);
Expect (Token::Double_Arrow);
scanner.scan();
Expr := Parse_Expression;
}
end if;
Set_Associated_Expr (Assoc, Expr);
Append_Subchain (Last, Res, Assoc);
exit when scanner.currentContext.token = Token::Right_Paren;
Expect (Token::Comma);
scanner.scan();
Expr := nullptr;
end loop;
scanner.scan();
return Res;
end Parse_Aggregate;

//  precond : NEW
//  postcond: next token
//
//  [LRM93 7.3.6]
//  allocator ::= NEW subtype_indication
//              | NEW qualified_expression
Parse_Allocator return Iir
        is
Loc: Location_Type;
Res : Iir;
Expr: Iir;
begin
        Loc := Get_Token_Location;

// Accept 'new'.
scanner.scan();
Expr := Parse_Name (Allow_Indexes: false);
if Get_Kind (Expr) != Iir_Qualified_Expression {
// This is a subtype_indication.
Res := Create_Iir (Iir_Allocator_By_Subtype);
Expr := Parse_Subtype_Indication (Expr);
Set_Subtype_Indication (Res, Expr);
else
Res := Create_Iir (Iir_Allocator_By_Expression);
Set_Expression (Res, Expr);
end if;

Set_Location (Res, Loc);
return Res;
end Parse_Allocator;

//  precond : Token::bit_string
//  postcond: Token::bit_string
//
//  Simply create the node for a bit string.
Parse_Bit_String return Iir
        is
use Name_Table;
Res : Iir;
C : Character;
B : Number_Base_Type;
begin
        Res := Create_Iir (Iir_String_Literal8);
Set_Location (Res);
Set_String8_Id (Res, Current_String_Id);
Set_String_Length (Res, Current_String_Length);
if Nam_Buffer (1) = 's' {
        Set_Has_Sign (Res, true);
Set_Has_Signed (Res, true);
pragma Assert (Nam_Length = 2);
C := Name_Table.Nam_Buffer (2);
elsif Nam_Buffer (1) = 'u' {
        Set_Has_Sign (Res, true);
Set_Has_Signed (Res, false);
pragma Assert (Nam_Length = 2);
C := Nam_Buffer (2);
else
Set_Has_Sign (Res, false);
Set_Has_Signed (Res, false);
pragma Assert (Nam_Length = 1);
C := Nam_Buffer (1);
end if;

switch (C is
case 'b':
B := Base_2;
case 'o':
B := Base_8;
case 'd':
B := Base_10;
case 'x':
B := Base_16;
default:
raise Internal_Error;
}
Set_Bit_String_Base (Res, B);

return Res;
end Parse_Bit_String;

//  Scan returns an expanded bit value.  Adjust the expanded bit value as
//  required by the length.
 Resize_Bit_String (Lit : Iir; Nlen : Nat32)
is
        use Str_Table;

Old_Len : constant Nat32 := Get_String_Length (Lit);
Is_Signed : constant Boolean := Get_Has_Signed (Lit);
Id : constant String8_Id := Get_String8_Id (Lit);
C : Nat8;
begin
if Nlen > Old_Len {
//  Extend.

//  LRM08 15.8
//  -- If the length is greater than the number of characters in the
//     expanded bit value and the base specifier is B, UB, O, UO, X,
//     UX or D, the bit string value is obtained by concatenating a
//     string of 0 digits to the left of the expanded bit value.  The
//     number of 0 digits in the string is such that the number of
//     characters in the result of the concatenation is the length of
//     the bit string literal.
//
//  -- If the length is greater than the number of characters in the
//     expanded bit value and the base specifier is SB, SO or SX, the
//     bit string value is obtained by concatenating the the left of
//     the expanded bit value a string, each of whose characters is
//     the leftmost character of the expanded bit value.  The number
//     of characters in the string is such that the number of
//     characters in the result of the concatenation is the length of
//     the bit string literal.
if Is_Signed {
if Old_Len = 0 {
        Error_Msg_Parse
        (+Lit, "cannot expand an empty signed bit string");
C := Character'Pos ('0');
else
C := Element_String8 (Id, 1);
end if;
else
C := Character'Pos ('0');
end if;
Resize_String8 (Nlen);
//  Shift (position 1 is the MSB).
for I in reverse 1 .. Old_Len loop
Set_Element_String8 (Id, I + Nlen - Old_Len,
Element_String8 (Id, I));
end loop;
for I in 1 .. Nlen - Old_Len loop
Set_Element_String8 (Id, I, C);
end loop;
Set_String_Length (Lit, Nlen);

elsif Nlen < Old_Len {
//  Reduce.

//  LRM08 15.8
//  -- If the length is less than the number of characters in the
//     expanded bit value and the base specifier is B, UB, O, UO, X,
//     UX or D, the bit string value is obtained by deleting
//     sufficient characters from the left of the expanded bit value
//     to yield a string whose length is the length of the bit string
//     literal.  It is an error if any of the character so deleted is
//     other than the digit 0.
//
//  -- If the length is less than the number of characters in the
//     expanded bit value and the base specifier is SB, SO or SX, the
//     bit string value is obtained by deleting sufficient characters
//     from the left of the expanded bit value to yield a string whose
//     length is the length of the bit string literal.  It is an error
//     if any of the characters so deleted differs from the leftmost
//     remaining character.
if Is_Signed {
C := Element_String8 (Id, 1 + Old_Len - Nlen);
else
C := Character'Pos ('0');
end if;
for I in 1 .. Old_Len - Nlen loop
if Element_String8 (Id, I) != C {
Error_Msg_Parse
(+Lit, "truncation of bit string changes the value");
//  Avoid error storm.
exit;
end if;
end loop;
//  Shift (position 1 is the MSB).
for I in 1 .. Nlen loop
Set_Element_String8 (Id, I,
        Element_String8 (Id, I + Old_Len - Nlen));
end loop;
Resize_String8 (Nlen);
Set_String_Length (Lit, Nlen);

else
//  LRM08 15.8
//  -- If the length is equal to the number of characters in the
//     expanded bit value, the string literal value is the expanded
//     bit value itself.
null;
end if;
end Resize_Bit_String;

//  Precond : next token after Token::integer
//  postcond: likewise
//
//  Return an integer_literal or a physical_literal.
Iir Parse_Integer_Literal (Val : Iir_Int64) 
        is
Res : Iir;
begin
if (scanner.currentContext.token = Token::Identifier {
// physical literal
Res := Create_Iir (Iir_Physical_Int_Literal);
Set_Unit_Name (Res, Parse_Name (Allow_Indexes: false));
else
// integer literal
Res := Create_Iir (Iir_Integer_Literal);
end if;
Set_Value (Res, Val);
return Res;
end Parse_Integer_Literal;
*/
//  precond : next token
//  postcond: next token
//
//  [ LRM93 7.1 ]
//  primary ::= name
//            | literal
//            | aggregate
//            | function_call
//            | qualified_expression
//            | type_conversion
//            | allocator
//            | ( expression )
//
//  [ LRM93 7.3.1 ]
//  literal ::= numeric_literal
//            | enumeration_literal
//            | string_literal
//            | bit_string_literal
//            | NULL
//
//  [ LRM93 7.3.1 ]
//  numeric_literal ::= abstract_literal | physical_literal
//
//  [ LRM93 13.4 ]
//  abstract_literal ::= decimal_literal | based_literal
//
//  [ LRM93 3.1.3 ]
//  physical_literal ::= [ abstract_literal ] UNIT_name

Iir* Parser::Parse_Primary() {
//Res: Iir*;
//Int: Iir_Int64;
//Fp: Iir_Fp64;
//Loc: Location_Type;
    switch (scanner.currentContext.token) {
    case Token::Integer:
        Int = Current_Iir_Int64;
        Loc = Get_Token_Location;

//  Skip integer
        scanner.scan();

        result = Parse_Integer_Literal(Int);
        Set_Location(result, Loc);
        return result;

    case Token::Real:
        Fp = Current_Iir_Fp64;
        Loc = Get_Token_Location;

//  Skip real
        scanner.scan();

        if (scanner.currentContext.token == Token::Identifier) {
// physical literal
            result = new Iir_Physical_Fp_Literal;
            Set_Unit_Name(result, Parse_Name(Allow_Indexes = > false));
        }
        else {
// real literal
            result = new Iir_Floating_Point_Literal;
        }
        Set_Location(result, Loc);
        Set_Fp_Value(result, Fp);
        return result;

    case Token::Identifier:
    case Token::Double_Less:
        result = Parse_Name(/*FIXME: Allow_Indexes:*/ true);
        if (Get_Kind(result) = Iir_Signature)
            throw std::runtime_error("PARSE ERROR:" + result + "signature not allowed in expression");
        else
            return result;

    case Token::Character:
        result = Current_Text;
        scanner.scan();
        if (scanner.currentContext.token = Token::Tick)
            throw std::runtime_error("PARSE ERROR: prefix of an attribute can't be a character literal");
        return result;
    case Token::Left_Paren:
        return Parse_Aggregate;
    case Token::String:
        return Parse_Name;
    case Token::Null:
        result = Create_Iir(Iir_Null_Literal);
        Set_Location(result);
        scanner.scan();
        return result;
    case Token::New:
        return Parse_Allocator;

    case Token::Integer_Letter:
        Int = Current_Iir_Int64;
        Loc = Get_Token_Location;

//  Skip integer
        scanner.scan();

        if (scanner.currentContext.token = Token::Bit_String) {
            result = Parse_Bit_String;
            Set_Has_Length(result, true);

//  Skip bit string
            scanner.scan();

//  resultize.
            resultize_Bit_String(result, Nat32(Int));
        }
        else
            throw std::runtime_error(
                    "PARSE ERROR: "/*Get_Token_Location,*/ + "space is required between number and unit name");

        Set_Location(result, Loc);
        return result;

    case Token::Bit_String:
        result = Parse_Bit_String();

//  Skip bit string
        scanner.scan();

        return result;

    case Token::Minus:
    case Token::Plus:
        throw std::runtime_error("PARSE ERROR: '-' and '+' are not allowed in primary, use parenthesis");
        return Parse_Simple_Expression();

    case Token::Comma:
    case Token::Semi_Colon:
    case Token::Eof:
    case Token::End:
//  Token not to be skipped
        throw std::runtime_error("PARSE ERROR: primary expression expected");

    default:
        Unexpected("primary");
    }
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  factor ::= primary [ ** primary ] | ABS primary | NOT primary | logical_operator primary  [ VHDL08 9.1 ]
template <typename T>
Iir* Parser::Build_Unary_Factor<T>(Iir* Primary, Vhdl_Std standard) {

    if (Primary)
        return Primary;

    if (standard == Vhdl_Std::Vhdl_08 && state.standard < Vhdl_Std::Vhdl_08)
        throw std::runtime_error("PARSE ERROR: missing left operand of logical expression");
    //FIXME:
//    scanner.scan();
//    return Parse_Primary;

    Iir* result = new T;
    Set_Location(result);
    scanner.scan();
    Set_Operand(result, Parse_Primary);
    return result;
}

Iir* Parser::Parse_Factor(Iir* Primary = nullptr) {
//result, Left: Iir*;

    switch (scanner.currentContext.token) {
    case Token::Abs:
        return Build_Unary_Factor<Iir_Absolute_Operator>(Primary);
    case Token::Not:
        return Build_Unary_Factor<Iir_Not_Operator>(Primary);

    case Token::And:
        return Build_Unary_Factor<Iir_Reduction_And_Operator>(Primary, Vhdl_Std::Vhdl_08);
    case Token::Or:
        return Build_Unary_Factor<Iir_Reduction_Or_Operator>(Primary, Vhdl_Std::Vhdl_08);
    case Token::Nand:
        return Build_Unary_Factor<Iir_Reduction_Nand_Operator>(Primary, Vhdl_Std::Vhdl_08);
    case Token::Nor:
        return Build_Unary_Factor<Iir_Reduction_Nor_Operator>(Primary, Vhdl_Std::Vhdl_08);
    case Token::Xor:
        return Build_Unary_Factor<Iir_Reduction_Xor_Operator>(Primary, Vhdl_Std::Vhdl_08);
    case Token::Xnor:
        return Build_Unary_Factor<Iir_Reduction_Xnor_Operator>(Primary, Vhdl_Std::Vhdl_08);

    default:
        if (Primary)
            Left = Primary;
        else
            Left = Parse_Primary();

        if (scanner.currentContext.token = Token::Double_Star) {
            result = Create_Iir(Iir_Exponentiation_Operator);
            Set_Location(result);
            scanner.scan();
            Set_Left(result, Left);
            Set_Right(result, Parse_Primary);
            return result;
        }
        else
            return Left;
    }
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  term ::= factor { multiplying_operator factor }
//
//  [ 7.2 ]
//  multiplying_operator ::= * | / | MOD | REM
Iir* Parser::Parse_Term (Iir* Primary) {
//    result, Tmp: Iir*;
            result = Parse_Factor(Primary);
    while (scanner.currentContext.token in
    Token_Multiplying_Operator_Type) {
        switch (scanner.currentContext.token) {
        case Token::Star:
            Tmp = Create_Iir(Iir_Multiplication_Operator);
            break;
        case Token::Slash:
            Tmp = Create_Iir(Iir_Division_Operator);
            break;
        case Token::Mod:
            Tmp = Create_Iir(Iir_Modulus_Operator);
            break;
        case Token::Rem:
            Tmp = Create_Iir(Iir_Remainder_Operator);
            break;
        default:
            throw std::logic_error("This cannot be a mode, parsing logic error");
        }
        Set_Location(Tmp);
        Set_Left(Tmp, result);
        scanner.scan();
        Set_Right(Tmp, Parse_Factor);
        result = Tmp;
    }
    return result;
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  simple_expression ::= [ sign ] term { adding_operator term }
//
//  [ 7.2 ]
//  sign ::= + | -
//
//  [ 7.2 ]
//  adding_operator ::= + | - | &
Iir* Parser::Parse_Simple_Expression (Iir* Primary = nullptr) {
//result, Tmp: Iir*;

    if (scanner.currentContext.token in Token_Sign_Type && !Primary) {
        switch (scanner.currentContext.token) {
        case Token::Plus:
            result = Create_Iir(Iir_Identity_Operator);
        case Token::Minus:
            result = Create_Iir(Iir_Negation_Operator);
        default:
            raise Program_Error;
        }
        Set_Location(result);
        scanner.scan();
        Set_Operand(result, Parse_Term(nullptr));
    }
    else
    result = Parse_Term(Primary);

    while (scanner.currentContext.token in Token_Adding_Operator_Type) {
        switch (scanner.currentContext.token) {
        case Token::Plus:
            Tmp = Create_Iir(Iir_Addition_Operator);
        case Token::Minus:
            Tmp = Create_Iir(Iir_Substraction_Operator);
        case Token::Ampersand:
            Tmp = Create_Iir(Iir_Concatenation_Operator);
        default:
            raise Program_Error;
        }
        Set_Location(Tmp);
        scanner.scan();
        Set_Left(Tmp, result);
        Set_Right(Tmp, Parse_Term(nullptr));
        result = Tmp;
    }
    return result;
}
/*
//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  shift_expression ::=
//      simple_expression [ shift_operator simple_expression ]
//
//  [ 7.2 ]
//  shift_operator ::= SLL | SRL | SLA | SRA | ROL | ROR
Parse_Shift_Expression return Iir* is
Res, Tmp: Iir*;
begin
        Tmp := Parse_Simple_Expression;
if (scanner.currentContext.token not in Token_Shift_Operator_Type {
return Tmp;
elsif Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: shift operators not allowed in vhdl 87");
end if;
switch (scanner.currentContext.token) {
case Token::Sll:
Res := Create_Iir (Iir_Sll_Operator);
case Token::Sla:
Res := Create_Iir (Iir_Sla_Operator);
case Token::Srl:
Res := Create_Iir (Iir_Srl_Operator);
case Token::Sra:
Res := Create_Iir (Iir_Sra_Operator);
case Token::Rol:
Res := Create_Iir (Iir_Rol_Operator);
case Token::Ror:
Res := Create_Iir (Iir_Ror_Operator);
default:
raise Program_Error;
}
Set_Location (Res);
scanner.scan();
Set_Left (Res, Tmp);
Set_Right (Res, Parse_Simple_Expression);
return Res;
end Parse_Shift_Expression;

//  precond : next token (relational_operator)
//  postcond: next token
//
//  [ 7.1 ]
//     relational_operator shift_expression
Iir Parse_Relation_Rhs (Left : Iir) 
        is
Res, Tmp: Iir*;
begin
        Tmp := Left;

//  This loop is just to handle errors such as a = b = c.
        loop
switch (scanner.currentContext.token) {
case Token::Equal:
Res := Create_Iir (Iir_Equality_Operator);
case Token::Not_Equal:
Res := Create_Iir (Iir_Inequality_Operator);
case Token::Less:
Res := Create_Iir (Iir_Less_Than_Operator);
case Token::Less_Equal:
Res := Create_Iir (Iir_Less_Than_Or_Equal_Operator);
case Token::Greater:
Res := Create_Iir (Iir_Greater_Than_Operator);
case Token::Greater_Equal:
Res := Create_Iir (Iir_Greater_Than_Or_Equal_Operator);
case Token::Match_Equal:
Res := Create_Iir (Iir_Match_Equality_Operator);
case Token::Match_Not_Equal:
Res := Create_Iir (Iir_Match_Inequality_Operator);
case Token::Match_Less:
Res := Create_Iir (Iir_Match_Less_Than_Operator);
case Token::Match_Less_Equal:
Res := Create_Iir (Iir_Match_Less_Than_Or_Equal_Operator);
case Token::Match_Greater:
Res := Create_Iir (Iir_Match_Greater_Than_Operator);
case Token::Match_Greater_Equal:
Res := Create_Iir
        (Iir_Match_Greater_Than_Or_Equal_Operator);
default:
raise Program_Error;
}
Set_Location (Res);
scanner.scan();
Set_Left (Res, Tmp);
Set_Right (Res, Parse_Shift_Expression);
exit when scanner.currentContext.token not in Token_Relational_Operator_Type;
Error_Msg_Parse
("use parenthesis for consecutive relational expressions");
Tmp := Res;
end loop;
return Res;
end Parse_Relation_Rhs;

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  relation ::= shift_expression [ relational_operator shift_expression ]
//
//  [ 7.2 ]
//  relational_operator ::= = | != | < | <= | > | >=
//                        | ?= | ?!= | ?< | ?<= | ?> | ?>=
Parse_Relation return Iir
        is
Tmp: Iir;
begin
        Tmp := Parse_Shift_Expression;
if (scanner.currentContext.token not in Token_Relational_Operator_Type {
return Tmp;
end if;

return Parse_Relation_Rhs (Tmp);
end Parse_Relation;

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  expression ::= relation { AND relation }
        --               | relation { OR relation }
        --               | relation { XOR relation }
        --               | relation [ NAND relation }
//               | relation [ NOR relation }
//               | relation { XNOR relation }
Iir Parse_Expression_Rhs (Left : Iir) 
        is
Res, Tmp: Iir;

//  OP_TOKEN contains the operator combinaison.
Op_Token: Token_Type;
begin
        Tmp := Left;
Op_Token := Token::Invalid;
loop
switch (scanner.currentContext.token) {
case Token::And:
Res := Create_Iir (Iir_And_Operator);
case Token::Or:
Res := Create_Iir (Iir_Or_Operator);
case Token::Xor:
Res := Create_Iir (Iir_Xor_Operator);
case Token::Nand:
Res := Create_Iir (Iir_Nand_Operator);
case Token::Nor:
Res := Create_Iir (Iir_Nor_Operator);
case Token::Xnor:
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'xnor' keyword not allowed in vhdl 87");
end if;
Res := Create_Iir (Iir_Xnor_Operator);
default:
return Tmp;
}

if Op_Token = Token::Invalid {
        Op_Token := scanner.currentContext.token;
else
//  Check after the case, since current_token may not be an
//  operator...
//  TODO: avoid repetition of this message ?
if Op_Token = Token::Nand or Op_Token = Token::Nor {
        Error_Msg_Parse
        ("sequence of 'nor' or 'nand' not allowed", Cont: true);
Error_Msg_Parse
("('nor' and 'nand' are not associative)");
end if;
if Op_Token != scanner.currentContext.token {
//  Expression is a sequence of relations, with the same
//  operator.
throw std::runtime_error("PARSE ERROR: only one type of logical operators may be "
& "used to combine relation");
end if;
end if;

Set_Location (Res);
scanner.scan();

//  Catch errors for Ada programmers.
if (scanner.currentContext.token = Token::Then or scanner.currentContext.token = Token::Else {
        throw std::runtime_error("PARSE ERROR: ""or else"" and ""&&"" sequences "
        & "are not allowed in vhdl", Cont: true);
throw std::runtime_error("PARSE ERROR: ""and"" and ""or"" are short-circuit "
& "operators for BIT and BOOLEAN types");
scanner.scan();
end if;

Set_Left (Res, Tmp);
Set_Right (Res, Parse_Relation);
Tmp := Res;
end loop;
end Parse_Expression_Rhs;

//  precond : next token
//  postcond: next token
//
//  LRM08 9.1 General
//  expression ::= condition_operator primary
//              |  logical_expression
        function Parse_Expression return Iir*
        is
Res : Iir;
begin
if (scanner.currentContext.token = Token::Condition {
        Res := Create_Iir (Iir_Condition_Operator);
Set_Location (Res);

//  Skip '??'
scanner.scan();

Set_Operand (Res, Parse_Primary);
else
Res := Parse_Expression_Rhs (Parse_Relation);
end if;

return Res;
end Parse_Expression;

//  precond : next token
//  postcond: next token.
//
//  [ 8.4 ]
//  waveform ::= waveform_element { , waveform_element }
        --             | UNAFFECTED
        --
        --  [ 8.4.1 ]
        --  waveform_element ::= VALUE_expression [ AFTER TIME_expression ]
//                     | NULL [ AFTER TIME_expression ]
Parse_Waveform return Iir_Waveform_Element
        is
Res: Iir_Waveform_Element;
We, Last_We : Iir_Waveform_Element;
begin
if (scanner.currentContext.token = Token::Unaffected {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'unaffected' is not allowed in vhdl87");
end if;

Res := Create_Iir (Iir_Unaffected_Waveform);
Set_Location (Res);

//  Skip 'unaffected'.
scanner.scan();
else
Sub_Chain_Init (Res, Last_We);
loop
        We := Create_Iir (Iir_Waveform_Element);
Sub_Chain_Append (Res, Last_We, We);
Set_Location (We);

//  Note: NULL is handled as a null_literal.
Set_We_Value (We, Parse_Expression);

if (scanner.currentContext.token = Token::After {
//  Skip 'after'.
scanner.scan();

Set_Time (We, Parse_Expression);
end if;

exit when scanner.currentContext.token != Token::Comma;

//  Skip ','.
scanner.scan();
end loop;
end if;

return Res;
end Parse_Waveform;

//  precond : next token
//  postcond: next token
//
//  [ 8.4 ]
//  delay_mechanism ::= TRANSPORT
        --                    | [ REJECT TIME_expression ] INERTIAL
        procedure Parse_Delay_Mechanism (Assign: Iir) is
        begin
if (scanner.currentContext.token = Token::Transport {
        Set_Delay_Mechanism (Assign, Iir_Transport_Delay);
scanner.scan();
else
Set_Delay_Mechanism (Assign, Iir_Inertial_Delay);
if (scanner.currentContext.token = Token::Reject {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'reject' delay mechanism not allowed in vhdl 87");
end if;
scanner.scan();
Set_Reject_Time_Expression (Assign, Parse_Expression);
Expect (Token::Inertial);
scanner.scan();
elsif (scanner.currentContext.token = Token::Inertial {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'inertial' keyword not allowed in vhdl 87");
end if;
scanner.scan();
end if;
end if;
end Parse_Delay_Mechanism;

//  precond : next token
//  postcond: next token
//
//  [ LRM93 9.5 ]
//  options ::= [ GUARDED ] [ delay_mechanism ]
 Parse_Options (Stmt : Iir) is
        begin
if (scanner.currentContext.token = Token::Guarded {
        Set_Guard (Stmt, Stmt);
scanner.scan();
end if;
Parse_Delay_Mechanism (Stmt);
end Parse_Options;

//  precond : next tkoen
//  postcond: next token (';')
//
//  [ LRM93 9.5.1 ]
//  conditional_waveforms ::=
//      { waveform WHEN condition ELSE }
        --      waveform [ WHEN condition ]
Parse_Conditional_Waveforms return Iir
        is
Wf : Iir;
Res : Iir;
Cond_Wf, N_Cond_Wf : Iir_Conditional_Waveform;
begin
        Wf := Parse_Waveform;
if (scanner.currentContext.token != Token::When {
return Wf;
else
Res := Create_Iir (Iir_Conditional_Waveform);
Set_Location (Res);
Set_Waveform_Chain (Res, Wf);

Cond_Wf := Res;
loop
//  Eat 'when'
scanner.scan();

Set_Condition (Cond_Wf, Parse_Expression);

if (scanner.currentContext.token != Token::Else {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: else missing in vhdl 87");
end if;
exit;
end if;

N_Cond_Wf := Create_Iir (Iir_Conditional_Waveform);
Set_Location (N_Cond_Wf);
Set_Chain (Cond_Wf, N_Cond_Wf);
Cond_Wf := N_Cond_Wf;

//  Eat 'else'
scanner.scan();

Set_Waveform_Chain (Cond_Wf, Parse_Waveform);

exit when scanner.currentContext.token != Token::When;
end loop;
return Res;
end if;
end Parse_Conditional_Waveforms;

//  precond : '<=' (or ':=')
//  postcond: ';'
//
//  [ LRM93 9.5.1 ]
//  concurrent_conditional_signal_assignment ::=
//      target <= [ GUARDED ] [ delay_mechanism ] conditional_waveforms ;
//
//  [ LRM08 10.5.2.1 ]
//  concurrent_simple_waveform_assignment ::=
//      target <= [ GUARDED ] [ delay_mechanism ] waveform ;
Parse_Concurrent_Conditional_Signal_Assignment (Target: Iir)
return Iir
        is
Res: Iir;
Loc : Location_Type;
N_Res : Iir;
Wf : Iir;
begin
        Loc := Get_Token_Location;
switch (scanner.currentContext.token) {
case Token::Less_Equal:
null;
case Token::Assign:
throw std::runtime_error("PARSE ERROR: ':=' not allowed in concurrent statement, "
& "replaced by '<='");
default:
Expect (Token::Less_Equal);
}

//  Eat '<='.
scanner.scan();

//  Assume simple signal assignment.
Res := Create_Iir (Iir_Concurrent_Simple_Signal_Assignment);
Parse_Options (Res);

Wf := Parse_Conditional_Waveforms;
if Wf != nullptr
        && Get_Kind (Wf) = Iir_Conditional_Waveform
{
        N_Res :=
        Create_Iir (Iir_Concurrent_Conditional_Signal_Assignment);
if Get_Guard (Res) != nullptr {
Set_Guard (N_Res, N_Res);
end if;
Set_Delay_Mechanism (N_Res, Get_Delay_Mechanism (Res));
Set_Reject_Time_Expression (N_Res, Get_Reject_Time_Expression (Res));
Free_Iir (Res);
Res := N_Res;
Set_Conditional_Waveform_Chain (Res, Wf);
else
Set_Waveform_Chain (Res, Wf);
end if;
Set_Location (Res, Loc);
Set_Target (Res, Target);
Expect (Token::Semi_Colon);
return Res;
end Parse_Concurrent_Conditional_Signal_Assignment;

//  precond : WITH
//  postcond: ';'
//
//  [ 9.5.2 ]
//  selected_signal_assignment ::=
//      WITH expresion SELECT
//          target <= options selected_waveforms ;
//
//  [ 9.5.2 ]
//  selected_waveforms ::=
//      { waveform WHEN choices , }
        --      waveform WHEN choices
        function Parse_Selected_Signal_Assignment return Iir
        is
use Iir_Chains.Selected_Waveform_Chain_Handling;
Res: Iir;
Assoc: Iir;
Wf_Chain : Iir_Waveform_Element;
Target : Iir;
Last : Iir;
begin
        Scan;  -- accept 'with' token.
Res := Create_Iir (Iir_Concurrent_Selected_Signal_Assignment);
Set_Location (Res);
Set_Expression (Res, Parse_Expression);

Expect (Token::Select, "'select' expected after expression");
scanner.scan();
if (scanner.currentContext.token = Token::Left_Paren {
        Target := Parse_Aggregate;
else
Target := Parse_Name (Allow_Indexes: true);
end if;
Set_Target (Res, Target);
Expect (Token::Less_Equal);
scanner.scan();

Parse_Options (Res);

Build_Init (Last);
loop
        Wf_Chain := Parse_Waveform;
Expect (Token::When, "'when' expected after waveform");
scanner.scan();
Assoc := Parse_Choices (nullptr);
Set_Associated_Chain (Assoc, Wf_Chain);
Append_Subchain (Last, Res, Assoc);
exit when scanner.currentContext.token = Token::Semi_Colon;
Expect (Token::Comma, "',' (comma) expected after choice");
scanner.scan();
end loop;
return Res;
end Parse_Selected_Signal_Assignment;

//  precond : next token
//  postcond: next token.
//
//  [ 8.1 ]
//  sensitivity_list ::= SIGNAL_name { , SIGNAL_name }
 Parse_Sensitivity_List (List: Iir_Designator_List)
is
        El : Iir;
begin
        loop
El := Parse_Name (Allow_Indexes: true);
switch (Get_Kind (El) is
        when Iir_Simple_Name
| Iir_Parenthesis_Name
| Iir_Selected_Name
| Iir_Slice_Name
| Iir_Attribute_Name
| Iir_Selected_By_All_Name
| Iir_Indexed_Name:
null;
default:
Error_Msg_Parse
("only names are allowed in a sensitivity list");
}
Append_Element (List, El);
exit when scanner.currentContext.token != Token::Comma;
scanner.scan();
end loop;
end Parse_Sensitivity_List;

//  precond : ASSERT
//  postcond: next token
//  Note: this fill an sequential or a concurrent statement.
//
//  [ 8.2 ]
//  assertion ::= ASSERT condition
//      [ REPORT expression ] [ SEVERITY expression ]
 Parse_Assertion (Stmt: Iir) is
        begin
Set_Location (Stmt);
scanner.scan();
Set_Assertion_Condition (Stmt, Parse_Expression);
if (scanner.currentContext.token = Token::Report {
        scanner.scan();
Set_Report_Expression (Stmt, Parse_Expression);
end if;
if (scanner.currentContext.token = Token::Severity {
        scanner.scan();
Set_Severity_Expression (Stmt, Parse_Expression);
if (scanner.currentContext.token = Token::Report {
//  Nice message in case of inversion.
Error_Msg_Parse
("report expression must precede severity expression");
scanner.scan();
Set_Report_Expression (Stmt, Parse_Expression);
end if;
end if;
end Parse_Assertion;

//  precond : REPORT
//  postcond: next token
//
//  [ 8.3 ]
//  report_statement ::= REPORT expression [ SEVERITY expression ]
Parse_Report_Statement return Iir_Report_Statement
        is
Res : Iir_Report_Statement;
begin
        Res := Create_Iir (Iir_Report_Statement);
Set_Location (Res);
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: report statement not allowed in vhdl87");
end if;
scanner.scan();
Set_Report_Expression (Res, Parse_Expression);
if (scanner.currentContext.token = Token::Severity {
        scanner.scan();
Set_Severity_Expression (Res, Parse_Expression);
end if;
return Res;
end Parse_Report_Statement;

// precond : WAIT
// postcond: ';'
//
//  [ 8.1 ]
//  wait_statement ::=
//      [ label : ] WAIT [ sensitivity_clause ] [ condition_clause ]
//          [ timeout_clause ] ;
//
//  [ 8.1 ]
//  sensitivity_clause ::= ON sensitivity_list
//
//  [ 8.1 ]
//  condition_clause ::= UNTIL conditiion
//
//  [ 8.1 ]
//  timeout_clause ::= FOR TIME_expression
        function Parse_Wait_Statement return Iir_Wait_Statement
        is
Res: Iir_Wait_Statement;
List: Iir_List;
begin
        Res := Create_Iir (Iir_Wait_Statement);
Set_Location (Res);
scanner.scan();
switch (scanner.currentContext.token) {
case Token::On:
List := Create_Iir_List;
Set_Sensitivity_List (Res, List);
scanner.scan();
Parse_Sensitivity_List (List);
case Token::Until:
null;
case Token::For:
null;
case Token::Semi_Colon:
return Res;
default:
throw std::runtime_error("PARSE ERROR: 'on', 'until', 'for' or ';' expected");
Eat_Tokens_Until_Semi_Colon;
return Res;
}
switch (scanner.currentContext.token) {
case Token::On:
throw std::runtime_error("PARSE ERROR: only one sensitivity is allowed");
// FIXME: sync
return Res;
case Token::Until:
scanner.scan();
Set_Condition_Clause (Res, Parse_Expression);
case Token::For:
null;
case Token::Semi_Colon:
return Res;
default:
throw std::runtime_error("PARSE ERROR: 'until', 'for' or ';' expected");
Eat_Tokens_Until_Semi_Colon;
return Res;
}
switch (scanner.currentContext.token) {
case Token::On:
throw std::runtime_error("PARSE ERROR: only one sensitivity clause is allowed");
// FIXME: sync
return Res;
case Token::Until:
throw std::runtime_error("PARSE ERROR: only one condition clause is allowed");
// FIXME: sync
return Res;
case Token::For:
scanner.scan();
Set_Timeout_Clause (Res, Parse_Expression);
return Res;
case Token::Semi_Colon:
return Res;
default:
throw std::runtime_error("PARSE ERROR: 'for' or ';' expected");
Eat_Tokens_Until_Semi_Colon;
return Res;
}
end Parse_Wait_Statement;

//  precond : IF
//  postcond: next token.
//
//  [ 8.7 ]
//  if_statement ::=
//    [ IF_label : ]
//        IF condition {
//            sequence_of_statements
//        { ELSIF condition {
//            sequence_of_statements }
//        [ ELSE
//            sequence_of_statements ]
//        END IF [ IF_label ] ;
//
// FIXME: end label.
Iir_If_Statement Parse_If_Statement (Parent : Iir) 
        is
Res: Iir_If_Statement;
Clause: Iir;
N_Clause: Iir;
begin
        Res := Create_Iir (Iir_If_Statement);
Set_Location (Res);
Set_Parent (Res, Parent);
scanner.scan();
Clause := Res;
loop
        Set_Condition (Clause, Parse_Expression);
Expect (Token::Then, "'then' is expected here");

//  Skip 'then'.
scanner.scan();

Set_Sequential_Statement_Chain
(Clause, Parse_Sequential_Statements (Res));
exit when scanner.currentContext.token = Token::End;
N_Clause := Create_Iir (Iir_Elsif);
Set_Location (N_Clause);
Set_Else_Clause (Clause, N_Clause);
Clause := N_Clause;
if (scanner.currentContext.token = Token::Else {

//  Skip 'else'.
scanner.scan();

Set_Sequential_Statement_Chain
(Clause, Parse_Sequential_Statements (Res));
exit;
elsif (scanner.currentContext.token = Token::Elsif {
//  Skip 'elsif'.
scanner.scan();
else
throw std::runtime_error("PARSE ERROR: 'else' or 'elsif' expected");
end if;
end loop;
Expect (Token::End);
Scan_Expect (Token::If);
scanner.scan();
return Res;
end Parse_If_Statement;

Parenthesis_Name_To_Procedure_Call (Name: Iir; Kind : Iir_Kind)
return Iir
        is
Res: Iir;
Call : Iir_Procedure_Call;
begin
        Res := Create_Iir (Kind);
Location_Copy (Res, Name);
Call := Create_Iir (Iir_Procedure_Call);
Location_Copy (Call, Name);
Set_Procedure_Call (Res, Call);
switch (Get_Kind (Name) is
        when Iir_Parenthesis_Name:
Set_Prefix (Call, Get_Prefix (Name));
Set_Parameter_Association_Chain
(Call, Get_Association_Chain (Name));
Free_Iir (Name);
case Iir_Simple_Name
| Iir_Selected_Name:
Set_Prefix (Call, Name);
case Iir_Attribute_Name:
throw std::runtime_error("PARSE ERROR: attribute cannot be used as procedure call");
default:
Error_Kind ("parenthesis_name_to_procedure_call", Name);
}
return Res;
end Parenthesis_Name_To_Procedure_Call;

//  precond : identifier
//  postcond: next token
//
//  [ LRM93 8.9 ]
//  parameter_specification ::= identifier IN discrete_range
Parse_Parameter_Specification (Parent : Iir)
return Iir_Iterator_Declaration
        is
Decl : Iir_Iterator_Declaration;
begin
        Decl := Create_Iir (Iir_Iterator_Declaration);
Set_Location (Decl);
Set_Parent (Decl, Parent);

Expect (Token::Identifier);
Set_Identifier (Decl, scanner.currentContext.Identifier);

//  Skip identifier
Scan_Expect (Token::In);

//  Skip 'in'
scanner.scan();

Set_Discrete_Range (Decl, Parse_Discrete_Range);
return Decl;
end Parse_Parameter_Specification;

//  precond:  '<='
//  postcond: next token
//
//  [ LRM93 8.4 ]
//  signal_assignment_statement ::=
//      [ label : ] target <= [ delay_mechanism ] waveform ;
//
//  [ LRM08 10.5 Signal assignment statement ]
//  signal_assignement_statement ::=
//      [ label : ] simple_signal_assignement
//    | [ label : ] conditional_signal_assignement
//    | [ label : ] selected_signal_assignement (TODO)
Iir Parse_Signal_Assignment_Statement (Target : Iir) 
        is
Stmt : Iir;
N_Stmt : Iir;
Wave_Chain : Iir;
begin
        Stmt := Create_Iir (Iir_Simple_Signal_Assignment_Statement);
Set_Location (Stmt);
Set_Target (Stmt, Target);

//  Eat '<='.
scanner.scan();

Parse_Delay_Mechanism (Stmt);

Wave_Chain := Parse_Conditional_Waveforms;

//  LRM 8.4 Signal assignment statement
//  It is an error is the reserved word UNAFFECTED appears as a
//  waveform in a (sequential) signal assignment statement.
if Get_Kind (Wave_Chain) = Iir_Unaffected_Waveform {
if Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("'unaffected' is not allowed in a sequential statement");
end if;
Set_Waveform_Chain (Stmt, Wave_Chain);
elsif Get_Kind (Wave_Chain) = Iir_Conditional_Waveform {
if Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("conditional signal assignment not allowed in before vhdl08");
end if;
N_Stmt :=
Create_Iir (Iir_Conditional_Signal_Assignment_Statement);
Location_Copy (N_Stmt, Stmt);
Set_Target (N_Stmt, Target);
Set_Delay_Mechanism (N_Stmt, Get_Delay_Mechanism (Stmt));
Set_Reject_Time_Expression
(N_Stmt, Get_Reject_Time_Expression (Stmt));
Set_Conditional_Waveform_Chain (N_Stmt, Wave_Chain);
Free_Iir (Stmt);
Stmt := N_Stmt;
else
Set_Waveform_Chain (Stmt, Wave_Chain);
end if;

return Stmt;
end Parse_Signal_Assignment_Statement;

//  precond:  WHEN
//  postcond: next token
//
//  [ LRM08 10.5.3 Conditional signal assignments ]
//  conditional_expressions ::=
//      expression WHEN condition
//    { ELSE expression WHEN condition }
//    [ ELSE expression ]
Iir Parse_Conditional_Expression (Expr : Iir) 
        is
Res : Iir;
El, N_El : Iir;
begin
        Res := Create_Iir (Iir_Conditional_Expression);
Set_Location (Res);
Set_Expression (Res, Expr);
El := Res;

loop
//  Eat 'when'
scanner.scan();

Set_Condition (El, Parse_Expression);

exit when scanner.currentContext.token != Token::Else;

N_El := Create_Iir (Iir_Conditional_Expression);
Set_Location (N_El);
Set_Chain (El, N_El);
El := N_El;

//  Eat 'else'
scanner.scan();

Set_Expression (N_El, Parse_Expression);

exit when scanner.currentContext.token != Token::When;
end loop;

return Res;
end Parse_Conditional_Expression;

//  precond:  ':='
//  postcond: next token
//
//  [ LRM93 8.5 ]
//  variable_assignment_statement ::=
//      [ label : ] target := expression ;
Iir Parse_Variable_Assignment_Statement (Target : Iir) 
        is
Stmt : Iir;
Loc : Location_Type;
Expr : Iir;
begin
        Loc := Get_Token_Location;

//  Eat ':='
scanner.scan();

Expr := Parse_Expression;

if (scanner.currentContext.token = Token::When {
if Flags.Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("conditional variable assignment not allowed before vhdl08");
end if;
Stmt :=
Create_Iir (Iir_Conditional_Variable_Assignment_Statement);
Set_Location (Stmt, Loc);
Set_Target (Stmt, Target);
Set_Conditional_Expression
(Stmt, Parse_Conditional_Expression (Expr));
else
Stmt := Create_Iir (Iir_Variable_Assignment_Statement);
Set_Location (Stmt, Loc);
Set_Target (Stmt, Target);
Set_Expression (Stmt, Expr);
end if;
return Stmt;
end Parse_Variable_Assignment_Statement;

//  precond:  '<=', ':=' or ';'
//  postcond: next token
        Iir function Parse_Sequential_Assignment_Statement (Target : Iir) 
        is
Stmt : Iir;
Call : Iir;
begin
if (scanner.currentContext.token = Token::Less_Equal {
return Parse_Signal_Assignment_Statement (Target);
elsif (scanner.currentContext.token = Token::Assign {
return Parse_Variable_Assignment_Statement (Target);
elsif (scanner.currentContext.token = Token::Semi_Colon {
return Parenthesis_Name_To_Procedure_Call
(Target, Iir_Procedure_Call_Statement);
else
Error_Msg_Parse
("""<="" or "":="" expected instead of %t", +scanner.currentContext.token);
Stmt := Create_Iir (Iir_Procedure_Call_Statement);
Call := Create_Iir (Iir_Procedure_Call);
Set_Prefix (Call, Target);
Set_Procedure_Call (Stmt, Call);
Set_Location (Call);
Eat_Tokens_Until_Semi_Colon;
return Stmt;
end if;
end Parse_Sequential_Assignment_Statement;

//  precond:  CASE
//  postcond: ';'
//
//  [ 8.8 ]
//  case_statement ::=
//      [ CASE_label : ]
//          CASE expression IS
//              case_statement_alternative
//              { case_statement_alternative }
//          END CASE [ CASE_label ] ;
//
//  [ 8.8 ]
//  case_statement_alternative ::= WHEN choices: sequence_of_statements
        Iir function Parse_Case_Statement (Label : Name_Id) 
        is
use Iir_Chains.Case_Statement_Alternative_Chain_Handling;
Stmt : Iir;
Assoc: Iir;
Last_Assoc : Iir;
begin
        Stmt := Create_Iir (Iir_Case_Statement);
Set_Label (Stmt, Label);
Set_Location (Stmt);

//  Skip 'case'.
scanner.scan();

Set_Expression (Stmt, Parse_Expression);

//  Skip 'is'.
Expect (Token::Is);
scanner.scan();
if (scanner.currentContext.token = Token::End {
        throw std::runtime_error("PARSE ERROR: missing alternative in case statement");
end if;
Build_Init (Last_Assoc);
while scanner.currentContext.token != Token::End loop
//  Eat 'when'
Expect (Token::When);
scanner.scan();

if (scanner.currentContext.token = Token::Double_Arrow {
        throw std::runtime_error("PARSE ERROR: missing expression in alternative");
Assoc := Create_Iir (Iir_Choice_By_Expression);
Set_Location (Assoc);
else
Assoc := Parse_Choices (nullptr);
end if;

//  Eat '=>'
Expect (Token::Double_Arrow);
scanner.scan();

Set_Associated_Chain (Assoc, Parse_Sequential_Statements (Stmt));
Append_Subchain (Last_Assoc, Stmt, Assoc);
end loop;

//  Eat 'end', 'case'
Scan_Expect (Token::Case);
scanner.scan();

if Flags.Vhdl_Std >= Vhdl_93c {
Check_End_Name (Stmt);
end if;

return Stmt;
end Parse_Case_Statement;

//  precond:  next token
//  postcond: next token
//
//  [ LRM93 8 ]
//  sequence_of_statement ::= { sequential_statement }
//
//  [ 8 ]
//  sequential_statement ::= wait_statement
        --                         | assertion_statement
        --                         | report_statement
        --                         | signal_assignment_statement
        --                         | variable_assignment_statement
        --                         | procedure_call_statement
        --                         | if_statement
        --                         | case_statement
        --                         | loop_statement
        --                         | next_statement
        --                         | exit_statement
        --                         | return_statement
        --                         | null_statement
        --
        --  [ 8.13 ]
        --  null_statement ::= [ label : ] NULL ;
//
//  [ 8.12 ]
//  return_statement ::= [ label : ] RETURN [ expression ]
//
//  [ 8.10 ]
//  next_statement ::= [ label : ] NEXT [ LOOP_label ] [ WHEN condition ] ;
//
//  [ 8.11 ]
//  exit_statement ::= [ label : ] EXIT [ LOOP_label ] [ WHEN condition ] ;
//
//  [ 8.9 ]
//  loop_statement ::=
//      [ LOOP_label : ]
//          [ iteration_scheme ] LOOP
//              sequence_of_statements
//          END LOOP [ LOOP_label ] ;
//
//  [ 8.9 ]
//  iteration_scheme ::= WHILE condition
//                     | FOR LOOP_parameter_specification
//
//  [ 8.2 ]
//  assertion_statement ::= [ label : ] assertion ;
//
//  [ 8.3 ]
//  report_statement ::= [ label : ] REPORT expression SEVERITY expression ;
Parse_Sequential_Statements (Parent : Iir)
return Iir
        is
First_Stmt : Iir;
Last_Stmt : Iir;
Stmt: Iir;
Label: Name_Id;
Loc : Location_Type;
Target : Iir;
begin
        First_Stmt := nullptr;
Last_Stmt := nullptr;
// Expect a current_token.
loop
        Loc := Get_Token_Location;
if (scanner.currentContext.token = Token::Identifier {
        Label := scanner.currentContext.Identifier;
scanner.scan();
if (scanner.currentContext.token = Token::Colon {
        scanner.scan();
else
Target := Create_Iir (Iir_Simple_Name);
Set_Identifier (Target, Label);
Set_Location (Target, Loc);
Label := Null_Identifier;
Target := Parse_Name_Suffix (Target, true);
Stmt := Parse_Sequential_Assignment_Statement (Target);
goto Has_Stmt;
end if;
else
Label := Null_Identifier;
end if;

switch (scanner.currentContext.token) {
case Token::Null:
Stmt := Create_Iir (Iir_Null_Statement);

//  Skip 'null'.
scanner.scan();

case Token::Assert:
Stmt := Create_Iir (Iir_Assertion_Statement);
Parse_Assertion (Stmt);
case Token::Report:
Stmt := Parse_Report_Statement;
case Token::If:
Stmt := Parse_If_Statement (Parent);
Set_Label (Stmt, Label);
Set_Location (Stmt, Loc);
if Flags.Vhdl_Std >= Vhdl_93c {
Check_End_Name (Stmt);
end if;
case Token::Case:
Stmt := Parse_Case_Statement (Label);
case Token::Identifier
| Token::String:
//  String for an expanded name with operator_symbol prefix.
Stmt := Parse_Sequential_Assignment_Statement (Parse_Name);
case Token::Left_Paren:
declare
        Target : Iir;
begin
        Target := Parse_Aggregate;
if (scanner.currentContext.token = Token::Less_Equal {
        Stmt := Parse_Signal_Assignment_Statement (Target);
elsif (scanner.currentContext.token = Token::Assign {
        Stmt := Parse_Variable_Assignment_Statement (Target);
else
throw std::runtime_error("PARSE ERROR: '<=' or ':=' expected");
return First_Stmt;
end if;
end;

case Token::Return:
Stmt := Create_Iir (Iir_Return_Statement);
scanner.scan();
if (scanner.currentContext.token != Token::Semi_Colon {
Set_Expression (Stmt, Parse_Expression);
end if;

case Token::For:
Stmt := Create_Iir (Iir_For_Loop_Statement);
Set_Location (Stmt, Loc);
Set_Label (Stmt, Label);

//  Skip 'for'
scanner.scan();

Set_Parameter_Specification
(Stmt, Parse_Parameter_Specification (Stmt));

//  Skip 'loop'
Expect (Token::Loop);
scanner.scan();

Set_Sequential_Statement_Chain
(Stmt, Parse_Sequential_Statements (Stmt));

//  Skip 'end'
Expect (Token::End);
Scan_Expect (Token::Loop);

//  Skip 'loop'
scanner.scan();

Check_End_Name (Stmt);
//  A loop statement can have a label, even in vhdl87.
Label := Null_Identifier;

case Token::While
| Token::Loop:
Stmt := Create_Iir (Iir_While_Loop_Statement);
Set_Location (Stmt);
Set_Label (Stmt, Label);
if (scanner.currentContext.token = Token::While {
        scanner.scan();
Set_Condition (Stmt, Parse_Expression);
Expect (Token::Loop);
end if;
scanner.scan();
Set_Sequential_Statement_Chain
(Stmt, Parse_Sequential_Statements (Stmt));
Expect (Token::End);
Scan_Expect (Token::Loop);
scanner.scan();
Check_End_Name (Stmt);
//  A loop statement can have a label, even in vhdl87.
Label := Null_Identifier;

case Token::Next
| Token::Exit:
if (scanner.currentContext.token = Token::Next {
        Stmt := Create_Iir (Iir_Next_Statement);
else
Stmt := Create_Iir (Iir_Exit_Statement);
end if;

//  Skip 'next' or 'exit'.
scanner.scan();

if (scanner.currentContext.token = Token::Identifier {
        Set_Loop_Label (Stmt, Parse_Name (Allow_Indexes: false));
end if;

if (scanner.currentContext.token = Token::When {
//  Skip 'when'.
scanner.scan();

Set_Condition (Stmt, Parse_Expression);
end if;

case Token::Wait:
Stmt := Parse_Wait_Statement;
default:
return First_Stmt;
}
<< Has_Stmt >> null;
Set_Parent (Stmt, Parent);
Set_Location (Stmt, Loc);
if Label != Null_Identifier {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Sem
        (+Stmt, "this statement can't have a label in vhdl 87");
else
Set_Label (Stmt, Label);
end if;
end if;
Scan_Semi_Colon ("statement");

//  Append it to the chain.
if First_Stmt = nullptr {
        First_Stmt := Stmt;
else
Set_Chain (Last_Stmt, Stmt);
end if;
Last_Stmt := Stmt;
end loop;
end Parse_Sequential_Statements;

//  precond : PROCEDURE, FUNCTION, PURE or IMPURE.
//  postcond: ';'
//
//  [ 2.1 ]
//  subprogram_declaration ::= subprogram_specification ;
//
//  [ 2.1 ]
//  subprogram_specification ::=
//      PROCEDURE designator [ ( formal_parameter_list ) ]
//    | [ PURE | IMPURE ] FUNCTION designator [ ( formal_parameter_list ) ]
//          RETURN type_mark
//
//  [ 2.2 ]
//  subprogram_body ::=
//      subprogram_specification IS
//          subprogram_declarative_part
//      BEGIN
//          subprogram_statement_part
//      END [ subprogram_kind ] [ designator ] ;
//
//  [ 2.1 ]
//  designator ::= identifier | operator_symbol
        --
        --  [ 2.1 ]
//  operator_symbol ::= string_literal
Parse_Subprogram_Declaration return Iir
        is
Kind : Iir_Kind;
Subprg: Iir;
Subprg_Body : Iir;
Old : Iir;
pragma Unreferenced (Old);
begin
//  Create the node.
switch (scanner.currentContext.token) {
case Token::Procedure:
Kind := Iir_Procedure_Declaration;
case Token::Function
| Token::Pure
| Token::Impure:
Kind := Iir_Function_Declaration;
default:
raise Internal_Error;
}
Subprg := Create_Iir (Kind);
Set_Location (Subprg);
Set_Implicit_Definition (Subprg, Iir_Predefined_None);

switch (scanner.currentContext.token) {
case Token::Procedure:
null;
case Token::Function:
//  LRM93 2.1
//  A function is impure if its specification contains the
//  reserved word IMPURE; otherwise it is said to be pure.
Set_Pure_Flag (Subprg, true);
case Token::Pure
| Token::Impure:
Set_Pure_Flag (Subprg, scanner.currentContext.token = Token::Pure);
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'pure' and 'impure' are not allowed in vhdl 87");
end if;
Set_Has_Pure (Subprg, true);
//  FIXME: what to do in case of error ??

//  Eat 'pure' or 'impure'.
scanner.scan();

Expect (Token::Function, "'function' must follow 'pure' or 'impure'");
default:
raise Internal_Error;
}

//  Eat 'procedure' or 'function'.
scanner.scan();

//  Designator.
Parse_Subprogram_Designator (Subprg);

Parse_Subprogram_Parameters_And_Return
(Subprg, Kind = Iir_Function_Declaration);

if (scanner.currentContext.token = Token::Semi_Colon {
return Subprg;
end if;

//  The body.
Set_Has_Body (Subprg, true);
if Kind = Iir_Function_Declaration {
        Subprg_Body := Create_Iir (Iir_Function_Body);
else
Subprg_Body := Create_Iir (Iir_Procedure_Body);
end if;
Location_Copy (Subprg_Body, Subprg);

Set_Subprogram_Body (Subprg, Subprg_Body);
Set_Subprogram_Specification (Subprg_Body, Subprg);
Set_Chain (Subprg, Subprg_Body);

//  Skip 'is'.
Expect (Token::Is);
scanner.scan();

Parse_Declarative_Part (Subprg_Body);

//  Skip 'begin'.
Expect (Token::Begin);
scanner.scan();

Set_Sequential_Statement_Chain
(Subprg_Body, Parse_Sequential_Statements (Subprg_Body));

//  Skip 'end'.
Expect (Token::End);
scanner.scan();

switch (scanner.currentContext.token) {
case Token::Function:
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'function' not allowed here by vhdl 87");
end if;
if Kind = Iir_Procedure_Declaration {
        throw std::runtime_error("PARSE ERROR: 'procedure' expected instead of 'function'");
end if;
Set_End_Has_Reserved_Id (Subprg_Body, true);

//  Skip 'function'.
scanner.scan();

case Token::Procedure:
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'procedure' not allowed here by vhdl 87");
end if;
if Kind = Iir_Function_Declaration {
        throw std::runtime_error("PARSE ERROR: 'function' expected instead of 'procedure'");
end if;
Set_End_Has_Reserved_Id (Subprg_Body, true);

//  Skip 'procedure'
scanner.scan();

default:
null;
}
switch (scanner.currentContext.token) {
case Token::Identifier:
Check_End_Name (Get_Identifier (Subprg), Subprg_Body);
case Token::String:
if Scan_To_Operator_Name (Get_Token_Location)
!= Get_Identifier (Subprg)
{
        throw std::runtime_error("PARSE ERROR: misspelling, %i expected", +Subprg);
end if;
Set_End_Has_Identifier (Subprg_Body, true);

//  Skip string.
scanner.scan();

default:
null;
}
Expect (Token::Semi_Colon);
return Subprg;
end Parse_Subprogram_Declaration;

//  precond:  PROCESS
//  postcond: null
//
//  [ LRM87 9.2 / LRM08 11.3 ]
//  process_statement ::=
//    [ PROCESS_label : ]
//       [ POSTPONED ] PROCESS [ ( process_sensitivity_list ) ] [ IS ]
//           process_declarative_part
//       BEGIN
//           process_statement_part
//       END [ POSTPONED ] PROCESS [ PROCESS_label ] ;
//
//  process_sensitivity_list ::= ALL | sensitivity_list
Parse_Process_Statement
        (Label: Name_Id; Loc : Location_Type; Is_Postponed : Boolean)
return Iir
        is
Res: Iir;
Sensitivity_List : Iir_List;
begin
//  Skip 'process'
scanner.scan();

if (scanner.currentContext.token = Token::Left_Paren {
        Res := Create_Iir (Iir_Sensitized_Process_Statement);

//  Skip '('
scanner.scan();

if (scanner.currentContext.token = Token::All {
if Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("all sensitized process allowed only in vhdl 08");
end if;
Sensitivity_List := Iir_List_All;

//  Skip 'all'
scanner.scan();
else
Sensitivity_List := Create_Iir_List;
Parse_Sensitivity_List (Sensitivity_List);
end if;
Set_Sensitivity_List (Res, Sensitivity_List);

//  Skip ')'
Expect (Token::Right_Paren);
scanner.scan();
else
Res := Create_Iir (Iir_Process_Statement);
end if;

Set_Location (Res, Loc);
Set_Label (Res, Label);
Set_Has_Label (Res, Label != Null_Identifier);

if (scanner.currentContext.token = Token::Is {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: ""is"" not allowed here by vhdl 87");
end if;
Set_Has_Is (Res, true);

//  Skip 'is'
scanner.scan();
end if;

// declarative part.
Parse_Declarative_Part (Res);

//  Skip 'begin'.
Expect (Token::Begin);
scanner.scan();

Set_Sequential_Statement_Chain (Res, Parse_Sequential_Statements (Res));

//  Skip 'end'.
Expect (Token::End);
scanner.scan();

if (scanner.currentContext.token = Token::Postponed {
if not Is_Postponed {
//  LRM93 9.2
//  If the reserved word POSTPONED appears at the end of a process
//  statement, the process must be a postponed process.
throw std::runtime_error("PARSE ERROR: process is not a postponed process");
end if;

Set_End_Has_Postponed (Res, true);

//  Skip 'postponed',
scanner.scan();
end if;

if (scanner.currentContext.token = Token::Semi_Colon {
        throw std::runtime_error("PARSE ERROR: ""end"" must be followed by ""process""");
else
Expect (Token::Process);
scanner.scan();
Set_End_Has_Reserved_Id (Res, true);
Check_End_Name (Res);
Expect (Token::Semi_Colon);
end if;
return Res;
end Parse_Process_Statement;

Iir is Check_Formal_Form (Formal : Iir) 
begin
if Formal = nullptr {
return Formal;
end if;

switch (Get_Kind (Formal) is
        when Iir_Simple_Name
| Iir_Slice_Name
| Iir_Selected_Name:
return Formal;
caseIir_Parenthesis_Name:
//  Could be an indexed name, so nothing to check within the
//  parenthesis.
return Formal;
caseIir_String_Literal8:
//  Operator designator
return String_To_Operator_Symbol (Formal);
default:
throw std::runtime_error("PARSE ERROR:" + +Formal, "incorrect formal name ignored");
return nullptr;
}
end Check_Formal_Form;

// precond : NEXT_TOKEN
// postcond: NEXT_TOKEN
//
//  [ LRM93 4.3.2.2 ]
//  association_list ::= association_element { , association_element }
        --
        --  [ LRM93 4.3.2.2 ]
//  association_element ::= [ formal_part: ] actual_part
//
//  [ LRM93 4.3.2.2 ]
//  actual_part ::= actual_designator
        --                | FUNCTION_name ( actual_designator )
        --                | type_mark ( actual_designator )
        --
        --  [ LRM93 4.3.2.2 ]
//  actual_designator ::= expression
        --                      | SIGNAL_name
        --                      | VARIABLE_name
        --                      | FILE_name
        --                      | OPEN
        --
        --  [ LRM93 4.3.2.2 ]
//  formal_part ::= formal_designator
        --                | FUNCTION_name ( formal_designator )
        --                | type_mark ( formal_designator )
        --
        --  [ LRM93 4.3.2.2 ]
//  formal_designator ::= GENERIC_name
        --                      | PORT_name
        --                      | PARAMETER_name
        --
        --  Note: an actual part is parsed as an expression.
Parse_Association_List return Iir
        is
Res, Last: Iir;
El: Iir;
Formal: Iir;
Actual: Iir;
Nbr_Assocs : Natural;
Loc : Location_Type;
begin
        Sub_Chain_Init (Res, Last);

if (scanner.currentContext.token = Token::Right_Paren {
        throw std::runtime_error("PARSE ERROR: empty association list is not allowed");
return Res;
end if;

Nbr_Assocs := 1;
loop
//  Parse formal and actual.
Loc := Get_Token_Location;
Formal := nullptr;

if (scanner.currentContext.token != Token::Open {
Actual := Parse_Expression;
switch (scanner.currentContext.token) {
caseToken::To
| Token::Downto:
//  To/downto can appear in slice name (which are parsed as
//  function call).

if Actual = nullptr {
//  Left expression is missing ie: (downto x).
scanner.scan();
Actual := Parse_Expression;
else
Actual := Parse_Range_Expression (Actual);
end if;
if Nbr_Assocs != 1 {
        throw std::runtime_error("PARSE ERROR: multi-dimensional slice is forbidden");
end if;

caseToken::Double_Arrow:
//  Check that FORMAL is a name and not an expression.
Formal := Check_Formal_Form (Actual);

//  Skip '=>'
scanner.scan();
Loc := Get_Token_Location;

if (scanner.currentContext.token != Token::Open {
Actual := Parse_Expression;
end if;

default:
null;
}
end if;

if (scanner.currentContext.token = Token::Open {
        El := Create_Iir (Iir_Association_Element_Open);
Set_Location (El);

//  Skip 'open'
scanner.scan();
else
El := Create_Iir (Iir_Association_Element_By_Expression);
Set_Location (El, Loc);
Set_Actual (El, Actual);
end if;
Set_Formal (El, Formal);

Sub_Chain_Append (Res, Last, El);
exit when scanner.currentContext.token = Token::Right_Paren;
Expect (Token::Comma);
scanner.scan();
Nbr_Assocs := Nbr_Assocs + 1;
end loop;

return Res;
end Parse_Association_List;

// precond : NEXT_TOKEN
// postcond: NEXT_TOKEN
//
// Parse: '(' association_list ')'
Parse_Association_List_In_Parenthesis return Iir
        is
Res : Iir;
begin
//  Skip '('
Expect (Token::Left_Paren);
scanner.scan();

Res := Parse_Association_List;

//  Skip ')'
scanner.scan();

return Res;
end Parse_Association_List_In_Parenthesis;

//  precond : GENERIC
//  postcond: next token
//
//  [ LRM93 5.2.1.2, LRM08 6.5.7.2 ]
//  generic_map_aspect ::= GENERIC MAP ( GENERIC_association_list )
Parse_Generic_Map_Aspect return Iir is
begin
        Expect (Token::Generic);

//  Skip 'generic'.
Scan_Expect (Token::Map);

//  Skip 'map'.
scanner.scan();

return Parse_Association_List_In_Parenthesis;
end Parse_Generic_Map_Aspect;

//  precond : PORT
//  postcond: next token
//
//  [ 5.2.1.2 ]
//  port_map_aspect ::= PORT MAP ( PORT_association_list )
Parse_Port_Map_Aspect return Iir is
begin
        Expect (Token::Port);
Scan_Expect (Token::Map);
scanner.scan();
return Parse_Association_List_In_Parenthesis;
end Parse_Port_Map_Aspect;

//  precond : COMPONENT | ENTIY | CONFIGURATION
//  postcond : next_token
//
//  instantiated_unit ::=
//      [ COMPONENT ] component_name
//      ENTITY entity_name [ ( architecture_identifier ) ]
//      CONFIGURATION configuration_name
Parse_Instantiated_Unit return Iir
        is
Res : Iir;
begin
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("component instantiation using keyword 'component', 'entity',",
                Cont: true);
throw std::runtime_error("PARSE ERROR:  or 'configuration' is not allowed in vhdl87");
end if;

switch (scanner.currentContext.token) {
caseToken::Component:
scanner.scan();
return Parse_Name (false);
caseToken::Entity:
Res := Create_Iir (Iir_Entity_Aspect_Entity);
Set_Location (Res);
scanner.scan();
Set_Entity_Name (Res, Parse_Name (false));
if (scanner.currentContext.token = Token::Left_Paren {
        Scan_Expect (Token::Identifier);
Set_Architecture (Res, Current_Text);
Scan_Expect (Token::Right_Paren);
scanner.scan();
end if;
return Res;
case Token::Configuration:
Res := Create_Iir (Iir_Entity_Aspect_Configuration);
Set_Location (Res);
Scan_Expect (Token::Identifier);
Set_Configuration_Name (Res, Parse_Name (false));
return Res;
default:
raise Internal_Error;
}
end Parse_Instantiated_Unit;

//  precond : next token
//  postcond: ';'
//
//  component_instantiation_statement ::=
//      INSTANTIATION_label :
//          instantiated_unit [ generic_map_aspect ] [ port_map_aspect ] ;
Parse_Component_Instantiation (Name: Iir)
return Iir_Component_Instantiation_Statement
        is
Res: Iir_Component_Instantiation_Statement;
begin
        Res := Create_Iir (Iir_Component_Instantiation_Statement);
Set_Location (Res);

Set_Instantiated_Unit (Res, Name);

if (scanner.currentContext.token = Token::Generic {
        Set_Generic_Map_Aspect_Chain (Res, Parse_Generic_Map_Aspect);
end if;
if (scanner.currentContext.token = Token::Port {
        Set_Port_Map_Aspect_Chain (Res, Parse_Port_Map_Aspect);
end if;
Expect (Token::Semi_Colon);
return Res;
end Parse_Component_Instantiation;

//  precond : next token
//  postcond: next token
//
//  [ 9.1 ]
//  block_header ::= [ generic_clause [ generic_map_aspect ; ] ]
//                   [ port_clause [ port_map_aspect ; ] ]
Parse_Block_Header return Iir_Block_Header is
Res : Iir_Block_Header;
begin
        Res := Create_Iir (Iir_Block_Header);
Set_Location (Res);
if (scanner.currentContext.token = Token::Generic {
        Parse_Generic_Clause (Res);
if (scanner.currentContext.token = Token::Generic {
        Set_Generic_Map_Aspect_Chain (Res, Parse_Generic_Map_Aspect);
Scan_Semi_Colon ("generic map aspect");
end if;
end if;
if (scanner.currentContext.token = Token::Port {
        Parse_Port_Clause (Res);
if (scanner.currentContext.token = Token::Port {
        Set_Port_Map_Aspect_Chain (Res, Parse_Port_Map_Aspect);
Scan_Semi_Colon ("port map aspect");
end if;
end if;
return Res;
end Parse_Block_Header;

//  precond : BLOCK
//  postcond: ';'
//
//  [ 9.1 ]
//  block_statement ::=
//      BLOCK_label :
//          BLOCK [ ( GUARD_expression ) ] [ IS ]
//              block_header
//              block_declarative_part
//          BEGIN
//              block_statement_part
//          END BLOCK [ BLOCK_label ] ;
//
//  [ 9.1 ]
//  block_declarative_part ::= { block_declarative_item }
//
//  [ 9.1 ]
//  block_statement_part ::= { concurrent_statement }
Parse_Block_Statement (Label: Name_Id; Loc : Location_Type)
return Iir_Block_Statement
        is
Res : Iir_Block_Statement;
Guard : Iir_Guard_Signal_Declaration;
begin
if Label = Null_Identifier {
        throw std::runtime_error("PARSE ERROR: a block statement must have a label");
end if;

// block was just parsed.
Res := Create_Iir (Iir_Block_Statement);
Set_Location (Res, Loc);
Set_Label (Res, Label);
scanner.scan();
if (scanner.currentContext.token = Token::Left_Paren {
        Guard := Create_Iir (Iir_Guard_Signal_Declaration);
Set_Location (Guard);
Set_Guard_Decl (Res, Guard);
scanner.scan();
Set_Guard_Expression (Guard, Parse_Expression);
Expect (Token::Right_Paren, "a ')' is expected after guard expression");
scanner.scan();
end if;
if (scanner.currentContext.token = Token::Is {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'is' not allowed here in vhdl87");
end if;
scanner.scan();
end if;
if (scanner.currentContext.token = Token::Generic or scanner.currentContext.token = Token::Port {
        Set_Block_Header (Res, Parse_Block_Header);
end if;
if (scanner.currentContext.token != Token::Begin {
Parse_Declarative_Part (Res);
end if;
Expect (Token::Begin);
scanner.scan();
Parse_Concurrent_Statements (Res);
Check_End_Name (Token::Block, Res);
return Res;
end Parse_Block_Statement;

//  Precond : next token
//  Postcond: next token after 'end'
//
//  [ LRM08 11.8 ] Generate statements
//  generate_statement_body ::=
//        [ block_declarative_part
//     BEGIN ]
//        { concurrent_statement }
//     [ END [ alternative_label ] ; ]
//
//  This corresponds to the following part of LRM93 9.7:
//        [ { block_declarative_item }
//     BEGIN ]
//        { concurrent_statement }
//  Note there is no END.  This part is followed by:
//     END GENERATE [ /generate/_label ] ;
Parse_Generate_Statement_Body (Parent : Iir; Label : Name_Id)
return Iir
        is
Is_Early_End return Boolean is
begin
switch (scanner.currentContext.token) {
case Token::Elsif
| Token::Else:
if Get_Kind (Parent) = Iir_If_Generate_Statement {
return true;
end if;
case Token::When:
if Get_Kind (Parent) = Iir_Case_Generate_Statement {
return true;
end if;
default:
null;
}
return false;
end Is_Early_End;

Bod : Iir;
begin
        Bod := Create_Iir (Iir_Generate_Statement_Body);
Set_Location (Bod);
Set_Parent (Bod, Parent);
Set_Alternative_Label (Bod, Label);
Set_Has_Label (Bod, Label != Null_Identifier);

//  Check for a block declarative item.
switch (scanner.currentContext.token) {
case
//  subprogram_declaration
//  subprogram_body
        Token::Procedure
| Token::Function
| Token::Pure
| Token::Impure
//  type_declaration
| Token::Type
//  subtype_declaration
| Token::Subtype
//  constant_declaration
| Token::Constant
//  signal_declaration
| Token::Signal
//  shared_variable_declaration
| Token::Shared
| Token::Variable
//  file_declaration
| Token::File
//  alias_declaration
| Token::Alias
//  component_declaration
| Token::Component
//  attribute_declaration
//  attribute_specification
| Token::Attribute
//  configuration_specification
| Token::For
//  disconnection_specification
| Token::Disconnect
//  use_clause
| Token::Use
//  group_template_declaration
//  group_declaration
| Token::Group
| Token::Begin:
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("declarations not allowed in a generate in vhdl87");
end if;
Parse_Declarative_Part (Bod);
Expect (Token::Begin);
Set_Has_Begin (Bod, true);

//  Skip 'begin'
scanner.scan();
default:
null;
}

Parse_Concurrent_Statements (Bod);

//  Return now if no 'end' (and not expected).
if Is_Early_End {
return Bod;
end if;

Expect (Token::End);

//  Skip 'end'
scanner.scan();

if Vhdl_Std >= Vhdl_08 && scanner.currentContext.token != Token::Generate {
//  This is the 'end' of the generate_statement_body.
Set_Has_End (Bod, true);
Check_End_Name (Label, Bod);
Scan_Semi_Colon ("generate statement body");

//  Return now if no 'end' (and not expected).
if Is_Early_End {
return Bod;
end if;

Expect (Token::End);

//  Skip 'end'
scanner.scan();
end if;

return Bod;
end Parse_Generate_Statement_Body;

//  precond : FOR
//  postcond: ';'
//
//  [ LRM93 9.7 ]
//  generate_statement ::=
//      GENERATE_label : generation_scheme GENERATE
//          [ { block_declarative_item }
//      BEGIN ]
//          { concurrent_statement }
//      END GENERATE [ GENERATE_label ] ;
//
//  [ LRM93 9.7 ]
//  generation_scheme ::=
//      FOR GENERATE_parameter_specification
//      | IF condition
//
//  [ LRM08 11.8 ]
//  for_generate_statement ::=
//     /generate/_label :
//        FOR /generate/_parameter_specification GENERATE
//           generate_statement_body
//        END GENERATE [ /generate/_label ] ;
//
//  FIXME: block_declarative item.
Parse_For_Generate_Statement (Label : Name_Id; Loc : Location_Type)
return Iir
        is
Res : Iir;
begin
if Label = Null_Identifier {
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");
end if;
Res := Create_Iir (Iir_For_Generate_Statement);
Set_Location (Res, Loc);
Set_Label (Res, Label);

//  Skip 'for'
scanner.scan();

Set_Parameter_Specification (Res, Parse_Parameter_Specification (Res));

//  Skip 'generate'
Expect (Token::Generate);
scanner.scan();

Set_Generate_Statement_Body
(Res, Parse_Generate_Statement_Body (Res, Null_Identifier));

Expect (Token::Generate);
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'generate'
scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
Check_End_Name (Res);
Expect (Token::Semi_Colon);
return Res;
end Parse_For_Generate_Statement;

//  precond : IF
//  postcond: ';'
//
//  [ LRM93 9.7 ]
//  generate_statement ::=
//      /generate/_label : generation_scheme GENERATE
//          [ { block_declarative_item }
//      BEGIN ]
//          { concurrent_statement }
//      END GENERATE [ /generate/_label ] ;
//
//  [ LRM93 9.7 ]
//  generation_scheme ::=
//      FOR GENERATE_parameter_specification
//      | IF condition
//
//  [ LRM08 11.8 ]
//  if_generate_statement ::=
//     /generate/_label :
//     IF [ /alternative/_label : ] condition GENERATE
//        generate_statement_body
//     { ELSIF [ /alternative/_label : ] condition GENERATE
//        generate_statement_body }
//     [ ELSE [ /alternative/_label : ] GENERATE
//        generate_statement_body ]
//     END GENERATE [ /generate/_label ] ;
Parse_If_Generate_Statement (Label : Name_Id; Loc : Location_Type)
return Iir_Generate_Statement
        is
Res : Iir_Generate_Statement;
Alt_Label : Name_Id;
Alt_Loc : Location_Type;
Cond : Iir;
Clause : Iir;
Bod : Iir;
Last : Iir;
begin
if Label = Null_Identifier {
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");
end if;
Res := Create_Iir (Iir_If_Generate_Statement);
Set_Location (Res, Loc);
Set_Label (Res, Label);

//  Skip 'if'.
scanner.scan();

Clause := Res;
Last := nullptr;
loop
        Cond := Parse_Expression;

Alt_Label := Null_Identifier;
if (scanner.currentContext.token = Token::Colon {
if Get_Kind (Cond) = Iir_Simple_Name {
if Vhdl_Std < Vhdl_08 {
        Error_Msg_Parse
        ("alternative label not allowed before vhdl08");
end if;

//  In fact the parsed condition was an alternate label.
Alt_Label := Get_Identifier (Cond);
Alt_Loc := Get_Location (Cond);
Free_Iir (Cond);
else
throw std::runtime_error("PARSE ERROR: alternative label must be an identifier");
Free_Iir (Cond);
end if;

//  Skip ':'
scanner.scan();

Cond := Parse_Expression;
end if;

Set_Condition (Clause, Cond);

//  Skip 'generate'
Expect (Token::Generate);
scanner.scan();

Bod := Parse_Generate_Statement_Body (Res, Alt_Label);

if Alt_Label != Null_Identifier {
//  Set location on the label, for xrefs.
Set_Location (Bod, Alt_Loc);
end if;

Set_Generate_Statement_Body (Clause, Bod);

//  Append clause to the generate statement.
if Last != nullptr {
Set_Generate_Else_Clause (Last, Clause);
end if;
Last := Clause;

exit when scanner.currentContext.token != Token::Elsif;

//  Create new alternative.
Clause := Create_Iir (Iir_If_Generate_Statement);
Set_Location (Clause, Loc);

//  Skip 'elsif'
scanner.scan();
end loop;

if (scanner.currentContext.token = Token::Else {
if Vhdl_Std < Vhdl_08 {
        throw std::runtime_error("PARSE ERROR: else generate not allowed before vhdl08");
end if;

Clause := Create_Iir (Iir_If_Generate_Else_Clause);
Set_Location (Clause);

//  Skip 'else'
scanner.scan();

if (scanner.currentContext.token = Token::Identifier {
        Alt_Label := scanner.currentContext.Identifier;
Alt_Loc := Get_Token_Location;

//  Skip identifier
scanner.scan();

Expect (Token::Colon);

//  Skip ':'
scanner.scan();
else
Alt_Label := Null_Identifier;
end if;

//  Skip 'generate'
Expect (Token::Generate);
scanner.scan();

Bod := Parse_Generate_Statement_Body (Res, Alt_Label);
if Alt_Label != Null_Identifier {
//  Set location on the label, for xrefs.
Set_Location (Bod, Alt_Loc);
end if;

Set_Generate_Statement_Body (Clause, Bod);

Set_Generate_Else_Clause (Last, Clause);
end if;

Expect (Token::Generate);
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'generate'
scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
Check_End_Name (Res);
Expect (Token::Semi_Colon);
return Res;
end Parse_If_Generate_Statement;

//  precond : WHEN
//  postcond: ?
//
//  [ LRM08 11.8 ]
//  case_generate_alternative ::=
//     WHEN [ /alternative/_label : ] choices:
//        generate_statement_body
        Iir function Parse_Case_Generate_Alternative (Parent : Iir) 
        is
Loc : Location_Type;
Alt_Label : Name_Id;
Bod : Iir;
Assoc : Iir;
Expr : Iir;
begin
        Loc := Get_Token_Location;

//  Eat 'when'
Expect (Token::When);
scanner.scan();

Alt_Label := Null_Identifier;
if (scanner.currentContext.token = Token::Double_Arrow {
        throw std::runtime_error("PARSE ERROR: missing expression in alternative");
Assoc := Create_Iir (Iir_Choice_By_Expression);
Set_Location (Assoc);
elsif (scanner.currentContext.token = Token::Others {
//  'others' is not an expression!
Assoc := Parse_Choices (nullptr);
else
Expr := Parse_Expression;

if (scanner.currentContext.token = Token::Colon {
if Get_Kind (Expr) = Iir_Simple_Name {
//  In fact the parsed condition was an alternate label.
Alt_Label := Get_Identifier (Expr);
Loc := Get_Location (Expr);
Free_Iir (Expr);
else
throw std::runtime_error("PARSE ERROR: alternative label must be an identifier");
Free_Iir (Expr);
end if;

Expr := nullptr;

//  Skip ':'
scanner.scan();
end if;

Assoc := Parse_Choices (Expr);
end if;

//  Set location of label (if any, for xref) or location of 'when'.
Set_Location (Assoc, Loc);

//  Eat '=>'
Expect (Token::Double_Arrow);
scanner.scan();

Bod := Parse_Generate_Statement_Body (Parent, Alt_Label);
Set_Associated_Block (Assoc, Bod);
if Alt_Label != Null_Identifier {
//  Set location on the label, for xrefs.
Set_Location (Bod, Loc);
end if;

return Assoc;
end Parse_Case_Generate_Alternative;

//  precond : CASE
//  postcond: ';'
//
//  [ LRM08 11.8 ]
//  case_generate_statement ::=
//     /generate/_label :
//     CASE expression GENERATE
//        case_generate_alternative
//      { case_generate_alternative }
//     END GENERATE [ /generate/_label ] ;
Parse_Case_Generate_Statement
        Iir (Label : Name_Id; Loc : Location_Type) 
        is
Res : Iir;
Alt : Iir;
Last_Alt : Iir;
begin
if Label = Null_Identifier {
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");
end if;
Res := Create_Iir (Iir_Case_Generate_Statement);
Set_Location (Res, Loc);
Set_Label (Res, Label);

//  Skip 'case'.
scanner.scan();

Set_Expression (Res, Parse_Expression);

Expect (Token::Generate);

//  Skip 'generate'
scanner.scan();

if (scanner.currentContext.token = Token::End {
        throw std::runtime_error("PARSE ERROR: no generate alternative");
end if;

Last_Alt := nullptr;
while scanner.currentContext.token = Token::When loop
        Alt := Parse_Case_Generate_Alternative (Res);
if Last_Alt = nullptr {
        Set_Case_Statement_Alternative_Chain (Res, Alt);
else
Set_Chain (Last_Alt, Alt);
end if;

//  Skip until last choice of the choices list.
loop
        Last_Alt := Alt;
Alt := Get_Chain (Alt);
exit when Alt = nullptr;
end loop;
end loop;

Expect (Token::Generate);
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'generate'
scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
Check_End_Name (Res);
Expect (Token::Semi_Colon);
return Res;
end Parse_Case_Generate_Statement;

//  precond : first token
//  postcond: END
//
//  [ LRM93 9 ]
//  concurrent_statement ::= block_statement
        --                         | process_statement
        --                         | concurrent_procedure_call_statement
        --                         | concurrent_assertion_statement
        --                         | concurrent_signal_assignment_statement
        --                         | component_instantiation_statement
        --                         | generate_statement
        --
        --  [ LRM93 9.4 ]
//  concurrent_assertion_statement ::=
//      [ label : ] [ POSTPONED ] assertion ;
//
//  [ LRM93 9.3 ]
//  concurrent_procedure_call_statement ::=
//      [ label : ] [ POSTPONED ] procedure_call ;
//
//  [ LRM93 9.5 ]
//  concurrent_signal_assignment_statement ::=
//      [ label : ] [ POSTPONED ] conditional_signal_assignment
//    | [ label : ] [ POSTPONED ] selected_signal_assignment
        Iir function Parse_Concurrent_Assignment (Target : Iir) 
        is
Res : Iir;
begin
switch (scanner.currentContext.token) {
case Token::Less_Equal
| Token::Assign:
// This is a conditional signal assignment.
// Error for ':=' is handled by the subprogram.
return Parse_Concurrent_Conditional_Signal_Assignment (Target);
case Token::Semi_Colon:
// a procedure call or a component instantiation.
// Parse it as a procedure call, may be revert to a
// component instantiation during sem.
Expect (Token::Semi_Colon);
return Parenthesis_Name_To_Procedure_Call
(Target, Iir_Concurrent_Procedure_Call_Statement);
case Token::Generic | Token::Port:
// or a component instantiation.
return Parse_Component_Instantiation (Target);
default:
// or a simple simultaneous statement
if AMS_Vhdl {
Res := Create_Iir (Iir_Simple_Simultaneous_Statement);
Set_Simultaneous_Left (Res, Parse_Simple_Expression (Target));
if (scanner.currentContext.token != Token::Equal_Equal {
throw std::runtime_error("PARSE ERROR: '==' expected after expression");
else
Set_Location (Res);
scanner.scan();
end if;
Set_Simultaneous_Right (Res, Parse_Simple_Expression);
Set_Tolerance (Res, Parse_Tolerance_Aspect_Opt);
Expect (Token::Semi_Colon);
return Res;
else
return Parse_Concurrent_Conditional_Signal_Assignment
(Parse_Simple_Expression (Target));
end if;
}
end Parse_Concurrent_Assignment;

//  Parse end of PSL assert/cover statement.
 Parse_Psl_Assert_Report_Severity (Stmt : Iir) is
        begin
if (scanner.currentContext.token = Token::Report {
//  Skip 'report'
scanner.scan();

Set_Report_Expression (Stmt, Parse_Expression);
end if;

if (scanner.currentContext.token = Token::Severity {
//  Skip 'severity'
scanner.scan();

Set_Severity_Expression (Stmt, Parse_Expression);
end if;

Expect (Token::Semi_Colon);
end Parse_Psl_Assert_Report_Severity;

Parse_Psl_Assert_Statement return Iir
        is
Res : Iir;
begin
        Res := Create_Iir (Iir_Psl_Assert_Statement);

//  Accept PSL tokens
if Flags.Vhdl_Std >= Vhdl_08 {
Scanner.Flag_Psl := true;
end if;

//  Skip 'assert'
scanner.scan();

Set_Psl_Property (Res, Parse_Psl.Parse_Psl_Property);

//  No more PSL tokens after the property.
Scanner.Flag_Psl := false;

Parse_Psl_Assert_Report_Severity (Res);

Scanner.Flag_Scan_In_Comment := false;
return Res;
end Parse_Psl_Assert_Statement;

Parse_Psl_Cover_Statement return Iir
        is
Res : Iir;
begin
        Res := Create_Iir (Iir_Psl_Cover_Statement);

//  Skip 'cover'
scanner.scan();

Set_Psl_Sequence (Res, Parse_Psl.Parse_Psl_Sequence);

//  No more PSL tokens after the property.
Scanner.Flag_Psl := false;

Parse_Psl_Assert_Report_Severity (Res);

Scanner.Flag_Scan_In_Comment := false;
return Res;
end Parse_Psl_Cover_Statement;

 Parse_Concurrent_Statements (Parent : Iir)
is
        Last_Stmt : Iir;
Stmt: Iir;
Label: Name_Id;
Id: Iir;
Postponed : Boolean;
Loc : Location_Type;
Target : Iir;

 Postponed_Not_Allowed is
        begin
if Postponed {
throw std::runtime_error("PARSE ERROR: 'postponed' not allowed here");
Postponed := false;
end if;
end Postponed_Not_Allowed;

 Label_Not_Allowed is
        begin
if Label != Null_Identifier {
throw std::runtime_error("PARSE ERROR: 'postponed' not allowed here");
Label := Null_Identifier;
end if;
end Label_Not_Allowed;
begin
// begin was just parsed.
Last_Stmt := nullptr;
loop
        Stmt := nullptr;
Label := Null_Identifier;
Postponed := false;
Loc := Get_Token_Location;

// Try to find a label.
if (scanner.currentContext.token = Token::Identifier {
        Label := scanner.currentContext.Identifier;

//  Skip identifier
scanner.scan();

if (scanner.currentContext.token = Token::Colon {
//  The identifier is really a label.

//  Skip ':'
scanner.scan();
else
//  This is not a label.  Assume a concurrent assignment.
Target := Create_Iir (Iir_Simple_Name);
Set_Location (Target, Loc);
Set_Identifier (Target, Label);
Label := Null_Identifier;
Target := Parse_Name_Suffix (Target);

Stmt := Parse_Concurrent_Assignment (Target);
goto Has_Stmt;
end if;
end if;

if (scanner.currentContext.token = Token::Postponed {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'postponed' is not allowed in vhdl 87");
else
Postponed := true;
end if;

//  Skip 'postponed'
scanner.scan();
end if;

switch (scanner.currentContext.token) {
case Token::End | Token::Else | Token::Elsif | Token::When:
//  End of list.  'else', 'elseif' and 'when' can be used to
//  separate statements in a generate statement.
Postponed_Not_Allowed;
if Label != Null_Identifier {
throw std::runtime_error("PARSE ERROR: label is not allowed here");
end if;
return;
case Token::Identifier:
Target := Parse_Name (Allow_Indexes: true);
Stmt := Parse_Concurrent_Assignment (Target);
if Get_Kind (Stmt) = Iir_Component_Instantiation_Statement
        && Postponed
        {
throw std::runtime_error("PARSE ERROR: 'postponed' not allowed for " &
"an instantiation statement");
Postponed := false;
end if;
case Token::Left_Paren:
Id := Parse_Aggregate;
if (scanner.currentContext.token = Token::Less_Equal {
// This is a conditional signal assignment.
Stmt := Parse_Concurrent_Conditional_Signal_Assignment (Id);
else
throw std::runtime_error("PARSE ERROR: '<=' expected after aggregate");
Eat_Tokens_Until_Semi_Colon;
end if;
case Token::Process:
Stmt := Parse_Process_Statement (Label, Loc, Postponed);
case Token::Assert:
if Vhdl_Std >= Vhdl_08
or else (Flag_Psl_Comment && Flag_Scan_In_Comment)
{
        Stmt := Parse_Psl_Assert_Statement;
else
Stmt := Create_Iir (Iir_Concurrent_Assertion_Statement);
Parse_Assertion (Stmt);
Expect (Token::Semi_Colon);
end if;
case Token::With:
Stmt := Parse_Selected_Signal_Assignment;
case Token::Block:
Postponed_Not_Allowed;
Stmt := Parse_Block_Statement (Label, Loc);
case Token::For:
Postponed_Not_Allowed;
Stmt := Parse_For_Generate_Statement (Label, Loc);
case Token::If:
Postponed_Not_Allowed;
Stmt := Parse_If_Generate_Statement (Label, Loc);
case Token::Case:
Postponed_Not_Allowed;
Stmt := Parse_Case_Generate_Statement (Label, Loc);
case Token::Eof:
throw std::runtime_error("PARSE ERROR: unexpected end of file, 'END;' expected");
return;
case Token::Component
| Token::Entity
| Token::Configuration:
Postponed_Not_Allowed;
declare
        Unit : Iir;
begin
        Unit := Parse_Instantiated_Unit;
Stmt := Parse_Component_Instantiation (Unit);
end;
case Token::Psl_Default:
Postponed_Not_Allowed;
Label_Not_Allowed;
Stmt := Parse_Psl_Default_Clock;
case Token::Psl_Property
| Token::Psl_Sequence
| Token::Psl_Endpoint:
Postponed_Not_Allowed;
Label_Not_Allowed;
Stmt := Parse_Psl_Declaration;
case Token::Psl_Cover:
Postponed_Not_Allowed;
Stmt := Parse_Psl_Cover_Statement;
default:
//  FIXME: improve message:
//  instead of 'unexpected token 'signal' in conc stmt list'
//  report: 'signal declarations are not allowed in conc stmt'
Unexpected ("concurrent statement list");
Eat_Tokens_Until_Semi_Colon;
}

<< Has_Stmt >> null;

//  Stmt can be null in case of error.
if Stmt != nullptr {
Set_Location (Stmt, Loc);
if Label != Null_Identifier {
Set_Label (Stmt, Label);
end if;
Set_Parent (Stmt, Parent);
if Postponed {
Set_Postponed_Flag (Stmt, true);
end if;
//  Append it to the chain.
if Last_Stmt = nullptr {
        Set_Concurrent_Statement_Chain (Parent, Stmt);
else
Set_Chain (Last_Stmt, Stmt);
end if;
Last_Stmt := Stmt;
end if;

scanner.scan();
end loop;
end Parse_Concurrent_Statements;

//  precond : LIBRARY
//  postcond: ;
//
//  [ LRM93 11.2 ]
//  library_clause ::= LIBRARY logical_name_list
        function Parse_Library_Clause return Iir
        is
First, Last : Iir;
Library: Iir_Library_Clause;
begin
        Sub_Chain_Init (First, Last);
Expect (Token::Library);
loop
        Library := Create_Iir (Iir_Library_Clause);

//  Skip 'library' or ','.
Scan_Expect (Token::Identifier);

Set_Identifier (Library, scanner.currentContext.Identifier);
Set_Location (Library);
Sub_Chain_Append (First, Last, Library);

//  Skip identifier.
scanner.scan();

exit when scanner.currentContext.token = Token::Semi_Colon;
Expect (Token::Comma);

Set_Has_Identifier_List (Library, true);
end loop;

//  Skip ';'.
scanner.scan();
return First;
end Parse_Library_Clause;

//  precond : USE
//  postcond: ;
//
//  [ 10.4 ]
//  use_clause ::= USE selected_name { , selected_name }
//
//  FIXME: should be a list.
Parse_Use_Clause return Iir_Use_Clause
        is
Use_Clause: Iir_Use_Clause;
First, Last : Iir;
begin
        First := nullptr;
Last := nullptr;
scanner.scan();
loop
        Use_Clause := Create_Iir (Iir_Use_Clause);
Set_Location (Use_Clause);
Expect (Token::Identifier);
Set_Selected_Name (Use_Clause, Parse_Name);

//  Chain use clauses.
if First = nullptr {
        First := Use_Clause;
else
Set_Use_Clause_Chain (Last, Use_Clause);
end if;
Last := Use_Clause;

exit when scanner.currentContext.token = Token::Semi_Colon;
Expect (Token::Comma);
scanner.scan();
end loop;
return First;
end Parse_Use_Clause;

//  precond : ARCHITECTURE
//  postcond: ';'
//
//  [ 1.2 ]
//  architecture_body ::=
//      ARCHITECTURE identifier OF ENTITY_name IS
//          architecture_declarative_part
//      BEGIN
//          architecture_statement_part
//      END [ ARCHITECTURE ] [ ARCHITECTURE_simple_name ] ;
 Parse_Architecture_Body (Unit : Iir_Design_Unit)
is
        Res: Iir_Architecture_Body;
begin
        Expect (Token::Architecture);
Res := Create_Iir (Iir_Architecture_Body);

// Get identifier.
Scan_Expect (Token::Identifier);
Set_Identifier (Res, scanner.currentContext.Identifier);
Set_Location (Res);
scanner.scan();
if (scanner.currentContext.token = Token::Is {
        throw std::runtime_error("PARSE ERROR: architecture identifier is missing");
else
Expect (Token::Of);
scanner.scan();
Set_Entity_Name (Res, Parse_Name (false));
Expect (Token::Is);
end if;

scanner.scan();
Parse_Declarative_Part (Res);

Expect (Token::Begin);
scanner.scan();
Parse_Concurrent_Statements (Res);
// end was scanned.
Set_End_Location (Unit);
scanner.scan();
if (scanner.currentContext.token = Token::Architecture {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'architecture' keyword not allowed here by vhdl 87");
end if;
Set_End_Has_Reserved_Id (Res, true);
scanner.scan();
end if;
Check_End_Name (Res);
Expect (Token::Semi_Colon);
Set_Library_Unit (Unit, Res);
end Parse_Architecture_Body;

//  precond : next token
//  postcond: a token
//
//  [ 5.2 ]
//  instantiation_list ::= INSTANTIATION_label { , INSTANTIATION_label }
        --                       | OTHERS
        --                       | ALL
Parse_Instantiation_List return Iir_List
        is
Res : Iir_List;
begin
switch (scanner.currentContext.token) {
case Token::All:
scanner.scan();
return Iir_List_All;
case Token::Others:
scanner.scan();
return Iir_List_Others;
case Token::Identifier:
Res := Create_Iir_List;
loop
        Append_Element (Res, Current_Text);
scanner.scan();
exit when scanner.currentContext.token != Token::Comma;
Expect (Token::Comma);
scanner.scan();
end loop;
return Res;
default:
throw std::runtime_error("PARSE ERROR: instantiation list expected");
return nullptr_List;
}
end Parse_Instantiation_List;

//  precond : next token
//  postcond: next token
//
//  [ 5.2 ]
//  component_specification ::= instantiation_list : COMPONENT_name
        procedure Parse_Component_Specification (Res : Iir)
is
        List : Iir_List;
begin
        List := Parse_Instantiation_List;
Set_Instantiation_List (Res, List);
Expect (Token::Colon);
Scan_Expect (Token::Identifier);
Set_Component_Name (Res, Parse_Name);
end Parse_Component_Specification;

//  precond : next token
//  postcond: next token
//
//  [ 5.2.1.1 ]
//  entity_aspect ::= ENTITY ENTITY_name [ ( ARCHITECTURE_identifier ) ]
//                  | CONFIGURATION CONFIGURATION_name
//                  | OPEN
        function Parse_Entity_Aspect return Iir
        is
Res : Iir;
begin
switch (scanner.currentContext.token) {
case Token::Entity:
Res := Create_Iir (Iir_Entity_Aspect_Entity);
Set_Location (Res);
Scan_Expect (Token::Identifier);
Set_Entity_Name (Res, Parse_Name (false));
if (scanner.currentContext.token = Token::Left_Paren {
        Scan_Expect (Token::Identifier);
Set_Architecture (Res, Current_Text);
Scan_Expect (Token::Right_Paren);
scanner.scan();
end if;
case Token::Configuration:
Res := Create_Iir (Iir_Entity_Aspect_Configuration);
Set_Location (Res);
Scan_Expect (Token::Identifier);
Set_Configuration_Name (Res, Parse_Name (false));
case Token::Open:
Res := Create_Iir (Iir_Entity_Aspect_Open);
Set_Location (Res);
scanner.scan();
default:
//  FIXME: if the token is an identifier, try as if the 'entity'
//  keyword is missing.
Error_Msg_Parse
("'entity', 'configuration' or 'open' keyword expected");
}
return Res;
end Parse_Entity_Aspect;

//  precond : next token
//  postcond: next token
//
//  [ 5.2.1 ]
//  binding_indication ::=
//      [ USE entity_aspect ]
//      [ generic_map_aspect ]
//      [ port_map_aspect ]
Parse_Binding_Indication return Iir_Binding_Indication
        is
Res : Iir_Binding_Indication;
begin
switch (scanner.currentContext.token) {
case Token::Use
| Token::Generic
| Token::Port:
null;
default:
return nullptr;
}
Res := Create_Iir (Iir_Binding_Indication);
Set_Location (Res);
if (scanner.currentContext.token = Token::Use {
        scanner.scan();
Set_Entity_Aspect (Res, Parse_Entity_Aspect);
end if;
if (scanner.currentContext.token = Token::Generic {
        Set_Generic_Map_Aspect_Chain (Res, Parse_Generic_Map_Aspect);
end if;
if (scanner.currentContext.token = Token::Port {
        Set_Port_Map_Aspect_Chain (Res, Parse_Port_Map_Aspect);
end if;
return Res;
end Parse_Binding_Indication;

//  precond : ':' after instantiation_list.
//  postcond: ';'
//
//  [ 1.3.2 ]
//  component_configuration ::=
//      FOR component_specification
//          [ binding_indication ; ]
//          [ block_configuration ]
//      END FOR ;
Parse_Component_Configuration (Loc : Location_Type;
Inst_List : Iir_List)
return Iir_Component_Configuration
        is
Res : Iir_Component_Configuration;
begin
        Res := Create_Iir (Iir_Component_Configuration);
Set_Location (Res, Loc);

//  Component specification.
Set_Instantiation_List (Res, Inst_List);
Expect (Token::Colon);
Scan_Expect (Token::Identifier);
Set_Component_Name (Res, Parse_Name);

switch (scanner.currentContext.token) {
case Token::Use
| Token::Generic
| Token::Port:
Set_Binding_Indication (Res, Parse_Binding_Indication);
Scan_Semi_Colon ("binding indication");
default:
null;
}
if (scanner.currentContext.token = Token::For {
        Set_Block_Configuration (Res, Parse_Block_Configuration);
//  Eat ';'.
scanner.scan();
end if;
Expect (Token::End);
Scan_Expect (Token::For);
Scan_Expect (Token::Semi_Colon);
return Res;
end Parse_Component_Configuration;

//  precond : FOR
//  postcond: ';'
//
//  [ 1.3.1 ]
//  block_configuration ::=
//      FOR block_specification
//          { use_clause }
//          { configuration_item }
//      END FOR ;
//
//  [ 1.3.1 ]
//  block_specification ::=
//      ARCHITECTURE_name
        --    | BLOCK_STATEMENT_label
        --    | GENERATE_STATEMENT_label [ ( index_specification ) ]
Parse_Block_Configuration_Suffix (Loc : Location_Type;
Block_Spec : Iir)
return Iir
        is
Res : Iir_Block_Configuration;
begin
        Res := Create_Iir (Iir_Block_Configuration);
Set_Location (Res, Loc);

Set_Block_Specification (Res, Block_Spec);

//  Parse use clauses.
if (scanner.currentContext.token = Token::Use {
        declare
Last : Iir;
use Declaration_Chain_Handling;
begin
        Build_Init (Last);

while scanner.currentContext.token = Token::Use loop
        Append_Subchain (Last, Res, Parse_Use_Clause);
//  Eat ';'.
scanner.scan();
end loop;
end;
end if;

//  Parse configuration item list
declare
        use Iir_Chains.Configuration_Item_Chain_Handling;
Last : Iir;
begin
        Build_Init (Last);
while scanner.currentContext.token != Token::End loop
Append (Last, Res, Parse_Configuration_Item);
//  Eat ';'.
scanner.scan();
end loop;
end;
Scan_Expect (Token::For);
Scan_Expect (Token::Semi_Colon);
return Res;
end Parse_Block_Configuration_Suffix;

Parse_Block_Configuration return Iir_Block_Configuration
        is
Loc : Location_Type;
begin
        Loc := Get_Token_Location;
Expect (Token::For);

//  Parse label.
scanner.scan();
return Parse_Block_Configuration_Suffix (Loc, Parse_Name);
end Parse_Block_Configuration;

//  precond : FOR
//  postcond: ';'
//
//  [ 1.3.1 ]
//  configuration_item ::= block_configuration
        --                       | component_configuration
Parse_Configuration_Item return Iir
        is
Loc : Location_Type;
List : Iir_List;
El : Iir;
begin
        Loc := Get_Token_Location;
Expect (Token::For);
scanner.scan();

//  ALL and OTHERS are tokens from an instantiation list.
//  Thus, the rule is a component_configuration.
switch (scanner.currentContext.token) {
case Token::All:
scanner.scan();
return Parse_Component_Configuration (Loc, Iir_List_All);
case Token::Others:
scanner.scan();
return Parse_Component_Configuration (Loc, Iir_List_Others);
case Token::Identifier:
El := Current_Text;
scanner.scan();
switch (scanner.currentContext.token) {
case Token::Colon:
//  The identifier was a label from an instantiation list.
List := Create_Iir_List;
Append_Element (List, El);
return Parse_Component_Configuration (Loc, List);
case Token::Comma:
//  The identifier was a label from an instantiation list.
List := Create_Iir_List;
Append_Element (List, El);
loop
        Scan_Expect (Token::Identifier);
Append_Element (List, Current_Text);
scanner.scan();
exit when scanner.currentContext.token != Token::Comma;
end loop;
return Parse_Component_Configuration (Loc, List);
case Token::Left_Paren:
El := Parse_Name_Suffix (El);
return Parse_Block_Configuration_Suffix (Loc, El);
case Token::Use | Token::For | Token::End:
//  Possibilities for a block_configuration.
//  FIXME: should use 'when others' ?
return Parse_Block_Configuration_Suffix (Loc, El);
default:
Error_Msg_Parse
("block_configuration or component_configuration "
& "expected");
raise Parse_Error;
}
default:
throw std::runtime_error("PARSE ERROR: configuration item expected");
raise Parse_Error;
}
end Parse_Configuration_Item;

//  precond : next token
//  postcond: next token
//
//  [ 1.3]
//  configuration_declarative_part ::= { configuration_declarative_item }
//
//  [ 1.3]
//  configuration_declarative_item ::= use_clause
        --                                   | attribute_specification
        --                                   | group_declaration
        --  FIXME: attribute_specification, group_declaration
 Parse_Configuration_Declarative_Part (Parent : Iir)
is
        use Declaration_Chain_Handling;
Last : Iir;
El : Iir;
begin
        Build_Init (Last);
loop
switch (scanner.currentContext.token) {
case Token::Invalid:
raise Internal_Error;
case Token::Use:
Append_Subchain (Last, Parent, Parse_Use_Clause);
case Token::Attribute:
El := Parse_Attribute;
if El != nullptr {
if Get_Kind (El) != Iir_Attribute_Specification {
Error_Msg_Parse
("attribute declaration not allowed here");
end if;
Append (Last, Parent, El);
end if;
case Token::Group:
El := Parse_Group;
if El != nullptr {
if Get_Kind (El) != Iir_Group_Declaration {
Error_Msg_Parse
("group template declaration not allowed here");
end if;
Append (Last, Parent, El);
end if;
default:
exit;
}
scanner.scan();
end loop;
end Parse_Configuration_Declarative_Part;

//  precond : CONFIGURATION
//  postcond: ';'
//
//  [ LRM93 1.3 ]
//  configuration_declaration ::=
//      CONFIGURATION identifier OF ENTITY_name IS
//          configuration_declarative_part
//          block_configuration
//      END [ CONFIGURATION ] [ CONFIGURATION_simple_name ] ;
//
//  [ LRM93 1.3 ]
//  configuration_declarative_part ::= { configuration_declarative_item }
 Parse_Configuration_Declaration (Unit : Iir_Design_Unit)
is
        Res : Iir_Configuration_Declaration;
begin
if (scanner.currentContext.token != Token::Configuration {
raise Program_Error;
end if;
Res := Create_Iir (Iir_Configuration_Declaration);

// Get identifier.
Scan_Expect (Token::Identifier);
Set_Identifier (Res, scanner.currentContext.Identifier);
Set_Location (Res);

//  Skip identifier.
Scan_Expect (Token::Of);

//  Skip 'of'.
scanner.scan();

Set_Entity_Name (Res, Parse_Name (false));

//  Skip 'is'.
Expect (Token::Is);
scanner.scan();

Parse_Configuration_Declarative_Part (Res);

Set_Block_Configuration (Res, Parse_Block_Configuration);

Scan_Expect (Token::End);
Set_End_Location (Unit);

//  Skip 'end'.
scanner.scan();

if (scanner.currentContext.token = Token::Configuration {
if Flags.Vhdl_Std = Vhdl_87 {
        Error_Msg_Parse
        ("'configuration' keyword not allowed here by vhdl 87");
end if;
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'configuration'.
scanner.scan();
end if;

// LRM93 1.3
// If a simple name appears at the end of a configuration declaration, it
// must repeat the identifier of the configuration declaration.
Check_End_Name (Res);
Expect (Token::Semi_Colon);
Set_Library_Unit (Unit, Res);
end Parse_Configuration_Declaration;

//  precond : generic
//  postcond: next token
//
//  LRM08 4.7
//  package_header ::=
//      [ generic_clause               -- LRM08 6.5.6.2
//      [ generic_map aspect ; ] ]
Parse_Package_Header return Iir
        is
Res : Iir;
begin
        Res := Create_Iir (Iir_Package_Header);
Set_Location (Res);
Parse_Generic_Clause (Res);

if (scanner.currentContext.token = Token::Generic {
        Set_Generic_Map_Aspect_Chain (Res, Parse_Generic_Map_Aspect);
Scan_Semi_Colon ("generic map aspect");
end if;
return Res;
end Parse_Package_Header;

//  precond : token (after 'IS')
//  postcond: ';'
//
//  [ LRM93 2.5, LRM08 4.7 ]
//  package_declaration ::=
//      PACKAGE identifier IS
//          package_header           -- LRM08
//          package_declarative_part
//      END [ PACKAGE ] [ PACKAGE_simple_name ] ;
Parse_Package_Declaration
        (Parent : Iir; Id : Name_Id; Loc : Location_Type)
return Iir
        is
Res: Iir_Package_Declaration;
begin
        Res := Create_Iir (Iir_Package_Declaration);
Set_Location (Res, Loc);
Set_Identifier (Res, Id);
Set_Parent (Res, Parent);

if (scanner.currentContext.token = Token::Generic {
if Vhdl_Std < Vhdl_08 {
        throw std::runtime_error("PARSE ERROR: generic packages not allowed before vhdl 2008");
end if;
Set_Package_Header (Res, Parse_Package_Header);
end if;

Parse_Declarative_Part (Res);

Expect (Token::End);
Set_End_Location (Parent);

//  Skip 'end'
scanner.scan();

if (scanner.currentContext.token = Token::Package {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'package' keyword not allowed here by vhdl 87");
end if;
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'package'.
scanner.scan();
end if;

Check_End_Name (Res);
Expect (Token::Semi_Colon);
return Res;
end Parse_Package_Declaration;

//  precond : BODY
//  postcond: ';'
//
//  [ LRM93 2.6, LRM08 4.8 ]
//  package_body ::=
//      PACKAGE BODY PACKAGE_simple_name IS
//          package_body_declarative_part
//      END [ PACKAGE BODY ] [ PACKAGE_simple_name ] ;
Iir Parse_Package_Body (Parent : Iir) 
        is
Res: Iir;
begin
        Res := Create_Iir (Iir_Package_Body);
Set_Location (Res);
Set_Parent (Res, Parent);

// Get identifier.
Expect (Token::Identifier);
Set_Identifier (Res, scanner.currentContext.Identifier);
Scan_Expect (Token::Is);
scanner.scan();

Parse_Declarative_Part (Res);

Expect (Token::End);
Set_End_Location (Parent);

//  Skip 'end'
scanner.scan();

if (scanner.currentContext.token = Token::Package {
if Flags.Vhdl_Std = Vhdl_87 {
        throw std::runtime_error("PARSE ERROR: 'package' keyword not allowed here by vhdl 87");
end if;
Set_End_Has_Reserved_Id (Res, true);

//  Skip 'package'
scanner.scan();

if (scanner.currentContext.token != Token::Body {
throw std::runtime_error("PARSE ERROR: missing 'body' after 'package'");
else
//  Skip 'body'
scanner.scan();
end if;
end if;

Check_End_Name (Res);
Expect (Token::Semi_Colon);
return Res;
end Parse_Package_Body;

//  precond : NEW
//  postcond: ';'
//
//  [ LRM08 4.9 ]
//  package_instantiation_declaration ::=
//      PACKAGE identifier IS NEW uninstantiated_package_name
//         [ generic_map_aspect ] ;
Parse_Package_Instantiation_Declaration
        Iir (Parent : Iir; Id : Name_Id; Loc : Location_Type) 
        is
Res: Iir;
begin
        Res := Create_Iir (Iir_Package_Instantiation_Declaration);
Set_Location (Res, Loc);
Set_Identifier (Res, Id);
Set_Parent (Res, Parent);

//  Skip 'new'
scanner.scan();

Set_Uninstantiated_Package_Name (Res, Parse_Name (false));

if (scanner.currentContext.token = Token::Generic {
        Set_Generic_Map_Aspect_Chain (Res, Parse_Generic_Map_Aspect);
elsif (scanner.currentContext.token = Token::Left_Paren {
        throw std::runtime_error("PARSE ERROR: missing 'generic map'");
Set_Generic_Map_Aspect_Chain
(Res, Parse_Association_List_In_Parenthesis);
end if;

Expect (Token::Semi_Colon);

return Res;
end Parse_Package_Instantiation_Declaration;

//  precond : PACKAGE
//  postcond: ';'
//
//    package_declaration
//  | package_body
//  | package_instantiation_declaration
        Iir function Parse_Package (Parent : Iir) 
        is
Loc : Location_Type;
Id : Name_Id;
Res : Iir;
begin
//  Skip 'package'
scanner.scan();

if (scanner.currentContext.token = Token::Body {
//  Skip 'body'
scanner.scan();

return Parse_Package_Body (Parent);
else
Expect (Token::Identifier);
Id := scanner.currentContext.Identifier;
Loc := Get_Token_Location;

//  Skip identifier.
scanner.scan();

//  Skip 'is'.
Expect (Token::Is);
scanner.scan();

if (scanner.currentContext.token = Token::New {
        Res := Parse_Package_Instantiation_Declaration (Parent, Id, Loc);
//  Note: there is no 'end' in instantiation.
Set_End_Location (Parent);
return Res;
else
return Parse_Package_Declaration (Parent, Id, Loc);
end if;
end if;
end Parse_Package;
*/
//  Precond:  next token
//  Postcond: next token
//
//  LRM93 11.3
//  LRM08 13.4 Context clauses
//  context_clause ::= { context_item }
//
//  context_item ::= library_clause | use_clause | context_reference
void Parser::Parse_Context_Clause(Iir* Unit) {
//Last : Iir;
//Els : Iir;
//        Build_Init (Last);
    bool flag = true;
    while (flag) {
        switch (scanner.currentContext.token) {
        case Token::Library:
            Els = Parse_Library_Clause();
            break;
        case Token::Use:
            Els = Parse_Use_Clause();
            scanner.scan();
            break;
        case Token::Context:
            Parse_Context_Declaration_Or_Reference(Unit, Els);
            if (!Els) {
//  This was a context declaration.  No more clause.

//  LRM08 13.1 Design units
//  It is an error if the context clause preceding a library
//  unit that is a context declaration is not empty.
                if (Get_Context_Items(Unit))
                    throw std::runtime_error("PARSE ERROR:" //(+Get_Context_Items(Unit),
                            "context declaration does not allow context clauses before it");
                return;
            }
            else
//  Skip ';'.
                scanner.scan();
            break;
        case Token::With:
//  Be Ada friendly.
            throw std::runtime_error("PARSE ERROR: 'with' not allowed in context clause  (try 'use' or 'library')");
        default:
            flag = false;
        }
        Append_Subchain(Last, Unit, Els);
    }
}

//  Precond:  IS
//
//  LRM08 13.13 Context declarations
//  context_declaration ::=
//    CONTEXT identifier IS
//       context_clause
//    END [ CONTEXT ] [ /context/_simple_name ] ;
void Parser::Parse_Context_Declaration(Iir* Unit, Iir* Decl) {
    Set_Library_Unit(Unit, Decl);

//  Skip 'is'
    scanner.scan();

    Parse_Context_Clause(Decl);

    Expect(Token::End);
    Set_End_Location(Unit);

//  Skip 'end'
    scanner.scan();

    if (scanner.currentContext.token = Token::Context) {
        Set_End_Has_Reserved_Id(Decl, true);

//  Skip 'context'.
        scanner.scan();
    }

    Check_End_Name(Decl);
    Expect(Token::Semi_Colon);
}


//  Precond:  next token after selected_name.
//  Postcond: ;
//
//  LRM08 13.4 Context clauses
//
//  context_reference ::=
//     CONTEXT selected_name { , selected_name }
Iir* Parser::Parse_Context_Reference(Location_Type Loc, Iir* Name) {
    auto ref = new Iir_Context_Reference;
    //FIXME: Set_Location (Ref, Loc);
    ref->Selected_Name = Name;
    auto first = ref;
    auto last = ref;

    while(scanner.currentContext.token != Token::Semi_Colon) {
        Expect(Token::Comma);
        scanner.scan();
        ref = new Iir_Context_Reference;
        //FIXME: Set_Location (Ref, Loc);
        ref->Selected_Name = Parse_Name();
        last->Context_Reference_Chain = ref;
        last = ref;
    }
    return first;
}

//  Precond:  CONTEXT
//
/*
 Parse_Context_Declaration_Or_Reference
        (Unit : Iir_Design_Unit; Clause : out Iir)
is
        Loc : Location_Type;
Name : Iir;
Res : Iir;
begin
        Loc := Get_Token_Location;

//  Skip 'context'.
scanner.scan();

Name := Parse_Name;

if (scanner.currentContext.token = Token::Is {
        Res := Create_Iir (Iir_Context_Declaration);
if Get_Kind (Name) = Iir_Simple_Name {
        Location_Copy (Res, Name);
Set_Identifier (Res, Get_Identifier (Name));
else
Set_Location (Res, Loc);
throw std::runtime_error("PARSE ERROR:" + +Name, "identifier for context expected");
end if;
Free_Iir (Name);

Parse_Context_Declaration (Unit, Res);
Clause := nullptr;
else
Clause := Parse_Context_Reference (Loc, Name);
end if;
end Parse_Context_Declaration_Or_Reference;
*/

// Parse a design_unit.
// The lexical scanner must have been initialized, but without a
// current_token.
//
//  [ 11.1 ]
//  design_unit ::= context_clause library_unit
inline Iir_Design_Unit* Parser::Parse_Design_Unit(Location_Type loc) {
    scanner.SetNewLocation(loc);
    return Parse_Design_Unit();
}

Iir_Design_Unit* Parser::Parse_Design_Unit() {
    assert(scanner.currentContext.token == Token::Invalid);
    //TODO: take care of eof errors
    scanner.scan();
    auto result = new Iir_Design_Unit();
    currentNode = result;
    result->Date_State = Date_Extern;
    Parse_Context_Clause(result);
    if(result->Library_Unit == nullptr) {
        switch (scanner.currentContext.token) {
        case Token::Entity:
            result = Parse_Entity_Declaration();
            break;
        case Token::Architecture:
            result = Parse_Architecture_Body();
            break;
        case Token::Package:
            result->Library_Unit = Parse_Package(result);
            break;
        case Token::Configuration:
            result = Parse_Configuration_Declaration();
            break;
        default:
            throw std::runtime_error("PARSE ERROR: entity, architecture, package or configuration keyword expected");
        }
    }
    auto unit = result->Library_Unit;

    //FIXME:
    static_cast<Iir_Parent_Abs*>(unit)->Parent = result;
    result->Identifier = unit->Identifier;
    result->Date = 4; //Date_Parsed
    scanner.currentContext.invalidateToken();
    return result;
}

//  [ 11.1 ]
//  design_file ::= design_unit { design_unit }
Iir_Design_File* Parser::Parse_Design_File () {
    auto result = new Iir_Design_File();
    currentNode = result;
    Iir_Design_Unit* lastDesign = nullptr;
    for (Iir_Design_Unit* design = Parse_Design_Unit(); design != nullptr; design = Parse_Design_Unit()) {
        design->Design_File = result;
        if(lastDesign == nullptr)
            result->First_Design_Unit = design;
        else
            lastDesign->Chain = design;
        lastDesign = design;
        result->Last_Design_Unit = lastDesign;
    }
    if (lastDesign == nullptr)
        throw std::runtime_error("PARSE ERROR: design file is empty (no design unit found)");
    return result;
}