# sets  e. g. to LINUX, OS_MAJOR_VERSION to 2 and OS_MINOR_VERSION to 6
# by calling the 'guess_env' shell script, where the actual probing happens
# Also sets PLATFORM_COMPILE_FLAGS to be included when compiling C/C++ code
#
# requires:
# - TOPDIR
#
# provides:
# - PLATFORM
# - OS_MAJOR_VERSION and OS_MINOR_VERSION
# - GCC_MAJOR_VERSION and GCC_MINOR_VERSION
# - PLATFORM_COMPILE_FLAGS
# - INSTALL
# - BOOST_LIBRARY_TAG
#
# author: Andreas Baumann, abaumann at yahoo dot com

-include $(TOPDIR)/makefiles/gmake/platform.mk.vars

# set up defaults for the build switches
WITH_SSL ?= 0
WITH_LUA ?= 0
WITH_PYTHON ?= 0
WITH_SASL ?= 0
WITH_PAM ?= 0
WITH_SYSTEM_SQLITE3 ?= 0
WITH_LOCAL_SQLITE3 ?= 0
WITH_PGSQL ?= 0
WITH_ORACLE ?= 0
WITH_LIBXML2 ?= 0
WITH_LIBXSLT ?= 0
WITH_CJSON ?= 0
WITH_SYSTEM_LIBHPDF ?= 0
WITH_LOCAL_LIBHPDF ?= 0
WITH_SYSTEM_FREEIMAGE ?= 0
WITH_LOCAL_FREEIMAGE ?= 0
WITH_ICU ?= 0
WITH_EXAMPLES ?= 1
ENABLE_NLS ?= 1
RUN_TESTS ?= 1

# variables guessed by 'guess_env'
PLATFORM ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --platform "$(CC)" "$(CURDIR)" $(TOPDIR))
OS_MAJOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-major-version "$(CC)" "$(CURDIR)" $(TOPDIR))
OS_MINOR_VERSION ?=	$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --os-minor-version "$(CC)" "$(CURDIR)" $(TOPDIR))
COMPILER ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --compiler "$(CC)" "$(CURDIR)" $(TOPDIR))
ifeq "$(PLATFORM)" "LINUX"
LINUX_DIST ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-dist "$(CC)" "$(CURDIR)" $(TOPDIR))
LINUX_REV ?=		$(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_env --linux-rev "$(CC)" "$(CURDIR)" $(TOPDIR))
endif

# set library path on Intel/AMD

ifeq "$(PLATFORM)" "LINUX"
ifeq "$(ARCH)" "x86"
ifeq "$(LINUX_DIST)" "arch"
LIBDIR=lib32
else
LIBDIR=lib
endif
else
LIBDIR=lib
endif
ifeq "$(ARCH)" "x86_64"
ifeq "$(LINUX_DIST)" "arch"
LIBDIR=lib
else
LIBDIR=lib64
endif
else
LIBDIR=lib
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
LIBDIR=lib
endif

ifeq "$(PLATFORM)" "NETBSD"
LIBDIR=lib
endif

# Sparc or Intel, always 'lib'
ifeq "$(PLATFORM)" "SUNOS"
LIBDIR=lib
endif

# platform specific flags
#########################

PLATFORM_COMPILE_FLAGS = \
			-D$(PLATFORM) \
			-DOS_MAJOR_VERSION=$(OS_MAJOR_VERSION) \
			-DOS_MINOR_VERSION=$(OS_MINOR_VERSION)

ifeq "$(PLATFORM)" "LINUX"
ifeq "$(LINUX_DIST)" "arch"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_ARCH=1
endif
ifeq "$(LINUX_DIST)" "debian"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_DEBIAN=1
endif
ifeq "$(LINUX_DIST)" "redhat"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_REDHAT=1
endif
ifeq "$(LINUX_DIST)" "slackware"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_SLACKWARE=1
endif
ifeq "$(LINUX_DIST)" "sles"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_SLES=1
endif
ifeq "$(LINUX_DIST)" "suse"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_SUSE=1
endif
ifeq "$(LINUX_DIST)" "ubuntu"
PLATFORM_COMPILE_FLAGS += -DLINUX_DIST_UBUNTU=1
endif
PLATFORM_COMPILE_FLAGS += \
			-DLINUX_REV=$(LINUX_REV)
endif

# name if the installation program
ifndef INSTALL
ifeq "$(PLATFORM)" "SUNOS"
INSTALL = /usr/ucb/install
else
INSTALL = install
endif
endif

# dynamic linker
################

# the linker library for dynamically loadable modules
ifeq "$(PLATFORM)" "LINUX"
LIBS_DL = -ldl
else
ifeq "$(PLATFORM)" "SUNOS"
LIBS_DL = -ldl
else
LIBS_DL =
endif
endif

# Note for dlopen to work (at least on FreeBSD) with rtti information we have to export all symbols
# in the binary and in the modules (see http://stackoverflow.com/questions/2351786/dynamic-cast-fails-when-used-with-dlopen-dlsym)
ifeq "$(PLATFORM)" "LINUX"
LDFLAGS_EXPORT_ALL_SYMBOLS = -Wl,-E
endif
ifeq "$(PLATFORM)" "FREEBSD"
LDFLAGS_EXPORT_ALL_SYMBOLS = -Wl,-E
endif
ifeq "$(PLATFORM)" "NETBSD"
LDFLAGS_EXPORT_ALL_SYMBOLS = -Wl,-E
endif
ifeq "$(PLATFORM)" "SUNOS"
LDFLAGS_EXPORT_ALL_SYMBOLS =
endif

# i18n, gettext/libintl
#######################

ifeq "$(ENABLE_NLS)" "1"

# we rely only on the GNU version, other versions (e.g. Solaris) are not
# so nice..

MSGFMT=msgfmt
MSGMERGE=msgmerge
XGETTEXT=xgettext

# the GNU internationalization support is sometimes in separate libarries
ifeq "$(PLATFORM)" "LINUX"
INCLUDE_FLAGS_LT =
LDFLAGS_LT =
LIBS_LT =
endif

ifeq "$(PLATFORM)" "SUNOS"
LIBLT_DIR ?= /opt/csw
INCLUDE_FLAGS_LT = -I$(LIBLT_DIR)/include
LDFLAGS_LT = -L$(LIBLT_DIR)/lib
LIBS_LT = -lintl
endif

ifeq "$(PLATFORM)" "FREEBSD"
LIBLT_DIR ?= /usr/local
INCLUDE_FLAGS_LT = -I$(LIBLT_DIR)/include
LDFLAGS_LT = -L$(LIBLT_DIR)/lib
LIBS_LT = -lintl
endif

ifeq "$(PLATFORM)" "NETBSD"
LIBLT_DIR ?= /usr
INCLUDE_FLAGS_LT = -I$(LIBLT_DIR)/include
LDFLAGS_LT = -L$(LIBLT_DIR)/lib
LIBS_LT = -lintl
endif

endif

PLATFORM_COMPILE_FLAGS += \
	-DENABLE_NLS=$(ENABLE_NLS) -DLOCALEDIR=\"$(localedir)\"

# TCP/IP, DNS
#############

ifeq "$(PLATFORM)" "LINUX"
LIBS_NET =
endif

ifeq "$(PLATFORM)" "FREEBSD"
LIBS_NET =
endif

ifeq "$(PLATFORM)" "NETBSD"
LIBS_NET =
endif

ifeq "$(PLATFORM)" "SUNOS"
LIBS_NET = -lsocket -lnsl
endif

# XSLT processor
################

XSLTPROC ?= xsltproc

# DocBook and Stylesheets
#########################

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
XSLT_VERSION ?= $(shell pacman -Q | grep docbook-xsl | cut -f 2 -d ' ' | cut -f 1 -d -)
DOCBOOK_HOME ?= /usr/share/xml/docbook/xsl-stylesheets-$(XSLT_VERSION)
XSLT_MAN_STYLESHEET ?= $(DOCBOOK_HOME)/manpages/docbook.xsl
XSLT_TOWORDML_STYLESHEET ?= $(DOCBOOK_HOME)/roundtrip/dbk2wordml.xsl
XSLT_TOWORDML_TEMPLATE ?= $(DOCBOOK_HOME)/roundtrip/template.xml
endif

# Ubuntu
ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif

ifeq "$(LINUX_REV)" "13.10"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif

ifeq "$(LINUX_REV)" "12.04"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif

ifeq "$(LINUX_REV)" "10.04"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif

endif

# Debian
ifeq "$(LINUX_DIST)" "debian"
ifeq "$(LINUX_REV)" "6"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
ifeq "$(LINUX_REV)" "7"
XSLT_MAN_STYLESHEET ?= /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 19
ifeq "$(LINUX_REV)" "19"
XSLT_VERSION ?= $(shell rpm -q --queryformat '%{VERSION}' docbook-style-xsl)
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
XSLT_VERSION ?= $(shell rpm -q --queryformat '%{VERSION}' docbook-style-xsl)
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets-$(XSLT_VERSION)/manpages/docbook.xsl
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
XSLT_MAN_STYLESHEET ?= /usr/share/sgml/docbook/xsl-stylesheets/manpages/docbook.xsl
endif

endif

endif

# Boost
#######

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif

ifeq "$(LINUX_DIST)" "slackware"

ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
ifeq "$(LINUX_REV)" "14.1"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
else
ifeq "$(LINUX_REV)" "14.0"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
else
$(warning no recent enough boost package on Slackware before version 14.0, compile your own version and set BOOST_DIR accordingly)
endif
endif
endif

endif

# Ubuntu
ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
BOOST_DIR ?= /usr
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_LIBRARY_TAG ?=
endif

ifeq "$(LINUX_REV)" "13.10"
BOOST_DIR ?= /usr
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_LIBRARY_TAG ?=
endif

ifeq "$(LINUX_REV)" "12.04"
BOOST_DIR ?= /usr
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_LIBRARY_TAG ?=
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on Ubuntu 12.04, compile your own version and set BOOST_DIR accordingly)
endif
endif

ifeq "$(LINUX_REV)" "10.04"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_LIBRARY_TAG ?=
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on Ubuntu 10.04, compile your own version and set BOOST_DIR accordingly)
endif
endif

endif

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on Debian 6, compile your own version and set BOOST_DIR accordingly)
endif
endif

ifeq "$(LINUX_REV)" "7"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif

endif

ifeq "$(LINUX_DIST)" "suse"

ifeq "$(LINUX_REV)" "12.3"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif

ifeq "$(LINUX_REV)" "13.1"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif

endif

# SUSE Linux Enterprise
ifeq "$(LINUX_DIST)" "sles"

ifeq "$(LINUX_REV)" "11"
ifdef BOOST_DIR
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on SLES, compile your own version and set BOOST_DIR accordingly)
endif
endif

endif

# Fedora 19
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "19"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?= -mt
endif
endif

# Fedora 20
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "20"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# RHEL5
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "5"
ifdef BOOST_DIR
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on RHEL 5, compile your own version and set BOOST_DIR accordingly)
endif
endif
endif

# RHEL6
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "6"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# RHEL7
ifeq "$(LINUX_DIST)" "redhat"
ifeq "$(LINUX_REV)" "7"
BOOST_DIR ?= /usr
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
BOOST_DIR ?= /usr/local
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
BOOST_DIR ?= /usr/local
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
BOOST_DIR ?= /usr/local
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
BOOST_DIR ?= /usr/pkg
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifdef BOOST_DIR
BOOST_LIB_DIR ?= $(BOOST_DIR)/lib
BOOST_INCLUDE_DIR ?= $(BOOST_DIR)/include
BOOST_LIBRARY_TAG ?=
endif
ifndef BOOST_DIR
$(warning no recent enough boost package on Solaris, compile your own version and set BOOST_DIR accordingly)
endif
endif

# OpenSSL
#########

ifeq ($(WITH_SSL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto -lz
endif

ifeq "$(LINUX_DIST)" "slackware"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

ifeq "$(LINUX_REV)" "13.10"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

ifeq "$(LINUX_REV)" "12.04"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

ifeq "$(LINUX_REV)" "10.04"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

ifeq "$(LINUX_REV)" "7"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

endif

ifeq "$(LINUX_DIST)" "sles"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

ifeq "$(LINUX_DIST)" "suse"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif

endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
OPENSSL_DIR ?=
OPENSSL_INCLUDE_DIR ?=
OPENSSL_INCLUDE_DIRS ?=
OPENSSL_LIB_DIR ?=
OPENSSL_LIB_DIRS ?=
OPENSSL_LIBS ?= -lssl -lcrypto
endif
endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
OPENSSL_DIR ?= /opt/csw
OPENSSL_INCLUDE_DIR ?= $(OPENSSL_DIR)/include
OPENSSL_INCLUDE_DIRS ?= -I$(OPENSSL_INCLUDE_DIR)
OPENSSL_LIB_DIR ?= $(OPENSSL_DIR)/lib
OPENSSL_LIB_DIRS ?= -L$(OPENSSL_LIB_DIR)
OPENSSL_LIBS ?= -lssl -lcrypto
endif
ifeq "$(OS_MINOR_VERSION)" "11"
OPENSSL_DIR ?= /opt/csw
OPENSSL_INCLUDE_DIR ?= $(OPENSSL_DIR)/include
OPENSSL_INCLUDE_DIRS ?= -I$(OPENSSL_INCLUDE_DIR)
OPENSSL_LIB_DIR ?= $(OPENSSL_DIR)/lib
OPENSSL_LIB_DIRS ?= -L$(OPENSSL_LIB_DIR)
OPENSSL_LIBS ?= -lssl -lcrypto
endif
endif
endif

endif

# Lua 5.2
#########

ifeq ($(WITH_LUA),1)

ifeq "$(PLATFORM)" "LINUX"
LUA_PLATFORM_CFLAGS = -DLUA_USE_POSIX -DLUA_USE_DLOPEN
LUA_PLATFORM_LDFLAGS =
LUA_PLATFORM_LIBS = $(LIBS_DL) -lm
endif

ifeq "$(PLATFORM)" "SUNOS"
LUA_PLATFORM_CFLAGS = -DLUA_USE_POSIX -DLUA_USE_DLOPEN
LUA_PLATFORM_LDFLAGS =
LUA_PLATFORM_LIBS = $(LIBS_DL) -lm
endif

ifeq "$(PLATFORM)" "FREEBSD"
LUA_PLATFORM_CFLAGS = -DLUA_USE_POSIX -DLUA_USE_DLOPEN
LUA_PLATFORM_LDFLAGS =
LUA_PLATFORM_LIBS = $(LIBS_DL) -lm
endif

ifeq "$(PLATFORM)" "NETBSD"
LUA_PLATFORM_CFLAGS = -DLUA_USE_POSIX -DLUA_USE_DLOPEN
LUA_PLATFORM_LDFLAGS = 
LUA_PLATFORM_LIBS = $(LIBS_DL) -lm
endif

endif

# PAM 1.1.3
###########

ifeq ($(WITH_PAM),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_DIST)" "slackware"
$(error There is currently no PAM support on Slackware)
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_REV)" "13.10"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_REV)" "12.04"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_REV)" "10.04"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_REV)" "7"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 19
ifeq "$(LINUX_REV)" "19"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# RHEL5
ifeq "$(LINUX_REV)" "5"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif

ifeq "$(LINUX_DIST)" "sles"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

ifeq "$(LINUX_DIST)" "suse"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= /lib
PAM_LIBS ?= -lpam
endif

endif

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= $(PAM_DIR)/lib
PAM_LIBS ?= -lpam
endif
ifeq "$(OS_MINOR_VERSION)" "11"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= $(PAM_DIR)/lib
PAM_LIBS ?= -lpam
endif
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
$(error There is currently no PAM support on FreeBSD)
endif

ifeq "$(PLATFORM)" "NETBSD"
PAM_DIR ?= /usr
PAM_INCLUDE_DIR ?= $(PAM_DIR)/include
PAM_LIB_DIR ?= $(PAM_DIR)/lib
PAM_LIBS ?= -lpam
endif

endif

# Cyrus SASL2
#############

ifeq ($(WITH_SASL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_DIST)" "slackware"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_REV)" "13.10"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_REV)" "12.04"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_REV)" "10.04"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_REV)" "7"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

ifeq "$(LINUX_DIST)" "sles"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

ifeq "$(LINUX_DIST)" "suse"
SASL_DIR ?= /usr
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif

endif

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
SASL_DIR ?= /opt/csw
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
ifeq "$(OS_MINOR_VERSION)" "11"
SASL_DIR ?= /opt/csw
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
endif
endif

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
SASL_DIR ?= /usr/local
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
SASL_DIR ?= /usr/local
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
SASL_DIR ?= /usr/local
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
endif

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
SASL_DIR ?= /usr/pkg
SASL_INCLUDE_DIR ?= $(SASL_DIR)/include
SASL_LIB_DIR ?= $(SASL_DIR)/lib
SASL_LIBS ?= -lsasl2
endif
endif

endif

# Sqlite3
#########

ifeq ($(WITH_SYSTEM_SQLITE3),1)
WITH_SQLITE3 = 1
ifeq ($(WITH_LOCAL_SQLITE3),1)
$(error Specify one of WITH_SYSTEM_SQLITE3 or WITH_LOCAL_SQLITE3, not both!)
endif
else
ifeq ($(WITH_LOCAL_SQLITE3),1)
WITH_SQLITE3 = 1
endif
endif

ifeq ($(WITH_SYSTEM_SQLITE3),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_DIST)" "slackware"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_REV)" "13.10"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_REV)" "12.04"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_REV)" "10.04"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_REV)" "7"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
ifdef SQLITE3_DIR
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifndef SQLITE3_DIR
$(warning no recent enough sqlite3 package on RHEL 5, compile your own version and set SQLITE3_DIR accordingly)
endif
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

ifeq "$(LINUX_DIST)" "sles"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

ifeq "$(LINUX_DIST)" "suse"
SQLITE3_DIR ?= /usr
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
SQLITE3_DIR ?= /opt/csw
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifeq "$(OS_MINOR_VERSION)" "11"
SQLITE3_DIR ?= /opt/csw
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
SQLITE3_DIR ?= /usr/local
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
SQLITE3_DIR ?= /usr/local
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
SQLITE3_DIR ?= /usr/local
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
endif

# NetBSD
ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
SQLITE3_DIR ?= /usr/pkg
SQLITE3_INCLUDE_DIR ?= $(SQLITE3_DIR)/include
SQLITE3_LIB_DIR ?= $(SQLITE3_DIR)/lib
SQLITE3_LIBS ?= -lsqlite3
endif
endif

# helpers for easier final Makefiles (local/system switch)
SQLITE3_INCLUDE_DIRS = -I$(SQLITE3_INCLUDE_DIR)
SQLITE3_LIB_DIRS = -L$(SQLITE3_LIB_DIR)

# we expect the shell to be in the path
SQLITE3 = sqlite3

endif

ifeq ($(WITH_LOCAL_SQLITE3),1)
SQLITE3_DIR = $(TOPDIR)/3rdParty/sqlite3
SQLITE3_INCLUDE_DIR = $(SQLITE3_DIR)
SQLITE3_INCLUDE_DIRS = -I$(SQLITE3_DIR)
SQLITE3_LIB_DIR =
SQLITE3_LIB_DIRS =
SQLITE3_LIBS = $(SQLITE3_DIR)/libsqlite3.a
SQLITE3 = $(SQLITE3_DIR)/sqlite3
endif

# Oracle
########

ifeq ($(WITH_ORACLE),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
ORACLE_DIR ?= /usr
ORACLE_INCLUDE_DIR ?= $(ORACLE_DIR)/include
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

ifeq "$(LINUX_REV)" "13.10"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

ifeq "$(LINUX_REV)" "12.04"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh -lnnz12 -lons -lclntshcore
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "7"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# Fedora 20
ifeq "$(LINUX_REV)" "20"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

endif


ifeq "$(LINUX_DIST)" "suse"

ifeq "$(LINUX_REV)" "13.1"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif

ifeq "$(LINUX_REV)" "12.3"
ORACLE_VERSION ?= 12.1
ifeq "$(ARCH)" "x86_64"
ORACLE_CLIENT_ARCH=client64
else
ORACLE_CLIENT_ARCH=client
endif
ORACLE_DIR ?= /usr/lib/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIR ?= /usr/include/oracle/$(ORACLE_VERSION)/$(ORACLE_CLIENT_ARCH)
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)/lib
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= -lclntsh
endif
endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ORACLE_LIB_VERSION ?= 11.1
ORACLE_DIR ?= /opt/oracle/instantclient_11_2
ORACLE_INCLUDE_DIR ?= $(ORACLE_DIR)/sdk/include
ORACLE_INCLUDE_DIRS = -I$(ORACLE_INCLUDE_DIR)
ORACLE_LIB_DIR ?= $(ORACLE_DIR)
ORACLE_LIB_DIRS = -L$(ORACLE_LIB_DIR)
ORACLE_LIBS ?= $(ORACLE_LIB_DIR)/libclntsh.so.$(ORACLE_LIB_VERSION)
endif

endif

# Postgresql
############

ifeq ($(WITH_PGSQL),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_DIST)" "slackware"
ifdef PGSQL_DIR
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
endif
ifndef PGSQL_DIR
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
endif
PGSQL_LIBS ?= -lpq
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
endif

# Ubuntu
ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_REV)" "13.10"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_REV)" "12.04"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_REV)" "10.04"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_REV)" "7"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIBS ?= -lpq
endif

endif

ifeq "$(LINUX_DIST)" "sles"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/pgsql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

ifeq "$(LINUX_DIST)" "suse"
PGSQL_DIR ?= /usr
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/pgsql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
PGSQL_DIR ?= /usr/local/pgsql
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
ifeq "$(OS_MINOR_VERSION)" "11"
PGSQL_DIR ?= /opt/csw
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
PGSQL_DIR ?= /usr/local
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
PGSQL_DIR ?= /usr/local
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
PGSQL_DIR ?= /usr/local
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
PGSQL_DIR ?= /usr/pkg
PGSQL_INCLUDE_DIR ?= $(PGSQL_DIR)/include/postgresql
PGSQL_INCLUDE_DIRS = -I$(PGSQL_INCLUDE_DIR)
PGSQL_LIB_DIR ?= $(PGSQL_DIR)/lib
PGSQL_LIB_DIRS = -L$(PGSQL_LIB_DIR)
PGSQL_LIBS ?= -lpq
endif
endif

endif

# libxml2
#########

ifeq ($(WITH_LIBXML2),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_DIST)" "slackware"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_REV)" "13.10"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_REV)" "12.04"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_REV)" "10.04"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_REV)" "7"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
ifndef LIBXML2_DIR
$(warning no recent enough libxml2 package on RHEL 5, compile your own one and set LIBXML2_DIR accordingly)
endif
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

ifeq "$(LINUX_DIST)" "sles"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

ifeq "$(LINUX_DIST)" "suse"
LIBXML2_DIR ?= /usr
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
LIBXML2_DIR ?= /opt/csw
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
ifeq "$(OS_MINOR_VERSION)" "11"
LIBXML2_DIR ?= /opt/csw
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
LIBXML2_DIR ?= /usr/local
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR) -I/usr/local/include
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
LIBXML2_DIR ?= /usr/local
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR) -I/usr/local/include
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
LIBXML2_DIR ?= /usr/local
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
LIBXML2_DIR ?= /usr/pkg
LIBXML2_INCLUDE_DIR ?= $(LIBXML2_DIR)/include/libxml2
LIBXML2_INCLUDE_DIRS = -I$(LIBXML2_INCLUDE_DIR)
LIBXML2_LIB_DIR ?= $(LIBXML2_DIR)/lib
LIBXML2_LIB_DIRS = -L$(LIBXML2_LIB_DIR)
LIBXML2_LIBS ?= -lxml2
endif
endif

endif

# libxslt
#########

ifeq ($(WITH_LIBXSLT),1)

ifneq ($(WITH_LIBXML2),1)
$(error Building with WITH_LIBXSLT also requires WITH_LIBXML2!)
endif

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_DIST)" "slackware"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_REV)" "13.10"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_REV)" "12.04"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_REV)" "10.04"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_REV)" "7"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
ifndef WITH_LIBXSLT
$(warning libxslt depends on a not recent enough version of libxml2 on RHEL 5, compile your own ones and set LIBXSLT_DIR and LIBXML2_DIR accordingly)
endif
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

endif

ifeq "$(LINUX_DIST)" "sles"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

ifeq "$(LINUX_DIST)" "suse"
LIBXSLT_DIR ?= /usr
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
LIBXSLT_DIR ?= /opt/csw
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
ifeq "$(OS_MINOR_VERSION)" "11"
LIBXSLT_DIR ?= /opt/csw
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
LIBXSLT_DIR ?= /usr/local
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
LIBXSLT_DIR ?= /usr/local
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
LIBXSLT_DIR ?= /usr/local
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
LIBXSLT_DIR ?= /usr/pkg
LIBXSLT_INCLUDE_DIR ?= $(LIBXSLT_DIR)/include
LIBXSLT_INCLUDE_DIRS = -I$(LIBXSLT_INCLUDE_DIR)
LIBXSLT_LIB_DIR ?= $(LIBXSLT_DIR)/lib
LIBXSLT_LIB_DIRS = -L$(LIBXSLT_LIB_DIR)
LIBXSLT_LIBS ?= -lxslt
endif
endif

endif


# libhpdf
#########

ifeq ($(WITH_SYSTEM_LIBHPDF),1)
WITH_LIBHPDF = 1
ifeq ($(WITH_LOCAL_LIBHPDF),1)
$(error Specify one of WITH_SYSTEM_LIBHPDF or WITH_LOCAL_LIBHPDF, not both!)
endif
else
ifeq ($(WITH_LOCAL_LIBHPDF),1)
WITH_LIBHPDF = 1
endif
endif

ifeq ($(WITH_SYSTEM_LIBHPDF),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

ifeq "$(LINUX_REV)" "13.10"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

ifeq "$(LINUX_REV)" "12.04"
$(warning no recent enough libhpdf package on Ubuntu 12.04, use WITH_LOCAL_LIBHPDF=1 instead)
endif

ifeq "$(LINUX_REV)" "10.04"
$(warning no recent enough libhpdf package on Ubuntu 10.04, use WITH_LOCAL_LIBHPDF=1 instead)
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
$(warning no recent enough libhpdf package on RHEL5, use WITH_LOCAL_LIBHPDF=1 instead)
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
$(warning no recent enough libhpdf package on RHEL6, use WITH_LOCAL_LIBHPDF=1 instead)
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
$(warning no recent enough libhpdf package on RHEL7, use WITH_LOCAL_LIBHPDF=1 instead)
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

endif

ifeq "$(LINUX_DIST)" "sles"
$(warning no recent enough libhpdf package on SLES, use WITH_LOCAL_LIBHPDF=1 instead)
endif

ifeq "$(LINUX_DIST)" "suse"
$(warning no recent enough libhpdf package on OpenSuSE, use WITH_LOCAL_LIBHPDF=1 instead)
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
$(warning no recent enough libhpdf package on Solaris, use WITH_LOCAL_LIBHPDF=1 instead)
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
$(warning no recent enough libhpdf package on FreeBSD 8, use WITH_LOCAL_LIBHPDF=1 instead)endif
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
$(warning no recent enough libhpdf package on FreeBSD 9, use WITH_LOCAL_LIBHPDF=1 instead)endif
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
LIBHPDF_DIR ?= /usr/local
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
$(warning no recent enough libhpdf package on NetBSD 6, use WITH_LOCAL_LIBHPDF=1 instead)endif
endif
endif

endif

ifeq ($(WITH_LOCAL_LIBHPDF),1)
LIBHPDF_DIR = $(TOPDIR)/3rdParty/libhpdf
LIBHPDF_INCLUDE_DIR = $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR = $(LIBHPDF_DIR)/src
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS = -lhpdf
endif

# libpng
########

ifeq ($(WITH_LIBHPDF),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_DIST)" "slackware"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_REV)" "7"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_REV)" "13.10"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_REV)" "12.04"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_REV)" "10.04"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_REV)" "7"
LIBHPDF_DIR ?= /usr
LIBHPDF_INCLUDE_DIR ?= $(LIBHPDF_DIR)/include
LIBHPDF_INCLUDE_DIRS = -I$(LIBHPDF_INCLUDE_DIR)
LIBHPDF_LIB_DIR ?= $(LIBHPDF_DIR)/lib
LIBHPDF_LIB_DIRS = -L$(LIBHPDF_LIB_DIR)
LIBHPDF_LIBS ?= -lhpdf
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

endif

ifeq "$(LINUX_DIST)" "sles"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

ifeq "$(LINUX_DIST)" "suse"
LIBPNG_DIR ?= /usr
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
LIBPNG_DIR ?= /opt/csw
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif
ifeq "$(OS_MINOR_VERSION)" "11"
LIBPNG_DIR ?= /opt/csw
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
LIBPNG_DIR ?= /usr/local
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
LIBPNG_DIR ?= /usr/local
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
LIBPNG_DIR ?= /usr/local
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
LIBPNG_DIR ?= /usr/pkg
LIBPNG_INCLUDE_DIR ?= $(LIBPNG_DIR)/include
LIBPNG_INCLUDE_DIRS = -I$(LIBPNG_INCLUDE_DIR)
LIBPNG_LIB_DIR ?= $(LIBPNG_DIR)/lib
LIBPNG_LIB_DIRS = -L$(LIBPNG_LIB_DIR)
LIBPNG_LIBS ?= -lpng16
endif
endif

endif

ifeq ($(WITH_LOCAL_LIBHPDF),1)
LIBHPDF_INCLUDE_DIRS += $(LIBPNG_INCLUDE_DIRS)
LIBHPDF_LIB_DIRS += $(LIBPNG_LIB_DIRS)
LIBHPDF_LIBS += $(LIBPNG_LIBS)
endif

# zlib
######

ifeq ($(WITH_LIBHPDF),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_DIST)" "slackware"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_REV)" "13.10"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_REV)" "12.04"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_REV)" "10.04"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_REV)" "7"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

endif

ifeq "$(LINUX_DIST)" "sles"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

ifeq "$(LINUX_DIST)" "suse"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
LIBZ_DIR ?= /opt/csw
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
ifeq "$(OS_MINOR_VERSION)" "11"
LIBZ_DIR ?= /opt/csw
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
LIBZ_DIR ?= /usr/local
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
LIBZ_DIR ?= /usr/local
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
LIBZ_DIR ?= /usr/local
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
LIBZ_DIR ?= /usr
LIBZ_INCLUDE_DIR ?= $(LIBZ_DIR)/include
LIBZ_INCLUDE_DIRS = -I$(LIBZ_INCLUDE_DIR)
LIBZ_LIB_DIR ?= $(LIBZ_DIR)/lib
LIBZ_LIB_DIRS = -L$(LIBZ_LIB_DIR)
LIBZ_LIBS ?= -lz
endif
endif

endif

ifeq ($(WITH_LOCAL_LIBHPDF),1)
LIBHPDF_INCLUDE_DIRS += $(LIBZ_INCLUDE_DIRS)
LIBHPDF_LIB_DIRS += $(LIBZ_LIB_DIRS)
LIBHPDF_LIBS += $(LIBZ_LIBS)
endif

# icu
#####

ifeq ($(WITH_ICU),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_DIST)" "slackware"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_REV)" "13.10"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_REV)" "12.04"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_REV)" "10.04"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_REV)" "7"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

endif

ifeq "$(LINUX_DIST)" "sles"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

ifeq "$(LINUX_DIST)" "suse"
ICU_DIR ?= /usr
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
ifdef ICU_DIR
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?= -licuuc -licudata -licui18n
endif
ifndef ICU_DIR
$(warning no usable ICU package for SPARC Solaris, compile your own version and set ICU_DIR accordingly)
endif
endif
ifeq "$(OS_MINOR_VERSION)" "11"
ifdef ICU_DIR
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?= -licuuc -licudata -licui18n
endif
ifndef ICU_DIR
ICU_DIR ?= /opt/csw
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
ICU_DIR ?= /usr/local
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
ICU_DIR ?= /usr/local
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
ICU_DIR ?= /usr/local
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif
endif

# NetBSD
ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
ICU_DIR ?= /usr/pkg
ICU_INCLUDE_DIR ?= $(ICU_DIR)/include
ICU_INCLUDE_DIRS = -I$(ICU_INCLUDE_DIR)
ICU_LIB_DIR ?= $(ICU_DIR)/lib
ICU_LIB_DIRS = -L$(ICU_LIB_DIR)
ICU_LIBS ?=
endif
endif

endif

# FreeImage
###########

ifeq ($(WITH_SYSTEM_FREEIMAGE),1)
WITH_FREEIMAGE = 1
ifeq ($(WITH_LOCAL_FREEIMAGE),1)
$(error Specify one of WITH_SYSTEM_FREEIMAGE or WITH_LOCAL_FREEIMAGE, not both!)
endif
else
ifeq ($(WITH_LOCAL_FREEIMAGE),1)
WITH_FREEIMAGE = 1
endif
endif

ifeq ($(WITH_SYSTEM_FREEIMAGE),1)

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "arch"
FREEIMAGE_DIR ?= /usr
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

# Ubuntu

ifeq "$(LINUX_DIST)" "ubuntu"

ifeq "$(LINUX_REV)" "14.04"
FREEIMAGE_DIR ?= /usr
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

ifeq "$(LINUX_REV)" "13.10"
FREEIMAGE_DIR ?= /usr
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

ifeq "$(LINUX_REV)" "12.04"
FREEIMAGE_DIR ?= /usr
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

ifeq "$(LINUX_REV)" "10.04"
FREEIMAGE_DIR ?= /usr
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

endif

# Debian

ifeq "$(LINUX_DIST)" "debian"

ifeq "$(LINUX_REV)" "6"
$(warning no recent enough FreeImage package on Debian 6, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

ifeq "$(LINUX_REV)" "7"
$(warning no recent enough FreeImage package on Debian 7, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

endif

ifeq "$(LINUX_DIST)" "redhat"

# RHEL5
ifeq "$(LINUX_REV)" "5"
$(warning no recent enough FreeImage package on RHEL5, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

# RHEL6
ifeq "$(LINUX_REV)" "6"
$(warning no recent enough FreeImage package on RHEL6, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

# RHEL7
ifeq "$(LINUX_REV)" "7"
$(warning no recent enough FreeImage package on RHEL7, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

# Fedora 19
ifeq "$(LINUX_REV)" "19"
$(warning no recent enough FreeImage package on Fedora 19, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

# Fedora 20
ifeq "$(LINUX_REV)" "20"
$(warning no recent enough FreeImage package on Fedora 20, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

endif

ifeq "$(LINUX_DIST)" "sles"
$(warning no recent enough FreeImage package on SLES, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

ifeq "$(LINUX_DIST)" "suse"
$(warning no recent enough FreeImage package on OpenSUSE, use WITH_LOCAL_FREEIMAGE=1 instead)
endif

endif

# SunOS

ifeq "$(PLATFORM)" "SUNOS"
ifeq "$(OS_MAJOR_VERSION)" "5"
ifeq "$(OS_MINOR_VERSION)" "10"
$(warning no recent enough FreeImage package on Solaris, use WITH_LOCAL_FREEIMAGE=1 instead)
endif
ifeq "$(OS_MINOR_VERSION)" "11"
$(warning no recent enough FreeImage package on Solaris, use WITH_LOCAL_FREEIMAGE=1 instead)
endif
endif
endif

# FreeBSD

ifeq "$(PLATFORM)" "FREEBSD"
ifeq "$(OS_MAJOR_VERSION)" "8"
$(warning no recent enough FreeImage package on FreeBSD, use WITH_LOCAL_FREEIMAGE=1 instead)
endif
ifeq "$(OS_MAJOR_VERSION)" "9"
$(warning no recent enough FreeImage package on FreeBSD, use WITH_LOCAL_FREEIMAGE=1 instead)
endif
ifeq "$(OS_MAJOR_VERSION)" "10"
FREEIMAGE_DIR ?= /usr/local
FREEIMAGE_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGE_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGE_LIBS ?= -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_INCLUDE_DIR ?= $(FREEIMAGE_DIR)/include
FREEIMAGEPLUS_INCLUDE_DIRS ?= -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGEPLUS_LIB_DIR ?= $(FREEIMAGE_DIR)/lib
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif
endif

# NetBSD

ifeq "$(PLATFORM)" "NETBSD"
ifeq "$(OS_MAJOR_VERSION)" "6"
$(warning no recent enough official FreeImage package on NetBSD, use WITH_LOCAL_FREEIMAGE=1 instead)
endif
endif

endif

ifeq ($(WITH_LOCAL_FREEIMAGE),1)
FREEIMAGE_DIR = $(TOPDIR)/3rdParty/freeimage
FREEIMAGE_INCLUDE_DIR = $(FREEIMAGE_DIR)/Source
FREEIMAGEPLUS_INCLUDE_DIR = $(FREEIMAGE_DIR)/Wrapper/FreeImagePlus
FREEIMAGE_INCLUDE_DIRS = -I$(FREEIMAGE_INCLUDE_DIR) $(INCLUDE_FLAGS_LT)
FREEIMAGEPLUS_INCLUDE_DIRS = -I$(FREEIMAGEPLUS_INCLUDE_DIR)
FREEIMAGE_LIB_DIR = $(FREEIMAGE_DIR)
FREEIMAGEPLUS_LIB_DIR = $(FREEIMAGE_DIR)/Wrapper/FreeImagePlus
FREEIMAGE_LIB_DIRS = -L$(FREEIMAGE_LIB_DIR) $(LDFLAGS_LT)
FREEIMAGEPLUS_LIB_DIRS = -L$(FREEIMAGEPLUS_LIB_DIR)
FREEIMAGE_LIBS = -lfreeimage $(LIBS_LT)
FREEIMAGEPLUS_LIBS = -lfreeimageplus
endif

# Python 3.x
############

ifeq ($(WITH_PYTHON),1)

-include $(TOPDIR)/makefiles/gmake/python.mk.vars

ifeq "$(PLATFORM)" "LINUX"

ifeq "$(LINUX_DIST)" "slackware"
PYTHON3_CONFIG ?= /usr/local/bin/python3-config
else
PYTHON3_CONFIG ?= /usr/bin/python3-config
endif
PYTHON_DIR ?= /usr
PYTHON_CFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --cflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LDFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --ldflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIBS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --libs "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIB_DIR ?= $(PYTHON_DIR)/lib
endif

ifeq "$(PLATFORM)" "FREEBSD"
PYTHON3_CONFIG ?= /usr/local/bin/python3.3-config
PYTHON_DIR ?= /usr/local
PYTHON_CFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --cflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LDFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --ldflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIBS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --libs "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIB_DIR ?= $(PYTHON_DIR)/lib
endif

ifeq "$(PLATFORM)" "NETBSD"
PYTHON3_CONFIG ?= /usr/pkg/bin/python3.3-config
PYTHON_DIR ?= /usr/pkg
PYTHON_CFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --cflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LDFLAGS_WRONG ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --ldflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LDFLAGS := -L/usr/pkg/lib $(PYTHON_LDFLAGS_WRONG)
PYTHON_LIBS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --libs "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIB_DIR ?= $(PYTHON_DIR)/lib
endif

ifeq "$(PLATFORM)" "SUNOS"
PYTHON3_CONFIG ?= /opt/csw/python-3.3.2/bin/python3-config
PYTHON_DIR ?= /opt/csw/python-3.3.2
PYTHON_CFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --cflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LDFLAGS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --ldflags "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIBS ?= $(shell $(SHELL) $(TOPDIR)/makefiles/gmake/guess_python --libs "$(PYTHON3_CONFIG)" "$(CURDIR)" $(TOPDIR))
PYTHON_LIB_DIR ?= $(PYTHON_DIR)/lib
endif

endif


# cJSON 0.11
#################

ifeq ($(WITH_CJSON),1)
CJSON_DIR = $(TOPDIR)/3rdParty/libcjson
CJSON_INCLUDE_DIR = $(CJSON_DIR)
CJSON_INCLUDE_DIRS = -I$(CJSON_INCLUDE_DIR)
CJSON_LIB_DIR =
CJSON_LIB_DIRS =
CJSON_LIBS = $(CJSON_DIR)/libcjson.a
endif


# Expect (for testing)
######################

ifeq ($(WITH_EXPECT),1)

ifeq "$(PLATFORM)" "LINUX"
EXPECT ?= /usr/bin/expect
endif

ifeq "$(PLATFORM)" "FREEBSD"
EXPECT ?= /usr/local/bin/expect
endif

ifeq "$(PLATFORM)" "NETBSD"
EXPECT ?= /usr/pkg/bin/expect
endif

ifeq "$(PLATFORM)" "SUNOS"
EXPECT ?= expect
endif

endif
