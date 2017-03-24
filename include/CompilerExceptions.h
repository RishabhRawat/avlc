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
#include <exception>
#include <string>
#include <iostream>
#include "iir_types.h"

class SyntaxError: std::runtime_error {
public:
    SyntaxError(std::string file, unsigned int line, const char* error) noexcept
            :std::runtime_error(file + std::to_string(line) + error) { }
};

class CompilationError: std::runtime_error {
public:
    CompilationError(const std::string& error) noexcept
            :std::runtime_error("[Compiler Exception] " + error) { }

    CompilationError(Location_Type loc, const char* error) noexcept
            :std::runtime_error(
            "[Compiler Exception] " + loc.filePath.string() + ": Line " + loc.Line + ", Col " + loc.Pos + ": "
                    + error) { }
};

class SemanticError: std::runtime_error {
public:
    SemanticError(const std::string& error) noexcept
            :std::runtime_error("[Semantic Error] " + error) { }

    SemanticError(Location_Type loc, const char* error) noexcept
            :std::runtime_error(
            "[Semantic Error] " + loc.filePath.string() + ": Line " + loc.Line + ", Col " + loc.Pos + ": "
                    + error) { }
};

inline void Warning(const std::string message, const bool warnSwitch = true) {
	if (warnSwitch)
		std::cout<<message<<"\n";
}

#endif // AVLC_COMPILEREXCEPTIONS_H
