TOPDIR = .

-include $(TOPDIR)/makefiles/gmake/platform.mk

SUBDIRS = 3rdParty src wtest tests clients include config docs

ifeq ($(WITH_EXAMPLES),1)
SUBDIRS += examples
endif

PACKAGE_NAME = wolframe
PACKAGE_VERSION = 0.0.3

-include $(TOPDIR)/makefiles/gmake/top.mk

ifeq ($(WITH_EXAMPLES),1)
examples: src
tests: src
endif
