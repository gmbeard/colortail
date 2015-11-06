SUBDIRS = example-conf tests

bin_PROGRAMS = colortail

colortail_CPPFLAGS = -I$(top_srcdir)/modules/shared_ptr

colortail_SOURCES = \
CfgFileParser.cc \
ColorTail.cc \
Colorizer.cc \
Info.cc \
OptionsParser.cc \
TailFile.cc \
Usage.cc \
main.cc \
$(top_srcdir)/src/parser/expression.cpp

EXTRA_DIST = README BUGS COPYING ChangeLog
