library_includedir=$(includedir)/xeta-1.0/xeta/xeta
library_include_HEADERS = config.h \
						  exceptions.h \
						  forwarddecl.h \
						  info.h \
						  infoimplsqlite.h \
						  md5.h \
						  packagespec.h \
						  transfer.h \
						  tree.h \
						  uui.d \
						  wget.h \
						  xeta_types.h \
## Add the others here

lib_LTLIBRARIES = libxeta.la

libxeta_la_SOURCES = config.cpp \
					 curl.cpp \
					 debug.cpp \
					 depparser.cpp \
					 exceptions.cpp \
					 gendeptree.cpp \
					 info.cpp \
					 infoimplsqlite.cpp \
					 md5.cpp \
					 packagespec.cpp \
					 transfer.cpp \
					 uuid.cpp \
					 wget.cpp

libxeta_la_CXXFLAGS = @SQLITE_CFLAGS@ @CURLPP_CFLAGS@ -pthread -I/usr/include/boost-1_32/
libxeta_la_LIBADD = @SQLITE_LIBS@ @CURLPP_LIBS@ -lboost_thread -lboost_filesystem
libxeta_la_LDFLAGS= -version-info $(XETA_LIBRARY_VERSION) -release $(XETA_RELEASE)

bin_PROGRAMS = dep_lang_test uuid_gen transfer_test md5sum

dep_lang_test_SOURCES = dep_lang_test.cpp
dep_lang_test_LDADD = -lxeta -L$(top_builddir)/src/xeta/

uuid_gen_SOURCES = uuid.cpp
uuid_gen_CXXFLAGS = -DUUIDGEN_TEST -I/usr/include/boost-1_32/

transfer_test_SOURCES = transfer.cpp 
transfer_test_CXXFLAGS = -DTRANSFER_TEST -pthread -I/usr/include/boost-1_32/
transfer_test_LDFLAGS = -lxeta

md5sum_SOURCES = md5.cpp
md5sum_CXXFLAGS = -DMD5SUM_TEST -I/usr/include/boost-1_32/
