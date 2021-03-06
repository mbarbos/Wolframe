# provides generic rules for C/C++ dependeny generation using
# 'makedepend', 'gcc -MM' or similar mechanisms
#
# requires:
# - compilers CC and CXX
# - INCLUDEDIRS
# - OBJS, CPP_OBJS and BIN_OBJS, CPP_BIN_OBJS
# - TEST_BINS, TEST_BIN_OBJS, TEST_CPP_BINS, TEST_CPP_BIN_OBJS
#
# provides:
# - included dependency files
#
# author: Andreas Baumann, abaumann at yahoo dot com

ifdef MAKEDEPEND

%.d : %.c
	@echo Generating dependencies for $<
	@$(MAKEDEPEND) $(@:.d=.o) $(ALL_CFLAGS) $< > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(MAKEDEPEND) $(@:.d=.o) $(ALL_CFLAGS) $< > $@

else

ifeq "$(COMPILER)" "gcc"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CXX) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CXXFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@
	  
endif

ifeq "$(COMPILER)" "clang"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CXX) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CXXFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@
	  
endif

ifeq "$(COMPILER)" "icc"

%.d : %.c
	@echo Generating dependencies for $<
	@$(CC) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@

%.d : %.cpp
	@echo Generating dependencies for $<
	@$(CXX) -DMAKE_DEPENDENCIES -MM -MT $(@:.d=.o) $(ALL_CXXFLAGS) $< | \
		( tr -d "\n"; echo "" ) | tr -d  '\\' | \
		sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\&\&\&\1 : \2,g' | \
		tr -s '&' "\n" > $@
	  
endif

endif

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(OBJS:.o=.d)
-include $(CPP_OBJS:.o=.d)
-include $(BIN_OBJS:.o=.d)
-include $(CPP_BIN_OBJS:.o=.d)
-include $(TEST_BIN_OBJS:.o=.d)
-include $(TEST_CPP_BIN_OBJS:.o=.d)
ifneq "$(DYNAMIC_MODULE)" ""
DYNAMIC_MODULE_OBJ=$(DYNAMIC_MODULE:.so=.o)
-include $(DYNAMIC_MODULE_OBJ:.o=.d)
endif

.PHONY: depend_recursive
depend_recursive:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d depend || exit 1); done)

.PHONY: depend
depend: depend_recursive $(OBJS:.o=.d) $(CPP_OBJS:.o=.d) $(BIN_OBJS:.o=.d) $(CPP_BIN_OBJS:.o=.d) $(TEST_BIN_OBJS:.o=.d) $(TEST_CPP_BIN_OBJS:.o=.d) $(DYNAMIC_MODULE_OBJ:.o=.d)

endif
endif
