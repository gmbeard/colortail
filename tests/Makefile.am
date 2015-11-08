bin_PROGRAMS = colortail_tests

colortail_tests_SOURCES = \
  $(top_srcdir)/CfgFileParser.cc \
  $(top_srcdir)/ColorTail.cc \
  $(top_srcdir)/Colorizer.cc \
  $(top_srcdir)/Info.cc \
  $(top_srcdir)/OptionsParser.cc \
  $(top_srcdir)/TailFile.cc \
  $(top_srcdir)/Usage.cc \
  $(top_srcdir)/src/parser/expression.cpp \
  main.cpp \
  Colorizer_tests.cpp \
  TailFile_tests.cpp \
  Parser_tests.cpp

colortail_tests_CPPFLAGS = \
  -I$(top_srcdir)/modules/catch/include \
  -I$(top_srcdir)/modules/shared_ptr \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src \
  -DCONF_DIR="$(topsrc_dir)/example-conf" \
  -DUNIT_TESTING=1


