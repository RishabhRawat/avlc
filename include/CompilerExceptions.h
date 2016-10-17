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

#ifndef AVLC_COMPILEREXCEPTIONS_H
#define AVLC_COMPILEREXCEPTIONS_H

#include <cstdio>
#include <cstring>
#include <exception>

class SyntaxError : std::exception {
  char fileName[50];
  unsigned int lineNumber;
  char errorMessage[100];
  char completeErrorMessage[200];

public:
  SyntaxError(std::string file, unsigned int line, const char *error) noexcept
      : lineNumber(line) {
    unsigned int i = 0;
    strncpy(fileName, file, 49);
    fileName[49] = '\0';
    strncpy(errorMessage, error, 99);
    errorMessage[99] = '\0';
  }

  const char *what() const noexcept override {
    if (sprintf(completeErrorMessage, "%s:%d: %s", fileName, lineNumber,
                errorMessage) > 0)
      return completeErrorMessage;
    else
      return "Error in throwing error, god what the hell is wrong with me";
  }
};

#endif // AVLC_COMPILEREXCEPTIONS_H