noinst_HEADERS = debug.h\
				 example.h\
				 xml_exception.h\
				 xml_parser.h\
				 xml_path.h\
				 xml_serialize.h
				 
noinst_LIBRARIES = libserializexml.a

libserializexml_a_CXXFLAGS = @EXPAT_CFLAGS@
# libserializexml_a_LIBADD = @EXPAT_LIBS@
libserializexml_a_SOURCES =  xml_parser.cpp xml_exception.cpp

bin_PROGRAMS = example recursive

example_SOURCES  = example.cpp \
				   example_main.cpp

example_CXXFLAGS = @EXPAT_CFLAGS@ -pedantic -Wall
example_LDADD = @EXPAT_LIBS@ libserializexml.a

recursive_SOURCES  = recursive.cpp 

recursive_CXXFLAGS = @EXPAT_CFLAGS@ -pedantic -Wall
recursive_LDADD = @EXPAT_LIBS@ libserializexml.a

