EXTRA_PROGRAMS = 
bin_PROGRAMS = colortail

colortail_CPPFLAGS = \
  -I$(top_srcdir)/modules/shared_ptr

allsrc = \
  CfgFileParser.cc \
  ColorTail.cc \
  Colorizer.cc \
  Info.cc \
  OptionsParser.cc \
  TailFile.cc \
  Usage.cc \
  $(top_srcdir)/src/parser/expression.cpp

colortail_SOURCES = \
  $(allsrc) \
  main.cc 

EXTRA_DIST = README BUGS COPYING ChangeLog

SUBDIRS = example-conf $(WITH_TESTS)

