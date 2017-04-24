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
throw std::runtime_error("PARSE ERROR: unexpected token %t in a " & Where, +scanner.currentContext.token);
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
            Decl->End_Has_Identifier = true;
            //TODO: Take care of Xrefs
            //Xrefs.Xref_End(Get_Token_Location, Decl);
        }
    }


//  Skip identifier.
    scanner.scan();

}

inline void Parser::Check_End_Name(Iir* Decl) {
    Check_End_Name(Decl->Identifier, Decl);
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
unsigned int Parser::Str_To_Operator_Name (std::string str) {
    //  LRM93 2.1
    //  Extra spaces are not allowed in an operator symbol, and the
    //  case of letters is not significant.

    //  LRM93 2.1
    //  The sequence of characters represented by an operator symbol
    //  must be an operator belonging to one of classes of operators
    //  defined in section 7.2.
    for (int i = 0; i < str.length(); ++i) {
        str[i] = Ada_Chars::toLower(str[i]);
    }

    if (auto it = Keywords::KeywordTable.find(str); it != Keywords::KeywordTable.end())
        return *it;
    else
        throw std::runtime_error("PARSE ERROR:" /*+ Loc +*/ "\"" + str + "\" is not an operator symbol");

}


inline unsigned int Parser::Scan_To_Operator_Name () {
    return Str_To_Operator_Name(scanner.currentContext.Identifier);
}


//  Convert string literal STR to an operator symbol.
//  Emit an error message if the string is not an operator name.
Iir* Parser::String_To_Operator_Symbol (Iir* str) {
    auto Id = Str_To_Operator_Name(static_cast<Iir_String*>(str)->str);
    auto result = new Iir_Operator_Symbol;
    result->Location = str->Location;
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
            if (Get_Kind(Prefix) == Iir_String) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

//  There is a signature. They are normally followed by an
//  attribute.
            result = Parse_Signature();
            result->Signature_Prefix = Prefix;
            break;

        case Token::Tick:
// There is an attribute.
            if (Get_Kind(Prefix) == Iir_String) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            scanner.scan();
            if (scanner.currentContext.token == Token::Left_Paren) {
                // A qualified expression.
                result = new Iir_Qualified_Expression;
                result->Type_Mark = Prefix;
                result->Location = Prefix->Location;
                result->Expression = Parse_Aggregate;
                return result;
            }
            else if (scanner.currentContext.token != Token::Range && scanner.currentContext.token != Token::Identifier
                    && not(scanner.state.options.standard >= Vhdl_Std::Vhdl_08 && scanner.currentContext.token == Token::Subtype)) {
                Expect(Token::Identifier, "attribute identifier expected after '");
                return nullptr;
            }
            result = new Iir_Attribute_Name;
            result->Identifier = scanner.currentContext.Identifier;
            result->Location = scanner.currentContext.getTokenLocation();
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

            if (Get_Kind(Prefix) = Iir_String) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            result = new Iir_Parenthesis_Name;
            result->Location = scanner.currentContext.getTokenLocation();
            Set_Prefix(result, Prefix);
            Set_Association_Chain(result, Parse_Association_List_In_Parenthesis);
            break;

        case Token::Dot:
            if (Get_Kind(Prefix) == Iir_String) {
                Prefix = String_To_Operator_Symbol(Prefix);
            }

            scanner.scan();
            switch (scanner.currentContext.token) {
            case Token::All:
                result = new Iir_Selected_By_All_Name;
                result->Location = scanner.currentContext.getTokenLocation();
                Set_Prefix(result, Prefix);
                break;
            case Token::Identifier:
            case Token::Character:
                result = new Iir_Selected_Name;
                result->Location = scanner.currentContext.getTokenLocation();
                Set_Prefix(result, Prefix);
                Set_Identifier(result, scanner.currentContext.Identifier);
                break;
            case Token::String:
                result = new Iir_Selected_Name;
                result->Location = scanner.currentContext.getTokenLocation();
                Set_Prefix(result, Prefix);
                    result->Identifier =Scan_To_Operator_Name();
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
//  external_pathname ::= package_pathname | absolute_pathname | relative_pathname
//
//  package_pathname ::= @ library_logical_name . package_simple_name . { package_simple_name . } object_simple_name
//
//  absolute_pathname ::=  . partial_pathname
//
//  relative_pathname ::= { ^ . } partial_pathname
//
//  partial_pathname ::= { pathname_element . } object_simple_name
//
//  pathname_element ::= entity_simple_name | component_instantiation_label | block_label
//      | generate_statement_label [ ( static_expression ) ] | package_simple_name
Iir* Parser::Parse_External_Pathname() {
    Iir* result;
    Iir* Last;
    Iir* El;
    switch (scanner.currentContext.token) {
    case Token::Arobase:
        result = new Iir_Package_Pathname;
        result->Location = scanner.currentContext.getTokenLocation();
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
        result->Location = scanner.currentContext.getTokenLocation();
        Last = result;

//  Skip '.'
        scanner.scan();
        break;
    case Token::Caret:
        Last = nullptr;
        do {
            El = new Iir_Relative_Pathname;
            El->Location = scanner.currentContext.getTokenLocation();

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
        El->Location = scanner.currentContext.getTokenLocation();
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
Iir_External_Name* Parser::Parse_External_Name() {

//Loc : Location_Type;
//result : Iir;
//Kind : Iir_Kind;
//begin
    auto Loc = scanner.currentContext.getTokenLocation();

//  Skip '<<'
    scanner.scan();
    auto result = new Iir_External_Name;

    switch (scanner.currentContext.token) {
    case Token::Constant:
        result->Name_Type = External_Name_Type::Constant;
//  Skip 'constant'
        scanner.scan();
        break;
    case Token::Signal:
        result->Name_Type = External_Name_Type::Signal;
//  Skip 'signal'
        scanner.scan();
        break;
    case Token::Variable:
        result->Name_Type = External_Name_Type::Variable;
//  Skip 'variable'
        scanner.scan();
        break;
    default:
        throw std::runtime_error("PARSE ERROR: constant, signal or variable expected after '<<'");
    }

    result->Location = Loc;

    result->External_Pathname = Parse_External_Pathname();

    if (scanner.currentContext.token != Token::Colon)
        throw std::runtime_error("PARSE ERROR: ':' expected after external pathname");
    else
//  Skip ':'
        scanner.scan();

    result->Subtype_Indication = Parse_Subtype_Indication();

    if (scanner.currentContext.token != Token::Double_Greater)
        throw std::runtime_error("PARSE ERROR: '>>' expected at end of external name");
    else
//  Skip '>>'
        scanner.scan();

    return result;
}


//  Precond: next token (identifier, string or '<<')
//  Postcond: next token
//
//  LRM08 8. Names
//  name ::= simple_name
//   | operator_symbol
//   | character_literal    --  FIXME: not handled.
//   | selected_name
//   | indexed_name
//   | slice_name
//   | attribute_name
//   | external_name

Iir *Parser::Parse_Name(bool Allow_Indexes) {
    Iir* Res;
    switch (scanner.currentContext.token) {
        case Token::Identifier:
            auto Res_ = new Iir_Simple_Name;
            Res_->Identifier = scanner.currentContext.Identifier;
            Res_->Location = scanner.currentContext.getTokenLocation();
            Res = Res_;
            //  Skip identifier
            scanner.scan();
            break;
        case Token::String:
            //  For operator symbol, such as: "+" (A, B).
            auto Res__ = new Iir_String;
            Res__->str = scanner.currentContext.Identifier;
            Res__->Location = scanner.currentContext.getTokenLocation();
            Res = Res__;
            //  Skip string
            scanner.scan();
            break;
        case Token::Double_Less:
            if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                throw std::runtime_error("PARSE ERROR: external name not allowed before vhdl 08");
            Res = Parse_External_Name();
            break;
        default:
            throw std::runtime_error("PARSE ERROR: identifier expected here");
    }
    return Parse_Name_Suffix(Res, Allow_Indexes);
}

//  Emit an error message if MARK doesn't have the form of a type mark.
void Parser::Check_Type_Mark(Iir *Mark) {

    switch (Get_Kind(Mark)) {
        case Iir_Simple_Name:
        case Iir_Selected_Name:
        case Iir_Attribute_Name:
//  For O'Subtype.
            break;
        default:
            throw std::runtime_error("PARSE ERROR:" + Mark->Identifier + "type mark must be a name of a type");
    }
}

//  precond : next token
//  postcond: next token
//
//  [ 4.2 ]
//  type_mark ::= type_name | subtype_name

Iir *Parser::Parse_Type_Mark(bool Check_Paren){
    auto Res = Parse_Name(false);
    // FIXME:     Check_Type_Mark(Res);
    if (Check_Paren && scanner.currentContext.token == Token::Left_Paren)
        throw std::runtime_error("PARSE ERROR: index constraint not allowed here");
    //NOTE: SHOULD THIS BE HERE -- Old = Parse_Name_Suffix (Res, true);
    return Res;
}

//  precond : CONSTANT, SIGNAL, VARIABLE. FILE or identifier
//  postcond: next token (';' or ')')
//
//  [ LRM93 4.3.2 ] [ LRM08 6.5.2 ] NOTE: This is interface_object_declaration in LRM08
//  interface_declaration ::= interface_constant_declaration
//      | interface_signal_declaration
//      | interface_variable_declaration
//      | interface_file_declaration
//
//  [ LRM93 3.2.2 ]
//  identifier_list ::= identifier { , identifier }
//
//  [ LRM93 4.3.2 ]
//  interface_constant_declaration ::=
//      [ CONSTANT ] identifier_list : [ IN ] subtype_indication [ = STATIC_expression ]
//
//  [ LRM93 4.3.2 ]
//  interface_file_declaration ::= FILE identifier_list : subtype_indication
//
//  [ LRM93 4.3.2 ]
//  interface_signal_declaration ::=
//      [ SIGNAL ] identifier_list : [ mode ] subtype_indication [ BUS ] [ = STATIC_expression ]
//
//  [ LRM93 4.3.2 ]
//  interface_variable_declaration ::=
//      [ VARIABLE ] identifier_list : [ mode ] subtype_indication [ = STATIC_expression ]
//
//  The default kind of interface declaration is DEFAULT.
std::vector<Iir_Interface_Object_Declaration*> Parser::Parse_Interface_Object_Declaration(Interface_Type Ctxt) {
    Iir_Interface_Object_Declaration::type Kind;


    //  LRM08 6.5.2 Interface object declarations
    //  Interface objects include interface constants that appear as generics of a design entity, a component,
    //  a block, a package or a subprogram, or as constant parameter of subprograms; interface signals that
    //  appear as ports of a design entity, component or block, or as signal parameters of subprograms;
    //  interface variables that appear as variable parameter subprograms; interface files that appear as
    //  file parameters of subprograms.
    switch (scanner.currentContext.token)
    {
    case Token::Identifier:
        //  The class of the object is unknown.  Select default according to the above rule,
        //  assuming the mode is IN.  If the mode is not IN, Parse_Interface_Object_Declaration will
        //  change the class.
        if (Ctxt != Interface_Type::Port)
            Kind = Iir_Interface_Object_Declaration::type::signal;
        else
            Kind = Iir_Interface_Object_Declaration::type::constant;
        break;
    case Token::Constant:
        Kind = Iir_Interface_Object_Declaration::type::constant;
        break;
    case Token::Signal:
        if (Ctxt == Interface_Type::Generic)
            throw std::runtime_error("PARSE ERROR: signal interface not allowed in generic clause");

        Kind = Iir_Interface_Object_Declaration::type::signal;
        break;
    case Token::Variable:
        if (Ctxt == Interface_Type::Function_Parameter || Ctxt == Interface_Type::Procedure_Parameter)
            throw std::runtime_error("PARSE ERROR: variable interface not allowed in generic or port clause");

        Kind = Iir_Interface_Object_Declaration::type::variable;
        break;
    case Token::File:
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: file interface not allowed in vhdl 87");

        if (Ctxt == Interface_Type::Function_Parameter || Ctxt == Interface_Type::Procedure_Parameter)
            throw std::runtime_error("PARSE ERROR: variable interface not allowed in generic or port clause");

        Kind = Iir_Interface_Object_Declaration::type::file;
        break;
    default:
        //  Fall back in case of parse error.
        throw std::logic_error("PARSE ERROR: Check this");
        //  Kind = Iir_Interface_Variable_Declaration;
    }

    bool Has_Class = false;
    bool Is_Default = false;

    if (scanner.currentContext.token == Token::Identifier) {
        Is_Default = true;
        Has_Class = false;
    } else {
        Is_Default = false;
        Has_Class = true;
        //  Skip 'signal', 'variable', 'constant' or 'file'.
        scanner.scan();
    }

    std::vector<Iir_Interface_Object_Declaration*> IdentifierList;
    Iir_Interface_Object_Declaration* element;
    while(1) {
        element = new Iir_Interface_Object_Declaration;

        if (scanner.currentContext.token != Token::Identifier)
            Expect(Token::Identifier);
        element->Identifier =  scanner.currentContext.Identifier;
        element->Location = scanner.currentContext.getTokenLocation();
        element->Interface_Object_Type = Kind;

        IdentifierList.push_back(element);

        //  Skip identifier
        scanner.scan();

        if (scanner.currentContext.token == Token::Colon)
            break;
        Expect(Token::Comma, "',' or ':' expected after identifier");

        //  Skip ','
        scanner.scan();

    }

    Expect (Token::Colon, "':' must follow the interface element identifier");

    //  Skip ':'
    scanner.scan();

    Iir_Mode Interface_Mode;
    bool Has_Mode;
    //  Parse mode.
    switch (scanner.currentContext.token) {
        case Token::In:
        case Token::Out:
        case Token::Inout:
        case Token::Linkage:
        case Token::Buffer:
            Interface_Mode = Parse_Mode();
            break;
        default:
            Interface_Mode = Iir_Unknown_Mode;
    }

    //  LRM93 2.1.1  LRM08 4.2.2.1
    //  If the mode is INOUT or OUT, and no object class is explicitly specified, variable is assumed.
    if (Is_Default && (Ctxt == Interface_Type::Function_Parameter || Ctxt == Interface_Type::Procedure_Parameter)
        && (Interface_Mode == Iir_Out_Mode || Interface_Mode == Iir_Inout_Mode)) {
        //  Convert into variable.
        for (auto &&item : IdentifierList) {
            item->Interface_Object_Type = Iir_Interface_Object_Declaration::type::variable;
        }
        Kind = Iir_Interface_Object_Declaration::type::variable;
    }

//  Parse mode (and handle default mode).
    switch (Kind) {
        case Iir_Interface_Object_Declaration::type::file:
            if (Interface_Mode != Iir_Unknown_Mode)
                throw std::runtime_error("PARSE ERROR: mode can't be specified for a file interface");

            Interface_Mode = Iir_Inout_Mode;
            break;
        case Iir_Interface_Object_Declaration::type::variable:
        case Iir_Interface_Object_Declaration::type::signal:
            //  LRM93 4.3.2
            //  If no mode is explicitly given in an interface declaration other than an interface file declaration,
            //  mode IN is assumed.
            if (Interface_Mode == Iir_Unknown_Mode)
                Interface_Mode = Iir_In_Mode;
            break;
        case Iir_Interface_Object_Declaration::type::constant:

            if (Interface_Mode == Iir_Unknown_Mode)
                Interface_Mode = Iir_In_Mode;
            else if (Interface_Mode != Iir_In_Mode)
                throw std::runtime_error("PARSE ERROR: mode must be 'in' for a constant");
    }
    IdentifierList.front()->Subtype_Indication = Parse_Subtype_Indication();

//  Signal kind (but only for signal).
    bool Is_Guarded;
    Iir_Signal_Kind Signal_Kind;
    if (Kind == Iir_Interface_Object_Declaration::type::signal)
        [Is_Guarded, Signal_Kind] = Parse_Signal_Kind();
    else {
        Is_Guarded = false;
        Signal_Kind = Iir_Register_Kind;
    }

    //  Subtype_Indication and Default_Value are set only on the first
    //  interface.
    if (scanner.currentContext.token == Token::Assign) {
        if (Kind == Iir_Interface_Object_Declaration::type::file)
            throw std::runtime_error("PARSE ERROR: default expression not allowed for an interface file");

        //  Skip ':='
        scanner.scan();

        IdentifierList.front()->Default_Value = Parse_Expression();
    } else
        IdentifierList.front()->Default_Value = nullptr;


    for (auto &&item : IdentifierList) {
        if (Interface_Mode != Iir_Mode::Iir_Unknown_Mode)
            item->Mode = Interface_Mode;
        else
            item->Mode = {};
        item->Is_Ref = item != IdentifierList.front();
        item->Has_Class = Has_Class;
        item->Has_Identifier_List = item != IdentifierList.back();
        if (Kind == Iir_Interface_Object_Declaration::type::signal) {
            item->extras = new Iir_Interface_Signal_Declaration_Extras;
            item->extras->Guarded_Signal_Flag = Is_Guarded;
            item->extras->Signal_Kind = Signal_Kind;
        }
    }
    return IdentifierList;
}

//  Precond : 'package'
//  Postcond: next token
//
//  LRM08 6.5.5 Interface package declarations
//  interface_package_declaration ::=
//    PACKAGE identifier IS NEW uninstantiated_package name
//      interface_package_generic_map_aspect
//
//  interface_package_generic_map_aspect ::= generic_map_aspect | GENERIC MAP ( <> ) | GENERIC MAP ( DEFAULT )
//
Iir* Parser::Parse_Interface_Package_Declaration() {
//        is
//Inter : Iir;
//Map : Iir;
//begin
    auto Inter = new Iir_Interface_Package_Declaration;

//  Skip 'package'
    Scan_Expect(Token::Identifier,
                "an identifier is expected after ""package""");
    Inter->Identifier = scanner.currentContext.Identifier;
    Inter->Location = scanner.currentContext.getTokenLocation();

//  Skip identifier
    Scan_Expect(Token::Is);

//  Skip 'is'
    Scan_Expect(Token::New);

//  Skip 'new'
    scanner.scan();

    Inter->Uninstantiated_Package_Name = Parse_Name(false);

    Expect(Token::Generic);

//  Skip 'generic'
    Scan_Expect(Token::Map);

//  Skip 'map'
    Scan_Expect(Token::Left_Paren);

//  Skip '('
    scanner.scan();
    Iir* Map;
    switch (scanner.currentContext.token) {
        case Token::Box:
            Map = nullptr;
//  Skip '<>'
            scanner.scan();
        default:
            Map = Parse_Association_List();
    }
    Inter->Generic_Map_Aspect_Chain = Map;

    Expect(Token::Right_Paren);

//  Skip ')'
    scanner.scan();

    return Inter;
}

//  Precond:  identifier or string
//  Postcond: next token
//
//  [ 2.1 ]
//  designator ::= identifier | operator_symbol
Iir* Parser::Parse_Subprogram_Designator(Iir* Subprg) {
    if (scanner.currentContext.token == Token::Identifier) {
        Subprg->Identifier = scanner.currentContext.Identifier;
        Subprg->Location = scanner.currentContext.getTokenLocation();
    } else if (scanner.currentContext.token == Token::String) {
        if (Kind_In(Subprg, Iir_Procedure_Declaration, Iir_Interface_Procedure_Declaration)
//  LRM93 2.1
//  A procedure designator is always an identifier.
            throw std::runtime_error("PARSE ERROR: a procedure name must be an identifier");

//  LRM93 2.1
//  A function designator is either an identifier or an operator
//  symbol.
        Subprg->Identifier = Scan_To_Operator_Name(Get_Token_Location());
        Subprg->Location = scanner.currentContext.getTokenLocation();
    } else
//  Just to display a parse error.
        Expect(Token::Identifier);

//  Eat designator (identifier or string).
    scanner.scan();
}
/*
// NOTE: This is obsolete (maybe), so maybe later :P
//  Precond: '(' or return or any
//  Postcond: next token
        procedure Parse_Subprogram_Parameters_And_Return
(Subprg : Iir; Is_Func : Boolean)
is
        Old : Iir;
pragma Unreferenced (Old);
Inters : Iir;
begin
if (scanner.currentContext.token == Token::Parameter {
        Set_Has_Parameter (Subprg, true);

//  Eat 'parameter'
scanner.scan();

if (scanner.currentContext.token != Token::Left_Paren {
throw std::runtime_error("PARSE ERROR: 'parameter' must be followed by a list of parameters");
end if;
end if;

if (scanner.currentContext.token == Token::Left_Paren {
//  Parse the interface declaration.
if Is_Func {
Inters = Parse_Interface_List
        (Function_Parameter_Interface_List, Subprg);
else
Inters = Parse_Interface_List
        (Procedure_Parameter_Interface_List, Subprg);
end if;
Set_Interface_Declaration_Chain (Subprg, Inters);
end if;

if (scanner.currentContext.token == Token::Return {
if not Is_Func {
throw std::runtime_error("PARSE ERROR: 'return' not allowed for a procedure", Cont: true);
throw std::runtime_error("PARSE ERROR: (remove return part or declare a function)");

//  Skip 'return'
scanner.scan();

Old = Parse_Type_Mark;
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
//        [ IS interface_subprogram_default ]
//
//  interface_subrpogram_specification ::=
//     interface_procedure_specification | interface_function_specification
//
//  interface_procedure_specification ::=
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
Kind = Iir_Interface_Procedure_Declaration;
case Token::Function
| Token::Pure
| Token::Impure:
Kind = Iir_Interface_Function_Declaration;
default:
throw std::logic_error();
}
Subprg = Create_Iir (Kind);
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
Set_Pure_Flag (Subprg, scanner.currentContext.token == Token::Pure);
Set_Has_Pure (Subprg, true);
//  FIXME: what to do in case of error ??

//  Eat 'pure' or 'impure'.
scanner.scan();

Expect (Token::Function, "'function' must follow 'pure' or 'impure'");
default:
throw std::logic_error();
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
*/
//  Precond : '('
//  Postcond: next token
//
//  LRM08 6.5.6 Interface lists
//  interface_list ::= interface_element { ';' interface_element }
//
//  interface_element ::= interface_declaration
Iir* Parser::Parse_Interface_List (Interface_Type Ctxt, Iir* Parent) {
//Res, Last : Iir;
//Inters : Iir;
//Next : Iir;
//Prev_Loc : Location_Type;

    Expect(Token::Left_Paren);

    Iir* Res = nullptr;
    Iir* Last = nullptr;
    Iir* Inters;
    while (1) {
        auto Prev_Loc = scanner.currentContext.getTokenLocation();

//  Skip '(' or ';'
        scanner.scan();

        switch (scanner.currentContext.token) {
            case Token::Identifier:
            case Token::Signal:
            case Token::Variable:
            case Token::Constant:
            case Token::File:
//  An inteface object.
                Inters = Parse_Interface_Object_Declaration(Ctxt);
                break;
            case Token::Package:
                if (Ctxt != Interface_Type::Generic)
                    throw std::runtime_error("PARSE ERROR: package interface only allowed in generic interface");
                else if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                    throw std::runtime_error("PARSE ERROR: package interface not allowed before vhdl 08");

                Inters = Parse_Interface_Package_Declaration();
                break;
            case Token::Type:
                if (Ctxt != Interface_Type::Generic)
                    throw std::runtime_error("PARSE ERROR: type interface only allowed in generic interface");
                else if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                    throw std::runtime_error("PARSE ERROR: type interface not allowed before vhdl 08");

                Inters = new Iir_Interface_Type_Declaration;
                Scan_Expect(Token::Identifier, "an identifier is expected after 'type'");
                Inters->Identifier = scanner.currentContext.Identifier;
                Inters->Location = scanner.currentContext.getTokenLocation();
                break;
//  Skip identifier
                scanner.scan();
            case Token::Procedure:
            case Token::Pure:
            case Token::Impure:
            case Token::Function:
                if (Ctxt != Interface_Type::Generic)
                    throw std::runtime_error("PARSE ERROR: subprogram interface only allowed in generic interface");
                else if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                    throw std::runtime_error("PARSE ERROR: subprogram interface not allowed before vhdl 08");

                Inters = Parse_Interface_Subprogram_Declaration();
                break;
            case Token::Right_Paren:
                if (!Res)
                    throw std::runtime_error("PARSE ERROR: empty interface list not allowed");
                else
                    throw std::runtime_error("PARSE ERROR: extra ';' at end of interface list");
                exit;
            default:
                throw std::runtime_error("PARSE ERROR: interface declaration expected");
        }

//  Chain
        if (!Last)
            Res = Inters;
        else
            Set_Chain(Last, Inters);


//  Set parent and set Last to the last interface.
        Last = Inters;
        while (1) {
            Set_Parent(Last, Parent);
            Next = Get_Chain(Last);
            if (!Next)
                break;
            Last = Next;
        }

        switch (scanner.currentContext.token) {
            case Token::Comma:
                throw std::runtime_error("PARSE ERROR: interfaces must be separated by ';' (found ',')");
            case Token::Semi_Colon:
                break;
            default:
                exit;
        }
    }

    if (scanner.currentContext.token != Token::Right_Paren)
        throw std::runtime_error("PARSE ERROR: ')' expected at end of interface list");


//  Skip ')'
    scanner.scan();

    return Res;
}

//  precond : PORT
//  postcond: next token
//
//  [ 1.1.1 ]
//  port_clause ::= PORT ( port_list ) ;
//
//  [ 1.1.1.2 ]
//  port_list ::= PORT_interface_list
void Parser::Parse_Port_Clause (Iir* Parent) {
//  Skip 'port'
    assert(scanner.currentContext.token == Token::Port);
    scanner.scan();

    auto Res = Parse_Interface_List(Interface_Type::Port, Parent);

//  Check the interface are signal interfaces.
    auto El = Res;
    while (El) {
        if (Get_Kind(El) != Iir_Interface_Signal_Declaration)
            throw std::runtime_error("PARSE ERROR:" + El->Identifier + "port must be a signal");

        El = Get_Chain(El);
    }

    Scan_Semi_Colon("port clause");
    Set_Port_Chain(Parent, Res);
}

//  precond : GENERIC
//  postcond: next token
//
//  [ LRM93 1.1.1, LRM08 6.5.6.2 ]
//  generic_clause ::= GENERIC ( generic_list ) ;
//
//  [ LRM93 1.1.1.1, LRM08 6.5.6.2]
//  generic_list ::= GENERIC _interface_list
void Parser::Parse_Generic_Clause (Iir* Parent) {
//  Skip 'generic'
    assert (scanner.currentContext.token == Token::Generic);
    scanner.scan();

    auto Res = Parse_Interface_List(Interface_Type::Generic, Parent);
    Parent->Generic_Chain = Res;

    Scan_Semi_Colon("generic clause");
}

//  precond : a token.
//  postcond: next token
//
//  [ 1.1.1 ]
//  entity_header ::= [ FORMAL_generic_clause ] [ FORMAL_port_clause ]
//
//  [ 4.5 ]
//          [ LOCAL_generic_clause ]
//          [ LOCAL_port_clause ]
void Parser::Parse_Generic_Port_Clauses (Iir* Parent) {
    bool Has_Port = false;
    bool Has_Generic = false;
    while (1) {
        if (scanner.currentContext.token == Token::Generic) {
            if (Has_Generic)
                throw std::runtime_error("PARSE ERROR: at most one generic clause is allowed");

            if (Has_Port)
                throw std::runtime_error("PARSE ERROR: generic clause must precede port clause");

            Has_Generic = true;
            Parse_Generic_Clause(Parent);
        } else if (scanner.currentContext.token == Token::Port) {
            if (Has_Port)
                throw std::runtime_error("PARSE ERROR: at most one port clause is allowed");

            Has_Port = true;
            Parse_Port_Clause(Parent);
        } else
            break;
    }
}

//  precond : a token
//  postcond: next token
//
//  [ LRM93 3.1.1 ]
//  enumeration_type_definition ::=
//      ( enumeration_literal { , enumeration_literal } )
//
//  [ LRM93 3.1.1 ]
//  enumeration_literal ::= identifier | character_literal
Iir_Enumeration_Type_Definition* Parser::Parse_Enumeration_Type_Definition(Iir* Parent) {
//Pos: Iir_Int32;
//Enum_Lit: Iir_Enumeration_Literal;
//Enum_Type: Iir_Enumeration_Type_Definition;
//Enum_List : Iir_List;

//  This is an enumeration.
    auto Enum_Type = new Iir_Enumeration_Type_Definition;
    Enum_Type->Location = scanner.currentContext.;

//  LRM93 3.1.1
//  The position number of the first listed enumeration literal is zero.

//  Eat '('.
    scanner.scan();

    if (scanner.currentContext.token == Token::Right_Paren)
        throw std::runtime_error("PARSE ERROR: at least one literal must be declared");

    while (1) {
        if (scanner.currentContext.token != Token::Identifier && scanner.currentContext.token != Token::Character) {
            if (scanner.currentContext.token == Token::Eof)
                throw std::runtime_error("PARSE ERROR: unexpected end of file");
            throw std::runtime_error("PARSE ERROR: identifier or character expected");
        }

        auto Enum_Lit = new Iir_Enumeration_Literal;
        Enum_Lit->Identifier = scanner.currentContext.Identifier;
        Enum_Lit->Parent = Parent;
        Enum_Lit->Location;
        Enum_Type->Enumeration_Literal_List.push_back(Enum_Lit);

// LRM93 3.1.1
// the position number for each additional enumeration literal is
// one more than that if its predecessor in the list.

//  Skip identifier or character.
        scanner.scan();

        if (scanner.currentContext.token == Token::Right_Paren)
            break;
        if (scanner.currentContext.token != Token::Comma)
            throw std::runtime_error("PARSE ERROR: ')' or ',' is expected after an enum literal");


//  Skip ','.
        scanner.scan();

        if (scanner.currentContext.token == Token::Right_Paren)
            throw std::runtime_error("PARSE ERROR: extra ',' ignored");
    }

//  Skip ')'.
    scanner.scan();

    return Enum_Type;
}

//  precond : ARRAY
//  postcond: ??
//
//  [ LRM93 3.2.1 ]
//  array_type_definition ::= unconstrained_array_definition | constrained_array_definition
//
//   unconstrained_array_definition ::=
//      ARRAY ( index_subtype_definition { , index_subtype_definition } ) OF element_subtype_indication
//
//   constrained_array_definition ::= ARRAY index_constraint OF element_subtype_indication
//
//   index_subtype_definition ::= type_mark RANGE <>
//
//   index_constraint ::= ( discrete_range { , discrete_range } )
//
//   discrete_range ::= discrete_subtype_indication | range
//
//  [ LRM08 5.3.2.1 ]
//  array_type_definition ::= unbounded_array_definition | constrained_array_definition
//
//   unbounded_array_definition ::=
//      ARRAY ( index_subtype_definition { , index_subtype_definition } ) OF element_subtype_indication
Iir* Parser::Parse_Array_Definition() {
//        is
bool Index_Constrained;
bool Array_Constrained;
//First : Boolean;
//Res_Type: Iir;
//Index_List : Iir_List;
//
//Loc : Location_Type;
//Def : Iir;
//Type_Mark : Iir;
//Element_Subtype : Iir;
//begin
    auto Loc = scanner.currentContext.getTokenLocation();

//  Skip 'array', scan '('
    Scan_Expect(Token::Left_Paren);
    scanner.scan();

    bool First = true;
    Index_List = Create_Iir_List;

    while (1) {
//  The accepted syntax can be one of:
//  * index_subtype_definition, which is:
//    * type_mark RANGE <>
//  * discrete_range, which is either:
//    * discrete_subtype_indication
//      * [ resolution_indication ] type_mark [ range_constraint ]
//        * range_constraint ::= RANGE range
//    * range
//      * range_attribute_name
//      * simple_expression direction simple_expression

//  Parse a simple expression (for the range), which can also parse a
//  name.
        auto Type_Mark = Parse_Simple_Expression();

        switch (scanner.currentContext.token) {
            case Token::Range:
//  Skip 'range'
                scanner.scan();

                if (scanner.currentContext.token == Token::Box) {
//  Parsed 'RANGE <>': this is an index_subtype_definition.
                    Index_Constrained = false;
                    scanner.scan();
                    Def = Type_Mark;
                } else {
//  This is a /discrete/_subtype_indication
                    Index_Constrained = true;
                    Def = Parse_Range_Constraint_Of_Subtype_Indication(Type_Mark);
                }
                break;
            case Token::To:
            case Token::Downto:
//  A range
                Index_Constrained = true;
                Def = Parse_Range_Expression(Type_Mark);
                break;
            default:
//  For a /range/_attribute_name
                Index_Constrained = true;
                Def = Type_Mark;
        }

        Append_Element(Index_List, Def);

        if (First) {
            Array_Constrained = Index_Constrained;
            First = false;
        } else if (Array_Constrained != Index_Constrained)
            throw std::runtime_error("PARSE ERROR: cannot mix constrained and unconstrained index");

        if (scanner.currentContext.token != Token::Comma)
            break;
        scanner.scan();
    }

//  Skip ')' and 'of'
    Expect(Token::Right_Paren);
    Scan_Expect(Token::Of);
    scanner.scan();

    auto Element_Subtype = Parse_Subtype_Indication();

    if (Array_Constrained) {
//  Sem_Type will create the array type.
        Res_Type = new Iir_Array_Subtype_Definition;
        Res_Type->Array_Element_Constraint = Element_Subtype;
        Res_Type->Index_Constraint_List = Index_List;
    } else {
        Res_Type = new Iir_Array_Type_Definition;
        Res_Type->Element_Subtype_Indication = Element_Subtype;
        Res_Type->Index_Subtype_Definition_List = Index_List;
    }
    Res_Type->Location = Loc;

    return Res_Type;
}

//  precond : UNITS
//  postcond: next token
//
//  [ LRM93 3.1.3 ]
//  physical_type_definition ::=
//      range_constraint
//          UNITS
//              base_unit_declaration
//              { secondary_unit_declaration }
//          END UNITS [ PHYSICAL_TYPE_simple_name ]
//
//  [ LRM93 3.1.3 ]
//  base_unit_declaration ::= identifier ;
//
//  [ LRM93 3.1.3 ]
//  secondary_unit_declaration ::= identifier = physical_literal ;
Iir_Physical_Type_Definition* Parser::Parse_Physical_Type_Definition(Iir* Parent) {
    auto Res = new Iir_Physical_Type_Definition;
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip 'units'
    Expect(Token::Units);
    scanner.scan();

//  Parse primary unit.
    Expect(Token::Identifier);
    auto Unit = new Iir_Unit_Declaration;
    Unit->Location = scanner.currentContext.getTokenLocation();
    Unit->Parent = Parent;
    Unit->Identifier = scanner.currentContext.Identifier;

//  Skip identifier
    scanner.scan();

    Scan_Semi_Colon("primary unit");

    Res->Unit_Chain.push_back(Unit);

//  Parse secondary units.
    while (scanner.currentContext.token != Token::End) {
        Unit = new Iir_Unit_Declaration;
        Unit->Location = scanner.currentContext.getTokenLocation();
        Unit->Parent = Parent;
        Unit->Identifier = scanner.currentContext.Identifier;

//  Skip identifier.
        Scan_Expect(Token::Equal);

//  Skip '='.
        scanner.scan();

        auto Multiplier = Parse_Primary();
        Unit->Physical_Literal = Multiplier;
        switch (Get_Kind(Multiplier)) {
            case Iir_Simple_Name:
            case Iir_Selected_Name:
            case Iir_Physical_Int_Literal:
                break;
            case Iir_Physical_Fp_Literal:
                throw std::runtime_error("PARSE ERROR: secondary units may only be defined with integer literals");
            default:
                throw std::runtime_error("PARSE ERROR: a physical literal is expected here");
        }

        Res->Unit_Chain.push_back(Unit);
        Scan_Semi_Colon("secondary unit");
    }

//  Skip 'end'.
    scanner.scan();

    Expect(Token::Units);
    Res->End_Has_Reserved_Id = true;

//  Skip 'units'.
    scanner.scan();

    return Res;
}

//  precond : RECORD
//  postcond: next token
//
//  [ LRM93 3.2.2 ]
//  record_type_definition ::=
//      RECORD
//          element_declaration
//          { element_declaration }
//      END RECORD [ RECORD_TYPE_simple_name ]
//
//  element_declaration ::= identifier_list : element_subtype_definition
//
//  element_subtype_definition ::= subtype_indication
Iir_Record_Type_Definition* Parser::Parse_Record_Type_Definition() {
//        is
//Res: Iir_Record_Type_Definition;
//El_List : Iir_List;
//El: Iir_Element_Declaration;
//First : Iir;
//Pos: Iir_Index32;
//Subtype_Indication: Iir;
//begin
    auto Res = new Iir_Record_Type_Definition;
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip 'record'
    scanner.scan();
    while (1) {
        std::vector<Iir_Element_Declaration*> tempList;

        while (1) {
//  Parse identifier_list
            auto El = new Iir_Element_Declaration;
            El->Location = scanner.currentContext.getTokenLocation();
            El->Parent = Res;

            Expect(Token::Identifier);
            El->Identifier = scanner.currentContext.Identifier;
            tempList.push_back(El);

//  Skip identifier
            scanner.scan();

            if (scanner.currentContext.token != Token::Comma)
                break;

            El->Has_Identifier_List = true;

//  Skip ','
            scanner.scan();
        }

//  Scan ':'.
        Expect(Token::Colon);
        scanner.scan();

//  Parse element subtype indication.
        auto subtype = Parse_Subtype_Indication();
        for (auto &&item : tempList) {
            item->Subtype_Indication = subtype;
            Res->Elements_Declaration_List.push_back(item);
        }

        Scan_Semi_Colon("element declaration");
        if (scanner.currentContext.token == Token::End)
            break;
    }

//  Skip 'end'
    Scan_Expect(Token::Record);
    Res->End_Has_Reserved_Id = true;

//  Skip 'record'
    scanner.scan();

    return Res;
}

//  precond : ACCESS
//  postcond: ?
//
//  [ LRM93 3.3]
//  access_type_definition ::= ACCESS subtype_indication.
Iir_Access_Type_Definition* Parser::Parse_Access_Type_Definition() {
    auto Res = new Iir_Access_Type_Definition;
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip 'access'
    Expect(Token::Access);
    scanner.scan();

    Res->Designated_Subtype_Indication = Parse_Subtype_Indication();

    return Res;
}

//  precond : FILE
//  postcond: next token
//
//  [ LRM93 3.4 ]
//  file_type_definition ::= FILE OF type_mark
Iir_File_Type_Definition* Parser::Parse_File_Type_Definition() {
    auto Res = new Iir_File_Type_Definition;
    Res->Location = scanner.currentContext.getTokenLocation();
// Accept token 'file'.
    Scan_Expect(Token::Of);
    scanner.scan();
    auto Type_Mark = Parse_Type_Mark(true);
    if (Get_Kind(Type_Mark) not in Iir_Kinds_Denoting_Name)
        throw std::runtime_error("PARSE ERROR: type mark expected");
    else
        Res->File_Type_Mark = Type_Mark;

    return Res;
}

//  precond : PROTECTED
//  postcond: ';'
//
//  [ 3.5 ]
//  protected_type_definition ::= protected_type_declaration | protected_type_body
//
//  [ 3.5.1 ]
//  protected_type_declaration ::= PROTECTED
//                                     protected_type_declarative_part
//                                 END PROTECTED [ simple_name ]
//
//  protected_type_declarative_part ::= { protected_type_declarative_item }
//
//  protected_type_declarative_item ::= subprogram_declaration | attribute_specification | use_clause
//
//  [ 3.5.2 ]
//  protected_type_body ::= PROTECTED BODY
//                              protected_type_body_declarative_part
//                          END PROTECTED BODY [ simple_name ]
//
//  protected_type_body_declarative_part ::= { protected_type_body_declarative_item }
//
Iir* Parser::Parse_Protected_Type_Definition(std::string Ident, Location_Type Loc) {
//        is
//Res : Iir;
//Decl : Iir;
//begin
    Iir* Decl;
    Iir* Res;
    scanner.scan();
    if (scanner.currentContext.token == Token::Body) {
        Res = new Iir_Protected_Type_Body;
        scanner.scan();
        Decl = Res;
    } else {
        Decl = new Iir_Type_Declaration;
        Res = new Iir_Protected_Type_Declaration;
        Res->Location = Loc;
        Decl->Type_Definition = Res;
    }
    Decl->Identifier = Ident;
    Decl->Location = Loc;

    Parse_Declarative_Part(Res);

    Expect(Token::End);
//  Eat 'end'.
    scanner.scan();
    if (scanner.state.options.standard >= Vhdl_Std::Vhdl_00)
        Expect(Token::Protected);
    else
//  Avoid weird message: 'protected' expected instead of 'protected'.
        Expect(Token::Identifier);

    Res->End_Has_Reserved_Id = true;
    if (Get_Kind(Res) = Iir_Protected_Type_Body)
        Scan_Expect(Token::Body);

    scanner.scan();
    Check_End_Name(Ident, Res);
    return Decl;
}

//  precond : TYPE
//  postcond: a token
//
//  [ LRM93 4.1 ]
//  type_definition ::= scalar_type_definition
//                    | composite_type_definition
//                    | access_type_definition
//                    | file_type_definition
//                    | protected_type_definition
//
//  [ LRM93 3.1 ]
//  scalar_type_definition ::= enumeration_type_definition
//                           | integer_type_definition
//                           | floating_type_definition
//                           | physical_type_definition
//
//  [ LRM93 3.2 ]
//  composite_type_definition ::= array_type_definition | record_type_definition
//
//  [ LRM93 3.1.2 ]
//  integer_type_definition ::= range_constraint
//
//  [ LRM93 3.1.4 ]
//  floating_type_definition ::= range_constraint
//
Iir* Parser::Parse_Type_Declaration (Iir* Parent) {
//Def : Iir;
    Iir* Decl;

// The current token must be type.
    assert (scanner.currentContext.token == Token::Type);

// Get the identifier
    Scan_Expect(Token::Identifier, "an identifier is expected after 'type' keyword");
    auto Loc = scanner.currentContext.getTokenLocation();
    auto Ident = scanner.currentContext.Identifier;

//  Skip identifier
    scanner.scan();

    if (scanner.currentContext.token == Token::Semi_Colon) {
//  If there is a ';', this is an imcomplete type declaration.
        scanner.currentContext.invalidateToken();
        Decl = Create_Iir(Iir_Type_Declaration);
        Set_Identifier(Decl, Ident);
        Set_Location(Decl, Loc);
        return Decl;
    }

    if (scanner.currentContext.token != Token::Is)
        throw std::runtime_error("PARSE ERROR: 'is' expected here");
//  Act as if IS token was forgotten.
    else
//  Eat IS token.
        scanner.scan();


    switch (scanner.currentContext.token) {
        case Token::Left_Paren:
//  This is an enumeration.
            Def = Parse_Enumeration_Type_Definition(Parent);
            Decl = nullptr;
            break;

        case Token::Range:
//  This is a range definition.
            Decl = new Iir_Anonymous_Type_Declaration;
            Set_Identifier(Decl, Ident);
            Set_Location(Decl, Loc);

//  Skip 'range'
            scanner.scan();

            Def = Parse_Range_Constraint();
            Set_Type_Definition(Decl, Def);

            if (scanner.currentContext.token == Token::Units) {
//  A physical type definition.

                auto Phys_Def = Parse_Physical_Type_Definition(Parent);
                if (scanner.currentContext.token == Token::Identifier) {
                    if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                        throw std::runtime_error("PARSE ERROR: simple_name not allowed here in vhdl87");

                    Check_End_Name(Get_Identifier(Decl), Phys_Def);
                }
                Set_Range_Constraint(Phys_Def, Def);
                Set_Type_Definition(Decl, Phys_Def);
            }
            break;

        case Token::Array:
            Def = Parse_Array_Definition();
            Decl = nullptr;
            break;
        case Token::Record:
            Decl = Create_Iir(Iir_Type_Declaration);
            Set_Identifier(Decl, Ident);
            Set_Location(Decl, Loc);
            Def = Parse_Record_Type_Definition;
            Set_Type_Definition(Decl, Def);
            if (scanner.currentContext.token == Token::Identifier) {
                if (scanner.state.options.standard == Vhdl_std::Vhdl_87)
                    throw std::runtime_error("PARSE ERROR: simple_name not allowed here in vhdl87");

                Check_End_Name(Get_Identifier(Decl), Def);
            }
            break;
        case Token::Access:
            Def = Parse_Access_Type_Definition();
            Decl = nullptr;
            break;
        case Token::File:
            Def = Parse_File_Type_Definition();
            Decl = nullptr;
            break;
        case Token::Identifier:
            if (scanner.currentContext.Identifier == Keywords::Name_Protected)
                throw std::runtime_error("PARSE ERROR: protected type not allowed in vhdl87/93");
                //FIXME chck this - Decl = Parse_Protected_Type_Definition (Ident, Loc);
            else
//                throw std::runtime_error("PARSE ERROR: type %i cannot be defined from another type", (1: +Ident), Cont:
//            true);
            throw std::runtime_error("PARSE ERROR: (you should declare a subtype)");
        case Token::Protected:
            if (scanner.state.options.standard < Vhdl_Std::Vhdl_00)
                throw std::runtime_error("PARSE ERROR: protected type not allowed in vhdl87/93");

            Decl = Parse_Protected_Type_Definition(Ident, Loc);
            break;
        default:
            throw std::runtime_error(
                    "PARSE ERROR: type definition starting with a keyword such as RANGE, ARRAY, FILE, RECORD or '(' is expected here");

    }

    if (!Decl) {
        switch (Get_Kind(Def)) {
            case Iir_Enumeration_Type_Definition:
            case Iir_Access_Type_Definition:
            case Iir_Array_Type_Definition:
            case Iir_File_Type_Definition:
                Decl = Create_Iir(Iir_Type_Declaration);
                break;
            case Iir_Array_Subtype_Definition:
                Decl = Create_Iir(Iir_Anonymous_Type_Declaration);
                break;
            default:
                Error_Kind("parse_type_declaration", Def);
        }
        Set_Type_Definition(Decl, Def);
    }
    Set_Identifier(Decl, Ident);
    Set_Location(Decl, Loc);

// ';' is expected after end of type declaration
    Expect(Token::Semi_Colon);
    scanner.currentContext.invalidateToken();
    return Decl;
}


//  precond: '(' or identifier
//  postcond: next token
//
//  [ LRM08 6.3 ]
//
//  resolution_indication ::= resolution_function_name | ( element_resolution )
//
//  element_resolution ::= array_element_resolution | record_resolution
//
//  array_element_resolution ::= resolution_indication
//
//  record_resolution ::= record_element_resolution { , record_element_resolution }
//
//  record_element_resolution ::= record_element_simple_name resolution_indication
//
Iir* Parser::Parse_Resolution_Indication() {
    if (scanner.currentContext.token == Token::Identifier)
//  Resolution function name.
        return Parse_Name(false);
    else if (scanner.currentContext.token == Token::Left_Paren) {
//  Element resolution.
        auto Loc = scanner.currentContext.getTokenLocation();

//  Eat '('
        scanner.scan();

        Ind = Parse_Resolution_Indication();
        if (scanner.currentContext.token == Token::Identifier || scanner.currentContext.token == Token::Left_Paren) {


            //  This was in fact a record_resolution.
            if (Get_Kind(Ind) == Iir_Simple_Name)
                Id = Get_Identifier(Ind);
            else
                throw std::runtime_error("PARSE ERROR:" +Ind+ "element name expected");

            Free_Iir(Ind);

            auto Def = new Iir_Record_Resolution;
            Def->Location = Loc;
            while (1) {
                auto El = new Iir_Record_Element_Resolution;
                El->Location = Loc;
                El->Identifier = Id;
                El->Resolution_Indication = Parse_Resolution_Indication();
                Def->chain.push_back(El);
                if (scanner.currentContext.token == Token::Right_Paren)
                    break;

//  Eat ','
                Expect(Token::Comma);
                scanner.scan();

                if (scanner.currentContext.token != Token::Identifier)
                    throw std::runtime_error("PARSE ERROR: record element identifier expected");

                Id = scanner.currentContext.Identifier;
                Loc = scanner.currentContext.getTokenLocation();

//  Eat identifier
                scanner.scan();
            }
        } else {
            Def = new Iir_Array_Element_Resolution;
            Def->Location = Loc;
            Def->Resolution_Indication = Ind;
        }

//  Eat ')'
        Expect(Token::Right_Paren);
        scanner.scan();

        return Def;
    } else
        throw std::runtime_error("PARSE ERROR: resolution indication expected");

}

//  precond : '('
//  postcond: next token
//
//  [ LRM08 6.3 Subtype declarations ]
//  element_constraint ::= array_constraint | record_constraint
//
//  [ LRM08 5.3.2.1 Array types ]
//  array_constraint ::= index_constraint [ array_element_constraint ] | ( open ) [ array_element_constraint ]
//
//  array_element_constraint ::= element_constraint
//
//  RES is the resolution_indication of the subtype indication.
//
Iir* Parser::Parse_Element_Constraint() {
    auto Def = new Iir_Array_Subtype_Definition;
    Def->Location = scanner.currentContext.getTokenLocation();


//  Eat '('.
    scanner.scan();

    if (scanner.currentContext.token == Token::Open)
//  Eat 'open'.
        scanner.scan();
    else {
//  index_constraint ::= (discrete_range {, discrete_range} )
        while (1) {
            auto El = Parse_Discrete_Range();
            Def->Index_Constraint_List.push_back(El);

            if (scanner.currentContext.token == Token::Right_Paren)
                break;

//  Eat ','
            Expect(Token::Comma);
            scanner.scan();
        }
    }

//  Eat ')'
    Expect(Token::Right_Paren);
    scanner.scan();

    if (scanner.currentContext.token == Token::Left_Paren)
        Def->Array_Element_Constraint = Parse_Element_Constraint();

    return Def;
}

//  precond : tolerance
//  postcond: next token
//
//  [ LRM93 4.2 ]
//  tolerance_aspect ::= TOLERANCE string_expression
Iir* Parser::Parse_Tolerance_Aspect_Opt() {
    if (scanner.state.options.AMS_Vhdl && scanner.currentContext.token == Token::Tolerance) {
        scanner.scan();
        return Parse_Expression();
    } else
        return nullptr;
}


//  precond : identifier or '('
//  postcond: next token
//
//  [ LRM93 4.2 ]
//  subtype_indication ::= [ RESOLUTION_FUNCTION_name ] type_mark [ constraint ]
//
//  constraint ::= range_constraint | index_constraint
//
//  [ LRM08 6.3 ]
//  subtype_indication ::= [ resolution_indication ] type_mark [ constraint ]
//
//  constraint ::= range_constraint | array_constraint | record_constraint
//
//  NAME is the type_mark when already parsed (in range expression or allocator by type).
Iir* Parser::Parse_Subtype_Indication (Iir* Name = nullptr) {

// FIXME: location.
    Iir* Resolution_Indication = nullptr;
    Iir* Def = nullptr;
    Iir* Type_Mark;

    if (Name) {
//  The type_mark was already parsed.
        Type_Mark = Name;
        Check_Type_Mark(Name);
    } else {
        if (scanner.currentContext.token == Token::Left_Paren) {
            if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                throw std::runtime_error("PARSE ERROR: resolution_indication not allowed before vhdl08");
            Resolution_Indication = Parse_Resolution_Indication();
        }
        if (scanner.currentContext.token != Token::Identifier)
            throw std::runtime_error("PARSE ERROR: type mark expected in a subtype indication");
        Type_Mark = Parse_Type_Mark(false);
    }

    if (scanner.currentContext.token == Token::Identifier) {
        if (Resolution_Indication)
            throw std::runtime_error("PARSE ERROR: resolution function already indicated");

        Resolution_Indication = Type_Mark;
        Type_Mark = Parse_Type_Mark(false);
    }

    switch (scanner.currentContext.token) {
        case Token::Left_Paren:
            //  element_constraint.
            Def = Parse_Element_Constraint();
            Def->Subtype_Type_Mark = Type_Mark;
            Def->Resolution_Indication = Resolution_Indication();
            Def->Tolerance = Parse_Tolerance_Aspect_Opt();
            break;

        case Token::Range:
            //  range_constraint.
            //  Skip 'range'
            scanner.scan();

            Def = Parse_Range_Constraint_Of_Subtype_Indication(Type_Mark, Resolution_Indication);
            break;

        default:
            Tolerance = Parse_Tolerance_Aspect_Opt();
            if (Resolution_Indication || Tolerance) {
                //  A subtype needs to be created.
                Def = new Iir_Subtype_Definition;
                Def->Location = Type_Mark->Location;
                Def->Subtype_Type_Mark = Type_Mark;
                Def->Resolution_Indication = Resolution_Indication;
                Def->Tolerance = Tolerance;
            } else
                //  This is just an alias.
                Def = Type_Mark;
    }
    return Def;
}

//  precond : SUBTYPE
//  postcond: ';'
//
//  [ 4.2 ]
//  subtype_declaration ::= SUBTYPE identifier IS subtype_indication ;
Iir_Subtype_Declaration* Parser::Parse_Subtype_Declaration (Iir* Parent) {
    auto Decl = new Iir_Subtype_Declaration;

    Scan_Expect(Token::Identifier);
    Decl->Identifier = scanner.currentContext.Identifier;
    Decl->Parent = Parent;
    Decl->Location = scanner.currentContext.getTokenLocation();

//  Skip identifier.
    Scan_Expect(Token::Is);

//  Skip 'is'.
    scanner.scan();

    auto Def = Parse_Subtype_Indication();
    Decl->Subtype_Indication = Def;

    Expect(Token::Semi_Colon);
    return Decl;
}

//  precond : NATURE
//  postcond: a token
//
//  [ 4.8 ]
//  nature_definition ::= scalar_nature_definition | composite_nature_definition
//
//  [ 3.5.1 ]
//  scalar_nature_definition ::= type_mark ACROSS
//                               type_mark THROUGH
//                               identifier REFERENCE
//
//  [ 3.5.2 ]
//  composite_nature_definition ::= array_nature_definition | record_nature_definition
Iir* Parser::Parse_Nature_Declaration() {
   assert(scanner.currentContext.token == Token::Nature);

// Get the identifier
    Scan_Expect(Token::Identifier,
                "an identifier is expected after 'nature'");
    auto Loc = scanner.currentContext.getTokenLocation();
    auto Ident = scanner.currentContext.Identifier;

    scanner.scan();

    if (scanner.currentContext.token != Token::Is)
        throw std::runtime_error("PARSE ERROR: 'is' expected here");
//  Act as if IS token was forgotten.
    else
//  Eat IS token.
        scanner.scan();

    Iir* Def;
    switch (scanner.currentContext.token) {
        case Token::Array:
//  TODO
            throw std::runtime_error("PARSE ERROR: array nature definition not supported");
            Def = nullptr;
            Eat_Tokens_Until_Semi_Colon;
        case Token::Record:
//  TODO
            throw std::runtime_error("PARSE ERROR: record nature definition not supported");
            Def = nullptr;
            Eat_Tokens_Until_Semi_Colon;
        case Token::Identifier:
            Def = new Iir_Scalar_Nature_Definition;
            Def->Location = Loc;
            Def->Across_Type = Parse_Type_Mark();
            if (scanner.currentContext.token == Token::Across)
                scanner.scan();
            else
                Expect(Token::Across, "'across' expected after type mark");

            Def->Through_Type = Parse_Type_Mark();
            if (scanner.currentContext.token == Token::Through)
                scanner.scan();
            else
                Expect(Token::Across, "'through' expected after type mark");

            if (scanner.currentContext.token == Token::Identifier) {
                auto Ref = new Iir_Terminal_Declaration;
                Ref->Identifier = scanner.currentContext.Identifier;
                Ref->Location;
                Def->Reference = Ref;
                scanner.scan();
                if (scanner.currentContext.token == Token::Reference)
                    scanner.scan();
                else
                    Expect(Token::Reference, "'reference' expected");
                Eat_Tokens_Until_Semi_Colon();
            } else
                throw std::runtime_error("PARSE ERROR: reference identifier expected");
        default:
            throw std::runtime_error("PARSE ERROR: nature definition expected here");
    }

    auto Decl = new Iir_Nature_Declaration;
    Decl->Nature = Def;
    Decl->Identifier = Ident;
    Decl->Location = Loc;

// ';' is expected after end of type declaration
    Expect(Token::Semi_Colon);
    scanner.currentContext.invalidateToken();
    return Decl;
}

//  precond : identifier
//  postcond: next token
//
//  LRM 4.8 Nature declaration
//
//  subnature_indication ::=
//      nature_mark [ index_constraint ]
//      [ TOLERANCE string_expression ACROSS string_expression THROUGH ]
//
//  nature_mark ::= nature_name | subnature_name
//
Iir* Parser::Parse_Subnature_Indication() {

    if (scanner.currentContext.token != Token::Identifier)
        throw std::runtime_error("PARSE ERROR: nature mark expected in a subnature indication");

    auto Nature_Mark = Parse_Name(false);

    if (scanner.currentContext.token == Token::Left_Paren)
//  TODO
        throw std::runtime_error("PARSE ERROR: index constraint not supported for subnature indication");

    if (scanner.currentContext.token == Token::Tolerance)
        throw std::runtime_error("PARSE ERROR: tolerance not supported for subnature indication");

    return Nature_Mark;
}

//  precond : TERMINAL
//  postcond: ;
//
//  [ 4.3.1.5 Terminal declarations ]
//  terminal_declaration ::=
//      TERMINAL identifier_list : subnature_indication
Iir* Parser::Parse_Terminal_Declaration (Iir* Parent) {
//  First and last element of the chain to be returned.
//First, Last : Iir;
//Terminal : Iir;
//Subnature : Iir;
    Sub_Chain_Init(First, Last);

    while (1) {
// 'terminal' or "," was just scanned.
        auto Terminal = new Iir_Terminal_Declaration;
        Scan_Expect(Token::Identifier);
        Terminal->Identifier = scanner.currentContext.Identifier;
        Terminal->Location = scanner.currentContext.getTokenLocation();
        Terminal->Parent = Parent;

        Sub_Chain_Append(First, Last, Terminal);

        scanner.scan();
        if (scanner.currentContext.token == Token::Colon)
            break;
        if (scanner.currentContext.token != Token::Comma)
            throw std::runtime_error("PARSE ERROR: ',' or ':' is expected after identifier in terminal declaration");

    }

// The colon was parsed.
    scanner.scan();
    auto Subnature = Parse_Subnature_Indication();

    Terminal = First;
    while (Terminal) {
// Type definitions are factorized.  This is OK, but not done by
// sem.
        if (Terminal == First)
            Terminal->Nature = Subnature;
        else
            Set_Nature(Terminal, nullptr);

        Terminal = Get_Chain(Terminal);
    }
    Expect(Token::Semi_Colon);
    return First;
}

//  precond : QUANTITY
//  postcond: ;
//
//  [ 4.3.1.6 Quantity declarations ]
//  quantity_declaration ::= free_quantity_declaration
//                       | branch_quantity_declaration
//                       | source_quantity_declaration
//
//  free_quantity_declaration ::= QUANTITY identifier_list : subtype_indication [ = expression ] ;
//
//  branch_quantity_declaration ::= QUANTITY [ across_aspect ] [ through_aspect ] terminal_aspect ;
//
//  source_quantity_declaration ::= QUANTITY identifier_list : subtype_indication source_aspect ;
//
//  across_aspect ::= identifier_list [ tolerance_aspect ] [ = expression ] ACROSS
//
//  through_aspect ::= identifier_list [ tolerance_aspect ] [ = expression ] THROUGH
//
//  terminal_aspect ::= plus_terminal_name [ TO minus_terminal_name ]
//
Iir* Parser::Parse_Quantity_Declaration (Iir* Parent) {

//  First and last element of the chain to be returned.
//First, Last : Iir;
//Object : Iir;
//New_Object : Iir;
//Tolerance : Iir;
//Default_Value : Iir;
//Kind : Iir_Kind;
//Plus_Terminal : Iir;
//begin
    Sub_Chain_Init(First, Last);

//  Eat 'quantity'
    scanner.scan();

    while (1) {
//  Quantity or "," was just scanned.  We assume a free quantity
//  declaration and will change to branch or source quantity if
//  necessary.
        auto Object = new Iir_Free_Quantity_Declaration;
        Expect(Token::Identifier);
        Object->Identifier = scanner.currentContext.Identifier;
        Object->Location = scanner.currentContext.getTokenLocation();
        Object->Parent = Parent;

        Sub_Chain_Append(First, Last, Object);

//  Eat identifier
        scanner.scan();
        if (scanner.currentContext.token != Token::Comma)
            break;

//  Eat ','
        scanner.scan();
    }

    switch (scanner.currentContext.token) {
        case Token::Colon:
//  Either a free quantity (or a source quantity)
//  TODO
            throw std::logic_error("this shouldnt happen");
        case Token::Tolerance:
        case Token::Assign:
        case Token::Across:
        case Token::Through:
//  A branch quantity

//  Parse tolerance aspect
            Tolerance = Parse_Tolerance_Aspect_Opt();

//  Parse default value
            if (scanner.currentContext.token == Token::Assign) {
                scanner.scan();
                Default_Value = Parse_Expression();
            } else
                Default_Value = nullptr;


            switch (scanner.currentContext.token) {
                case Token::Across:
                    Kind = Iir_Across_Quantity_Declaration;
                    break;
                case Token::Through:
                    Kind = Iir_Through_Quantity_Declaration;
                    break;
                default:
                    throw std::runtime_error("PARSE ERROR: 'across' or 'through' expected here");
            }

//  Eat across/through
            scanner.scan();

//  Change declarations
            Object = First;
            Sub_Chain_Init(First, Last);
            while (Object) {
                New_Object = Create_Iir(Kind);
                New_Object->Location = Object->Location;
                New_Object->Identifier = Get_Identifier(Object);
                New_Object->Parent = Parent;
                New_Object->Tolerance = Tolerance;
                New_Object->Default_Value = Default_Value;

                Sub_Chain_Append(First, Last, New_Object);

                if (Object != First)
                    Set_Plus_Terminal(New_Object, nullptr);

                New_Object = Get_Chain(Object);
                Free_Iir(Object);
                Object = New_Object;
            }

//  Parse terminal (or first identifier of through declarations)
            Plus_Terminal = Parse_Name();

            switch (scanner.currentContext.token) {
                case Token::Comma:
                case Token::Tolerance:
                case Token::Assign:
                case Token::Through:
                case Token::Across:
//  Through quantity declaration.  Convert the Plus_Terminal
//  to a declaration.
                    auto Object = new Iir_Through_Quantity_Declaration;
                    New_Object = Object;
                    Object->Location = Plus_Terminal->Location;
                    if (Get_Kind(Plus_Terminal) != Iir_Simple_Name)
                        throw std::runtime_error("PARSE ERROR: identifier for quantity declaration expected");
                    else
                        Object->Identifier = Get_Identifier(Plus_Terminal);

                    Object->Plus_Terminal = nullptr;
                    Free_Iir(Plus_Terminal);

                    while (1) {
                        Object->Parent = Parent;
                        Sub_Chain_Append(First, Last, Object);
                        if (scanner.currentContext.token != Token::Comma)
                            break;
                        scanner.scan();

                        Object = new Iir_Through_Quantity_Declaration;
                        Object->Location = scanner.currentContext.getTokenLocation();
                        if (scanner.currentContext.token != Token::Identifier)
                            throw std::runtime_error("PARSE ERROR: identifier for quantity declaration expected");
                        else {
                            Object->Identifier = scanner.currentContext.Identifier;
                            scanner.scan();
                        }
                        Object->Plus_Terminal = nullptr;

                    }

//  Parse tolerance aspect
                    Object->Tolerance = Parse_Tolerance_Aspect_Opt();

//  Parse default value
                    if (scanner.currentContext.token == Token::Assign) {
                        scanner.scan();
                        Object->Default_Value = Parse_Expression();
                    }

//  Scan 'through'
                    if (scanner.currentContext.token == Token::Through)
                        scanner.scan();
                    else if (scanner.currentContext.token == Token::Across) {
                        throw std::runtime_error(
                                "PARSE ERROR: across quantity declaration must appear before though declaration");
                        scanner.scan();
                    } else
                        throw std::runtime_error("PARSE ERROR: 'through' expected");

//  Parse plus terminal
                    Plus_Terminal = Parse_Name();
                    break;
                default:
                    break;
            }

            Set_Plus_Terminal(First, Plus_Terminal);

//  Parse minus terminal (if present)
            if (scanner.currentContext.token == Token::To) {
                scanner.scan();
                Set_Minus_Terminal(First, Parse_Name);
            }
        default:
            throw std::runtime_error("PARSE ERROR: missing type or across/throught aspect in quantity declaration");
    }
    Expect(Token::Semi_Colon);
    return First;
}

//  precond : token (CONSTANT, SIGNAL, VARIABLE, FILE)
//  postcond: ;
//
//  KIND can be iir_kind_constant_declaration, iir_kind_file_declaration or iir_kind_variable_declaration
//
//  [ LRM93 4.3.1 ]
//  object_declaration ::= constant_declaration
//                       | signal_declaration
//                       | variable_declaration
//                       | file_declaration
//
//  [ LRM93 4.3.1.1 ]
//  constant_declaration ::= CONSTANT identifier_list : subtype_indication [ = expression ]
//
//  [ LRM87 4.3.2 ]
//  file_declaration ::= FILE identifier : subtype_indication IS [ mode ] file_logical_name
//
//  [ LRM93 4.3.1.4 ]
//  file_declaration ::= FILE identifier_list : subtype_indication [ file_open_information ]
//
//  [ LRM93 4.3.1.4 ]
//  file_open_information ::= [ OPEN FILE_OPEN_KIND_expression ] IS file_logical_name
//
//  [ LRM93 4.3.1.4 ]
//  file_logical_name ::= STRING_expression
//
//  [ LRM93 4.3.1.3 ]
//  variable_declaration ::= [ SHARED ] VARIABLE identifier_list : subtype_indication [ = expression ]
//
//  [ LRM93 4.3.1.2 ]
//  signal_declaration ::= SIGNAL identifier_list : subtype_information [ signal_kind ] [ = expression ]
//
//  [ LRM93 4.3.1.2 ]
//  signal_kind ::= REGISTER | BUS
//
//  FIXME: file_open_information.
Iir* Parser::Parse_Object_Declaration (Iir* Parent) {
//  First and last element of the chain to be returned.
//First, Last : Iir;
//Object: Iir;
//Object_Type: Iir;
//Default_Value : Iir;
//Mode: Iir_Mode;
//Signal_Kind : Iir_Signal_Kind;
//Is_Guarded : Boolean;
//Open_Kind : Iir;
//Logical_Name : Iir;
//Kind: Iir_Kind;
//Shared : Boolean;
//Has_Mode : Boolean;
//begin
    Sub_Chain_Init(First, Last);

// object keyword was just scanned.
    switch (scanner.currentContext.token) {
        case Token::Signal:
            Kind = Iir_Signal_Declaration;
            break;
        case Token::Constant:
            Kind = Iir_Constant_Declaration;
            break;
        case Token::File:
            Kind = Iir_File_Declaration;
            break;
        case Token::Variable:
            Kind = Iir_Variable_Declaration;
            Shared = false;
            break;
        case Token::Shared:
            Kind = Iir_Variable_Declaration;
            Shared = true;
            Scan_Expect(Token::Variable);
        default:
            throw std::logic_error();
    }

    while (1) {
// object or "," was just scanned.
        auto Object = new Kind;
        if (Kind = Iir_Variable_Declaration)
            Set_Shared_Flag(Object, Shared);
        Scan_Expect(Token::Identifier);
        Set_Identifier(Object, scanner.currentContext.Identifier);
        Set_Location(Object);
        Set_Parent(Object, Parent);

        Sub_Chain_Append(First, Last, Object);

        scanner.scan();
        if (scanner.currentContext.token == Token::Colon)
            break;
        if (scanner.currentContext.token != Token::Comma) {
            switch (scanner.currentContext.token) {
                case Token::Assign:
                    throw std::runtime_error("PARSE ERROR: missing type in " & Disp_Name(Kind));
                default:
                    throw std::runtime_error(
                            "PARSE ERROR: ',' or ':' is expected after identifier in   Disp_Name (Kind)");
            }
        }
        Set_Has_Identifier_List(Object, true);
    }

// Eat ':'
    scanner.scan();

    Object_Type = Parse_Subtype_Indication();

    if (Kind = Iir_Signal_Declaration)
        Parse_Signal_Kind(Is_Guarded, Signal_Kind);


    if (scanner.currentContext.token == Token::Assign) {
        if (Kind = Iir_File_Declaration)
            throw std::runtime_error("PARSE ERROR: default expression not allowed for a file declaration");


//  Skip ':='.
        scanner.scan();

        Default_Value = Parse_Expression();
    } else if (scanner.currentContext.token == Token::Equal)
        throw std::runtime_error("PARSE ERROR: = should be := for initial value");
    else
        Default_Value = nullptr;


    if (Kind = Iir_File_Declaration) {
        if (scanner.currentContext.token == Token::Open) {
            if (scanner.state.options.standard = Vhdl_Std::Vhdl_87)
                throw std::runtime_error("PARSE ERROR: 'open' and open kind expression not allowed in vhdl 87");

            scanner.scan();
            Open_Kind = Parse_Expression;
        } else
            Open_Kind = nullptr;

//  LRM 4.3.1.4
//  The default mode is IN, if no mode is specified.
        Mode = Iir_In_Mode;

        Logical_Name = nullptr;
        Has_Mode = false;
        if (scanner.currentContext.token == Token::Is) {
//  Skip 'is'.
            scanner.scan();

            switch (scanner.currentContext.token) {
                case Token::In | Token::Out | Token::Inout:
                    if (scanner.state.options.standard >= Vhdl_Std::Vhdl_93)
                        throw std::runtime_error("PARSE ERROR: mode allowed only in vhdl 87");

                    Mode = Parse_Mode;
                    if (Mode = Iir_Inout_Mode)
                        throw std::runtime_error("PARSE ERROR: inout mode not allowed for file");

                    Has_Mode = true;
                default:
                    break;
            }
            Logical_Name = Parse_Expression();
        } else if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: file name expected (vhdl 87)");
    }


    Set_Subtype_Indication(First, Object_Type);
    if (Kind != Iir_File_Declaration)
        Set_Default_Value(First, Default_Value);


    Object = First;
    while (Object) {
        switch (Kind) {
            case Iir_File_Declaration:
                Set_Mode(Object, Mode);
                Set_File_Open_Kind(Object, Open_Kind);
                Set_File_Logical_Name(Object, Logical_Name);
                Set_Has_Mode(Object, Has_Mode);
                break;
            case Iir_Signal_Declaration:
                Set_Guarded_Signal_Flag(Object, Is_Guarded);
                Set_Signal_Kind(Object, Signal_Kind);
                break;
            default:
                break;
        }
        Object = Get_Chain(Object);
    }

//  ';' is not eaten.
    Expect(Token::Semi_Colon);

    return First;
}

//  precond : COMPONENT
//  postcond: ';'
//
//  [ LRM93 4.5 ]
//  component_declaration ::=
//      COMPONENT identifier [ IS ]
//          [ LOCAL_generic_clause ]
//          [ LOCAL_port_clause ]
//      END COMPONENT [ COMPONENT_simple_name ] ;
Iir_Component_Declaration* Parser::Parse_Component_Declaration() {
    auto Component = new Iir_Component_Declaration;
    Scan_Expect(Token::Identifier, "an identifier is expected after 'component'");
    Component->Identifier = scanner.currentContext.Identifier;
    Component->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    if (scanner.currentContext.token == Token::Is) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: ""is"" keyword is not allowed here by vhdl 87");

        Component->Has_Is = true;
        scanner.scan();
    }
    Parse_Generic_Port_Clauses(Component);
    Check_End_Name(Token::Component, Component);
    return Component;
}

//  precond : '['
//  postcond: next token after ']'
//
//  [ 2.3.2 ]
//  signature ::= [ [ type_mark { , type_mark } ] [ RETURN type_mark ] ]
Iir_Signature* Parser::Parse_Signature() {
    Expect(Token::Left_Bracket);
    auto Res = new Iir_Signature;
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip '['
    scanner.scan();

//  List of type_marks.
    if (scanner.currentContext.token == Token::Identifier) {
        while (1) {
            Res->Type_Marks_List.push_back(Parse_Type_Mark(true))
            if (scanner.currentContext.token != Token::Comma)
                break;
            scanner.scan();
        }
    }

    if (scanner.currentContext.token == Token::Return) {
//  Skip 'return'
        scanner.scan();

        Res->Return_Type_Mark = Parse_Name();
    }

//  Skip ']'
    Expect(Token::Right_Bracket);
    scanner.scan();

    return Res;
}

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
//
//  FIXME: signature is not part of the node.
Iir* Parser::Parse_Alias_Declaration() {
    std::string Ident;
//  Eat 'alias'.
    scanner.scan();

    auto Res = new Iir_Object_Alias_Declaration;
    Res->Location = scanner.currentContext.getTokenLocation();

    switch (scanner.currentContext.token) {
        case Token::Identifier:
            Ident = scanner.currentContext.Identifier;
        case Token::Character:
            Ident = scanner.currentContext.Identifier;
        case Token::String:
            Ident = Scan_To_Operator_Name(scanner.currentContext.getTokenLocation());
//  FIXME: vhdl87
//  FIXME: operator symbol.
        default:
            throw std::runtime_error("PARSE ERROR: alias designator expected");
    }

//  Eat identifier.
    Res->Identifier = Ident;
    scanner.scan();

    if (scanner.currentContext.token == Token::Colon) {
        scanner.scan();
        Res->Subtype_Indication = Parse_Subtype_Indication();
    }

//  FIXME: nice message if token is ':=' ?
    Expect(Token::Is);
    scanner.scan();
    Res->Name = Parse_Name();

    return Res;
}

//  precond : FOR
//  postcond: ';'
//
//  [ 5.2 ]
//  configuration_specification ::= FOR component_specification binding_indication ;
//
Iir_Configuration_Specification* Parser::Parse_Configuration_Specification() {
    auto Res = new Iir_Configuration_Specification;
    Res->Location = scanner.currentContext.getTokenLocation();
    Expect(Token::For);
    scanner.scan();
    Parse_Component_Specification(Res);
    Res->Binding_Indication = Parse_Binding_Indication();
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : next token
//  postcond: next token
//
//  [  5.2 ]
//  entity_class = ENTITY | ARCHITECTURE | CONFIGURATION | PROCEDURE | FUNCTION | PACKAGE | TYPE
//                  | SUBTYPE | CONSTANT | SIGNAL | VARIABLE | COMPONENT | LABEL | LITERAL | UNITS | GROUP | FILE
Token::Token Parser::Parse_Entity_Class() {
    switch (scanner.currentContext.token) {
        case Token::Entity:
        case Token::Architecture:
        case Token::Configuration:
        case Token::Procedure:
        case Token::Function:
        case Token::Package:
        case Token::Type:
        case Token::Subtype:
        case Token::Constant:
        case Token::Signal:
        case Token::Variable:
        case Token::Component:
        case Token::Label:
            break;
        case Token::Literal:
        case Token::Units:
        case Token::Group:
        case Token::File:
            break;
        default:
            throw std::runtime_error("PARSE ERROR: " + scanner.currentContext.token + " is not a entity class");
    }
    auto Res = scanner.currentContext.token;
    scanner.scan();
    return Res;
}

Iir_Entity_Class* Parser::Parse_Entity_Class_Entry() {
    auto Res = new Iir_Entity_Class;
    Res->Location = scanner.currentContext.getTokenLocation();
    Res->Entity_Class = Parse_Entity_Class();
    return Res;
}

//  precond : next token
//  postcond: next token
//
//  [ 5.1 ]
//  entity_designator ::= entity_tag [ signature ]
//
//  entity_tag ::= simple_name | character_literal | operator_symbol
Iir* Parser::Parse_Entity_Designator() {
    Iir* Res;
    switch (scanner.currentContext.token) {
        case Token::Identifier:
            Res = new Iir_Simple_Name;
            Res->Location = scanner.currentContext.getTokenLocation();
            Res->Identifier = scanner.currentContext.Identifier;
            break;
        case Token::Character:
            Res = new Iir_Character_Literal;
            Res->Location = scanner.currentContext.getTokenLocation();
            Res->Identifier = scanner.currentContext.Identifier;
            break;
        case Token::String:
            Res = new Iir_Operator_Symbol;
            Res->Location = scanner.currentContext.getTokenLocation();
            Res->Identifier = Scan_To_Operator_Name(Get_Token_Location);
            break;
        default:
            throw std::runtime_error("PARSE ERROR: identifier, character or string expected");
    }
    scanner.scan();
    if (scanner.currentContext.token == Token::Left_Bracket) {
        auto Name = Res;
        Res = Parse_Signature();
        Res->Signature_Prefix = Name;
    }
    return Res;
}

//  precond : next token
//  postcond: IS
//
//  [ 5.1 ]
//  entity_name_list ::= entity_designator { , entity_designator } | OTHERS | ALL
//
void Parser::Parse_Entity_Name_List(Iir_Attribute_Specification* Attribute) {

    switch (scanner.currentContext.token) {
        case Token::All:
            List = Iir_List_All;
            scanner.scan();
            break;
        case Token::Others:
            List = Iir_List_Others;
            scanner.scan();
            break;
        default:
            List = Create_Iir_List;
            while (1) {
                auto El = Parse_Entity_Designator();
                Append_Element(List, El);
                if (scanner.currentContext.token != Token::Comma)
                    break;
                scanner.scan();
            }
    }
    Set_Entity_Name_List(Attribute, List);
    if (scanner.currentContext.token == Token::Colon) {
        scanner.scan();
        Attribute->Entity_Class = Parse_Entity_Class();
    } else
        throw std::runtime_error("PARSE ERROR: missing ':' and entity kind in attribute specification");

}

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
Iir* Parser::Parse_Attribute() {

    Expect(Token::Attribute);
    Scan_Expect(Token::Identifier);
    auto Loc = scanner.currentContext.getTokenLocation();
    auto Ident = scanner.currentContext.Identifier;

//  Skip identifier.
    scanner.scan();

    switch (scanner.currentContext.token) {
        case Token::Colon:
            auto Res = new Iir_Attribute_Declaration;
            Res->Location = Loc;
            Res->Identifier = Ident;

//  Skip ':'.
            scanner.scan();

            Res->Type_Mark = Parse_Type_Mark(true);
            Expect(Token::Semi_Colon);
            return Res;


        case Token::Of:
            auto Res = new Iir_Attribute_Specification;
            Res->Location = Loc;
            auto Designator = new Iir_Simple_Name;
            Designator->Location = Loc;
            Designator->Identifier = Ident;
            Res->Attribute_Designator = Designator;

//  Skip 'of'.
            scanner.scan();

            Parse_Entity_Name_List(Res);
            Expect(Token::Is);

//  Skip 'is'.
            scanner.scan();

            Res->Expression = Parse_Expression();
            Expect(Token::Semi_Colon);
            return Res;

        default:
            throw std::runtime_error("PARSE ERROR: ':' or 'of' expected after identifier");
            return nullptr;
    }
}

//  precond : GROUP
//  postcond: ';'
//
//  [ 4.6 ]
//  group_template_declaration ::=
//     GROUP identifier IS (entity_class_entry_list) ;
//
//  entity_class_entry_list ::= entity_class_entry { , entity_class_entry }
//
//  entity_class_entry ::= entity_class [ <> ]
Iir* Parser::Parse_Group() {
    Expect(Token::Group);
    Scan_Expect(Token::Identifier);
    auto Loc = scanner.currentContext.getTokenLocation();
    auto Ident = scanner.currentContext.Identifier;
    scanner.scan();
    switch (scanner.currentContext.token) {
        case Token::Is:

            Res = Create_Iir(Iir_Group_Template_Declaration);
            Set_Location(Res, Loc);
            Set_Identifier(Res, Ident);
            Scan_Expect(Token::Left_Paren);
            scanner.scan();
            Build_Init(Last);
            while (1) {
                Append(Last, Res, Parse_Entity_Class_Entry);
                if (scanner.currentContext.token == Token::Box) {
                    El = Create_Iir(Iir_Entity_Class);
                    Set_Location(El);
                    Set_Entity_Class(El, Token::Box);
                    Append(Last, Res, El);
                    scanner.scan();
                    if (scanner.currentContext.token == Token::Comma)
                        throw std::runtime_error("PARSE ERROR: '<>' is allowed only for the last entity class entry");

                }
                if (scanner.currentContext.token == Token::Right_Paren)
                    break;
                Expect(Token::Comma);
                scanner.scan();
            }
            Scan_Expect(Token::Semi_Colon);
            return Res;

        case Token::Colon:

            Res = Create_Iir(Iir_Group_Declaration);
            Set_Location(Res, Loc);
            Set_Identifier(Res, Ident);
            scanner.scan();
            Set_Group_Template_Name(Res, Parse_Name(false));
            Expect(Token::Left_Paren);
            scanner.scan();
            List = Create_Iir_List;
            Set_Group_Constituent_List(Res, List);
            while (1) {
                Append_Element(List, Parse_Name(false));
                if (scanner.currentContext.token == Token::Right_Paren)
                    break;
                Expect(Token::Comma);
                scanner.scan();
            }
            Scan_Expect(Token::Semi_Colon);
            return Res;

        default:
            throw std::runtime_error("PARSE ERROR: ':' or 'is' expected here");
            return nullptr;
    }
}

//  precond : next token
//  postcond: ':'
//
//  [ 5.4 ]
//  signal_list ::= signal_name { , signal_name } | OTHERS | ALL
//
std::vector<Iir*> Parser::Parse_Signal_List() {
    std::vector<Iir*> Res;
    switch (scanner.currentContext.token) {
        case Token::Others:
            scanner.scan();
            return Iir_List_Others;
        case Token::All:
            scanner.scan();
            return Iir_List_All;
        default:
            while (1) {
                Res.push_back(Parse_Name());
                if (scanner.currentContext.token == Token::Colon)
                    break;
                Expect(Token::Comma);
                scanner.scan();
            }
            return Res;
    }
}

//  precond : DISCONNECT
//  postcond: ';'
//
//  [ 5.4 ]
//  disconnection_specification ::=
//      DISCONNECT guarded_signal_specification AFTER time_expression ;
Iir_Disconnection_Specification* Parser::Parse_Disconnection_Specification() {
    auto Res = new Iir_Disconnection_Specification;
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip 'disconnect'
    Expect(Token::Disconnect);
    scanner.scan();

    Res->Signal_List = Parse_Signal_List();

//  Skip ':'
    Expect(Token::Colon);
    scanner.scan();

    Res->Type_Mark = Parse_Type_Mark(true);

//  Skip 'after'
    Expect(Token::After);
    scanner.scan();

    Res->Expression = Parse_Expression();
    return Res;
}

Iir* Parser::Parse_Psl_Default_Clock() {
    auto Res = new Iir_Psl_Default_Clock;
    Res->Location = scanner.currentContext.getTokenLocation();

//  FIXME: Recognize PSL keywords.
    //scanner.state.psl = true;

//  Skip 'default'.
    Scan_Expect(Token::Psl_Clock);
    //FIXME:
    //Xrefs.Xref_Keyword(Get_Token_Location);

//  Skip 'clock'.
    Scan_Expect(Token::Is);

//  Skip 'is'.
    scanner.scan();

//    FIXME: Res->Psl_Boolean = Parse_Psl.Parse_Psl_Boolean();
    Expect(Token::Semi_Colon);

    //FIXME:
    //Scanner.Flag_Scan_In_Comment = false;
    //Scanner.Flag_Psl = false;

    return Res;
}
/*
 * TODO: NOTE: PSL
 */
/*
Parse_Psl_Declaration return Iir
        is
Tok : constant Token_Type = scanner.currentContext.token;
Loc : constant Location_Type = scanner.currentContext.getTokenLocation();
Res : Iir;
Decl : PSL_Node;
Id : Name_Id;
begin
//  Skip 'property', 'sequence' or 'endpoint'.
scanner.scan();

if (scanner.currentContext.token != Token::Identifier {
throw std::runtime_error("PARSE ERROR: declaration name expected here");
Id = Null_Identifier;
else
Id = scanner.currentContext.Identifier;
end if;

//  Parse PSL declaration.
Scanner.Flag_Psl = true;
Decl = Parse_Psl.Parse_Psl_Declaration (Tok);
Expect (Token::Semi_Colon);
Scanner.Flag_Scan_In_Comment = false;
Scanner.Flag_Psl = false;

if Tok = Token::Psl_Endpoint
        && Parse_Psl.Is_Instantiated_Declaration (Decl)
{
//  Instantiated endpoint: make it visible from VHDL.
Res = Create_Iir (Iir_Psl_Endpoint_Declaration);
else
//  Otherwise, it will be visible only from PSL.
Res = Create_Iir (Iir_Psl_Declaration);
end if;
Set_Location (Res, Loc);
Set_Identifier (Res, Id);
Set_Psl_Declaration (Res, Decl);

return Res;
end Parse_Psl_Declaration;
*/
//  Return the parent of a nested package.  Used to check if some
//  declarations are allowed in a package.
Iir* Parser::Get_Package_Parent (Iir* Decl) {
    auto Res = Decl;
    while (1) {
        switch (Get_Kind(Res)) {
            case Iir_Package_Declaration:
            case Iir_Package_Body:
                Parent = Get_Parent(Res);
                if (Get_Kind(Parent) = Iir_Design_Unit)
                    return Res;
                else
                    Res = Parent;

            default:
                return Res;
        }
    }
}


//  precond : next token
//  postcond: next token
//
//  [ LRM08 3.2.3 Entity declarative part ]
//  entity_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | signal_declaration
//     | shared_variable_declaration
//     | file_declaration
//     | alias_declaration
//     | attribute_declaration
//     | attribute_specification
//     | disconnection_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//     | PSL_property_declaration
//     | PSL_sequence_declaration
//     | PSL_clock_declaration
//
//  [ LRM08 3.3.2 Architecture declarative part ]
//  block_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | signal_declaration
//     | shared_variable_declaration
//     | file_declaration
//     | alias_declaration
//     | component_declaration
//     | attribute_declaration
//     | attribute_specification
//     | configuration_specification
//     | disconnection_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//     | PSL_property_declaration
//     | PSL_sequence_declaration
//     | PSL_clock_declaration
//
//  [ LRM08 4.3 Subprogram bodies ]
//  subprogram_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | variable_declaration
//     | file_declaration
//     | alias_declaration
//     | attribute_declaration
//     | attribute_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//
//  [ LRM08 4.7 Package declarations ]
//  package_declarative_item ::=
//       subprogram_declaration
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | signal_declaration
//     | variable_declaration
//     | file_declaraton
//     | alias_declaration
//     | component_declaration
//     | attribute_declaration
//     | attribute_specification
//     | disconnection_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//     | PSL_property_declaration
//     | PSL_sequence_declaration
//
//  [ LRM08 4.8 Package bodies ]
//  package_body_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | variable_declaration
//     | file_declaration
//     | alias_declaration
//     | attribute_declaration
//     | attribute_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//
//  [ LRM08 5.6.2 Protected type declarations ]
//  protected_type_declarative_item ::=
//       subprogram_declaration
//     | subprogram_instantiation_declaration
//     | attribute_specification
//     | use_clause
//
//  [ LRM08 5.6.3 Protected type bodies ]
//  protected_type_body_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | variable_declaration
//     | file_declaration
//     | alias_declaration
//     | attribute_declaration
//     | attribute_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//
//  [ LRM08 11.3 Process statement ]
//  process_declarative_item ::=
//       subprogram_declaration
//     | subprogram_body
//     | subprogram_instantiation_declaration
//     | package_declaration
//     | package_body
//     | package_instantiation_declaration
//     | type_declaration
//     | subtype_declaration
//     | constant_declaration
//     | variable_declaration
//     | file_declaration
//     | alias_declaration
//     | attribute_declaration
//     | attribute_specification
//     | use_clause
//     | group_template_declaration
//     | group_declaration
//
//  Some declarations are not allowed in every declarative part:
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
void Parser::Parse_Declarative_Part (Iir* Parent) {
/*
Package_Parent return Iir is
begin
if Package_Parent_Cache = nullptr {
        Package_Parent_Cache = Get_Package_Parent (Parent);
end if;
return Package_Parent_Cache;
end Package_Parent;*/

    auto Package_Parent_Cache = nullptr;
    Build_Init(Last_Decl);
    while (1) {
        Iir* Decl = nullptr;
        switch (scanner.currentContext.token) {
            case Token::Invalid:
                throw std::logic_error("");
            case Token::Type:
                Decl = Parse_Type_Declaration(Parent);

//  LRM 2.5  Package declarations
//  If a package declarative item is a type declaration that is
//  a full type declaration whose type definition is a
//  protected_type definition, then that protected type
//  definition must not be a protected type body.
                if (Decl && Get_Kind(Decl) == Iir_Protected_Type_Body) {
                    switch (Get_Kind(Parent)) {
                        case Iir_Package_Declaration:
                            throw std::runtime_error("PARSE ERROR:" + +Decl,
                                                     "protected type body not allowed in package declaration");
                        default:
                            break;
                    }
                }
            case Token::Subtype:
                Decl = Parse_Subtype_Declaration(Parent);
                break;
            case Token::Nature:
                Decl = Parse_Nature_Declaration()
                break;;
            case Token::Terminal:
                Decl = Parse_Terminal_Declaration(Parent);
                break;
            case Token::Quantity:
                Decl = Parse_Quantity_Declaration(Parent);
                break;
            case Token::Signal:
//  LRM08 4.7 Package declarations
//  For package declaration that appears in a subprogram body,
//  a process statement, or a protected type body, [...]
//  Moreover, it is an eror if [...] a signal declaration [...]
//  appears as a package declarative item of such a package
//  declaration.
                switch (Get_Kind(Package_Parent)) {
                    case Iir_Function_Body:
                    case Iir_Procedure_Body:
                        throw std::runtime_error("PARSE ERROR: signal declaration not allowed in subprogram body");
                    case Iir_Kinds_Process_Statement:
                        throw std::runtime_error("PARSE ERROR: signal declaration not allowed in process");
                    case Iir_Protected_Type_Body:
                    case Iir_Protected_Type_Declaration:
                        throw std::runtime_error("PARSE ERROR: signal declaration not allowed in protected type");
                    case Iir_Package_Body:
                        throw std::runtime_error("PARSE ERROR: signal declaration not allowed in package body");
                    case Iir_Entity_Declaration:
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                    case Iir_Package_Declaration:
                        break;
                    default:
                        Error_Kind("parse_declarative_part", Package_Parent);
                }
                Decl = Parse_Object_Declaration(Parent);
                break;
            case Token::Constant:
                Decl = Parse_Object_Declaration(Parent);
                break;
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
                switch (Get_Kind(Package_Parent)) {
                    case Iir_Entity_Declaration:
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                    case Iir_Package_Declaration:
                    case Iir_Package_Body:
                    case Iir_Protected_Type_Declaration:
//  FIXME: replace HERE with the kind of declaration
//  ie: "not allowed in a package" rather than "here".
                        throw std::runtime_error("PARSE ERROR: non-shared variable declaration not allowed here");
                    case Iir_Function_Body:
                    case Iir_Procedure_Body:
                    case Iir_Kinds_Process_Statement:
                    case Iir_Protected_Type_Body:
                        null;
                    default:
                        Error_Kind("parse_declarative_part", Package_Parent);
                }
                Decl = Parse_Object_Declaration(Parent);
            case Token::Shared:
                if (scanner.state.options.standard <= Vhdl_Std::Vhdl_87)
                    throw std::runtime_error("PARSE ERROR: shared variable not allowed in vhdl 87");
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
                switch (Get_Kind(Package_Parent)) {
                    case Iir_Entity_Declaration:
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                    case Iir_Package_Declaration:
                    case Iir_Package_Body:
                    case Iir_Protected_Type_Declaration:
                        null;
                    case Iir_Function_Body:
                    case Iir_Procedure_Body:
                    case Iir_Kinds_Process_Statement:
                    case Iir_Protected_Type_Body:
                        throw std::runtime_error("PARSE ERROR: shared variable declaration not allowed here");
                    default:
                        Error_Kind("parse_declarative_part", Package_Parent);
                }
                Decl = Parse_Object_Declaration(Parent);
                break;
            case Token::File:
                Decl = Parse_Object_Declaration(Parent);
                break;
            case Token::Function:
            case Token::Procedure:
            case Token::Pure:
            case Token::Impure:
                Decl = Parse_Subprogram_Declaration;
                if (Decl && Get_Subprogram_Body(Decl) != nullptr) {
                    if (Get_Kind(Parent) = Iir_Package_Declaration)
                        Error_Msg_Parse(+Decl, "subprogram body not allowed in a package");
                }
                break;
            case Token::Alias:
                Decl = Parse_Alias_Declaration();
                break;
            case Token::Component:
                switch (Get_Kind(Parent)) {
                    case Iir_Entity_Declaration:
                    case Iir_Procedure_Body:
                    case Iir_Function_Body:
                    case Iir_Kinds_Process_Statement:
                    case Iir_Package_Body:
                    case Iir_Protected_Type_Body:
                    case Iir_Protected_Type_Declaration:
                        throw std::runtime_error("PARSE ERROR: component declaration are not allowed here");
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                    case Iir_Package_Declaration:
                        break;
                    default:
                        throw std::runtime_error("parse_declarative_part", Package_Parent);
                }
                Decl = Parse_Component_Declaration();
                break;
            case Token::For:
                switch (Get_Kind(Parent)) {
                    case Iir_Entity_Declaration:
                    case Iir_Function_Body:
                    case Iir_Procedure_Body:
                    case Iir_Kinds_Process_Statement:
                    case Iir_Package_Declaration:
                    case Iir_Package_Body:
                    case Iir_Protected_Type_Body:
                    case Iir_Protected_Type_Declaration:
                        throw std::runtime_error("PARSE ERROR: configuration specification not allowed here");
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                        break;
                    default:
                        throw std::runtime_error("parse_declarative_part", Package_Parent);
                }
                Decl = Parse_Configuration_Specification();
                break;
            case Token::Attribute:
                Decl = Parse_Attribute();
                break;
            case Token::Disconnect:
//  LRM08 4.7 Package declarations
//  For package declaration that appears in a subprogram body,
//  a process statement, or a protected type body, [...]
//  Moreover, it is an eror if [...] a disconnection
//  specification [...] appears as a package declarative item
//  of such a package declaration.
                switch (Get_Kind(Parent)) {
                    case Iir_Function_Body:
                    case Iir_Procedure_Body:
                    case Iir_Kinds_Process_Statement:
                    case Iir_Protected_Type_Body:
                    case Iir_Package_Body:
                    case Iir_Protected_Type_Declaration:
                        throw std::runtime_error("PARSE ERROR: disconnect specification not allowed here");
                    case Iir_Entity_Declaration:
                    case Iir_Architecture_Body:
                    case Iir_Block_Statement:
                    case Iir_Generate_Statement_Body:
                    case Iir_Package_Declaration:
                        break;
                    default:
                        throw std::runtime_error("parse_declarative_part", Parent);
                }
                Decl = Parse_Disconnection_Specification();
                break;
            case Token::Use:
                Decl = Parse_Use_Clause();
                break;
            case Token::Group:
                Decl = Parse_Group();
                break;
            case Token::Package:
                if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                    throw std::runtime_error("PARSE ERROR: nested package not allowed before vhdl 2008");

                Decl = Parse_Package(Parent);
                if (Decl && Get_Kind(Decl) = Iir_Package_Body) {
                    if (Get_Kind(Parent) = Iir_Package_Declaration)
                        throw std::runtime_error(+Decl, "package body not allowed in a package");
                }
                break;
            case Token::Identifier:
                if (scanner.state.options.standard >= Vhdl_Std::Vhdl_08 &&
                    scanner.currentContext.Identifier == Keywords::Name_Default) {
//  This identifier is a PSL keyword.
//TODO: Xrefs.Xref_Keyword (Get_Token_Location);

//  Check whether default clock are allowed in this region.
                    switch (Get_Kind(Parent)) {
                        case Iir_Function_Body:
                        case Iir_Procedure_Body:
                        case Iir_Kinds_Process_Statement:
                        case Iir_Protected_Type_Body:
                        case Iir_Package_Declaration:
                        case Iir_Package_Body:
                        case Iir_Protected_Type_Declaration:
                            throw std::runtime_error("PARSE ERROR: PSL default clock declaration not allowed here");
                        case Iir_Entity_Declaration:
                        case Iir_Architecture_Body:
                        case Iir_Block_Statement:
                        case Iir_Generate_Statement_Body:
                            break;
                        default:
                            throw std::runtime_error("parse_declarative_part", Parent);
                    }
                    Decl = Parse_Psl_Default_Clock();
                } else
                    throw std::runtime_error("PARSE ERROR: object class keyword such as 'variable' is expected");
                break;
            case Token::Semi_Colon:
                throw std::runtime_error("PARSE ERROR: ';' (semi colon) not allowed alone");
            default:
                //TODO: BREAK EXTERNAL LOOP
        }
        if (Decl)
            Append_Subchain(Last_Decl, Parent, Decl);


        if (scanner.currentContext.token == Token::Semi_Colon or scanner.currentContext.token == Token::Invalid)
            scanner.scan();

    }
}

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
void Parser::Parse_Entity_Declaration (Iir_Design_Unit* Unit) {
    Expect(Token::Entity);
    auto Res = new Iir_Entity_Declaration;

//  Get identifier.
    Scan_Expect(Token::Identifier, "an identifier is expected after ""entity""");
    Res->Identifier = scanner.currentContext.Identifier;
    Res->Location = scanner.currentContext.getTokenLocation();

    Scan_Expect(Token::Is, "missing ""is"" after identifier");
    scanner.scan();

    Parse_Generic_Port_Clauses(Res);

    Parse_Declarative_Part(Res);

    if (scanner.currentContext.token == Token::Begin) {
        Res->Has_Begin = true;
        scanner.scan();
        Parse_Concurrent_Statements(Res);
    }

//   end keyword is expected to finish an entity declaration
    Expect(Token::End);
    Unit->End_Location = scanner.currentContext.getTokenLocation();

    scanner.scan();
    if (scanner.currentContext.token == Token::Entity) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'entity' keyword not allowed here by vhdl 87");

        Res->End_Has_Reserved_Id = true;
        scanner.scan();
    }
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    scanner.currentContext.invalidateToken();
    Unit->Library_Unit = Res;
}

//  [ LRM93 7.3.2 ]
//  choice ::= simple_expression | discrete_range | ELEMENT_simple_name | OTHERS
Iir* Parser::Parse_A_Choice (Iir* Expr) {
    Iir* A_Choice;
    if (!Expr) {
        if (scanner.currentContext.token == Token::Others) {
            A_Choice = new Iir_Choice_By_Others;
            A_Choice->Location = scanner.currentContext.getTokenLocation();

//  Skip 'others'
            scanner.scan();

            return A_Choice;
        } else {
            Expr1 = Parse_Expression();

            if (!Expr1) {
//  Handle parse error now.
//  FIXME: skip until '=>'.
                A_Choice = new Iir_Choice_By_Expression;
                A_Choice->Location = scanner.currentContext.getTokenLocation();
                return A_Choice;
            }
        }
    } else
        Expr1 = Expr;

    if (Is_Range_Attribute_Name(Expr1)) {
        A_Choice = new Iir_Choice_By_Range;
        A_Choice->Location = Expr1->Location;
        A_Choice->Choice_Range = Expr1;
        return A_Choice;
    } else if (scanner.currentContext.token == Token::To || scanner.currentContext.token == Token::Downto) {
        A_Choice = new Iir_Choice_By_Range;
        A_Choice->Location = Expr1->Location;
        A_Choice->Choice_Range = Parse_Range_Expression(Expr1);
        return A_Choice;
    } else {
        A_Choice = new Iir_Choice_By_Expression;
        A_Choice->Location = Expr1->Location;
        A_Choice->Choice_Expression = Expr1;
        return A_Choice;
    }
}

//  [ LRM93 7.3.2 ]
//  choices ::= choice { | choice }
//
// Leave Token::double_arrow as current token.
Iir* Parser::Parse_Choices (Iir* Expr) {
//    A_Choice:
//    Iir;
//    Expr1 :
//    Iir;
    Sub_Chain_Init(First, Last);
    auto Expr1 = Expr;
    while (1) {
        A_Choice = Parse_A_Choice(Expr1);
        if (First) {
            Set_Same_Alternative_Flag(A_Choice, true);
            if (Get_Kind(A_Choice) == Iir_Choice_By_Others)
                throw std::runtime_error("PARSE ERROR: 'others' choice must be alone");
        }
        Sub_Chain_Append(First, Last, A_Choice);
        if (scanner.currentContext.token != Token::Bar)
            return First;

        scanner.scan();
        Expr1 = nullptr;
    }
}

//  precond : '('
//  postcond: next token
//
//  This can be an expression or an aggregate.
//
//  [ LRM93 7.3.2 ]
//  aggregate ::= ( element_association { , element_association } )
//
//  [ LRM93 7.3.2 ]
//  element_association ::= [ choices: ] expression
Iir* Parser::Parse_Aggregate() {
    auto Loc = scanner.currentContext.getTokenLocation();

//  Skip '('
    scanner.scan();

    if (scanner.currentContext.token != Token::Others) {
        auto Expr = Parse_Expression();
        switch (scanner.currentContext.token) {
            case Token::Comma:
            case Token::Double_Arrow:
            case Token::Bar:
//  This is really an aggregate
                break;
            case Token::Right_Paren:
// This was just a braced expression.

// Eat ')'.
                scanner.scan();

                if (Get_Kind(Expr) == Iir_Aggregate)
//  Parenthesis around aggregate is useless and change the
//  context for array aggregate.
                    Warning_Msg_Sem(Warnid_Parenthesis, Expr, "suspicious parenthesis around aggregate");
                else if (!Flag_Parse_Parenthesis)
                    return Expr;


//  Create a node for the parenthesis.
                auto Res = new Iir_Parenthesis_Expression;
                Set_Location(Res, Loc);
                Set_Expression(Res, Expr);
                return Res;

            case Token::Semi_Colon:
//  Surely a missing parenthesis.
//  FIXME: in case of multiple missing parenthesises, several
//    messages will be displayed
                throw std::runtime_error("PARSE ERROR: missing ')' for opening parenthesis at %l", +Loc);
                return Expr;
            default:
//  Surely a parse error...
                //FIXME: CHEK
                throw std::logic_error("");
        }
    } else
        Expr = nullptr;

    Res = Create_Iir(Iir_Aggregate);
    Set_Location(Res, Loc);
    Build_Init(Last);
    while (1) {
        if (scanner.currentContext.token == Token::Others) {
            Assoc = Parse_A_Choice(nullptr);
            Expect(Token::Double_Arrow);
            scanner.scan();
            Expr = Parse_Expression;
        } else {
            if (!Expr)
                Expr = Parse_Expression;

            if (!Expr)
                return nullptr;

            switch (scanner.currentContext.token) {
                case Token::Comma:
                case Token::Right_Paren:
                    Assoc = Create_Iir(Iir_Choice_By_None);
                    Assoc->Location = Expr->Location;
                default:
                    Assoc = Parse_Choices(Expr);
                    Expect(Token::Double_Arrow);
                    scanner.scan();
                    Expr = Parse_Expression();
            }
        }
        Set_Associated_Expr(Assoc, Expr);
        Append_Subchain(Last, Res, Assoc);
        if (scanner.currentContext.token == Token::Right_Paren)
            break;
        Expect(Token::Comma);
        scanner.scan();
        Expr = nullptr;
    }
    scanner.scan();
    return Res;
}

//  precond : NEW
//  postcond: next token
//
//  [LRM93 7.3.6]
//  allocator ::= NEW subtype_indication | NEW qualified_expression
//
Iir* Parser::Parse_Allocator() {

    auto Loc = scanner.currentContext.getTokenLocation();

// Accept 'new'.
    scanner.scan();
    auto Expr = Parse_Name(false);
    if (Get_Kind(Expr) != Iir_Qualified_Expression) {
// This is a subtype_indication.
        Res = new Iir_Allocator_By_Subtype;
        Expr = Parse_Subtype_Indication(Expr);
        Res->Subtype_Indication = Expr;
    } else {
        Res = new Iir_Allocator_By_Expression;
        Res->Expression = Expr;
    }

    Set_Location(Res, Loc);
    return Res;
}

//  precond : Token::bit_string
//  postcond: Token::bit_string
//
//  Simply create the node for a bit string.
Iir* Parser::Parse_Bit_String() {
    char C;
    auto Res = new Iir_String;
    Res->Location = scanner.currentContext.getTokenLocation();
    Res->str = scanner.currentContext.Identifier;
    if (Res->str[1] = 's') {
        Set_Has_Sign(Res, true);
        Set_Has_Signed(Res, true);
        assert(Nam_Length = 2);
        C = Name_Table.Nam_Buffer(2);
    } else if (Nam_Buffer(1) = 'u') {
        Set_Has_Sign(Res, true);
        Set_Has_Signed(Res, false);
        assert(Nam_Length = 2);
        C = Nam_Buffer(2);
    } else {
        Set_Has_Sign(Res, false);
        Set_Has_Signed(Res, false);
        assert(Nam_Length = 1);
        C = Nam_Buffer(1);
    }
    Number_Base_Type B;
    switch (C) {
        case 'b':
            Res->Bit_String_Base = Base_2;
            break;
        case 'o':
            Res->Bit_String_Base = Base_8;
            break;
        case 'd':
            Res->Bit_String_Base = Base_10;
            break;
        case 'x':
            Res->Bit_String_Base = Base_16;
            break;
        default:
            throw std::logic_error("");
    }
    return Res;
}

//  Scan returns an expanded bit value.  Adjust the expanded bit value as
//  required by the length.
void Parser::Resize_Bit_String (Iir_String* Lit, int Nlen) {
//is
//        use Str_Table;
//
//Old_Len : constant Nat32 = Get_String_Length (Lit);
//Is_Signed : constant Boolean = Get_Has_Signed (Lit);
//Id : constant String8_Id = Get_String8_Id (Lit);
//C : Nat8;
//begin
    char C;
    if (Nlen > Lit->str.length()) {
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
        if (Lit->Has_Signed) {
            if (Lit->str.empty())
                throw std::runtime_error("PARSE ERROR: " + Lit->str + "cannot expand an empty signed bit string");
            else
                C = Lit->str.front();
        } else
            C = '0';

        Lit->str.insert(0, Nlen - Lit->str.length(), C);

    } else if (Nlen < Lit->str.length()) {
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
        if (Lit->Has_Signed)
            C = Lit->str[0];
        else
            C = '0';

        for (int i = 0; i < Lit->str.length() - Nlen; i++) {
            if (C != Lit->str[i])
                throw std::runtime_error("truncation of bit string changes the value");
        }
//  Shift (position 1 is the MSB).
        Lit->str.erase(0, Lit->str.length() - Nlen);
    } else {
//  LRM08 15.8
//  -- If the length is equal to the number of characters in the
//     expanded bit value, the string literal value is the expanded
//     bit value itself.

    }
}

//  Precond : next token after Token::integer
//  postcond: likewise
//
//  Return an integer_literal or a physical_literal.
Iir* Parser::Parse_Integer_Literal (long Val) {

    if (scanner.currentContext.token == Token::Identifier) {
// physical literal
        Res = new Iir_Physical_Int_Literal;
        Res->Unit_Name = Parse_Name(false);
    } else
// integer literal
        Res = new Iir_Integer_Literal;
    Res->Set_Value = Val;
    return Res;
}

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

Iir *Parser::Parse_Primary() {
//Res: Iir*;
    int64_t Int;
    double Fp;
    Location_Type Loc;

    switch (scanner.currentContext.token) {
        case Token::Integer:
            Int = scanner.currentContext.Int64;
            Loc = scanner.currentContext.getTokenLocation();

            //  Skip integer
            scanner.scan();

            auto result = Parse_Integer_Literal(Int);
            result->Location = Loc;
            return result;

        case Token::Real:
            Fp = scanner.currentContext.Fp64;
            Loc = scanner.currentContext.getTokenLocation();

            //  Skip real
            scanner.scan();

            if (scanner.currentContext.token == Token::Identifier) {
                // physical literal
                result = new Iir_Physical_Fp_Literal;
                result->Unit_Name = Parse_Name(Allow_Indexes = > false);
            } else {
                // real literal
                result = new Iir_Floating_Point_Literal;
            }
            result->Location = Loc;
            result->Fp_Value = Fp;
            return result;

        case Token::Identifier:
        case Token::Double_Less:
            result = Parse_Name(/*FIXME: Allow_Indexes:*/ true);
            if (Get_Kind(result) = Iir_Signature)
                throw std::runtime_error("PARSE ERROR:" + result + "signature not allowed in expression");
            else
                return result;

        case Token::Character:
            result = scanner.Current_Character_Iir();
            scanner.scan();
            if (scanner.currentContext.token == Token::Tick)
                throw std::runtime_error("PARSE ERROR: prefix of an attribute can't be a character literal");
            return result;
        case Token::Left_Paren:
            return Parse_Aggregate();
        case Token::String:
            return Parse_Name();
        case Token::Null:
            result = new Iir_Null_Literal;
            result->Location = scanner.currentContext.getTokenLocation();
            scanner.scan();
            return result;
        case Token::New:
            return Parse_Allocator();

        case Token::Integer_Letter:
            Int = scanner.currentContext.Int64;
            Loc = scanner.currentContext.getTokenLocation();

//  Skip integer
            scanner.scan();

            if (scanner.currentContext.token == Token::Bit_String) {
                result = Parse_Bit_String();
                Set_Has_Length(result, true);

//  Skip bit string
                scanner.scan();

//  resultize.
                resultize_Bit_String(result, Nat32(Int));
            } else
                throw std::runtime_error(
                        "PARSE ERROR: "/*Get_Token_Location,*/ + "space is required between number and unit name");

            result->Location = Loc;
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
Iir* Parser::Build_Unary_Factor(Iir_Unary_Operator_Type Op, Iir* Primary, bool isVhdl_08) {

    if (Primary)
        return Primary;

    if (isVhdl_08 && scanner.state.options.standard < Vhdl_Std::Vhdl_08)
        throw std::runtime_error("PARSE ERROR: missing left operand of logical expression");

    auto result = new Iir_Unary_Operator;
    result->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    result->Operand = Parse_Primary();
    return result;
}

Iir* Parser::Parse_Factor(Iir* Primary = nullptr) {
//result, Left: Iir*;

    switch (scanner.currentContext.token) {
    case Token::Abs:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Absolute, Primary, false);
    case Token::Not:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Not, Primary, false);
    case Token::And:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_And, Primary, true);
    case Token::Or:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_Or, Primary, true);
    case Token::Nand:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_Nand, Primary, true);
    case Token::Nor:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_Nor, Primary, true);
    case Token::Xor:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_Xor, Primary, true);
    case Token::Xnor:
        return Build_Unary_Factor(Iir_Unary_Operator_Type::Reduction_Xnor, Primary, true);

    default:
        Iir* Left;
        if (Primary)
            Left = Primary;
        else
            Left = Parse_Primary();

        if (scanner.currentContext.token == Token::Double_Star) {
            auto result = new Iir_Binary_Operator;
            result->Location = scanner.currentContext.getTokenLocation();
            scanner.scan();
            result->Left = Left;
            result->Right = Parse_Primary();
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
    Iir* result = Parse_Factor(Primary);
    while (Token::isMultiplyingOperator(scanner.currentContext.token)) {
        auto Tmp = new Iir_Binary_Operator;
        switch (scanner.currentContext.token) {
        case Token::Star:
            Tmp->Operator_Type = Iir_Binary_Operator_Type::Multiplication;
            break;
        case Token::Slash:
            Tmp->Operator_Type = Iir_Binary_Operator_Type::Division;
            break;
        case Token::Mod:
            Tmp->Operator_Type = Iir_Binary_Operator_Type::Modulus;
            break;
        case Token::Rem:
            Tmp->Operator_Type = Iir_Binary_Operator_Type::Remainder;
            break;
        default:
            throw std::logic_error("This cannot be a mode, parsing logic error");
        }
        Tmp->Location = scanner.currentContext.getTokenLocation();
        Tmp->Left = result;
        scanner.scan();
        Tmp->Right = Parse_Factor();
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
Iir *Parser::Parse_Simple_Expression(Iir *Primary) {

    Iir_Unary_Operator *unary_Operator = new Iir_Unary_Operator;
    Iir* result;
    if (Token::isSignType(scanner.currentContext.token) && !Primary) {
        switch (scanner.currentContext.token) {
            case Token::Plus:
                unary_Operator->Operator_Type = Iir_Unary_Operator_Type::Identity;
                break;
            case Token::Minus:
                unary_Operator->Operator_Type = Iir_Unary_Operator_Type::Negation;
                break;
            default:
                throw std::logic_error("");
        }

        unary_Operator->Location = scanner.currentContext.getTokenLocation();
        scanner.scan();
        unary_Operator->Operand = Parse_Term(nullptr);
        result = unary_Operator;
    } else
        result = Parse_Term(Primary);


    while (Token::isAddingOperator(scanner.currentContext.token)) {
        auto tmp = new Iir_Binary_Operator;
        switch (scanner.currentContext.token) {
            case Token::Plus:
                tmp->Operator_Type = Iir_Binary_Operator_Type::Addition;
                break;
            case Token::Minus:
                tmp->Operator_Type = Iir_Binary_Operator_Type::Subtraction;
                break;
            case Token::Ampersand:
                tmp->Operator_Type = Iir_Binary_Operator_Type::Concatenation;
                break;
            default:
                throw std::logic_error("");
        }
        tmp->Location = scanner.currentContext.getTokenLocation();
        scanner.scan();
        tmp->Left = result;
        tmp->Right = Parse_Term(nullptr);
        result = tmp;
    }
    return result;
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  shift_expression ::=
//      simple_expression [ shift_operator simple_expression ]
//
//  [ 7.2 ]
//  shift_operator ::= SLL | SRL | SLA | SRA | ROL | ROR
Iir *Parser::Parse_Shift_Expression() {
    auto Tmp = Parse_Simple_Expression();
    if (Token::isShiftingOperator(scanner.currentContext.token))
        return Tmp;
    else if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
        throw std::runtime_error("PARSE ERROR: shift operators not allowed in vhdl 87");
    auto Res = new Iir_Binary_Operator;
    switch (scanner.currentContext.token) {
        case Token::Sll:
            Res->Operator_Type = Iir_Binary_Operator_Type::Sll;
            break;
        case Token::Sla:
            Res->Operator_Type = Iir_Binary_Operator_Type::Sla;
            break;
        case Token::Srl:
            Res->Operator_Type = Iir_Binary_Operator_Type::Srl;
            break;
        case Token::Sra:
            Res->Operator_Type = Iir_Binary_Operator_Type::Sra;
            break;
        case Token::Rol:
            Res->Operator_Type = Iir_Binary_Operator_Type::Rol;
            break;
        case Token::Ror:
            Res->Operator_Type = Iir_Binary_Operator_Type::Ror;
            break;
        default:
            throw std::logic_error("");
    }
    Res->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    Res->Left = Tmp;
    Res->Right = Parse_Simple_Expression();
    return Res;
}

//  precond : next token (relational_operator)
//  postcond: next token
//
//  [ 7.1 ]
//     relational_operator shift_expression
Iir_Binary_Operator *Parser::Parse_Relation_Rhs(Iir *Left) {
    auto result = new Iir_Binary_Operator;
    switch (scanner.currentContext.token) {
        case Token::Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Equality;
            break;
        case Token::Not_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Inequality;
            break;
        case Token::Less:
            result->Operator_Type = Iir_Binary_Operator_Type::Less_Than;
            break;
        case Token::Less_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Less_Than_Or_Equal;
            break;
        case Token::Greater:
            result->Operator_Type = Iir_Binary_Operator_Type::Greater_Than;
            break;
        case Token::Greater_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Greater_Than_Or_Equal;
            break;
        case Token::Match_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Equality;
            break;
        case Token::Match_Not_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Inequality;
            break;
        case Token::Match_Less:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Less_Than;
            break;
        case Token::Match_Less_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Less_Than_Or_Equal;
            break;
        case Token::Match_Greater:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Greater_Than;
            break;
        case Token::Match_Greater_Equal:
            result->Operator_Type = Iir_Binary_Operator_Type::Match_Greater_Than_Or_Equal;
            break;
        default:
            throw std::logic_error("");
    }
    result->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    result->Left = Left;
    result->Right = Parse_Shift_Expression();
    //  This loop is just to handle errors such as a = b = c.
    if (Token::isRelationalOperator(scanner.currentContext.token))
        throw std::runtime_error("use parenthesis for consecutive relational expressions");
    return result;
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  relation ::= shift_expression [ relational_operator shift_expression ]
//
//  [ 7.2 ]
//  relational_operator ::= = | != | < | <= | > | >= | ?= | ?!= | ?< | ?<= | ?> | ?>=

Iir* Parser::Parse_Relation() {
    auto Tmp = Parse_Shift_Expression();
    if (Token::isRelationalOperator(scanner.currentContext.token))
        return Tmp;
    else
        return Parse_Relation_Rhs(Tmp);
}

//  precond : next token
//  postcond: next token
//
//  [ 7.1 ]
//  expression ::= relation { AND relation }
//               | relation { OR relation }
//               | relation { XOR relation }
//               | relation [ NAND relation ]
//               | relation [ NOR relation ]
//               | relation { XNOR relation }
Iir *Parser::Parse_Expression_Rhs(Iir *Left) {
    //  OP_TOKEN contains the operator combination.
    auto Tmp = Left;
    auto Op_Token = Token::Invalid;
    while (1) {
        auto Res = new Iir_Binary_Operator;
        switch (scanner.currentContext.token) {
            case Token::And:
                Res->Operator_Type = Iir_Binary_Operator_Type::And;
                break;
            case Token::Or:
                Res->Operator_Type = Iir_Binary_Operator_Type::Or;
                break;
            case Token::Xor:
                Res->Operator_Type = Iir_Binary_Operator_Type::Xor;
                break;
            case Token::Nand:
                Res->Operator_Type = Iir_Binary_Operator_Type::Nand;
                break;
            case Token::Nor:
                Res->Operator_Type = Iir_Binary_Operator_Type::Nor;
                break;
            case Token::Xnor:
                if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                    throw std::runtime_error("PARSE ERROR: 'xnor' keyword not allowed in vhdl 87");

                Res->Operator_Type = Iir_Binary_Operator_Type::Xnor;
            default:
                return Tmp;
        }

        if (Op_Token == Token::Invalid)
            Op_Token = scanner.currentContext.token;
        else {
//  Check after the case, since current_token may not be an
//  operator...
//  TODO: avoid repetition of this message ?
            if (Op_Token == Token::Nand || Op_Token == Token::Nor) {
                throw std::runtime_error(
                        "sequence of 'nor' or 'nand' not allowed ('nor' and 'nand' are not associative)");
            }
            if (Op_Token != scanner.currentContext.token) {
//  Expression is a sequence of relations, with the same operator.
                throw std::runtime_error(
                        "PARSE ERROR: only one type of logical operators may be used to combine relation");
            }
        }
        Res->Location = scanner.currentContext.getTokenLocation();
        scanner.scan();

//  Catch errors for Ada programmers.
        if (scanner.currentContext.token == Token::Then || scanner.currentContext.token == Token::Else) {
            throw std::runtime_error(
                    "PARSE ERROR: \"||\" and \"&&\" sequences are not allowed in vhdl, \"and\" and \"or\" are short-circuit operators for BIT and BOOLEAN types");
        }

        Res->Left = Tmp;
        Res->Right = Parse_Relation();
        Tmp = Res;
    }
}

//  precond : next token
//  postcond: next token
//
//  LRM08 9.1 General
//  expression ::= condition_operator primary |  logical_expression

Iir *Parser::Parse_Expression() {
    if (scanner.currentContext.token == Token::Condition) {
        auto Res = new Iir_Unary_Operator;
        Res->Operator_Type = Iir_Unary_Operator_Type::Condition;
        Res->Location = scanner.currentContext.getTokenLocation();
        //  Skip '??'
        scanner.scan();
        Res->Operand = Parse_Primary();
        return Res;
    } else
        return Parse_Expression_Rhs(Parse_Relation());

}
//  precond : next token
//  postcond: next token.
//
//  [ 8.4 ]
//  waveform ::= waveform_element { , waveform_element } | UNAFFECTED
//
//  [ 8.4.1 ]
//  waveform_element ::= VALUE_expression [ AFTER TIME_expression ] | NULL [ AFTER TIME_expression ]

Iir_Waveform Parser::Parse_Waveform() {
    if (scanner.currentContext.token == Token::Unaffected) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'unaffected' is not allowed in vhdl87");

        auto Res = new Iir_Unaffected_Waveform;
        Res->Location = scanner.currentContext.getTokenLocation();

        //  Skip 'unaffected'.
        scanner.scan();
        return {};
    } else {
        std::vector<Iir_Waveform_Element *> list;
        do {
            auto We = new Iir_Waveform_Element;
            We->Location = scanner.currentContext.getTokenLocation();

//  Note: NULL is handled as a null_literal.
            We->We_Value = Parse_Expression();

            if (scanner.currentContext.token == Token::After) {
//  Skip 'after'.
                scanner.scan();

                We->Time = Parse_Expression();
            }
            list.push_back(We);
        } while (scanner.currentContext.token != Token::Comma);

//  Skip ','.
        scanner.scan();
        return list;
    }
}


//  precond : next token
//  postcond: next token
//
//  [ 8.4 ]
//  delay_mechanism ::= TRANSPORT | [ REJECT TIME_expression ] INERTIAL
void Parser::Parse_Delay_Mechanism(Iir_Guarded_Target_State_Abs* Assign) {
    if (scanner.currentContext.token == Token::Transport) {
        Assign->Delay_Mechanism = Iir_Transport_Delay;
        scanner.scan();
    } else {
        Assign->Delay_Mechanism = Iir_Inertial_Delay;
        if (scanner.currentContext.token == Token::Reject) {
            if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                throw std::runtime_error("'reject' delay mechanism not allowed in vhdl 87");
            scanner.scan();
            Assign->Reject_Time_Expression = Parse_Expression();
            Expect(Token::Inertial);
            scanner.scan();
        } else if (scanner.currentContext.token == Token::Inertial) {
            if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                throw std::runtime_error("'inertial' keyword not allowed in vhdl 87");
            scanner.scan();
        }
    }
}

//  precond : next token
//  postcond: next token
//
//  [ LRM93 9.5 ]
//  options ::= [ GUARDED ] [ delay_mechanism ]
void Parser::Parse_Options(Iir *Stmt) {
    if (scanner.currentContext.token == Token::Guarded) {
        Stmt->Guard = Stmt;
        scanner.scan();
    }
    Parse_Delay_Mechanism(Stmt);
}

//  precond : next tkoen
//  postcond: next token (';')
//
//  [ LRM93 9.5.1 ]
//  conditional_waveforms ::= { waveform WHEN condition ELSE }  waveform [ WHEN condition ]
std::variant<Iir_Conditional_Waveforms, Iir_Waveform> Parser::Parse_Conditional_Waveforms() {
    auto Wf = Parse_Waveform();
    if (scanner.currentContext.token != Token::When) {
        return Wf;
    }
    else {
        std::vector<Iir_Conditional_Waveform*> C_Wf_List;
        auto Cond_Wf = new Iir_Conditional_Waveform;
        Cond_Wf->Location = scanner.currentContext.getTokenLocation();
        Cond_Wf->waveform_elements = Wf;
        //NOTE: Check this logic
        while(1) {
            //  Eat 'when'
            scanner.scan();

            Cond_Wf->Condition = Parse_Expression();

            if (scanner.currentContext.token != Token::Else) {
                if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                    throw std::runtime_error("PARSE ERROR: else missing in vhdl 87");
                break;
            }
            C_Wf_List.push_back(Cond_Wf);
            Cond_Wf = new Iir_Conditional_Waveform;
            Cond_Wf->Location = scanner.currentContext.getTokenLocation();

            //  Eat 'else'
            scanner.scan();
            Cond_Wf->waveform_elements = Parse_Waveform();
            assert(scanner.currentContext.token == Token::When);
        }
        return C_Wf_List;
    }
}

//  precond : '<=' (or ':=')
//  postcond: ';'
//
//  [ LRM93 9.5.1 ]
//  concurrent_conditional_signal_assignment ::= target <= [ GUARDED ] [ delay_mechanism ] conditional_waveforms ;
//
//  [ LRM08 10.5.2.1 ]
//  concurrent_simple_waveform_assignment ::= target <= [ GUARDED ] [ delay_mechanism ] waveform ;
std::variant<Iir_Concurrent_Simple_Signal_Assignment*, Iir_Concurrent_Conditional_Signal_Assignment*> Parser::Parse_Concurrent_Conditional_Signal_Assignment(Iir* Target) {
    auto Loc = scanner.currentContext.getTokenLocation();
    switch (scanner.currentContext.token) {
        case Token::Less_Equal:
            break;
        case Token::Assign:
            throw std::runtime_error("PARSE ERROR: ':=' not allowed in concurrent statement, replaced by '<='");
        default:
            Expect(Token::Less_Equal);
    }

//  Eat '<='.
    scanner.scan();

//  Assume simple signal assignment.
    auto Res = new Iir_Concurrent_Simple_Signal_Assignment;
    Parse_Options(Res);

    auto Wf = Parse_Conditional_Waveforms();
    if(auto Waveform = std::get_if<Iir_Conditional_Waveforms>(Wf)) {
        auto Res_ = new Iir_Concurrent_Conditional_Signal_Assignment;
        Res_->Guard = Res->Guard;
        Res_->Delay_Mechanism = Res->Delay_Mechanism;
        Res_->Reject_Time_Expression = Res->Reject_Time_Expression;
        delete Res;
        Res_->Location = Loc;
        Res_->Target = Target;
        Expect(Token::Semi_Colon);
        return Res_;
    }
    else {
        Res->waveform = Waveform;
        Res->Location = Loc;
        Res->Target = Target;
        Expect(Token::Semi_Colon);
        return Res;
    }
}

//  precond : WITH
//  postcond: ';'
//
//  [ 9.5.2 ]
//  selected_signal_assignment ::=  WITH expresion SELECT target <= options selected_waveforms ;
//
//  [ 9.5.2 ]
//  selected_waveforms ::=  { waveform WHEN choices , }  waveform WHEN choices
Iir *Parser::Parse_Selected_Signal_Assignment() {
//use Iir_Chains.Selected_Waveform_Chain_Handling;
//Res: Iir;
//Assoc: Iir;
//Wf_Chain : Iir_Waveform_Element;
//Target : Iir;
//Last : Iir;
//begin
    scanner.scan();  // accept 'with' token.
    auto Res = new Iir_Concurrent_Selected_Signal_Assignment;
    Res->Location = scanner.currentContext.getTokenLocation();
    Res->Expression = Parse_Expression();

    Expect(Token::Select, "'select' expected after expression");
    scanner.scan();
    Iir *Target;
    if (scanner.currentContext.token == Token::Left_Paren) {
        Res->Target = Parse_Aggregate();
    } else {
        Res->Target = Parse_Name(/*Allow_Indexes:*/ true);
    }
    Expect(Token::Less_Equal);
    scanner.scan();

Parse_Options (Res);

    while (1) {
        auto Wf_Chain = Parse_Waveform();
        Expect(Token::When, "'when' expected after waveform");
        scanner.scan();
        auto Assoc = Parse_Choices(nullptr);
        Set_Associated_Chain(Assoc, Wf_Chain);
        Append_Subchain(Last, Res, Assoc);
        if (scanner.currentContext.token == Token::Semi_Colon)
            break;
        Expect(Token::Comma, "',' (comma) expected after choice");
        scanner.scan();
    }
return Res;
}

//  precond : next token
//  postcond: next token.
//
//  [ 8.1 ]
//  sensitivity_list ::= SIGNAL_name { , SIGNAL_name }
Iir* Parser::Parse_Sensitivity_List(std::vector<Iir*>* List) {
    while (1) {
        auto El = Parse_Name(true);
        switch (Get_Kind(El)) {
            case Iir_Simple_Name:
            case Iir_Parenthesis_Name:
            case Iir_Selected_Name:
            case Iir_Slice_Name:
            case Iir_Attribute_Name:
            case Iir_Selected_By_All_Name:
            case Iir_Indexed_Name:
                break;
            default:
                throw std::runtime_error("PARSE ERROR: only names are allowed in a sensitivity list");
        }
        List->push_back(El);
        if (scanner.currentContext.token != Token::Comma)
            break;
        scanner.scan();
    }
}

//  precond : ASSERT
//  postcond: next token
//  Note: this fill an sequential or a concurrent statement.
//
//  [ 8.2 ]
//  assertion ::= ASSERT condition [ REPORT expression ] [ SEVERITY expression ]
void Parser::Parse_Assertion(Iir *Stmt) {
    Stmt->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    Stmt->Assertion_Condition = Parse_Expression();
    if (scanner.currentContext.token == Token::Report) {
        scanner.scan();
        Stmt->Report_Expression = Parse_Expression();
    }
    if (scanner.currentContext.token == Token::Severity) {
        scanner.scan();
        Stmt->Severity_Expression = Parse_Expression();
        if (scanner.currentContext.token == Token::Report)
            //  Nice message in case of inversion.
            throw std::runtime_error("report expression must precede severity expression");
    }
}

//  precond : REPORT
//  postcond: next token
//
//  [ 8.3 ]
//  report_statement ::= REPORT expression [ SEVERITY expression ]
Iir_Report_Statement* Parser::Parse_Report_Statement() {
    auto Res = new Iir_Report_Statement;
    Res->Location = scanner.currentContext.getTokenLocation();
    if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
        throw std::runtime_error("PARSE ERROR: report statement not allowed in vhdl87");

    scanner.scan();
    Res->Report_Expression = Parse_Expression();
    if (scanner.currentContext.token == Token::Severity)
        scanner.scan();
    Res->Severity_Expression = Parse_Expression();

    return Res;
}

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
Iir_Wait_Statement* Parser::Parse_Wait_Statement() {
    auto Res = new Iir_Wait_Statement;
    Res->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    switch (scanner.currentContext.token) {
        case Token::On:
            scanner.scan();
            Parse_Sensitivity_List(&Res->Sensitivity_List);
        case Token::Until:
        case Token::For:
            break;
        case Token::Semi_Colon:
            return Res;
        default:
            throw std::runtime_error("PARSE ERROR: 'on', 'until', 'for' or ';' expected");
            Eat_Tokens_Until_Semi_Colon();
            return Res;
    }
    switch (scanner.currentContext.token) {
        case Token::On:
            throw std::runtime_error("PARSE ERROR: only one sensitivity is allowed");
// FIXME: sync
            return Res;
        case Token::Until:
            scanner.scan();
            Res->Condition_Clause = Parse_Expression();
        case Token::For:
            break;
        case Token::Semi_Colon:
            return Res;
        default:
            throw std::runtime_error("PARSE ERROR: 'until', 'for' or ';' expected");
            Eat_Tokens_Until_Semi_Colon();
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
            Res->Timeout_Clause = Parse_Expression();
            return Res;
        case Token::Semi_Colon:
            return Res;
        default:
            throw std::runtime_error("PARSE ERROR: 'for' or ';' expected");
            Eat_Tokens_Until_Semi_Colon;
            return Res;
    }
}

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
Iir_If_Statement* Parser::Parse_If_Statement (Iir* Parent) {
    auto Res = new Iir_If_Statement;
    Res->Location = scanner.currentContext.getTokenLocation();
    Res->Parent = Parent;
    scanner.scan();
    auto Clause = Res;
    while (1) {
        Clause->Condition = Parse_Expression();
        Expect(Token::Then, "'then' is expected here");

//  Skip 'then'.
        scanner.scan();

        Clause->Sequential_Statement_Chain  = Parse_Sequential_Statements(Res);
        if (scanner.currentContext.token == Token::End)
            break;
        auto N_Clause = new Iir_Elsif;
        N_Clause->Location = scanner.currentContext.getTokenLocation();
        Clause->Else_Clause = N_Clause;
        Clause = N_Clause;
        if (scanner.currentContext.token == Token::Else) {

//  Skip 'else'.
            scanner.scan();

            Clause->Sequential_Statement_Chain = Parse_Sequential_Statements(Res);
            break;
        } else if (scanner.currentContext.token == Token::Elsif)
//  Skip 'elsif'.
            scanner.scan();
        else
            throw std::runtime_error("PARSE ERROR: 'else' or 'elsif' expected");

    }
    Expect(Token::End);
    Scan_Expect(Token::If);
    scanner.scan();
    return Res;
}
/* TODO:FIXME
Iir* Parser::Parenthesis_Name_To_Procedure_Call (Iir* Name,Iir_Kind Kind ) {
//return Iir
//        is
//Res: Iir;
//Call : Iir_Procedure_Call;
//begin
        auto Res = new Kind;
Location_Copy (Res, Name);
Call = Create_Iir (Iir_Procedure_Call);
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
*/
//  precond : identifier
//  postcond: next token
//
//  [ LRM93 8.9 ]
//  parameter_specification ::= identifier IN discrete_range
Iir_Iterator_Declaration* Parser::Parse_Parameter_Specification (Iir* Parent) {
    auto Decl = new Iir_Iterator_Declaration;
    Decl->Location = scanner.currentContext.getTokenLocation();
    Decl->Parent = Parent;

    Expect(Token::Identifier);
    Decl->Identifier = scanner.currentContext.Identifier;

//  Skip identifier
    Scan_Expect(Token::In);

//  Skip 'in'
    scanner.scan();

    Decl->Discrete_Range = Parse_Discrete_Range();
    return Decl;
}

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
Iir* Parser::Parse_Signal_Assignment_Statement (Iir* Target) {
//        is
//Stmt : Iir;
//N_Stmt : Iir;
//Wave_Chain : Iir;
//begin
    auto Stmt = new Iir_Simple_Signal_Assignment_Statement;
    Stmt->Location = scanner.currentContext.getTokenLocation();
    Set_Target(Stmt, Target);

//  Eat '<='.
    scanner.scan();

    Parse_Delay_Mechanism(Stmt);

    auto Wave_Chain = Parse_Conditional_Waveforms;

//  LRM 8.4 Signal assignment statement
//  It is an error is the reserved word UNAFFECTED appears as a
//  waveform in a (sequential) signal assignment statement.
    if (Get_Kind(Wave_Chain) = Iir_Unaffected_Waveform) {
        if (scanner.state.options.standard <= Vhdl_Std::Vhdl_08)
            throw std::runtime_error("PARSE ERROR: 'unaffected' is not allowed in a sequential statement");
        Set_Waveform_Chain(Stmt, Wave_Chain);
    } else if (Get_Kind(Wave_Chain) = Iir_Conditional_Waveform) {
                if (scanner.state.options.standard <= Vhdl_Std::Vhdl_08)
                throw std::runtime_error("PARSE ERROR: conditional signal assignment not allowed in before vhdl08");

                N_Stmt = Create_Iir (Iir_Conditional_Signal_Assignment_Statement);
                N_Stmt->Location = Stmt->Location;
                Set_Target (N_Stmt, Target);
                Set_Delay_Mechanism (N_Stmt, Get_Delay_Mechanism (Stmt));
                Set_Reject_Time_Expression (N_Stmt, Get_Reject_Time_Expression (Stmt));
                Set_Conditional_Waveform_Chain (N_Stmt, Wave_Chain);
                Free_Iir (Stmt);
                Stmt = N_Stmt;
        }
    else
        Set_Waveform_Chain(Stmt, Wave_Chain);

    return Stmt;
}

//  precond:  WHEN
//  postcond: next token
//
//  [ LRM08 10.5.3 Conditional signal assignments ]
//  conditional_expressions ::=
//      expression WHEN condition
//    { ELSE expression WHEN condition }
//    [ ELSE expression ]
Iir* Parser::Parse_Conditional_Expression (Iir* Expr) {

    auto Res = new Iir_Conditional_Expression;
    Res->Location = scanner.currentContext.getTokenLocation();
    Res->Expression = Expr;
    auto El = Res;

    while (1) {
//  Eat 'when'
        scanner.scan();

        El->Condition = Parse_Expression();

        if (scanner.currentContext.token != Token::Else)
            break;

        auto N_El = Create_Iir(Iir_Conditional_Expression);
        N_El->Location = scanner.currentContext.getTokenLocation();
        El->Chain = N_El;
        El = N_El;

//  Eat 'else'
        scanner.scan();

        N_El->Expression = Parse_Expression();

        if (scanner.currentContext.token != Token::When)
            break;
    }

    return Res;
}

//  precond:  ':='
//  postcond: next token
//
//  [ LRM93 8.5 ]
//  variable_assignment_statement ::= [ label : ] target = expression ;
//
Iir* Parser::Parse_Variable_Assignment_Statement (Iir* Target) {
    auto Loc = scanner.currentContext.getTokenLocation();

//  Eat ':='
    scanner.scan();

    auto Expr = Parse_Expression();

    if (scanner.currentContext.token == Token::When) {
        if (scanner.state.options.standard <= Vhdl_Std::Vhdl_08)
            throw std::runtime_error("PARSE ERROR: conditional variable assignment not allowed before vhdl08");

        auto Stmt = new Iir_Conditional_Variable_Assignment_Statement;
        Stmt->Location = Loc;
        Stmt->Target = Target;
        Stmt->Conditional_Expression = Parse_Conditional_Expression(Expr);
        return Stmt;
    } else {
        auto Stmt = new Iir_Variable_Assignment_Statement;
        Stmt->Location = Loc;
        Stmt->Target = Target;
        Stmt->Expression = Expr;
        return Stmt;
    }
}

//  precond:  '<=', ':=' or ';'
//  postcond: next token
Iir* Parser::Parse_Sequential_Assignment_Statement (Iir* Target) {
    if (scanner.currentContext.token == Token::Less_Equal)
        return Parse_Signal_Assignment_Statement(Target);
    else if (scanner.currentContext.token == Token::Assign)
        return Parse_Variable_Assignment_Statement(Target);
    else if (scanner.currentContext.token == Token::Semi_Colon)
        return Parenthesis_Name_To_Procedure_Call(Target, Iir_Procedure_Call_Statement);
    else
        throw std::runtime_error("PARSE ERROR: ""<="" or "":="" expected instead of %t" +scanner.currentContext.token);
}


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
Iir_Case_Statement* Parser::Parse_Case_Statement (std::string Label ) {
    auto Stmt = new Iir_Case_Statement;
    Stmt->Label = Label;
    Stmt->Location = scanner.currentContext.getTokenLocation();

//  Skip 'case'.
    scanner.scan();

    Stmt->Expression = Parse_Expression();

//  Skip 'is'.
    Expect(Token::Is);
    scanner.scan();
    if (scanner.currentContext.token == Token::End)
        throw std::runtime_error("PARSE ERROR: missing alternative in case statement");

    while (scanner.currentContext.token != Token::End) {
//  Eat 'when'
        Expect(Token::When);
        scanner.scan();

        if (scanner.currentContext.token == Token::Double_Arrow)
            throw std::runtime_error("PARSE ERROR: missing expression in alternative");

        auto Assoc = Parse_Choices(nullptr);

//  Eat '=>'
        Expect(Token::Double_Arrow);
        scanner.scan();

        Assoc->Associated_Chain = Parse_Sequential_Statements(Stmt);
        Stmt->Case_Statement_Alternative.push_back(Assoc);
    }

//  Eat 'end', 'case'
    Scan_Expect(Token::Case);
    scanner.scan();

    if (scanner.state.options.standard >= Vhdl_Std::Vhdl_93c)
        Check_End_Name(Stmt);


    return Stmt;
}

//  precond:  next token
//  postcond: next token
//
//  [ LRM93 8 ]
//  sequence_of_statement ::= { sequential_statement }
//
//  [ 8 ]
//  sequential_statement ::= wait_statement
//                         | assertion_statement
//                         | report_statement
//                         | signal_assignment_statement
//                         | variable_assignment_statement
//                         | procedure_call_statement
//                         | if_statement
//                         | case_statement
//                         | loop_statement
//                         | next_statement
//                         | exit_statement
//                         | return_statement
//                         | null_statement
//
//  [ 8.13 ]
//  null_statement ::= [ label : ] NULL ;
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
Iir* Parser::Parse_Sequential_Statements (Iir* Parent) {
//First_Stmt : Iir;
//Last_Stmt : Iir;
//Stmt: Iir;
//Label: Name_Id;
//Loc : Location_Type;
//Target : Iir;
    Iir* First_Stmt = nullptr;
    Iir* Last_Stmt = nullptr;
// Expect a current_token.
    while (1) {
        auto Loc = scanner.currentContext.getTokenLocation();
        if (scanner.currentContext.token == Token::Identifier) {
            Label = scanner.currentContext.Identifier;
            scanner.scan();
            if (scanner.currentContext.token == Token::Colon)
                scanner.scan();
            else {
                Target = Create_Iir(Iir_Simple_Name);
                Set_Identifier(Target, Label);
                Set_Location(Target, Loc);
                Label = Null_Identifier;
                Target = Parse_Name_Suffix(Target, true);
                Stmt = Parse_Sequential_Assignment_Statement(Target);
                goto Has_Stmt;
            }
        } else
            Label = Null_Identifier;


        switch (scanner.currentContext.token) {
            case Token::Null:
                Stmt = Create_Iir(Iir_Null_Statement);

//  Skip 'null'.
                scanner.scan();
                break;
            case Token::Assert:
                Stmt = Create_Iir(Iir_Assertion_Statement);
                Parse_Assertion(Stmt);
                break;
            case Token::Report:
                Stmt = Parse_Report_Statement();
                break;
            case Token::If:
                Stmt = Parse_If_Statement(Parent);
                Set_Label(Stmt, Label);
                Set_Location(Stmt, Loc);
                if (scanner.state.options.standard >= Vhdl_Std::Vhdl_93c)
                    Check_End_Name(Stmt);
                break;
            case Token::Case:
                Stmt = Parse_Case_Statement(Label);
                break;
            case Token::Identifier:
            case Token::String:
//  String for an expanded name with operator_symbol prefix.
                Stmt = Parse_Sequential_Assignment_Statement(Parse_Name);
                break;
            case Token::Left_Paren:
                auto Target = Parse_Aggregate();
                if (scanner.currentContext.token == Token::Less_Equal)
                    Stmt = Parse_Signal_Assignment_Statement(Target);
                else if (scanner.currentContext.token == Token::Assign)
                    Stmt = Parse_Variable_Assignment_Statement(Target);
                else
                    throw std::runtime_error("PARSE ERROR: '<=' or ':=' expected");
                return First_Stmt;

            case Token::Return:
                Stmt = Create_Iir(Iir_Return_Statement);
                scanner.scan();
                if (scanner.currentContext.token != Token::Semi_Colon)
                    Set_Expression(Stmt, Parse_Expression);
                break;

            case Token::For:
                Stmt = Create_Iir(Iir_For_Loop_Statement);
                Set_Location(Stmt, Loc);
                Set_Label(Stmt, Label);

//  Skip 'for'
                scanner.scan();

                Set_Parameter_Specification(Stmt, Parse_Parameter_Specification(Stmt));

//  Skip 'loop'
                Expect(Token::Loop);
                scanner.scan();

                Set_Sequential_Statement_Chain(Stmt, Parse_Sequential_Statements(Stmt));

//  Skip 'end'
                Expect(Token::End);
                Scan_Expect(Token::Loop);

//  Skip 'loop'
                scanner.scan();

                Check_End_Name(Stmt);
//  A loop statement can have a label, even in vhdl87.
                Label = Null_Identifier;
                break;
            case Token::While:
            case Token::Loop:
                Stmt = Create_Iir(Iir_While_Loop_Statement);
                Set_Location(Stmt);
                Set_Label(Stmt, Label);
                if (scanner.currentContext.token == Token::While) {
                    scanner.scan();
                    Set_Condition(Stmt, Parse_Expression);
                    Expect(Token::Loop);
                }
                scanner.scan();
                Set_Sequential_Statement_Chain(Stmt, Parse_Sequential_Statements(Stmt));
                Expect(Token::End);
                Scan_Expect(Token::Loop);
                scanner.scan();
                Check_End_Name(Stmt);
//  A loop statement can have a label, even in vhdl87.
                Label = Null_Identifier;
                break;
            case Token::Next:
            case Token::Exit:
                if (scanner.currentContext.token == Token::Next)
                    Stmt = Create_Iir(Iir_Next_Statement);
                else
                    Stmt = Create_Iir(Iir_Exit_Statement);


//  Skip 'next' or 'exit'.
                scanner.scan();

                if (scanner.currentContext.token == Token::Identifier)
                    Set_Loop_Label(Stmt, Parse_Name(false));


                if (scanner.currentContext.token == Token::When) {
//  Skip 'when'.
                    scanner.scan();

                    Set_Condition(Stmt, Parse_Expression);
                }
                break;
            case Token::Wait:
                Stmt = Parse_Wait_Statement();
                break;
            default:
                return First_Stmt;
        }
        << Has_Stmt >> null;
        Set_Parent(Stmt, Parent);
        Set_Location(Stmt, Loc);
        if (!Label.empty()) {
            if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                Error_Msg_Sem(+Stmt, "this statement can't have a label in vhdl 87");
            else
                Set_Label(Stmt, Label);

        }
        Scan_Semi_Colon("statement");

//  Append it to the chain.
        if (!First_Stmt)
            First_Stmt = Stmt;
        else
            Set_Chain(Last_Stmt, Stmt);

        Last_Stmt = Stmt;
    }
}
/*
 * NOTE:  THIS IS DEPRECATED
 */
/*
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
//
//  [ 2.1 ]
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
Kind = Iir_Procedure_Declaration;
case Token::Function
| Token::Pure
| Token::Impure:
Kind = Iir_Function_Declaration;
default:
throw std::logic_error();
}
Subprg = Create_Iir (Kind);
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
Set_Pure_Flag (Subprg, scanner.currentContext.token == Token::Pure);
if (scanner.state.options.standard == Vhdl_Std::Vhdl_87) {
        throw std::runtime_error("PARSE ERROR: 'pure' and 'impure' are not allowed in vhdl 87");
end if;
Set_Has_Pure (Subprg, true);
//  FIXME: what to do in case of error ??

//  Eat 'pure' or 'impure'.
scanner.scan();

Expect (Token::Function, "'function' must follow 'pure' or 'impure'");
default:
throw std::logic_error();
}

//  Eat 'procedure' or 'function'.
scanner.scan();

//  Designator.
Parse_Subprogram_Designator (Subprg);

Parse_Subprogram_Parameters_And_Return
(Subprg, Kind = Iir_Function_Declaration);

if (scanner.currentContext.token == Token::Semi_Colon {
return Subprg;
end if;

//  The body.
Set_Has_Body (Subprg, true);
if Kind = Iir_Function_Declaration {
        Subprg_Body = Create_Iir (Iir_Function_Body);
else
Subprg_Body = Create_Iir (Iir_Procedure_Body);
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
if (scanner.state.options.standard == Vhdl_Std::Vhdl_87) {
        throw std::runtime_error("PARSE ERROR: 'function' not allowed here by vhdl 87");
end if;
if Kind = Iir_Procedure_Declaration {
        throw std::runtime_error("PARSE ERROR: 'procedure' expected instead of 'function'");
end if;
Set_End_Has_Reserved_Id (Subprg_Body, true);

//  Skip 'function'.
scanner.scan();

case Token::Procedure:
if (scanner.state.options.standard == Vhdl_Std::Vhdl_87) {
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
*/
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
Iir* Parser::Parse_Process_Statement(std::string Label, Location_Type Loc, bool Is_Postponed) {
//Res: Iir;
//Sensitivity_List : Iir_List;

//  Skip 'process'
    scanner.scan();

    if (scanner.currentContext.token == Token::Left_Paren) {
        Res = new Iir_Sensitized_Process_Statement;

//  Skip '('
        scanner.scan();

        if (scanner.currentContext.token == Token::All) {
            if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                throw std::runtime_error("PARSE ERROR: all sensitized process allowed only in vhdl 08");

            Sensitivity_List = Iir_List_All;

//  Skip 'all'
            scanner.scan();
        } else {
            Sensitivity_List = Create_Iir_List;
            Parse_Sensitivity_List(Sensitivity_List);
        }
        Set_Sensitivity_List(Res, Sensitivity_List);

//  Skip ')'
        Expect(Token::Right_Paren);
        scanner.scan();
    } else
        Res = Create_Iir(Iir_Process_Statement);


    Set_Location(Res, Loc);
    Set_Label(Res, Label);
    Set_Has_Label(Res, Label != Null_Identifier);

    if (scanner.currentContext.token == Token::Is) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: ""is"" not allowed here by vhdl 87");

        Set_Has_Is(Res, true);

//  Skip 'is'
        scanner.scan();
    }

// declarative part.
    Parse_Declarative_Part(Res);

//  Skip 'begin'.
    Expect(Token::Begin);
    scanner.scan();

    Set_Sequential_Statement_Chain(Res, Parse_Sequential_Statements(Res));

//  Skip 'end'.
    Expect(Token::End);
    scanner.scan();

    if (scanner.currentContext.token == Token::Postponed) {
        if (!Is_Postponed)
//  LRM93 9.2
//  If the reserved word POSTPONED appears at the end of a process
//  statement, the process must be a postponed process.
            throw std::runtime_error("PARSE ERROR: process is not a postponed process");


        Set_End_Has_Postponed(Res, true);

//  Skip 'postponed',
        scanner.scan();
    }

    if (scanner.currentContext.token == Token::Semi_Colon)
        throw std::runtime_error("PARSE ERROR: ""end"" must be followed by ""process""");
    else {
        Expect(Token::Process);
        scanner.scan();
        Set_End_Has_Reserved_Id(Res, true);
        Check_End_Name(Res);
        Expect(Token::Semi_Colon);
    }

    return Res;
}

Iir* Parser::Check_Formal_Form (Iir* Formal) {
    if (!Formal)
        return Formal;


    switch (Get_Kind(Formal)) {
        case Iir_Simple_Name:
        case Iir_Slice_Name:
        case Iir_Selected_Name:
            return Formal;
        case Iir_Parenthesis_Name:
//  Could be an indexed name, so nothing to check within the
//  parenthesis.
            return Formal;
        case Iir_String:
//  Operator designator
            return String_To_Operator_Symbol(Formal);
        default:
            throw std::runtime_error("PARSE ERROR:" + Formal + "incorrect formal name ignored");
    }
}

// precond : NEXT_TOKEN
// postcond: NEXT_TOKEN
//
//  [ LRM93 4.3.2.2 ]
//  association_list ::= association_element { , association_element }
//
//  [ LRM93 4.3.2.2 ]
//  association_element ::= [ formal_part: ] actual_part
//
//  [ LRM93 4.3.2.2 ]
//  actual_part ::= actual_designator
//                | FUNCTION_name ( actual_designator )
//                | type_mark ( actual_designator )
//
//  [ LRM93 4.3.2.2 ]
//  actual_designator ::= expression
//                      | SIGNAL_name
//                      | VARIABLE_name
//                      | FILE_name
//                      | OPEN
//
//  [ LRM93 4.3.2.2 ]
//  formal_part ::= formal_designator
//                | FUNCTION_name ( formal_designator )
//                | type_mark ( formal_designator )
//
//  [ LRM93 4.3.2.2 ]
//  formal_designator ::= GENERIC_name
//                      | PORT_name
//                      | PARAMETER_name
//
//  Note: an actual part is parsed as an expression.
Iir* Parser::Parse_Association_List() {
//Res, Last: Iir;
//El: Iir;
//Formal: Iir;
//Actual: Iir;
//Nbr_Assocs : Natural;
//Loc : Location_Type;
    Sub_Chain_Init(Res, Last);

    if (scanner.currentContext.token == Token::Right_Paren)
        throw std::runtime_error("PARSE ERROR: empty association list is not allowed");

    Nbr_Assocs = 1;
    while (1) {
//  Parse formal and actual.
        auto Loc = scanner.currentContext.getTokenLocation();
        Formal = nullptr;

        if (scanner.currentContext.token != Token::Open) {
            Actual = Parse_Expression();
            switch (scanner.currentContext.token) {
                case Token::To:
                case Token::Downto:
//  To/downto can appear in slice name (which are parsed as
//  function call).

                    if (!Actual) {
//  Left expression is missing ie: (downto x).
                        scanner.scan();
                        Actual = Parse_Expression();
                    } else
                        Actual = Parse_Range_Expression(Actual);
                    if (Nbr_Assocs != 1)
                        throw std::runtime_error("PARSE ERROR: multi-dimensional slice is forbidden");
                    break;
                case Token::Double_Arrow:
//  Check that FORMAL is a name and not an expression.
                    Formal = Check_Formal_Form(Actual);

//  Skip '=>'
                    scanner.scan();
                    Loc = scanner.currentContext.getTokenLocation();

                    if (scanner.currentContext.token != Token::Open)
                        Actual = Parse_Expression();
                    break;
//default:
//null;
            }
        }

        if (scanner.currentContext.token == Token::Open) {
            El = Create_Iir(Iir_Association_Element_Open);
            Set_Location(El);

//  Skip 'open'
            scanner.scan();
        } else {
            El = Create_Iir(Iir_Association_Element_By_Expression);
            Set_Location(El, Loc);
            Set_Actual(El, Actual);
        }
        Set_Formal(El, Formal);

        Sub_Chain_Append(Res, Last, El);
        if (scanner.currentContext.token == Token::Right_Paren)
            break;
        Expect(Token::Comma);
        scanner.scan();
        Nbr_Assocs = Nbr_Assocs + 1;
    }

    return Res;
}

// precond : NEXT_TOKEN
// postcond: NEXT_TOKEN
//
// Parse: '(' association_list ')'
Iir* Parser::Parse_Association_List_In_Parenthesis() {

//Res : Iir;
//  Skip '('
    Expect(Token::Left_Paren);
    scanner.scan();

    auto Res = Parse_Association_List();

//  Skip ')'
    scanner.scan();

    return Res;
}

//  precond : GENERIC
//  postcond: next token
//
//  [ LRM93 5.2.1.2, LRM08 6.5.7.2 ]
//  generic_map_aspect ::= GENERIC MAP ( GENERIC_association_list )
Iir* Parser::Parse_Generic_Map_Aspect() {
    Expect(Token::Generic);

//  Skip 'generic'.
    Scan_Expect(Token::Map);

//  Skip 'map'.
    scanner.scan();

    return Parse_Association_List_In_Parenthesis();
}

//  precond : PORT
//  postcond: next token
//
//  [ 5.2.1.2 ]
//  port_map_aspect ::= PORT MAP ( PORT_association_list )
Iir* Parser::Parse_Port_Map_Aspect() {
    Expect(Token::Port);
    Scan_Expect(Token::Map);
    scanner.scan();
    return Parse_Association_List_In_Parenthesis();
}

//  precond : COMPONENT | ENTITY | CONFIGURATION
//  postcond : next_token
//
//  instantiated_unit ::=
//      [ COMPONENT ] component_name
//      ENTITY entity_name [ ( architecture_identifier ) ]
//      CONFIGURATION configuration_name
Iir* Parser::Parse_Instantiated_Unit() {
    if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
        throw std::runtime_error(
                "PARSE ERROR: component instantiation using keyword 'component', 'entity', or 'configuration' is not allowed in vhdl87");


    switch (scanner.currentContext.token) {
        case Token::Component:
            scanner.scan();
            return Parse_Name(false);
        case Token::Entity:
            Res = Create_Iir(Iir_Entity_Aspect_Entity);
            Res->Location = scanner.currentContext.getTokenLocation();
            scanner.scan();
            Set_Entity_Name(Res, Parse_Name(false));
            if (scanner.currentContext.token == Token::Left_Paren) {
                Scan_Expect(Token::Identifier);
                Set_Architecture(Res, Current_Text);
                Scan_Expect(Token::Right_Paren);
                scanner.scan();
            }
            return Res;
        case Token::Configuration:
            Res = Create_Iir(Iir_Entity_Aspect_Configuration);
            Res->Location = scanner.currentContext.getTokenLocation();
            Scan_Expect(Token::Identifier);
            Set_Configuration_Name(Res, Parse_Name(false));
            return Res;
        default:
            throw std::logic_error();
    }
}

//  precond : next token
//  postcond: ';'
//
//  component_instantiation_statement ::=
//      INSTANTIATION_label :
//          instantiated_unit [ generic_map_aspect ] [ port_map_aspect ] ;
Iir_Component_Instantiation_Statement* Parser::Parse_Component_Instantiation (Iir* Name) {
    auto Res = new Iir_Component_Instantiation_Statement;
    Res->Location = scanner.currentContext.getTokenLocation();

    Res->Instantiated_Unit = Name;

    if (scanner.currentContext.token == Token::Generic)
        Res->Generic_Map_Aspect_Chain = Parse_Generic_Map_Aspect();
    if (scanner.currentContext.token == Token::Port)
        Res->Port_Map_Aspect_Chain = Parse_Port_Map_Aspect();
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : next token
//  postcond: next token
//
//  [ 9.1 ]
//  block_header ::= [ generic_clause [ generic_map_aspect ; ] ]
//                   [ port_clause [ port_map_aspect ; ] ]
Iir_Block_Header* Parser::Parse_Block_Header() {
    auto Res = new Iir_Block_Header;
    Res->Location = scanner.currentContext.getTokenLocation();
    if (scanner.currentContext.token == Token::Generic) {
        Parse_Generic_Clause(Res);
        if (scanner.currentContext.token == Token::Generic) {
            Res->Generic_Map_Aspect_Chain = Parse_Generic_Map_Aspect();
            Scan_Semi_Colon("generic map aspect");
        }
    }
    if (scanner.currentContext.token == Token::Port) {
        Parse_Port_Clause(Res);
        if (scanner.currentContext.token == Token::Port) {
            Res->Port_Map_Aspect_Chain = Parse_Port_Map_Aspect();
            Scan_Semi_Colon("port map aspect");
        }
    }
    return Res;
}

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
Iir_Block_Statement* Parser::Parse_Block_Statement(std::string Label, Location_Type Loc) {
    if (Label.empty())
        throw std::runtime_error("PARSE ERROR: a block statement must have a label");

// block was just parsed.
    auto Res = new Iir_Block_Statement;
    Res->Location = Loc;
    Res->Label = Label;
    scanner.scan();
    if (scanner.currentContext.token == Token::Left_Paren) {
        auto Guard = new Iir_Guard_Signal_Declaration;
        Guard->Location = scanner.currentContext.getTokenLocation();
        Res->Guard_Decl = Guard;
        scanner.scan();
        Guard->Guard_Expression = Parse_Expression();
        Expect(Token::Right_Paren, "a ')' is expected after guard expression");
        scanner.scan();
    }
    if (scanner.currentContext.token == Token::Is) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'is' not allowed here in vhdl87");
        scanner.scan();
    }
    if (scanner.currentContext.token == Token::Generic || scanner.currentContext.token == Token::Port) {
        Res->Block_Header = Parse_Block_Header();
    }
    if (scanner.currentContext.token != Token::Begin)
        Parse_Declarative_Part(Res);

    Expect(Token::Begin);
    scanner.scan();
    Parse_Concurrent_Statements(Res);
    Check_End_Name(Token::Block, Res);
    return Res;
}

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
bool Parser::Is_Early_End() {

    switch (scanner.currentContext.token) {
        case Token::Elsif:
        case Token::Else:
            if (Get_Kind(Parent) = Iir_If_Generate_Statement)
                return true;

        case Token::When:
            if (Get_Kind(Parent) = Iir_Case_Generate_Statement)
                return true;
        default:
            break;
    }
    return false;
}

Iir* Parser::Parse_Generate_Statement_Body (Iir* Parent, std::string Label ) {

    auto Bod = new Iir_Generate_Statement_Body;
    Bod->Location = scanner.currentContext.getTokenLocation();
    Bod->Parent = Parent;
    Bod->Alternative_Label = Label;
    Bod->Has_Label = !Label.empty();

//  Check for a block declarative item.
    switch (scanner.currentContext.token) {
        case
//  subprogram_declaration
//  subprogram_body
            Token::Procedure:
        case Token::Function:
        case Token::Pure:
        case Token::Impure:
//  type_declaration
        case Token::Type:
//  subtype_declaration
        case Token::Subtype:
//  constant_declaration
        case Token::Constant:
//  signal_declaration
        case Token::Signal:
//  shared_variable_declaration
        case Token::Shared:
        case Token::Variable:
//  file_declaration
        case Token::File:
//  alias_declaration
        case Token::Alias:
//  component_declaration
        case Token::Component:
//  attribute_declaration
//  attribute_specification
        case Token::Attribute:
//  configuration_specification
        case Token::For:
//  disconnection_specification
        case Token::Disconnect:
//  use_clause
        case Token::Use:
//  group_template_declaration
//  group_declaration
        case Token::Group:
        case Token::Begin:
            if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
                throw std::runtime_error("PARSE ERROR: declarations not allowed in a generate in vhdl87");

            Parse_Declarative_Part(Bod);
            Expect(Token::Begin);
            Bod->Has_Begin = true;

//  Skip 'begin'
            scanner.scan();
            break;
        default:
            break;
    }

    Parse_Concurrent_Statements(Bod);

//  Return now if no 'end' (and not expected).
    if (Is_Early_End())
        return Bod;


    Expect(Token::End);

//  Skip 'end'
    scanner.scan();

    if (scanner.state.options.standard >= Vhdl_Std::Vhdl_08 && scanner.currentContext.token != Token::Generate) {
//  This is the 'end' of the generate_statement_body.
        Bod->Has_End = true;
        Check_End_Name(Label, Bod);
        Scan_Semi_Colon("generate statement body");

//  Return now if no 'end' (and not expected).
        if (Is_Early_End())
            return Bod;

        Expect(Token::End);

//  Skip 'end'
        scanner.scan();
    }

    return Bod;
}

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
Iir_For_Generate_Statement* Parser::Parse_For_Generate_Statement (std::string Label, Location_Type Loc) {
    if (Label.empty())
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");

    auto Res = new Iir_For_Generate_Statement;
    Res->Location = Loc;
    Res->Label = Label;

//  Skip 'for'
    scanner.scan();

    Res->Parameter_Specification = Parse_Parameter_Specification(Res);

//  Skip 'generate'
    Expect(Token::Generate);
    scanner.scan();

    Res->Generate_Statement_Body = Parse_Generate_Statement_Body(Res, Null_Identifier);

    Expect(Token::Generate);
    Res->End_Has_Reserved_Id = true;

//  Skip 'generate'
    scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : IF
//  postcond: ';'
//
//  [ LRM93 9.7 ]
//  generate_statement ::=
//      generate_label : generation_scheme GENERATE
//          [ { block_declarative_item }
//      BEGIN ]
//          { concurrent_statement }
//      END GENERATE [ generate_label ] ;
//
//  [ LRM93 9.7 ]
//  generation_scheme ::=
//      FOR GENERATE_parameter_specification
//      | IF condition
//
//  [ LRM08 11.8 ]
//  if_generate_statement ::=
//     generate_label :
//     IF [ alternative_label : ] condition GENERATE
//        generate_statement_body
//     { ELSIF [ alternative_label : ] condition GENERATE
//        generate_statement_body }
//     [ ELSE [ alternative_label : ] GENERATE
//        generate_statement_body ]
//     END GENERATE [ generate_label ] ;
Iir_If_Generate_Statement* Parser::Parse_If_Generate_Statement (std::string Label , Location_Type Loc) {
//Alt_Label : Name_Id;
//Alt_Loc : Location_Type;
//Cond : Iir;
//Clause : Iir;
//Bod : Iir;
//Last : Iir;
    if (Label.empty)
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");

    auto Res = new Iir_If_Generate_Statement;
    Set_Location(Res, Loc);
    Set_Label(Res, Label);

//  Skip 'if'.
    scanner.scan();

    Clause = Res;
    Last = nullptr;
    while (1) {
        Cond = Parse_Expression();

        Alt_Label = Null_Identifier;
        if (scanner.currentContext.token == Token::Colon) {
            if (Get_Kind(Cond) = Iir_Simple_Name) {
                if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
                    throw std::runtime_error("PARSE ERROR: alternative label not allowed before vhdl08");


//  In fact the parsed condition was an alternate label.
                Alt_Label = Get_Identifier(Cond);
                Alt_Loc = Get_Location(Cond);
                Free_Iir(Cond);
            } else
                throw std::runtime_error("PARSE ERROR: alternative label must be an identifier");

//  Skip ':'
            scanner.scan();

            Cond = Parse_Expression();
        }

        Set_Condition(Clause, Cond);

//  Skip 'generate'
        Expect(Token::Generate);
        scanner.scan();

        Bod = Parse_Generate_Statement_Body(Res, Alt_Label);

        if (!Alt_Label.empty())
//  Set location on the label, for xrefs.
            Set_Location(Bod, Alt_Loc);


        Set_Generate_Statement_Body(Clause, Bod);

//  Append clause to the generate statement.
        if (Last)
            Set_Generate_Else_Clause(Last, Clause);

        Last = Clause;

        if (scanner.currentContext.token != Token::Elsif)
            break;

//  Create new alternative.
        Clause = Create_Iir(Iir_If_Generate_Statement);
        Set_Location(Clause, Loc);

//  Skip 'elsif'
        scanner.scan();
    }

    if (scanner.currentContext.token == Token::Else) {
        if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
            throw std::runtime_error("PARSE ERROR: else generate not allowed before vhdl08");

        Clause = Create_Iir(Iir_If_Generate_Else_Clause);
        Set_Location(Clause);

//  Skip 'else'
        scanner.scan();

        if (scanner.currentContext.token == Token::Identifier) {
            Alt_Label = scanner.currentContext.Identifier;
            Alt_Loc = scanner.currentContext.getTokenLocation();

//  Skip identifier
            scanner.scan();

            Expect(Token::Colon);

//  Skip ':'
            scanner.scan();
        } else
            Alt_Label = "";

//  Skip 'generate'
        Expect(Token::Generate);
        scanner.scan();

        Bod = Parse_Generate_Statement_Body(Res, Alt_Label);
        if (!Alt_Label.empty())
//  Set location on the label, for xrefs.
            Set_Location(Bod, Alt_Loc);


        Set_Generate_Statement_Body(Clause, Bod);

        Set_Generate_Else_Clause(Last, Clause);
    }

    Expect(Token::Generate);
    Set_End_Has_Reserved_Id(Res, true);

//  Skip 'generate'
    scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : WHEN
//  postcond: ?
//
//  [ LRM08 11.8 ]
//  case_generate_alternative ::=
//     WHEN [ alternative_label : ] choices:
//        generate_statement_body
Iir Parser::Parse_Case_Generate_Alternative (Iir* Parent) {

    auto Loc = scanner.currentContext.getTokenLocation();

//  Eat 'when'
    Expect(Token::When);
    scanner.scan();

    auto Alt_Label = "";
    if (scanner.currentContext.token == Token::Double_Arrow)
        throw std::runtime_error("PARSE ERROR: missing expression in alternative");
    else if (scanner.currentContext.token == Token::Others)
//  'others' is not an expression!
        Assoc = Parse_Choices(nullptr);
    else {
        Expr = Parse_Expression;

        if (scanner.currentContext.token == Token::Colon) {
            if (Get_Kind(Expr) == Iir_Simple_Name) {
//  In fact the parsed condition was an alternate label.
                Alt_Label = Get_Identifier(Expr);
                Loc = Get_Location(Expr);
                Free_Iir(Expr);
            } else
                throw std::runtime_error("PARSE ERROR: alternative label must be an identifier");

            Expr = nullptr;

//  Skip ':'
            scanner.scan();
        }

        Assoc = Parse_Choices(Expr);
    }

//  Set location of label (if any, for xref) or location of 'when'.
    Set_Location(Assoc, Loc);

//  Eat '=>'
    Expect(Token::Double_Arrow);
    scanner.scan();

    Bod = Parse_Generate_Statement_Body(Parent, Alt_Label);
    Set_Associated_Block(Assoc, Bod);
    if (!Alt_Label.empty())
//  Set location on the label, for xrefs.
        Set_Location(Bod, Loc);


    return Assoc;
}

//  precond : CASE
//  postcond: ';'
//
//  [ LRM08 11.8 ]
//  case_generate_statement ::=
//     generate_label :
//     CASE expression GENERATE
//        case_generate_alternative
//      { case_generate_alternative }
//     END GENERATE [ generate_label ] ;
Iir_Case_Generate_Statement* Parser::Parse_Case_Generate_Statement(std::string Label, Location_Type Loc) {
//Res : Iir;
//Alt : Iir;
//Last_Alt : Iir;
    if (Label.empty)
        throw std::runtime_error("PARSE ERROR: a generate statement must have a label");

    auto Res = new Iir_Case_Generate_Statement;
    Set_Location(Res, Loc);
    Set_Label(Res, Label);

//  Skip 'case'.
    scanner.scan();

    Set_Expression(Res, Parse_Expression);

    Expect(Token::Generate);

//  Skip 'generate'
    scanner.scan();

    if (scanner.currentContext.token == Token::End)
        throw std::runtime_error("PARSE ERROR: no generate alternative");


    Last_Alt = nullptr;
    while (scanner.currentContext.token == Token::When) {
        Alt = Parse_Case_Generate_Alternative(Res);
        if (Last_Alt)
            Set_Case_Statement_Alternative_Chain(Res, Alt);
        else
            Set_Chain(Last_Alt, Alt);


//  Skip until last choice of the choices list.
        while (!Alt) {
            Last_Alt = Alt;
            Alt = Get_Chain(Alt);
        }
    }

    Expect(Token::Generate);
    Set_End_Has_Reserved_Id(Res, true);

//  Skip 'generate'
    scanner.scan();

//  LRM93 9.7
//  If a label appears at the end of a generate statement, it must repeat
//  the generate label.
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    return Res;
}
/*
//  precond : first token
//  postcond: END
//
//  [ LRM93 9 ]
//  concurrent_statement ::= block_statement
//                         | process_statement
//                         | concurrent_procedure_call_statement
//                         | concurrent_assertion_statement
//                         | concurrent_signal_assignment_statement
//                         | component_instantiation_statement
//                         | generate_statement
//
//  [ LRM93 9.4 ]
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
Res = Create_Iir (Iir_Simple_Simultaneous_Statement);
Set_Simultaneous_Left (Res, Parse_Simple_Expression (Target));
if (scanner.currentContext.token != Token::Equal_Equal {
throw std::runtime_error("PARSE ERROR: '==' expected after expression");
else
Res->Location = scanner.currentContext.getTokenLocation();
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
if (scanner.currentContext.token == Token::Report {
//  Skip 'report'
scanner.scan();

Set_Report_Expression (Stmt, Parse_Expression);
end if;

if (scanner.currentContext.token == Token::Severity {
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
        Res = Create_Iir (Iir_Psl_Assert_Statement);

//  Accept PSL tokens
if (scanner.state.options.standard >== Vhdl_Std::Vhdl_08) {
Scanner.Flag_Psl = true;
end if;

//  Skip 'assert'
scanner.scan();

Set_Psl_Property (Res, Parse_Psl.Parse_Psl_Property);

//  No more PSL tokens after the property.
Scanner.Flag_Psl = false;

Parse_Psl_Assert_Report_Severity (Res);

Scanner.Flag_Scan_In_Comment = false;
return Res;
end Parse_Psl_Assert_Statement;

Parse_Psl_Cover_Statement return Iir
        is
Res : Iir;
begin
        Res = Create_Iir (Iir_Psl_Cover_Statement);

//  Skip 'cover'
scanner.scan();

Set_Psl_Sequence (Res, Parse_Psl.Parse_Psl_Sequence);

//  No more PSL tokens after the property.
Scanner.Flag_Psl = false;

Parse_Psl_Assert_Report_Severity (Res);

Scanner.Flag_Scan_In_Comment = false;
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
Postponed = false;
end if;
end Postponed_Not_Allowed;

 Label_Not_Allowed is
        begin
if Label != Null_Identifier {
throw std::runtime_error("PARSE ERROR: 'postponed' not allowed here");
Label = Null_Identifier;
end if;
end Label_Not_Allowed;
begin
// begin was just parsed.
Last_Stmt = nullptr;
loop
        Stmt = nullptr;
Label = Null_Identifier;
Postponed = false;
Loc = scanner.currentContext.getTokenLocation();

// Try to find a label.
if (scanner.currentContext.token == Token::Identifier {
        Label = scanner.currentContext.Identifier;

//  Skip identifier
scanner.scan();

if (scanner.currentContext.token == Token::Colon {
//  The identifier is really a label.

//  Skip ':'
scanner.scan();
else
//  This is not a label.  Assume a concurrent assignment.
Target = Create_Iir (Iir_Simple_Name);
Set_Location (Target, Loc);
Set_Identifier (Target, Label);
Label = Null_Identifier;
Target = Parse_Name_Suffix (Target);

Stmt = Parse_Concurrent_Assignment (Target);
goto Has_Stmt;
end if;
end if;

if (scanner.currentContext.token == Token::Postponed {
if (scanner.state.options.standard == Vhdl_Std::Vhdl_87) {
        throw std::runtime_error("PARSE ERROR: 'postponed' is not allowed in vhdl 87");
else
Postponed = true;
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
Target = Parse_Name (Allow_Indexes: true);
Stmt = Parse_Concurrent_Assignment (Target);
if Get_Kind (Stmt) = Iir_Component_Instantiation_Statement
        && Postponed
        {
throw std::runtime_error("PARSE ERROR: 'postponed' not allowed for " &
"an instantiation statement");
Postponed = false;
end if;
case Token::Left_Paren:
Id = Parse_Aggregate;
if (scanner.currentContext.token == Token::Less_Equal {
// This is a conditional signal assignment.
Stmt = Parse_Concurrent_Conditional_Signal_Assignment (Id);
else
throw std::runtime_error("PARSE ERROR: '<=' expected after aggregate");
Eat_Tokens_Until_Semi_Colon;
end if;
case Token::Process:
Stmt = Parse_Process_Statement (Label, Loc, Postponed);
case Token::Assert:
if Vhdl_Std >= Vhdl_08
|| (Flag_Psl_Comment && Flag_Scan_In_Comment)
{
        Stmt = Parse_Psl_Assert_Statement;
else
Stmt = Create_Iir (Iir_Concurrent_Assertion_Statement);
Parse_Assertion (Stmt);
Expect (Token::Semi_Colon);
end if;
case Token::With:
Stmt = Parse_Selected_Signal_Assignment;
case Token::Block:
Postponed_Not_Allowed;
Stmt = Parse_Block_Statement (Label, Loc);
case Token::For:
Postponed_Not_Allowed;
Stmt = Parse_For_Generate_Statement (Label, Loc);
case Token::If:
Postponed_Not_Allowed;
Stmt = Parse_If_Generate_Statement (Label, Loc);
case Token::Case:
Postponed_Not_Allowed;
Stmt = Parse_Case_Generate_Statement (Label, Loc);
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
        Unit = Parse_Instantiated_Unit;
Stmt = Parse_Component_Instantiation (Unit);
end;
case Token::Psl_Default:
Postponed_Not_Allowed;
Label_Not_Allowed;
Stmt = Parse_Psl_Default_Clock;
case Token::Psl_Property
| Token::Psl_Sequence
| Token::Psl_Endpoint:
Postponed_Not_Allowed;
Label_Not_Allowed;
Stmt = Parse_Psl_Declaration;
case Token::Psl_Cover:
Postponed_Not_Allowed;
Stmt = Parse_Psl_Cover_Statement;
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
Last_Stmt = Stmt;
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
        Library = Create_Iir (Iir_Library_Clause);

//  Skip 'library' or ','.
Scan_Expect (Token::Identifier);

Set_Identifier (Library, scanner.currentContext.Identifier);
Set_Location (Library);
Sub_Chain_Append (First, Last, Library);

//  Skip identifier.
scanner.scan();

if (scanner.currentContext.token == Token::Semi_Colon)
 break;
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
        First = nullptr;
Last = nullptr;
scanner.scan();
loop
        Use_Clause = Create_Iir (Iir_Use_Clause);
Set_Location (Use_Clause);
Expect (Token::Identifier);
Set_Selected_Name (Use_Clause, Parse_Name);

//  Chain use clauses.
if First = nullptr {
        First = Use_Clause;
else
Set_Use_Clause_Chain (Last, Use_Clause);
end if;
Last = Use_Clause;

if (scanner.currentContext.token == Token::Semi_Colon)
 break;
Expect (Token::Comma);
scanner.scan();
end loop;
return First;
end Parse_Use_Clause;
*/
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
Iir_Architecture_Body* Parser::Parse_Architecture_Body (Iir_Design_Unit Unit) {
    Expect(Token::Architecture);
    auto Res = new Iir_Architecture_Body;

// Get identifier.
    Scan_Expect(Token::Identifier);
    Set_Identifier(Res, scanner.currentContext.Identifier);
    Res->Location = scanner.currentContext.getTokenLocation();
    scanner.scan();
    if (scanner.currentContext.token == Token::Is)
        throw std::runtime_error("PARSE ERROR: architecture identifier is missing");
    else {
        Expect(Token::Of);
        scanner.scan();
        Set_Entity_Name(Res, Parse_Name(false));
        Expect(Token::Is);
    }

    scanner.scan();
    Parse_Declarative_Part(Res);

    Expect(Token::Begin);
    scanner.scan();
    Parse_Concurrent_Statements(Res);
// end was scanned.
    Set_End_Location(Unit);
    scanner.scan();
    if (scanner.currentContext.token == Token::Architecture) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'architecture' keyword not allowed here by vhdl 87");

        Set_End_Has_Reserved_Id(Res, true);
        scanner.scan();
    }
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    Set_Library_Unit(Unit, Res);
}

//  precond : next token
//  postcond: a token
//
//  [ 5.2 ]
//  instantiation_list ::= INSTANTIATION_label { , INSTANTIATION_label }
//                       | OTHERS
//                       | ALL
std::vector<Iir*> Parser::Parse_Instantiation_List() {

    switch (scanner.currentContext.token) {
        case Token::All:
            scanner.scan();
            return Iir_List_All;
        case Token::Others:
            scanner.scan();
            return Iir_List_Others;
        case Token::Identifier:
            Res = Create_Iir_List;
            while (1) {
                Append_Element(Res, Current_Text);
                scanner.scan();
                if (scanner.currentContext.token != Token::Comma)
                    break;
                Expect(Token::Comma);
                scanner.scan();
            }
            return Res;
        default:
            throw std::runtime_error("PARSE ERROR: instantiation list expected");
    }
}

//  precond : next token
//  postcond: next token
//
//  [ 5.2 ]
//  component_specification ::= instantiation_list : COMPONENT_name
void Parser::Parse_Component_Specification (Iir* Res) {
    auto List = Parse_Instantiation_List();
    Set_Instantiation_List(Res, List);
    Expect(Token::Colon);
    Scan_Expect(Token::Identifier);
    Set_Component_Name(Res, Parse_Name);
}

//  precond : next token
//  postcond: next token
//
//  [ 5.2.1.1 ]
//  entity_aspect ::= ENTITY ENTITY_name [ ( ARCHITECTURE_identifier ) ]
//                  | CONFIGURATION CONFIGURATION_name
//                  | OPEN
Iir* Parser::Parse_Entity_Aspect() {

    switch (scanner.currentContext.token) {
        case Token::Entity:
            Res = Create_Iir(Iir_Entity_Aspect_Entity);
            Res->Location = scanner.currentContext.getTokenLocation();
            Scan_Expect(Token::Identifier);
            Set_Entity_Name(Res, Parse_Name(false));
            if (scanner.currentContext.token == Token::Left_Paren) {
                Scan_Expect(Token::Identifier);
                Set_Architecture(Res, Current_Text);
                Scan_Expect(Token::Right_Paren);
                scanner.scan();
            }
            break;
        case Token::Configuration:
            Res = Create_Iir(Iir_Entity_Aspect_Configuration);
            Res->Location = scanner.currentContext.getTokenLocation();
            Scan_Expect(Token::Identifier);
            Set_Configuration_Name(Res, Parse_Name(false));
            break;
        case Token::Open:
            Res = Create_Iir(Iir_Entity_Aspect_Open);
            Res->Location = scanner.currentContext.getTokenLocation();
            scanner.scan();
        default:
//  FIXME: if the token is an identifier, try as if the 'entity'
//  keyword is missing.
            throw std::runtime_error("PARSE ERROR: 'entity', 'configuration' or 'open' keyword expected");
    }
    return Res;
}

//  precond : next token
//  postcond: next token
//
//  [ 5.2.1 ]
//  binding_indication ::=
//      [ USE entity_aspect ]
//      [ generic_map_aspect ]
//      [ port_map_aspect ]
Iir_Binding_Indication* Parser::Parse_Binding_Indication() {

    switch (scanner.currentContext.token) {
        case Token::Use:
        case Token::Generic:
        case Token::Port:
            break;
        default:
            return nullptr;
    }
    auto Res = new Iir_Binding_Indication;
    Res->Location = scanner.currentContext.getTokenLocation();
    if (scanner.currentContext.token == Token::Use) {
        scanner.scan();
        Res->Entity_Aspect = Parse_Entity_Aspect();
    }
    if (scanner.currentContext.token == Token::Generic)
        Res->Generic_Map_Aspect_Chain = Parse_Generic_Map_Aspect();

    if (scanner.currentContext.token == Token::Port)
        Res->Port_Map_Aspect_Chain = Parse_Port_Map_Aspect();

    return Res;
}

//  precond : ':' after instantiation_list.
//  postcond: ';'
//
//  [ 1.3.2 ]
//  component_configuration ::=
//      FOR component_specification
//          [ binding_indication ; ]
//          [ block_configuration ]
//      END FOR ;
Iir_Component_Configuration* Parser::Parse_Component_Configuration (Location_Type Loc, std::vector<Iir*> Inst_List) {
    auto Res = new Iir_Component_Configuration;
    Res->Location = Loc;

//  Component specification.
    Res->Instantiation_List = Inst_List;
    Expect(Token::Colon);
    Scan_Expect(Token::Identifier);
    Res->Component_Name = Parse_Name();

    switch (scanner.currentContext.token) {
        case Token::Use:
        case Token::Generic:
        case Token::Port:
            Res->Binding_Indication = Parse_Binding_Indication();
            Scan_Semi_Colon("binding indication");
        default:
            break;
    }
    if (scanner.currentContext.token == Token::For) {
        Res->Block_Configuration = Parse_Block_Configuration();
//  Eat ';'.
        scanner.scan();
    }
    Expect(Token::End);
    Scan_Expect(Token::For);
    Scan_Expect(Token::Semi_Colon);
    return Res;
}

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
//    | BLOCK_STATEMENT_label
//    | GENERATE_STATEMENT_label [ ( index_specification ) ]
Iir_Block_Configuration* Parser::Parse_Block_Configuration_Suffix (Location_Type Loc, Iir* Block_Spec ) {
    auto Res = new Iir_Block_Configuration;
    Res->Location = Loc;

    Res->Block_Specification = Block_Spec;

//  Parse use clauses.
    if (scanner.currentContext.token == Token::Use) {

        Build_Init(Last);

        while (scanner.currentContext.token == Token::Use) {
            Append_Subchain(Last, Res, Parse_Use_Clause);
//  Eat ';'.
            scanner.scan();
        }
    }

//  Parse configuration item list
    {
        Build_Init(Last);

        while (scanner.currentContext.token != Token::End) {
            Append(Last, Res, Parse_Configuration_Item);
//  Eat ';'.
            scanner.scan();
        }
    }
    Scan_Expect(Token::For);
    Scan_Expect(Token::Semi_Colon);
    return Res;
}

Iir_Block_Configuration* Parser::Parse_Block_Configuration() {
    auto Loc = scanner.currentContext.getTokenLocation();
    Expect(Token::For);

//  Parse label.
    scanner.scan();
    return Parse_Block_Configuration_Suffix(Loc, Parse_Name());
}

//  precond : FOR
//  postcond: ';'
//
//  [ 1.3.1 ]
//  configuration_item ::= block_configuration
//                       | component_configuration
Iir* Parser::Parse_Configuration_Item() {

    auto Loc = scanner.currentContext.getTokenLocation();
    Expect(Token::For);
    scanner.scan();

//  ALL and OTHERS are tokens from an instantiation list.
//  Thus, the rule is a component_configuration.
    switch (scanner.currentContext.token) {
        case Token::All:
            scanner.scan();
            return Parse_Component_Configuration(Loc, Iir_List_All);
        case Token::Others:
            scanner.scan();
            return Parse_Component_Configuration(Loc, Iir_List_Others);
        case Token::Identifier:
            El = Current_Text;
            scanner.scan();
            switch (scanner.currentContext.token) {
                case Token::Colon:
//  The identifier was a label from an instantiation list.
                    List = Create_Iir_List;
                    Append_Element(List, El);
                    return Parse_Component_Configuration(Loc, List);
                case Token::Comma:
//  The identifier was a label from an instantiation list.
                    List = Create_Iir_List;
                    Append_Element(List, El);
                    while (1) {
                        Scan_Expect(Token::Identifier);
                        Append_Element(List, Current_Text);
                        scanner.scan();
                        if (scanner.currentContext.token != Token::Comma)
                            break;
                    }
                    return Parse_Component_Configuration(Loc, List);
                case Token::Left_Paren:
                    El = Parse_Name_Suffix(El);
                    return Parse_Block_Configuration_Suffix(Loc, El);
                case Token::Use | Token::For | Token::End:
//  Possibilities for a block_configuration.
//  FIXME: should use 'when others' ?
                    return Parse_Block_Configuration_Suffix(Loc, El);
                default:
                    throw std::runtime_error("PARSE ERROR: block_configuration or component_configuration expected");

            }
        default:
            throw std::runtime_error("PARSE ERROR: configuration item expected");
    }
}

//  precond : next token
//  postcond: next token
//
//  [ 1.3]
//  configuration_declarative_part ::= { configuration_declarative_item }
//
//  [ 1.3]
//  configuration_declarative_item ::= use_clause
//                                   | attribute_specification
//                                   | group_declaration
//  FIXME: attribute_specification, group_declaration
void Parser::Parse_Configuration_Declarative_Part (Iir* Parent) {
    Build_Init(Last);
    while (1) {
        switch (scanner.currentContext.token) {
            case Token::Invalid:
                throw std::logic_error("");
            case Token::Use:
                Append_Subchain(Last, Parent, Parse_Use_Clause);
                break;
            case Token::Attribute:
                El = Parse_Attribute();
                if (El) {
                    if (Get_Kind(El) != Iir_Attribute_Specification)
                        throw std::runtime_error("PARSE ERROR: attribute declaration not allowed here");

                    Append(Last, Parent, El);
                }
                break;
            case Token::Group:
                El = Parse_Group();
                if (El) {
                    if (Get_Kind(El) != Iir_Group_Declaration)
                        throw std::runtime_error("PARSE ERROR: group template declaration not allowed here");
                    Append(Last, Parent, El);
                }
            default:
                break
                from loop;
        }
        scanner.scan();
    }
}

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
void Parser::Parse_Configuration_Declaration (Iir_Design_Unit* Unit) {
    if (scanner.currentContext.token != Token::Configuration)
        throw std::logic_error("");

    auto Res = new Iir_Configuration_Declaration;

// Get identifier.
    Scan_Expect(Token::Identifier);
    Set_Identifier(Res, scanner.currentContext.Identifier);
    Res->Location = scanner.currentContext.getTokenLocation();

//  Skip identifier.
    Scan_Expect(Token::Of);

//  Skip 'of'.
    scanner.scan();

    Set_Entity_Name(Res, Parse_Name(false));

//  Skip 'is'.
    Expect(Token::Is);
    scanner.scan();

    Parse_Configuration_Declarative_Part(Res);

    Set_Block_Configuration(Res, Parse_Block_Configuration);

    Scan_Expect(Token::End);
    Set_End_Location(Unit);

//  Skip 'end'.
    scanner.scan();

    if (scanner.currentContext.token == Token::Configuration) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'configuration' keyword not allowed here by vhdl 87");

        Set_End_Has_Reserved_Id(Res, true);

//  Skip 'configuration'.
        scanner.scan();
    }

// LRM93 1.3
// If a simple name appears at the end of a configuration declaration, it
// must repeat the identifier of the configuration declaration.
    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    Set_Library_Unit(Unit, Res);
}

//  precond : generic
//  postcond: next token
//
//  LRM08 4.7, 6.5.6.2
//  package_header ::= [ generic_clause  [ generic_map aspect ; ] ]
Iir_Package_Header* Parser::Parse_Package_Header() {

    auto Res = new Iir_Package_Header;
    Res->Location = scanner.currentContext.getTokenLocation();
    Parse_Generic_Clause(Res);

    if (scanner.currentContext.token == Token::Generic) {
        Res->Generic_Map_Aspect_Chain =Parse_Generic_Map_Aspect();
        Scan_Semi_Colon("generic map aspect");
    }
    return Res;
}

//  precond : token (after 'IS')
//  postcond: ';'
//
//  [ LRM93 2.5, LRM08 4.7 ]
//  package_declaration ::=
//      PACKAGE identifier IS
//          package_header   // LRM08
//          package_declarative_part
//      END [ PACKAGE ] [ PACKAGE_simple_name ] ;
Iir_Package_Declaration* Parser::Parse_Package_Declaration(Iir* Parent, std::string Id, Location_Type Loc) {
    auto Res = new Iir_Package_Declaration;
    Res->Location = Loc;
    Res->Identifier = Id;
    Res->Parent = Parent;

    if (scanner.currentContext.token == Token::Generic) {
        if (scanner.state.options.standard < Vhdl_Std::Vhdl_08)
            throw std::runtime_error("PARSE ERROR: generic packages not allowed before vhdl 2008");

        Res->Package_Header = Parse_Package_Header();
    }

    Parse_Declarative_Part(Res);

    Expect(Token::End);
    Parent->End_Location = scanner.currentContext.getTokenLocation();

//  Skip 'end'
    scanner.scan();

    if (scanner.currentContext.token == Token::Package) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'package' keyword not allowed here by vhdl 87");

        Res->End_Has_Reserved_Id = true;

//  Skip 'package'.
        scanner.scan();
    }

    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : BODY
//  postcond: ';'
//
//  [ LRM93 2.6, LRM08 4.8 ]
//  package_body ::=
//      PACKAGE BODY PACKAGE_simple_name IS
//          package_body_declarative_part
//      END [ PACKAGE BODY ] [ PACKAGE_simple_name ] ;
Iir_Package_Body* Parser::Parse_Package_Body (Iir* Parent) {
    auto Res = new Iir_Package_Body;
    Res->Location = scanner.currentContext.getTokenLocation();
    Set_Parent(Res, Parent);

// Get identifier.
    Expect(Token::Identifier);
    Set_Identifier(Res, scanner.currentContext.Identifier);
    Scan_Expect(Token::Is);
    scanner.scan();

    Parse_Declarative_Part(Res);

    Expect(Token::End);
    Set_End_Location(Parent);

//  Skip 'end'
    scanner.scan();

    if (scanner.currentContext.token == Token::Package) {
        if (scanner.state.options.standard == Vhdl_Std::Vhdl_87)
            throw std::runtime_error("PARSE ERROR: 'package' keyword not allowed here by vhdl 87");

        Set_End_Has_Reserved_Id(Res, true);

//  Skip 'package'
        scanner.scan();

        if (scanner.currentContext.token != Token::Body)
            throw std::runtime_error("PARSE ERROR: missing 'body' after 'package'");
        else
//  Skip 'body'
            scanner.scan();
    }

    Check_End_Name(Res);
    Expect(Token::Semi_Colon);
    return Res;
}

//  precond : NEW
//  postcond: ';'
//
//  [ LRM08 4.9 ]
//  package_instantiation_declaration ::=
//      PACKAGE identifier IS NEW uninstantiated_package_name
//         [ generic_map_aspect ] ;
Iir_Package_Instantiation_Declaration* Parser::Parse_Package_Instantiation_Declaration(Iir* Parent, std::string Id, Location_Type Loc) {
    auto Res = new Iir_Package_Instantiation_Declaration;
    Set_Location(Res, Loc);
    Set_Identifier(Res, Id);
    Set_Parent(Res, Parent);

//  Skip 'new'
    scanner.scan();

    Set_Uninstantiated_Package_Name(Res, Parse_Name(false));

    if (scanner.currentContext.token == Token::Generic)
        Set_Generic_Map_Aspect_Chain(Res, Parse_Generic_Map_Aspect);
    else if (scanner.currentContext.token == Token::Left_Paren)
        throw std::runtime_error("PARSE ERROR: missing 'generic map'");


    Expect(Token::Semi_Colon);

    return Res;
}

//  precond : PACKAGE
//  postcond: ';'
//
//    package_declaration
//  | package_body
//  | package_instantiation_declaration
Iir* Parser::Parse_Package (Iir* Parent) {

//  Skip 'package'
    scanner.scan();

    if (scanner.currentContext.token == Token::Body) {
//  Skip 'body'
        scanner.scan();

        return Parse_Package_Body(Parent);
    } else {
        Expect(Token::Identifier);
        Id = scanner.currentContext.Identifier;
        Loc = scanner.currentContext.getTokenLocation();

//  Skip identifier.
        scanner.scan();

//  Skip 'is'.
        Expect(Token::Is);
        scanner.scan();

        if (scanner.currentContext.token == Token::New) {
            Res = Parse_Package_Instantiation_Declaration(Parent, Id, Loc);
//  Note: there is no 'end' in instantiation.
            Set_End_Location(Parent);
            return Res;
        } else
            return Parse_Package_Declaration(Parent, Id, Loc);

    }
}

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
    Unit->Library_Unit = Decl;

//  Skip 'is'
    scanner.scan();

    Parse_Context_Clause(Decl);

    Expect(Token::End);
    Unit->End_Location = scanner.currentContext.getTokenLocation();

//  Skip 'end'
    scanner.scan();

    if (scanner.currentContext.token == Token::Context) {
        Decl->End_Has_Reserved_Id = true;

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
        Loc = scanner.currentContext.getTokenLocation();

//  Skip 'context'.
scanner.scan();

Name = Parse_Name;

if (scanner.currentContext.token == Token::Is {
        Res = Create_Iir (Iir_Context_Declaration);
if Get_Kind (Name) = Iir_Simple_Name {
        Location_Copy (Res, Name);
Set_Identifier (Res, Get_Identifier (Name));
else
Set_Location (Res, Loc);
throw std::runtime_error("PARSE ERROR:" + +Name, "identifier for context expected");
end if;
Free_Iir (Name);

Parse_Context_Declaration (Unit, Res);
Clause = nullptr;
else
Clause = Parse_Context_Reference (Loc, Name);
end if;
end Parse_Context_Declaration_Or_Reference;
*/

// Parse a design_unit.
// The lexical scanner must have been initialized, but without a current_token.
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
    currentNode = first_term;
    first_term->Date_State = Date_Extern;
    Parse_Context_Clause(first_term);
    if(first_term->Library_Unit == nullptr) {
        switch (scanner.currentContext.token) {
        case Token::Entity:
            first_term = Parse_Entity_Declaration();
            break;
        case Token::Architecture:
            first_term = Parse_Architecture_Body();
            break;
        case Token::Package:
            first_term->Library_Unit = Parse_Package(first_term);
            break;
        case Token::Configuration:
            first_term = Parse_Configuration_Declaration();
            break;
        default:
            throw std::runtime_error("PARSE ERROR: entity, architecture, package or configuration keyword expected");
        }
    }
    auto unit = first_term->Library_Unit;

    //FIXME:
    static_cast<Iir_Parent_Abs*>(unit)->Parent = first_term;
    first_term->Identifier = unit->Identifier;
    first_term->Date = 4; //Date_Parsed
    scanner.currentContext.invalidateToken();
    return first_term;
}

//  [ 11.1 ]
//  design_file ::= design_unit { design_unit }
Iir_Design_File* Parser::Parse_Design_File () {
    auto result = new Iir_Design_File();
    currentNode = result;
    for (Iir_Design_Unit* design = Parse_Design_Unit(); design; design = Parse_Design_Unit()) {
        result->Design_Units.push_back(design);
        design->Design_File = result;
    }
    if (result->Design_Units.empty())
        throw std::runtime_error("PARSE ERROR: design file is empty (no design unit found)");
    return result;
}
