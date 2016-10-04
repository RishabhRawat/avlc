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
    optionHandler.add_options("Main Options")("work", "use as work library", cxxopts::value<std::string>())(
        "workdir", "use for the file library", cxxopts::value<std::string>())("P,PATH", "add in the library path list",
                                                                              cxxopts::value<std::string>())(
        "std,std", "select vhdl 87/93/93c/00/02/08 standard",
        cxxopts::value<std::string>()->default_value("93c"))("vital-checks,vital-checks", "check VITAL restrictions")(
        "no-vital-checks,no-vital-checks", "do not check VITAL restrictions")("h,help",
                                                                              "do not check VITAL restrictions");

    optionHandler.add_options("Warnings")("warn-binding", "warns for component not bound")(
        "warn-reserved", "warns use of 93 reserved words in vhdl87")("warn-library",
                                                                     "warns for redefinition of a design unit")(
        "warn-vital-generic", "warns of non-vital generic names")("warn-delayed-checks",
                                                                  "warns for checks performed at elaboration")(
        "warn-body", "warns for not necessary package body")("warn-specs", "warns if a all/others spec does not apply")(
        "warn-unused", "warns if a subprogram is never used")("warn-error", "turns warnings into errors");

    optionHandler.add_options("Extensions:")("fexplicit,fexplicit", "give priority to explicitly declared operator")(
        "frelaxed-rules,frelaxed-rules", "relax some LRM rules")(
        "C,mb-comments", "allow multi-bytes chars in a comment")("bootstra", "allow --work=std")(
        "syn-binding", "use synthesis default binding rule")("fpsl,fpsl", "parse psl in comments");

    optionHandler.add_options("Compilation list:")("ls,ls", "after semantics")("lc,lc", "after canon")(
        "la,la", "after annotation")("lall", "-lX options apply to all files")("lv,lv", "verbose list")(
        "v,v", "discompilation stages");

    optionHandler.add_options("Compilation dump (choose only one)")("dp,dp", "dumptree after parse")(
        "ds,ds", "dumptree after semantics")("da,da", "dumptree after annotate")("dall",
                                                                                 "-dX options apply to all files");
  }

  inline void displayHelp() { std::cout << optionHandler.help(); }

  inline const cxxopts::OptionDetails &operator[](const std::string &option) const { return optionHandler[option]; }

  void parse(int &argc, char **&argv) {
    optionHandler.parse(argc, argv);
    std::string standard{optionHandler["std"].as<std::string>()};

    if (!(standard == "87" || standard == "93" || standard == "93c" || standard == "00" || standard == "02" ||
          standard == "08"))
      throw invalid_argument("standard can only be among 87, 93, 93c, 00, 02, 08");
  }
};

#endif // AVLC_OPTIONS_H
