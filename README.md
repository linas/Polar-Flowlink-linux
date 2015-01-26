=Polar Flowlink=

* Objective

This is a fork of the github samop/Polar-Flowlink-linux project,
meant to read data from the Polar RS300X.  The program that is
built, `bin/polar` produces an output which is a python struct.
This struct can be used to send data to
http://www.polarpersonaltrainer.com with script `polarwritexml.py.`

The FlowLink code here can connect to the Polar FT7, but currently
returns garbage data for this model.

* Supported OS

GNU/Linux

* Requirements:

The following pakcages need to be installed:
`libusb-dev, libudev-dev`

* How to compile

Create bin directory in the project tree: `mkdir bin`
then run `make` in command line: `make`

* How to use

Put your RS300X on the Polar Flowlink then execute :
```
sudo ./bin/polar > polardata
python polarwritexml.py
```

* Inclusion of files from other authors

Part of package includes hidapi project, licensed under GPLv3, BSD
and HIDAPI license. Terms under GPLv3 are chosen in this project.

* License

This work is licenced under GPL v3 or later. Please read LICENSE
for more information.

