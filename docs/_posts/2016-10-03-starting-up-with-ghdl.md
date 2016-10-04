---
layout: post
title: Starting up with GHDL
---
It has been a bad couple of days. My laptop has been rendered unusable cuz display issues, So I have been relying on my lab machine to work on all my projects. Placements and DDP are both coming close giving me little time to do something else.

I ended up using cxxopts as my option parser for a variety of reasons, though it does force me to use its own "option conventions" *(I don't know the right word for that :P)*. And I sat down and rewrote the **options.adb** file only to realise these are not the main ghdl options, but the analyzer options. 

So I went looking where is the main function of ghdl ~~*(Ada sucks cuz there ain't no main)* ~~. Luckily *hurray to coding sane conventions*, I found the grt (ghdl run time) folder which contains the main code for ghdl runtime (and ghdl_main). I have translated the options for now, and now am ready to wade into the deep waters of ghdl.
