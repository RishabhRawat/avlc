---
layout: post
title: Finally setting up everything
---
After a bad day (6 hrs of invigilation) and a week of no laptop (sent for repair), I finally borrowed my friends laptop and set up jekyll and everything. Took more time than I thought ~~or had~~. 

I plan to use cmake not for the reasons you think but so that I can use [CLion](https://www.jetbrains.com/clion/). For all you emacs loving junkies try using a real IDE :P. Moreover I feel reading cmake files are way more easier but maybe thats just me. 

I started going through the ghdl source code, struggling through ADA (which is awefully similar to VHDL ~~I hate VHDL~~). In the src/ directory there a a number of basic libraries - 

* **table.ads**: parallel to std::vector
* **str_table.ads**: parallel to std::strings
* **types.ads**: some typedefs
* **version.ads**: duh...
* **options.ads**: optparse library
* **std_names.ads**: a bigass file for all vhdl keywords!!

and some more files which are way to ADAish for me to understand now.

I also went through some mails in ghdl mailing list archives *(Thanks a lot guys for maintaining that)*, and figured that I should read more about parsers and Abstract Syntax Tree and look into **src/vhdl/parser.adb** *(which is 9k loc file)*. #$%^ you guys. But still undeterred currently going through things like Backus-Naur forma and Abstract Syntax Trees. :/

