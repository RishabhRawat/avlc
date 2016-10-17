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

Parser::Parser(const std::string sourceFile)
        : fileName(sourceFile), sourceFile(sourceFile) {}

void Parser::parse() {}

std::string Parser::ParseComment() {
    std::string commment;
    sourceFile.getline(comment, INT16_MAX);
    if (sourceFile.fail())
        throw SyntaxError(fileName, lineNumber, "Too long line, for gods sake dont "
                "write the whole world in a single "
                "line");
    sourceFile.unget();
    lastComment = commment;
    return commment;
}
Parser::Token Parser::getNextToken() {

    while (1) {
        int inputCharacter = sourceFile.get();
        switch (inputCharacter) {
            case -1:
                return Token::Tok_Eof;
            case '\r':
                break;
            case '\n':
                lineNumber++;
                break;
            case '+':
                return Token::Tok_Plus;
            case '-':
                if (sourceFile.get() == '-') {
                    switch (sourceFile.get()) {
                        case '#':
                            return Token::Tok_Line_Number;
                        case 'F':
                            return Token::Tok_File_Name;
                        case ' ':
                            ParseComment();
                            return Token::Tok_Comment;
                        default:
                            throw SyntaxError(fileName, lineNumber, "Invalid syntax for comments");_
                    }
                } else {
                    sourceFile.unget();
                    return Token::Tok_Minus;
                }
            case '/':
                if(sourceFile.get() == '=')
                    return Token::Tok_Not_Equal;
                else {
                    sourceFile.unget();
                    return Token::Tok_Div;
                }
            case '*':
                return Token::Tok_Star;
            case '#':
                return Token::Tok_Sharp;
            case '=':
                if (sourceFile.get()=='>')
                    return Token::Tok_Arrow;
                else {
                    sourceFile.unget();
                    return Token::Tok_Equal;
                }
            case '>':
                if (sourceFile.get()=='=')
                    return Token::Tok_Greater_Eq;
                else {
                    sourceFile.unget();
                    return Token::Tok_Greater;
                }
            case '<':
                if (sourceFile.get()=='=')
                    return Token::Tok_Less_Eq;
                else {
                    sourceFile.unget();
                    return Token::Tok_Less;
                }
            case '(':
                return Token::Tok_Left_Paren;
            case ')':
                return Token::Tok_Right_Paren;
            case '{':
                return Token::Tok_Left_Brace;
            case '}':
                return Token::Tok_Right_Brace;
            case '[':
                return Token::Tok_Left_Brack;
            case ']':
                return Token::Tok_Right_Brack;
            case ':':
                if (sourceFile.get()=='=')
                    return Token::Tok_Assign;
                else {
                    sourceFile.unget();
                    return Token::Tok_Colon;
                }
            case '.':
                if(sourceFile.get() == '.') {
                    if(sourceFile.get() == '.')
                        return Token::Tok_Elipsis;
                    else
                        throw SyntaxError(fileName, lineNumber, "Expecting ellipsis (...)");
                }
                else {
                    sourceFile.unget();
                    return Token::Tok_Colon;
                }
            case ';':
                return Token::Tok_Semicolon;
            case ',':
                return Token::Tok_Comma;
            case '@':
                return Token::Tok_Arob;
            case '\'':
                if (lastToken == Token::Tok_Ident)
                    return Token::Tok_Tick;
                else {

                }


            default:
                return Token::Tok_Eof;
        }
    }

    when
    ''
    ' => if Tok_Previous = Tok_Ident then return Tok_Tick;
    else Token_Number : = Character
    'Pos (Get_Char); C : = Get_Char;
    if
        C /= ''
    ' then Scan_Error("ending single quote expected");
    end
    if;
    return Tok_Num;
    end
    if;
    when
    '"' = > --"
            --
    Eat
    double quote.C : = Get_Char;
    Token_Idlen:
    = 0;
    loop Scan_Char(C);
    C:
    = Get_Char;
    exit
    when C = '"';
    --"
    end loop;
    return Tok_String;
    when
    '0'..
    '9' = > return Scan_Number(C);
    when
    'a'..
    'z' | 'A'..
    'Z' | '_' = > Token_Idlen : = 0;
    Token_Hash:
    = 0;
    loop Token_Idlen : = Token_Idlen + 1;
    Token_Ident(Token_Idlen) : = C;
    Token_Hash:
    = Token_Hash * 31 + Character
    'Pos (C); C : = Get_Char;
    exit
    when(C < 'A' or C > 'Z') and (C < 'a' or C > 'z') and
    (C < '0' or C > '9') and (C /= '_');
    end loop;
    Unget_Char;
    return Get_Ident_Token;
    when
    others =>
    Scan_Error("Bad character:"
               & Integer
    'Image (Character'Pos(C))
    &C);
    return Tok_Eof;
    end
    case;
    end loop;
    end Get_Token;
}
