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
#ifndef AVLC_PARSER_H
#define AVLC_PARSER_H

#include <tuple>
#include "iir.h"
#include "Scanner.h"

//  Recursive descendant parser.
//  Each subprogram (should) parse one production rules.
//  Rules are written in a comment just before the subprogram.
//  terminals are written in upper case.
//  non-terminal are written in lower case.
//  syntaxic category of a non-terminal are written in upper case.
//  eg: next_statement ::= [ label : ] NEXT [ LOOP_label ] [ WHEN condition ] ;
//  Or (|) must be aligned by the previous or, or with the '=' character.
//  Indentation is 4.
//
//  To document what is expected for input and what is left as an output
//  concerning token stream, a precond and a postcond comment shoud be
//  added before the above rules.
//    a token (such as IF or ';') means the current token is this token.
//    'a token' means the current token was analysed.
//    'next token' means the current token is to be analysed.

class Parser {
    Scanner scanner;
public:
    //  If True, create nodes for parenthesis expressions.
    bool Flag_Parse_Parenthesis = false;

// Parse an expression.
// (Used by PSL).
    Iir* Parse_Expression ();
    Iir* Parse_Expression_Rhs (Iir* Left);

// Parse an relationnal operator and its rhs.
    Iir* Parse_Relation_Rhs (Iir* Left);

//  Convert the STR (0 .. LEN - 1) into a operator symbol identifier.
//  Emit an error message if the name is not an operator name.
    unsigned int Str_To_Operator_Name(std::string str);

//  Convert string literal STR to an operator symbol.
//  Emit an error message if the string is not an operator name.
    Iir* String_To_Operator_Symbol (Iir* Str);

// Parse a single design unit.
// The scanner must have been initialized, however, the current_token
// shouldn't have been set.
// At return, the last token accepted is the semi_colon that terminates
// the library unit.
// Return Null_Iir when end of file.
    Iir_Design_Unit* Parse_Design_Unit ();

//  Parse a file.
//  The scanner must habe been initialized as for parse_design_unit.
//  Return Null_Iir in case of error.
    Iir_Design_File* Parse_Design_File ();

private:

    Iir* currentNode;

    // current_token must be valid.
    // Leaves a token.
    Iir* Parse_Simple_Expression (Iir* Primary = nullptr);
    Iir* Parse_Primary();
    Iir_Use_Clause* Parse_Use_Clause();

    Iir* Parse_Association_List();
    Iir* Parse_Association_List_In_Parenthesis();

    Iir* Parse_Sequential_Statements (Iir* parent);
    Iir* Parse_Configuration_Item();
    Iir_Block_Configuration* Parse_Block_Configuration();
    void Parse_Concurrent_Statements (Iir* parent);
    Iir* Parse_Subprogram_Declaration();
    Iir* Parse_Subtype_Indication (Iir* Name = nullptr);
    Iir* Parse_Interface_List (Ctxt : Interface_Kind_Type; Iir* parent);
    void Parse_Component_Specification (Res : Iir);
    Iir_Binding_Indication Parse_Binding_Indication();
    Iir* Parse_Aggregate();
    Iir_Signature Parse_Signature();
    void Parse_Declarative_Part (Iir* parent);
    Iir* Parse_Tolerance_Aspect_Opt();
    Iir* Parse_Package (Iir* parent);

    Iir* Parse_Context_Reference(Location_Type Loc, Iir* Name);

    void Expect(Token::Token token, std::string message);

    void Scan_Expect(Token::Token token, std::string message);

    void Check_End_Name(std::string name, Iir* decl);

    void Check_End_Name(Iir* Decl);

    void Check_End_Name(Token::Token token, Iir* Decl);

    void Eat_Tokens_Until_Semi_Colon();

    void Scan_Semi_Colon(std::string Msg);

    Iir_Mode Parse_Mode();

    std::tuple<bool, Iir_Signal_Kind> Parse_Signal_Kind();

    Iir* Parse_Range_Expression(Iir* Left);

    Iir* Parse_Range();

    Iir* Parse_Range_Constraint_Of_Subtype_Indication(Iir* Type_Mark, Iir* Resolution_Indication);

    Iir* Parse_Range_Constraint();

    Iir* Parse_Discrete_Range();

    unsigned int Scan_To_Operator_Name();

    Iir* Parse_Name_Suffix(Iir* Pfx, bool Allow_Indexes);

    Iir* Parse_External_Pathname();

    Iir* Parse_External_Name();

    Iir* Parse_Term(Iir* Primary);

    Iir* Build_Unary_Factor(Iir* Primary, Vhdl_Std standard);
};
#endif // AVLC_PARSER_H
