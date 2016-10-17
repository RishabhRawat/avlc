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

// FIXME: What if sourcefile sets eof at some point!!
Parser::Token Parser::parseNumericLiteral() {
  int initialDigit = sourceFile.get();
  if (initialDigit < 0 || !std::isdigit(static_cast<char>(initialDigit)))
    throw std::runtime_error("Wrong call to Parser::parseNumericLiteral, "
                             "ensure that the first character is a numeric "
                             "literal");
  std::string number1(1, static_cast<char>(initialDigit));

  while (isdigit(sourceFile.peek()) || sourceFile.peek() == '_') {
    if (isdigit(sourceFile.peek()))
      number1.push_back(static_cast<char>(sourceFile.get()));
  }

  if (sourceFile.peek() == '#') {
    int base = std::stoi(number1);
    lastNumber = 0;
    while (sourceFile.peek() != '#') {
      int character = sourceFile.get();

      int baseChar;
      if (character >= 'A' && character <= 'F')
        baseChar = 'A';
      else if (character >= 'a' && character <= 'f')
        baseChar = 'a';
      else if (character >= '0' && character <= '9')
        baseChar = '0';
      else
        throw SyntaxError(fileName, lineNumber,
                          "invalid character in numeric literal");
      lastNumber = lastNumber * base + (character - baseChar);
    }
    return Token::Tok_Num;

  } else if (sourceFile.peek() == '.') {
    while (isdigit(sourceFile.peek()))
      number1.push_back(static_cast<char>(sourceFile.get()));
    if (sourceFile.peek() == 'e' || sourceFile.peek() == 'E') {
      number1.push_back('e');
      while (isdigit(sourceFile.peek()))
        number1.push_back(static_cast<char>(sourceFile.get()));
    } else
      SyntaxError(fileName, lineNumber, "Invalid numeric literal");
    lastDouble = std::stod(number1);
    return Token::Tok_Float_Num;
  } else {
    lastNumber = std::stoi(number1);
    return Token::Tok_Num;
  }
}

std::string Parser::parseComment() {
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
          parseComment();
          return Token::Tok_Comment;
        default:
          throw SyntaxError(fileName, lineNumber,
                            "Invalid syntax for comments");
        }
      } else {
        sourceFile.unget();
        return Token::Tok_Minus;
      }
    case '/':
      if (sourceFile.get() == '=')
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
      if (sourceFile.get() == '>')
        return Token::Tok_Arrow;
      else {
        sourceFile.unget();
        return Token::Tok_Equal;
      }
    case '>':
      if (sourceFile.get() == '=')
        return Token::Tok_Greater_Eq;
      else {
        sourceFile.unget();
        return Token::Tok_Greater;
      }
    case '<':
      if (sourceFile.get() == '=')
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
      if (sourceFile.get() == '=')
        return Token::Tok_Assign;
      else {
        sourceFile.unget();
        return Token::Tok_Colon;
      }
    case '.':
      if (sourceFile.get() == '.') {
        if (sourceFile.get() == '.')
          return Token::Tok_Elipsis;
        else
          throw SyntaxError(fileName, lineNumber, "Expecting ellipsis (...)");
      } else {
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
        // TODO: Implement character??
        throw std::runtime_error("unimplemented single quotes!!");
        if (sourceFile.get() != '\'')
          throw SyntaxError(fileName, lineNumber,
                            "ending single quote expected");
        else
          return Token::Tok_Num;
      }
    case '\"':
      // TODO: Implement double code handler??
      while (sourceFile.get() != '\"')
        if (sourceFile.eof())
          throw throw SyntaxError(fileName, lineNumber,
                                  "ending double quote expected");
      return Token::Tok_String;
    default:
      if (inputCharacter >= '0' && inputCharacter <= '9') {
        sourceFile.unget();
        return parseNumericLiteral();
      } else if ((inputCharacter >= 'A' && inputCharacter <= 'z') ||
                 (inputCharacter >= 'A' && inputCharacter <= 'Z') ||
                 inputCharacter == '_') {
        // TODO: Implement character??
        throw std::runtime_error("unimplemented single quotes!!");

      } else
        throw SyntaxError(fileName, lineNumber,
                          "invalid character \'" + inputCharacter + '\'');
    }
  }
}
