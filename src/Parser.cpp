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

void Parser::parse() {

  auto createDeclSymbol = [&](std::string symbol, Token keyword) {
    symbolTable[symbol] = new Symbol(
        symbol, new Name(new DeclKeyword(keyword), nullptr, nullptr));
  };

  createDeclSymbol("type", Token::Tok_Type);
  createDeclSymbol("type", Token::Tok_Type);
  createDeclSymbol("return", Token::Tok_Return);
  createDeclSymbol("if", Token::Tok_If);
  createDeclSymbol("then", Token::Tok_Then);
  createDeclSymbol("else", Token::Tok_Else);
  createDeclSymbol("elsif", Token::Tok_Elsif);
  createDeclSymbol("loop", Token::Tok_Loop);
  createDeclSymbol("exit", Token::Tok_Exit);
  createDeclSymbol("next", Token::Tok_Next);
  createDeclSymbol("signed", Token::Tok_Signed);
  createDeclSymbol("unsigned", Token::Tok_Unsigned);
  createDeclSymbol("float", Token::Tok_Float);
  createDeclSymbol("is", Token::Tok_Is);
  createDeclSymbol("of", Token::Tok_Of);
  createDeclSymbol("all", Token::Tok_All);
  createDeclSymbol("not", Token::Tok_Not);
  createDeclSymbol("abs", Token::Tok_Abs);
  createDeclSymbol("or", Token::Tok_Or);
  createDeclSymbol("and", Token::Tok_And);
  createDeclSymbol("xor", Token::Tok_Xor);
  createDeclSymbol("mod", Token::Tok_Mod);
  createDeclSymbol("rem", Token::Tok_Rem);
  createDeclSymbol("array", Token::Tok_Array);
  createDeclSymbol("access", Token::Tok_Access);
  createDeclSymbol("record", Token::Tok_Record);
  createDeclSymbol("union", Token::Tok_Union);
  createDeclSymbol("end", Token::Tok_End);
  createDeclSymbol("boolean", Token::Tok_Boolean);
  createDeclSymbol("enum", Token::Tok_Enum);
  createDeclSymbol("external", Token::Tok_External);
  createDeclSymbol("private", Token::Tok_Private);
  createDeclSymbol("public", Token::Tok_Public);
  createDeclSymbol("local", Token::Tok_Local);
  createDeclSymbol("procedure", Token::Tok_Procedure);
  createDeclSymbol("function", Token::Tok_Function);
  createDeclSymbol("constant", Token::Tok_Constant);
  createDeclSymbol("var", Token::Tok_Var);
  createDeclSymbol("subarray", Token::Tok_Subarray);
  createDeclSymbol("declare", Token::Tok_Declare);
  createDeclSymbol("begin", Token::Tok_Begin);
  createDeclSymbol("end", Token::Tok_End);
  createDeclSymbol("null", Token::Tok_Null);
  createDeclSymbol("case", Token::Tok_Case);
  createDeclSymbol("when", Token::Tok_When);
  createDeclSymbol("default", Token::Tok_Default);

  lineNumber = 1;
  position = 1;

  pushScope();
  popScope();
}

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
    tokenNumber = 0;
    while (sourceFile.peek() != '#') {
      int character = sourceFile.get();

      int baseChar;
      if (character >= 'A' && character <= 'F')
        baseChar = 'A';
      else if (character >= 'a' && character <= 'f')
        baseChar = 'a';
      else if (character >= '0' && character <= '9')
        baseChar = '0';
      else if (character == '_') // This is not there in ghdl!!
        continue;
      else
        throw SyntaxError(fileName, lineNumber,
                          "invalid character in numeric literal");
      tokenNumber = tokenNumber * base + (character - baseChar);
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
    tokenDouble = std::stod(number1);
    return Token::Tok_Float_Num;
  } else {
    tokenNumber = std::stoi(number1);
    return Token::Tok_Num;
  }
}

std::string Parser::parseComment() {
  std::string comment;
  getline(sourceFile, comment);
  if (sourceFile.fail())
    throw SyntaxError(fileName, lineNumber, "Too long line, for gods sake "
                                            "don't write the whole world in a "
                                            "single line");
  sourceFile.unget();
  tokenString = comment;
  return comment;
}

Parser::Token Parser::getNextToken() {

  int inputCharacter = sourceFile.get();
  switch (inputCharacter) {
  case -1:
    return Token::Tok_Eof;
  case '\r':
    return getNextToken();
  case '\n':
    lineNumber++;
    return getNextToken();
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
        throw SyntaxError(fileName, lineNumber, "Invalid syntax for comments");
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
      tokenString = std::string(1, sourceFile.get());
      if (sourceFile.get() != '\'')
        throw SyntaxError(fileName, lineNumber, "ending single quote expected");
      else
        return Token::Tok_Num;
    }
  case '\"':
    tokenString = "";
    while (sourceFile.peek() != '\"') {
      char nextChar = static_cast<char>(sourceFile.get());
      if (nextChar != '\\') {
        switch (sourceFile.get()) {
        case 'n':
          tokenString.push_back('\n');
          break;
        case 'r':
          tokenString.push_back('\r');
          break;
        case '\'':
          tokenString.push_back('\'');
          break;
        case '\"':
          tokenString.push_back('\"');
          break;
        default:
          throw SyntaxError(fileName, lineNumber,
                            "bad character sequence at \\ ");
        }
      } else
        tokenString.push_back(nextChar);
    }
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

void Parser::pushScope() { scopeStack.push(new Scope()); }

void Parser::popScope() {
  auto scope = scopeStack.top();
  scopeStack.pop();
  for (Symbol *symbol : *scope) {
    auto name = symbol->name;
    symbol->name = name->OuterInterpretation;
    delete name;
  }
  delete scope;
}

Parser::DeclKeyword::DeclKeyword(Parser::Token keyword) : keyword(keyword) {}

Parser::DeclType::DeclType(Parser::StorageType declStorage, bool declDefined,
                           Parser::Node *declDtype)
    : declStorage(declStorage), declDefined(declDefined), declDtype(declDtype) {
}

Parser::DeclParam::DeclParam(Parser::StorageType declStorage, bool declDefined,
                             Parser::Node *declDtype)
    : declStorage(declStorage), declDefined(declDefined), declDtype(declDtype) {
}

Parser::Name::Name(Parser::Node *interpretation,
                   Parser::Name *OuterInterpretation,
                   Parser::Scope *currentScope)
    : interpretation(interpretation), OuterInterpretation(OuterInterpretation),
      currentScope(currentScope) {}

Parser::Symbol::Symbol(const std::string &identity, Parser::Name *name)
    : identity(identity), name(name) {}

