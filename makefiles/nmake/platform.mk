# Sets Windows specific variables
#
# provides:
# - BOOST_LIBRARY_TAG: how libraries are auto-tagged, for example
#   libboost_log-vc90-mt-1_42
# - OPENSSL_LIBS: libraries to link against for SSL/TLS support
# - EXPECT: location of the expect binary for automated testing

!INCLUDE $(TOPDIR)\makefiles\nmake\config.mk

# Boost
#######

BOOST_LIBRARY_TAG=-$(BOOST_VC_VER)$(BOOST_MT)-$(BOOST_VERSION)

# OpenSSL
#########

!IFDEF WITH_SSL
OPENSSL_LIBS = libeay32.lib ssleay32.lib
!ENDIF

# Tcl (for testing with Expect)
###############################

!IFDEF WITH_EXPECT
EXPECT = $(TCL_DIR)\bin\tclsh.exe
!ENDIF

# WIX Microsoft Installer (for deployment)
##########################################

CANDLE = "$(WIX_DIR)\bin\candle.exe"
LIGHT = "$(WIX_DIR)\bin\light.exe"
LIT = "$(WIX_DIR)\bin\lit.exe"
SMOKE = "$(WIX_DIR)\bin\smoke.exe"
SETUPBLD = "$(WIX_DIR)\bin\setupbld.exe"
