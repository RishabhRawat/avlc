/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat  <name of author>
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

#ifndef AVLC_OPTIONS_H
#define AVLC_OPTIONS_H

#include "cxxopts.hpp"
#include <iostream>
#include <stdexcept>

class Options {
    cxxopts::Options optionHandler;

  public:
    Options() : optionHandler{"avlc", "Another VHDL language compiler"} {
        auto addMainOption = optionHandler.add_options("Main Options");
        addMainOption("work", "use as work library", cxxopts::value<std::string>());
        addMainOption("workdir", "use for the file library", cxxopts::value<std::string>());
        addMainOption("P,PATH", "add in the library path list", cxxopts::value<std::string>());
        addMainOption("std", "select vhdl 87/93/93c/00/02/08 standard",
                      cxxopts::value<std::string>()->default_value("93c"));
        addMainOption("vital-checks", "check VITAL restrictions");
        addMainOption("no-vital-checks", "do not check VITAL restrictions");
        addMainOption("h,help", "display this help");

        auto addWarnings = optionHandler.add_options("Warnings");
        addWarnings("warn-binding", "warns for component not bound");
        addWarnings("warn-reserved", "warns use of 93 reserved words in vhdl87");
        addWarnings("warn-library", "warns for redefinition of a design unit");
        addWarnings("warn-vital-generic", "warns of non-vital generic names");
        addWarnings("warn-delayed-checks", "warns for checks performed at elaboration");
        addWarnings("warn-body", "warns for not necessary package body");
        addWarnings("warn-specs", "warns if a all/others spec does not apply");
        addWarnings("warn-unused", "warns if a subprogram is never used");
        addWarnings("warn-error", "turns warnings into errors");

        auto addExtensions = optionHandler.add_options("Extensions");
        addExtensions("fexplicit", "give priority to explicitly declared operator");
        addExtensions("frelaxed-rules", "relax some LRM rules");
        addExtensions("C,mb-comments", "allow multi-bytes chars in a comment");
        addExtensions("bootstrap", "allow --work=std");
        addExtensions("syn-binding", "use synthesis default binding rule");
        addExtensions("fpsl", "parse psl in comments");

        auto addCompilationList = optionHandler.add_options("Compilation list");
        addCompilationList("ls", "after semantics");
        addCompilationList("lc", "after canon");
        addCompilationList("la", "after annotation");
        addCompilationList("lall", "-lX options apply to all files");
        addCompilationList("lv", "verbose list");
        addCompilationList("ld", "display compilation stages");

        auto addCompilationDump = optionHandler.add_options("Compilation dump");
        addCompilationDump("dp", "dumptree after parse");
        addCompilationDump("ds", "dumptree after semantics");
        addCompilationDump("da", "dumptree after annotate");
        addCompilationDump("dall", "-dX options apply to all files");
    }

    inline void displayHelp() {
        std::cout << optionHandler.help(
            {"Main Options", "Warnings", "Extensions", "Compilation list", "Compilation dump"});
    }

    inline const cxxopts::OptionDetails &operator[](const std::string &option) const { return optionHandler[option]; }

    void parse(int &argc, char **&argv) {
        optionHandler.parse(argc, argv);
        std::string standard{optionHandler["std"].as<std::string>()};

        if (!(standard == "87" || standard == "93" || standard == "93c" || standard == "00" || standard == "02" ||
              standard == "08"))
            throw std::invalid_argument("standard can only be among 87, 93, 93c, 00, 02, 08");
    }
};

#endif // AVLC_OPTIONS_H
