# Configuration supposed to be configured here by the user
#
# must configure:
# - BOOST_VERSION: version of boost
# - BOOST_DIR: location of boost intallation
# - BOOST_INCLUDE_DIR and BOOST_LDFLAGS
# - BOOST_VC_VER: visual studio boost version tag for libraries
#
# optionally:
# - OPENSSL_DIR: location of the OpenSSL library (WITH_SSL=1 only)
# - PGSQL_DIR: location of Postgres libpq and header files (WITH_PGSQL=1 only)
# - TCL_DIR: location of the Active Tcl installation (for testing only)
# - EXPECT: location of the expect interpreter (for testing only)

# please customize

# Boost (http://www.boostpro.com/download)
###########################################

# version of the boost library

#BOOST_VERSION = 1_46_1
BOOST_VERSION = 1_52

# base dir where boost is installed

#BOOST_DIR = C:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Program Files\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = D:\Programme\boost\boost_$(BOOST_VERSION)
#BOOST_DIR = C:\Cygwin\home\Andreas Baumann\boost_1_51_0-win32
#BOOST_DIR = D:\boost\boost_1_45_0
#BOOST_DIR = D:\Program Files\boost\boost_$(BOOST_VERSION)
BOOST_DIR = C:\Boost

# depends on the choosen boost layout
BOOST_INCLUDE_DIR = $(BOOST_DIR)\Include\boost-$(BOOST_VERSION)
#BOOST_INCLUDE_DIR = $(BOOST_DIR)

# visual studio version used for compiling

#BOOST_VC_VER = vc80
#BOOST_VC_VER = vc90
BOOST_VC_VER = vc100

# TODO: probe those
BOOST_MT = -mt

# preinstalled binaries
BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib64"

# self-compiled
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\stage\lib"
#BOOST_LDFLAGS = /LIBPATH:"$(BOOST_DIR)\lib\Win32"

# precompiled libraries from http://boost.teeks99.com
#BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib32
BOOST_LDFLAGS = /LIBPATH:$(BOOST_DIR)\lib64

# ICU (icu4c)
#############

#ICU_DIR = C:\Cygwin\home\Andreas Baumann\icu-49.1.2-win32-debug

# the library version (how the DLLs are tagged)
#ICU_LIB_VERSION = 49

# OpenSSL (http://www.slproweb.com/products/Win32OpenSSL.html)
##############################################################

#OPENSSL_DIR = C:\Programme\OpenSSL-1.0.0a
#OPENSSL_DIR = c:\OpenSSL\openssl-1.0.1-win32-debug
#OPENSSL_DIR = D:\Programme\OpenSSL
OPENSSL_DIR = D:\OpenSSL\Win64
#OPENSSL_DIR = D:\Program Files\OpenSSL-1.0.0d

# Postgresql libpq
# (http://www.postgresql.org/)
##############################

#PGSQL_DIR = C:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = D:\Program Files\PostgreSQL\9.0
#PGSQL_DIR = C:\cygwin\home\Andreas Baumann\postgresql-9.1.3-win32-debug

# enable if Postgresql has been compiled with internationalization support
# (libintl and libiconv)
#PGDLL_WITH_I18N = 1

# enable depending on libintl.dll and libiconv.dll (deployment only)
PGDLL_WITHOUT_MAJOR_VERSION = 1

# win_iconv
###########

WIN_ICONV_DIR = C:\cygwin\home\Andreas Baumann\win-iconv-0.0.3-win32-debug

# libxml2
#########

LIBXML2_DIR = C:\cygwin\home\Andreas Baumann\libxml2-2.7.8-win32-debug

# libxslt
#########

LIBXSLT_DIR = C:\cygwin\home\Andreas Baumann\libxslt-1.1.26-win32-debug

# Tcl 8.5 and Expect
####################

TCL_DIR = C:\Program Files\Tcl
#TCL_DIR = D:\Program Files\Tcl8.5.9
EXPECT = $(TCL_DIR)\bin\tclsh.exe

# Windows Installer (WIX)
# used for deployment only!
#########################

WIX_DIR = C:\Program Files\Windows Installer XML v3.5

#WIX_LIBS = dutil_2008.lib wcautil_2008.lib
WIX_LIBS = dutil_2008_x64.lib wcautil_2008_x64.lib

# architecture of resulting msi
WIX_ARCH = x86
#WIX_ARCH = x64

#PGDLL_LIBRARIES=1

#PGDLL_WITHOUT_MAJOR_VERSION=1

# define for 64 bit, undefine for 32 bit
#WIN64=1

# DocBook and Stylesheets
#########################

XSLT_HTMLHELP_STYLESHEET = C:\cygwin\home\Andreas Baumann\docbook-xsl-1.76.1\htmlhelp\htmlhelp.xsl

# Doxygen binary to generate API documentation (for docu and deployment)
########################################################################

DOXYGEN = C:\Program Files\Doxygen\bin\doxygen.exe

# Microsoft HTML Help Workshop (for documentation building and deployment)
##########################################################################

HHC_LOCATION = C:\Program Files\HTML Help Workshop\hhc.exe

# CCacche
#########

#CC="C:\cygwin\home\Andreas Baumann\ccache.exe" "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\cl.exe"
#CXX="C:\cygwin\home\Andreas Baumann\ccache.exe" "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\cl.exe"
