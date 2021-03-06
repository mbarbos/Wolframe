TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/clients/lib \
	-I$(BOOST_INCLUDE_DIR) \
	-I$(TOPDIR)/3rdParty/textwolf/include

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/clients/lib

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) \
	-lboost_program_options -lboost_system -lboost_filesystem \
	-lwolframe_client

# openssl
ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_DIRS += \
	$(OPENSSL_INCLUDE_DIRS)
	
INCLUDE_LDFLAGS += \
	$(OPENSSL_LIB_DIRS)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)
endif

CPP_OBJS = \
	utils.o \
	options.o

CPP_BINS = \
	wolframec$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0755 $(CPP_BINS) $(DESTDIR)$(bindir)

local_uninstall:
	-rm -f $(DESTDIR)$(bindir)/wolframec$(EXE)
	-rmdir $(DESTDIR)$(bindir)

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton/:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/clients/lib"

local_test:
ifeq ($(RUN_TESTS),1)
ifeq ($(WITH_EXPECT),1)
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(EXPECT) ./testWolframec
endif
endif

run:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframec 127.0.0.1 7661

runssl:
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframec -S --CA-cert-file=../../tests/SSL/CAchain.pem \
		--client-cert-file=../../tests/SSL/client.crt \
		--client-cert-key=../../tests/SSL/client.key \
		127.0.0.1 7961

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
