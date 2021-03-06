TOPDIR = ..

SUBDIRS = libwolframe libwfsingleton libwolframe_serialize libwolframe_database libwolframe_langbind modules

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

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

LOCAL_STATIC_LIB = \
	libwolframed.a

PROVIDER_OBJS = \
	provider/Authentication.o \
	provider/Authorization.o \
	provider/Audit.o \
	provider/AAAAproviderImpl.o \
	provider/AAAAproviderConfig.o \
	provider/DBproviderImpl.o \
	provider/DBproviderConfig.o \
	provider/procProviderConfig.o \
	provider/procProvider.o

PROGRAM_OBJS = \
	prgbind/normalizeProgram.o \
	prgbind/ddlProgram.o \
	prgbind/programLibrary.o

LOCAL_STATIC_LIB_OBJS = \
	$(PROVIDER_OBJS) \
	$(PROGRAM_OBJS) \
	wolframedCommandLine.o \
	wolfilterCommandLine.o \
	wolfilterIostreamFilter.o \
	wolfwizardCommandLine.o \
	mainConnectionHandler.o \
	contentOnlyProtocolHandler.o \
	moduleDirectory.o \
	moduleLoader.o \
	serviceConfig.o \
	serviceBanner.o \
	appConfig.o \
	jobScheduler.o \
	acceptor.o \
	serverEndpoint.o \
	connection.o \
	serverConfig.o \
	loggerConfig.o \
	server.o

#SSL
ifeq ($(WITH_SSL),1)
LOCAL_STATIC_LIB_OBJS += \
	SSLcertificateInfo.o
endif


CPP_OBJS = \
	appProperties.o \
	appConfigDefinition.o \
	$(LOCAL_STATIC_LIB_OBJS) \
	wolframeHandler.o \
	posixMain.o

CPP_BINS = \
	wolframed$(EXE) \
	wolfilter$(EXE) \
	wolfwizard$(EXE)


-include $(TOPDIR)/makefiles/gmake/sub.mk

# set default module load directory for configuration option 'directory' in 'LoadModules'
ifdef DEFAULT_MODULE_LOAD_DIR
INCLUDE_CXXFLAGS += -DDEFAULT_MODULE_LOAD_DIR=$(DEFAULT_MODULE_LOAD_DIR)
endif

# set default main configuration file location
ifdef DEFAULT_MAIN_CONFIGURATION_FILE
INCLUDE_CXXFLAGS += -DDEFAULT_MAIN_CONFIGURATION_FILE=$(DEFAULT_MAIN_CONFIGURATION_FILE)
endif

libwfsingleton: libwolframe
modules: libwfsingleton libwolframe libwolframe_serialize libwolframe_langbind
libwolframe_serialize: libwolframe
libwolframe_langbind: libwolframe libwolframe_serialize libwolframe_database
libwolframe_database: libwfsingleton libwolframe

wolframed$(EXE): libwfsingleton libwolframe libwolframe_serialize libwolframe_langbind libwolframe_database

wolframed$(EXE): wolframed.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

wolfilter$(EXE): libwolframe libwolframe_serialize libwolframe_langbind libwolframe_database

wolfilter$(EXE): wolfilter.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

wolfwizard$(EXE): libwolframe libwolframe_serialize libwolframe_langbind libwolframe_database

wolfwizard$(EXE): wolfwizard.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)
	@rm -f primary.db

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sbindir)
	$(INSTALL) -m 0775 wolframed$(EXE) $(DESTDIR)$(sbindir)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfilter$(EXE) $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfwizard$(EXE) $(DESTDIR)$(bindir)

local_uninstall:
	@-rm -f $(DESTDIR)$(bindir)/wolfwizard$(EXE)
	@-rm -f $(DESTDIR)$(bindir)/wolfilter$(EXE)
	@-rmdir $(DESTDIR)$(bindir)
	@-rm -f $(DESTDIR)$(sbindir)/wolframed$(EXE)
	@-rmdir $(DESTDIR)$(sbindir)

local_test:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton/:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database"

ifeq ($(WITH_ICU),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(ICU_LIB_DIR)"
endif

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

run:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -d DATA -c wolframe.conf -f

check:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -t

print:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -p

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
