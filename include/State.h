/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVLC_STATE_H
#define AVLC_STATE_H

#include <vector>
#include <unordered_map>

enum class Vhdl_Std {
	Vhdl_87, Vhdl_93c, Vhdl_93, Vhdl_00, Vhdl_02, Vhdl_08
};

struct FileState {
	std::vector<int> linePositions;
};

struct Options {
	// Standard accepted.
	Vhdl_Std standard = Vhdl_Std::Vhdl_93c;

	//Enable AMS-VHDL extensions
	bool AMS_Vhdl = false;

	//  Any note
	bool Msgid_Note = false;

	// Any warning
	bool Msgid_Warning = false;

	// Specific warnings

	// Design unit redefines another design unit.
	bool Warnid_Library = false;

	// Missing Xref in pretty print.
	bool Warnid_Missing_Xref = false;

	// No default binding for a component instantiation.
	bool Warnid_Default_Binding = false;

	// Unbound component.
	bool Warnid_Binding = false;

	// Vhdl93 reserved word is used as a vhdl87 identifier.
	bool Warnid_Reserved_Word = false;

	// Start of block comment ('/*') appears in a block comment.
	bool Warnid_Nested_Comment = false;

	// Weird use of parenthesis.
	bool Warnid_Parenthesis = false;

	// Generic of a vital entity is not a vital name.
	bool Warnid_Vital_Generic = false;

	// Delayed checks (checks performed at elaboration time).
	bool Warnid_Delayed_Checks = false;

	// Package body is not required but is analyzed.
	bool Warnid_Body = false;

	// An all/others specification does not apply, because there is no such
	// named entities.
	bool Warnid_Specs = false;

	// Incorrect use of universal value.
	bool Warnid_Universal = false;

	// Mismatch of bounds between actual and formal in a scalar port association
	bool Warnid_Port_Bounds = false;

	// Runtime error detected at analysis time.
	bool Warnid_Runtime_Error = false;

	// Signal assignment creates a delta cycle in a postponed process.
	bool Warnid_Delta_Cycle = false;

	// Declaration of a shared variable with a non-protected type.
	bool Warnid_Shared = false;

	// Emit a warning when a declaration is never used.
	// FIXME: currently only subprograms are handled.
	bool Warnid_Unused = false;

	// Violation of pure rules.
	bool Warnid_Pure = false;

	// Any error
	bool Msgid_Error = false;

	// Any fatal error
	bool Msgid_Fatal = false;

	// If set, a multi-bytes sequence can appear in a comment, ie, all characters except VT, CR, LF and FF are
	// allowed in a comment. Set by -C and //mb-comments
	bool Mb_Comment = false;

	// If set, relax rules about std library: working library can be std.
	bool Bootstrap = false;

	// Options -dX
	// -dp: disp tree after parsing
	bool Dump_Parse = false;

	// -ds: disp tree after semantic
	bool Dump_Sem = false;

	// -dc: disp tree after canon
	bool Dump_Canon = false;

	// -da: disp tree after annotation
	bool Dump_Annotate = false;

	// -do: don't dump origin expression but evaluated expressions.
	bool Dump_Origin_Flag = false;

	// //dall: makes -dX options to apply to all files.
	bool Dump_All = false;

	// -dstats: disp statistics.
	bool Dump_Stats = false;

	// -lX options: list tree as a vhdl file.

	// //lall option: makes -lX options to apply to all files
	bool List_All = false;

	// -lv: list verbose
	bool List_Verbose = false;

	// -ls: list tree after semantic.
	bool List_Sem = false;

	// -lc: list tree after canon.
	bool List_Canon = false;

	// -la: list tree after back-end annotation.
	bool List_Annotate = false;

	// -v: disp phase of compilation.
	bool Verbose = false;

	// TODO: add backend options

	// If not 0, do internal consistency and leaks check on the AST after analysis.
	unsigned int Check_Ast_Level = 0;

	bool Flag_Psl_Comment = false;
	bool Flag_Comment_Keyword = false;

	// If set to true, it means that analyze is done for elaboration. The purpose is to avoid spurious warning "will
	// be checked  at elaboration"
	bool Flag_Elaborate = false;

	// If set, a default aspect entity aspect might be an outdated unit. Used by ghdldrv. 
	bool Flag_Elaborate_With_Outdated = false;

	// Do not display parse and sem warnings.  Used during elaboration.
	bool Flag_Only_Elab_Warnings = false;

	// If set, explicit subprogram declarations take precedence over implicit declarations, even through use clauses.
	bool Flag_Explicit = false;

	// If set, use 'L.C' rule from VHDL02 to do default component binding.
	bool Flag_Syn_Binding = false;

	// If set, performs VITAL checks.
	bool Flag_Vital_Checks = true;

	// --time-resolution=X, Where X corresponds to:
	// fs => 'f'
	// ps => 'p'
	// ns => 'n'
	// us => 'u'
	// ms => 'm'
	// sec => 's'
	// min => 'M'
	// hr => 'h'
	char Time_Resolution = 'f';

	// If set, generate cross-references during sem.
	bool Flag_Xref = false;

	// If set, all the design units are analyzed in whole to do the simulation.
	bool Flag_Whole_Analyze = false;

	// If true, relax some rules: * the scope of an object declaration names start after the declaration, so that it 
	// is possible to use the old name in the default expression: 
	// constant x : xtype := x;
	bool Flag_Relaxed_Rules = false;

	//  --warn-error
	//  Turns warnings into errors.
	bool Warn_Error = false;

	//  If True, disp original source line and a caret indicating the column.
	bool Flag_Caret_Diagnostics = false;

	//  -fcolor-diagnostics,  -fno-color-diagnostics
	//  Enable colors in diagnostic messages. The default is auto, which turns  on when a terminal is detected on the
	// standard error.
	bool Flag_Color_Diagnostics = true;

	//  -fdiagnostics-show-option
	bool Flag_Diagnostics_Show_Option = true;
};

struct State {
	Options options;
	bool psl = false;
	std::vector<std::pair<std::string, FileState>> fileList;
};

#endif //AVLC_STATE_H
