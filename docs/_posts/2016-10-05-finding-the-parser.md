---
layout: post
title: Finding the parser
---
**"Wow that was complicated"**, that statemnet sums up exactly how I have been feeling about ghdl lately. I was trying to figure out how ghdl works and after a long time I realized that the main executable of ghdl is nothing by a huge script which determines what command is used to invoke other parts of ghdl.

Well let me step back and qualify that, apparently ghdl is made up of several subparts (simulator, compiler, runner, iamnotsure etc etc). The main ghdl executable spawns one of these other parts which exists as a different executable.

**TL;DR: GHDL executable is a startup script written in ada**

I cannot express the amount of pain I went through to understand this... *Why ADA!!! Use python for scripting pleaseeeee*. Okay now that I have expended my frustation, I can tell you about other things. So now I had to look for the next executables main, apparently there are some files called othro-XXXX-main.adb somewhere in ortho/XXXX directory which is the main program for the analyzer executable. This file makes a lot of sense (compared to option parsing script written in ADA). The compilation part is divided into frontend and backend (for llvm I believe). The parsing is done in the front part.

Now before I delve too deep into the parsing rules implemented in ADA, I wanted to learn a bit more about recursive descent parsing in general, so up next read through some theory!! I got copy of Parsing Techniques: [A Practical Guide](http://www.springer.com/gp/book/9780387202488), so see you guys... I should be a bit smarter by next time I post something
