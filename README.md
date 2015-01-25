=Polar Flowlink 

* Objective

The goal of this fork was to modify origin source code to read data 
from Polar RS300X.  Rather than original project, the program produce
an output which is a python struct.  This struct could be send data
to http://www.polarpersonaltrainer.com with script polarwritexml.py.

* Supported OS

GNU/Linux

* Requirements: 

For driver: libusb-dev, libudev-dev, libpq-dev

* How to compile

Firstly create bin directory in the project tree: mkdir bin
then run ``make'' in command line: make

* How to use

Put your RS300X on Polar Flowlink then execute :
sudo ./bin/polar > polardata
python polarwritexml.py

* Inclusion of files from other authors

Part of package includes hidapi project, licensed under GPLv3, BSD
and HIDAPI license. Terms under GPLv3 are chosen in this project.

* License

This work is licenced under GPL v3 or later. Please read LICENSE
for more information.

