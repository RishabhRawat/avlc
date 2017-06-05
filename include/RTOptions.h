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
#ifndef AVLC_RTOPTIONS_H
#define AVLC_RTOPTIONS_H

#include "cxxopts.hpp"
#include <iostream>
#include <stdexcept>

class RTOptions {
  cxxopts::Options optionHandler;

public:
  RTOptions() : optionHandler{"avlc", "Another VHDL language compiler"} {
    // clang-format off
        optionHandler.add_options()
                ("h,help", "display this help")
                ("assert-level",
                 "stop simulation if assert above assert-level (levels are note,warning,error,failure,none)",
                 cxxopts::value<std::string>())
                ("ieee-asserts",
                 "enable or disable asserts from IEEE (policies are enable,disable,disable-at-0)",
                 cxxopts::value<std::string>())
                ("stop-time",
                 "stop the simulation at time arg (arg is expressed as a time value, without spaces: 1ns, ps...",
                 cxxopts::value<std::string>())
                ("stop-delta", "stop the simulation cycle after X delta", cxxopts::value<std::string>())
                ("expect-failure", "invert exit status")
                ("no-run", "do not simulate, only elaborate")
                ("unbuffered",
                 "disable buffering on stdout, stderr and files opened in write or append mode (TEXTIO).")
                ("wave-opt-file", "read a wave option file", cxxopts::value<std::string>());

        optionHandler.add_options("additional")
                ("has-feature", "test presence of feature \"arg\"")
                ("list-features", "display the list of features");
//        Grt.Hooks.Call_Help_Hooks;
        optionHandler.add_options("trace")
                ("disp-time", "disp time as simulation advances")
                ("trace-signals", "disp signals after each cycle")
                ("trace-processes", "disp process name before each cycle")
                ("stats", "display run-time statistics");
        optionHandler.add_options("debug")
                ("disp-order", "disp signals order")
                ("disp-sources", "disp sources while displaying signals")
                ("disp-sig-types", "disp signal types")
                ("disp-signals-map", "disp map bw declared sigs and internal sigs")
                ("disp-signals-table", "disp internal signals")
                ("checks", "do internal checks after each process run")
                ("activity", "watch activity of \"arg\"signals (LEVEL is all, min (default) or none (unsafe)");
        //clang-format on
    }


    inline void displayHelp() {
        std::cout << optionHandler.help(
                {"", "Warnings", "additional", "trace", "debug"});
    }

    inline const cxxopts::OptionDetails &operator[](const std::string &option) const { return optionHandler[option]; }

    void parse(int &argc, char **&argv) {
        optionHandler.parse(argc, argv);
    }
};

#endif // AVLC_RTOPTIONS_H
