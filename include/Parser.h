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
#include "Nodes.h"
#include <algorithm>
#include <bits/unordered_set.h>
#include <fstream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

class Parser {

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

  enum class NodeType {
    Decl_Keyword,
    Decl_Type,
    Decl_Param,
    Node_Function,
    Node_Procedure,
    Node_Object,
    Node_Field,
    Node_Lit,
    Type_Boolean,
    Type_Enum,
    Type_Unsigned,
    Type_Signed,
    Type_Float,
    Type_Array,
    Type_Subarray,
    Type_Access,
    Type_Record,
    Type_Union
  };

  enum StorageType { External, Public, Private, Local };

  struct Symbol;

  using Scope = std::vector<Symbol *>;
  using ScopeStack = std::stack<Scope *>;

  ScopeStack scopeStack;

  struct Node {};

  using Identity = void *;

  struct Name {
    Node *interpretation;
    Name *OuterInterpretation;
    Scope *currentScope;

    Name(Node *interpretation, Name *OuterInterpretation, Scope *currentScope);
  };

  struct Symbol {
    std::string identity;
    Name *name;

    Symbol(const std::string &identity, Name *name);
  };

  struct DeclKeyword : Node {
    Token keyword;

    DeclKeyword(Token keyword);
  };

  struct DeclType : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;

    DeclType(StorageType declStorage, bool declDefined, Node *declDtype);
  };

  struct DeclParam : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;
    Symbol *paramName;
    O_Dnode *paramNode;
    Node *next; // TODO: FIX
  };

  /*
      case Kind is
                 when Node_Lit =>
                    --  Name of the literal.
                    Lit_Name : O_Ident;
                    --  Enum literal
                    Lit_Cnode : O_Cnode;
                    --  Next literal for the type.
                    Lit_Next : Node_Acc;
                 when others =>
                    null;
              end case;
  */
  struct NodeFunction : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;
    Symbol *subprgName;
    Node *subprgParams;
    O_Dnode *Subprg_Node;
  };
  struct NodeProcedure : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;
    Symbol *subprgName;
    Node *subprgParams;
    O_Dnode *Subprg_Node;
  };
  struct NodeObject : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;
    std::string objName;
    O_Dnode *objNode;
  };
  struct NodeField : Node {
    Symbol fieldIdent;
    O_Fnode fieldFnode;
    Node *fieldType;
    Node *fieldNext;
    Node *fieldHashNext;
  };
  struct NodeLit : Node {
    StorageType declStorage;
    bool declDefined;
    Node *declDtype;
    std::string litName;
    O_Cnode *litCnode;
    Node *litNext;
  };
  struct TypeBoolean : Node {
    O_Tnode *typeNode;
    Node *enumLits;
  };
  struct TypeEnum : Node {
    O_Tnode *typeNode;
    Node *enumLits;
  };
  struct TypeUnsigned : Node {
    O_Tnode *typeNode;
  };
  struct TypeSigned : Node {
    O_Tnode *typeNode;
  };
  struct TypeFloat : Node {
    O_Tnode *typeNode;
  };
  struct TypeArray : Node {
    O_Tnode *typeNode;
    Node *arrayIndex;
    Node *arrayElement;
  };
  struct TypeSubarray : Node {
    O_Tnode *typeNode;
    Node *subarrayBase;
  };
  struct TypeAccess : Node {
    O_Tnode *typeNode;
    Node *accessDType;
  };
  struct TypeRecord : Node {
    O_Tnode *typeNode;
    Node *Record_Union_Fields;
    SymbolTable *Record_Union_Map; // TODO: Fix
  };

  struct TypeUnion : Node {
    O_Tnode *typeNode;

    Node *Record_Union_Fields;
    SymbolTable *Record_Union_Map; // TODO: Fix
  };

  const std::string fileName;
  std::ifstream sourceFile;
  unsigned int lineNumber = 0;
  unsigned int position = 0;

  Token lastToken;
  // TODO: Replace this using variant
  std::string tokenString;
  double tokenDouble;
  long tokenNumber;

  // TODO: Rename these
  Symbol *Id_Address = new Symbol("address", nullptr);
  Symbol *Id_Unchecked_Address = new Symbol("unchecked_address", nullptr);
  Symbol *Id_Subprg_Addr = new Symbol("subprg_addr", nullptr);
  Symbol *Id_Conv = new Symbol("conv", nullptr);
  Symbol *Id_Sizeof = new Symbol("sizeof", nullptr);
  Symbol *Id_Alignof = new Symbol("alignof", nullptr);
  Symbol *Id_Alloca = new Symbol("alloca", nullptr);
  Symbol *Id_Offsetof = new Symbol("offsetof", nullptr);

  Token getNextToken();

  std::string parseComment();

  using SymbolTable = std::unordered_map<std::string, Symbol *>;

  SymbolTable symbolTable;

  inline Symbol *getFromSymbolTable(const std::string key) {
    std::string lowerKey;
    for (char k : key) {
      lowerKey.push_back(tolower(k));
    }
    return symbolTable[lowerKey];
  }

  void pushScope();

public:
  Parser(const std::string sourceFile);

  void parse();

  Token parseNumericLiteral();

  void popScope();
};

#endif // AVLC_PARSER_H
