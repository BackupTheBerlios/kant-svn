SUBDIRS = xeta util serialize

bin_PROGRAMS = ../xeta

___xeta_SOURCES = gtest.cpp\
			   xeta.cpp \
			   gxt/deptreepopup.cpp \
			   gxt/deptreeview.cpp \
			   gxt/menu.cpp \
			   gxt/packagespecbin.cpp \
			   gxt/widget.cpp


___xeta_CXXFLAGS = @GTKMM_CFLAGS@ -DLOCALEDIR=\"@datadir@/locale\" 
___xeta_LDADD = @GTKMM_LIBS@
___xeta_LDFLAGS = -lxeta -L$(top_builddir)/src/xeta/

