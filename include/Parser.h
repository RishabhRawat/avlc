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

#include "CompilerExceptions.h"
#include <fstream>
#include <string>
#include <unordered_map>

class Parser {

    const std::string fileName;
    std::ifstream sourceFile;
    unsigned int lineNumber = 0;
    unsigned int position = 0;
    std::string lastComment;
    Token lastToken;


    Token getNextToken();

    std::string ParseComment();

    enum class Token {
        Tok_Eof,
        Tok_Line_Number,
        Tok_File_Name,
        Tok_Comment,
        Tok_Ident,
        Tok_Num,
        Tok_String,
        Tok_Float_Num,
        Tok_Plus,
        Tok_Minus,
        Tok_Star,
        Tok_Div,
        Tok_Mod,
        Tok_Rem,
        Tok_Sharp,
        Tok_Not,
        Tok_Abs,
        Tok_Or,
        Tok_And,
        Tok_Xor,
        Tok_Equal,
        Tok_Not_Equal,
        Tok_Greater,
        Tok_Greater_Eq,
        Tok_Less,
        Tok_Less_Eq,
        Tok_Colon,
        Tok_Semicolon,
        Tok_Comma,
        Tok_Dot,
        Tok_Tick,
        Tok_Arob,
        Tok_Elipsis,
        Tok_Assign,
        Tok_Left_Paren,
        Tok_Right_Paren,
        Tok_Left_Brace,
        Tok_Right_Brace,
        Tok_Left_Brack,
        Tok_Right_Brack,
        Tok_Unsigned,
        Tok_Signed,
        Tok_Float,
        Tok_Array,
        Tok_Subarray,
        Tok_Access,
        Tok_Record,
        Tok_Union,
        Tok_Boolean,
        Tok_Enum,
        Tok_If,
        Tok_Then,
        Tok_Else,
        Tok_Elsif,
        Tok_Loop,
        Tok_Exit,
        Tok_Next,
        Tok_Is,
        Tok_Of,
        Tok_All,
        Tok_Return,
        Tok_Type,
        Tok_External,
        Tok_Private,
        Tok_Public,
        Tok_Local,
        Tok_Procedure,
        Tok_Function,
        Tok_Constant,
        Tok_Var,
        Tok_Declare,
        Tok_Begin,
        Tok_End,
        Tok_Case,
        Tok_When,
        Tok_Default,
        Tok_Arrow,
        Tok_Null
    };
    const std::unordered_map<std::string, Token> symbolTable = {
            {"type",      Token::Tok_Type},
            {"return",    Token::Tok_Return},
            {"if",        Token::Tok_If},
            {"then",      Token::Tok_Then},
            {"else",      Token::Tok_Else},
            {"elsif",     Token::Tok_Elsif},
            {"loop",      Token::Tok_Loop},
            {"exit",      Token::Tok_Exit},
            {"next",      Token::Tok_Next},
            {"signed",    Token::Tok_Signed},
            {"unsigned",  Token::Tok_Unsigned},
            {"float",     Token::Tok_Float},
            {"is",        Token::Tok_Is},
            {"of",        Token::Tok_Of},
            {"all",       Token::Tok_All},
            {"not",       Token::Tok_Not},
            {"abs",       Token::Tok_Abs},
            {"or",        Token::Tok_Or},
            {"and",       Token::Tok_And},
            {"xor",       Token::Tok_Xor},
            {"mod",       Token::Tok_Mod},
            {"rem",       Token::Tok_Rem},
            {"array",     Token::Tok_Array},
            {"access",    Token::Tok_Access},
            {"record",    Token::Tok_Record},
            {"union",     Token::Tok_Union},
            {"end",       Token::Tok_End},
            {"boolean",   Token::Tok_Boolean},
            {"enum",      Token::Tok_Enum},
            {"external",  Token::Tok_External},
            {"private",   Token::Tok_Private},
            {"public",    Token::Tok_Public},
            {"local",     Token::Tok_Local},
            {"procedure", Token::Tok_Procedure},
            {"function",  Token::Tok_Function},
            {"constant",  Token::Tok_Constant},
            {"var",       Token::Tok_Var},
            {"subarray",  Token::Tok_Subarray},
            {"declare",   Token::Tok_Declare},
            {"begin",     Token::Tok_Begin},
            {"end",       Token::Tok_End},
            {"null",      Token::Tok_Null},
            {"case",      Token::Tok_Case},
            {"when",      Token::Tok_When},
            {"default",   Token::Tok_Default}};
    //    Id_Address := New_Symbol ("address");
    //    Id_Unchecked_Address := New_Symbol ("unchecked_address");
    //    Id_Subprg_Addr := New_Symbol ("subprg_addr");
    //    Id_Conv := New_Symbol ("conv");
    //    Id_Sizeof := New_Symbol ("sizeof");
    //    Id_Alignof := New_Symbol ("alignof");
    //    Id_Alloca := New_Symbol ("alloca");
    //    Id_Offsetof := New_Symbol ("offsetof");
public:
    Parser(const std::string sourceFile);

    void parse();
};

#endif // AVLC_PARSER_H
