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
  return commment;
}

// procedure Scan_Comment
// is
//        C :
// Character;
// begin
//        Token_Idlen : = 0;
// loop
//        C : = Get_Char;
// exit when
// C = CR or C = LF;
// Token_Idlen : = Token_Idlen + 1;
// Token_Ident (Token_Idlen) : = C;
// end loop;
// Unget_Char;
// end Scan_Comment;

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
          return Token::Tok_
        }
      } else {
        sourceFile.unget();
        break;
      }
    C:
      = Get_Char;
      if
        C = '#' then return Tok_Line_Number;
      elsif C = 'F' then Scan_Comment;
      return Tok_File_Name;
      elsif C = ' ' then Scan_Comment;
      return Tok_Comment;
      else Scan_Error("bad comment");
      end if;
      else Unget_Char;
      return Tok_Minus;
      end if;
    default:
      return Token::Tok_Eof;
    }
  }

case C is when NUL = > return Tok_Eof; when ' ' | HT = > null;
    when LF = > Lineno:
  = Lineno + 1;
C:
  = Get_Char;
  if
    C /= CR then goto Again;
  end if;
  when CR = > Lineno : = Lineno + 1;
C:
  = Get_Char;
  if
    C /= LF then goto Again;
  end if;
  when '+' = > return Tok_Plus;
  when '-' = > C : = Get_Char;
  if
    C = '-' then C : = Get_Char;
  if
    C = '#' then return Tok_Line_Number;
  elsif C = 'F' then Scan_Comment;
  return Tok_File_Name;
  elsif C = ' ' then Scan_Comment;
  return Tok_Comment;
  else Scan_Error("bad comment");
  end if;
  else Unget_Char;
  return Tok_Minus;
  end if;
  when '/' = > C : = Get_Char;
  if
    C = '=' then return Tok_Not_Equal;
  else
    Unget_Char;
  return Tok_Div;
  end if;
  when '*' = > return Tok_Star;
  when '#' = > return Tok_Sharp;
  when '=' = > C : = Get_Char;
  if
    C = '>' then return Tok_Arrow;
  else
    Unget_Char;
  return Tok_Equal;
  end if;
  when '>' = > C : = Get_Char;
  if
    C = '=' then return Tok_Greater_Eq;
  else
    Unget_Char;
  return Tok_Greater;
  end if;
  when '(' = > return Tok_Left_Paren;
  when ')' = > return Tok_Right_Paren;
  when '{' = > return Tok_Left_Brace;
  when '}' = > return Tok_Right_Brace;
  when '[' = > return Tok_Left_Brack;
  when ']' = > return Tok_Right_Brack;
  when '<' = > C : = Get_Char;
  if
    C = '=' then return Tok_Less_Eq;
  else
    Unget_Char;
  return Tok_Less;
  end if;
  when ':' = > C : = Get_Char;
  if
    C = '=' then return Tok_Assign;
  else
    Unget_Char;
  return Tok_Colon;
  end if;
  when '.' = > C : = Get_Char;
  if
    C = '.' then C : = Get_Char;
  if
    C = '.' then return Tok_Elipsis;
  else
    Scan_Error("'...' expected");
  end if;
  else Unget_Char;
  return Tok_Dot;
  end if;
  when ';' = > return Tok_Semicolon;
  when ',' = > return Tok_Comma;
  when '@' = > return Tok_Arob;
  when
    ''
    ' => if Tok_Previous = Tok_Ident then return Tok_Tick;
  else Token_Number : = Character
    'Pos (Get_Char); C : = Get_Char;
  if
    C /= ''
    ' then Scan_Error("ending single quote expected");
  end if;
  return Tok_Num;
  end if;
  when '"' = > --"
                 --Eat double quote.C : = Get_Char;
Token_Idlen:
  = 0;
  loop Scan_Char(C);
C:
  = Get_Char;
  exit when C = '"';
  --"
      end loop;
  return Tok_String;
  when '0'..'9' = > return Scan_Number(C);
  when 'a'..'z' | 'A'..'Z' | '_' = > Token_Idlen : = 0;
Token_Hash:
  = 0;
  loop Token_Idlen : = Token_Idlen + 1;
  Token_Ident(Token_Idlen) : = C;
Token_Hash:
  = Token_Hash * 31 + Character
    'Pos (C); C : = Get_Char;
  exit when(C < 'A' or C > 'Z') and (C < 'a' or C > 'z') and
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
    end case;
    end loop;
    end Get_Token;
}