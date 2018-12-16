PROJECT := cfr

CONFIG_FILE := Makefile.cfg
include $(CONFIG_FILE)

CXX=g++
CXXFLAGS=-Wall -g -std=c++11

BUILD_DIR := build
BUILD_DIR_LINK := $(BUILD_DIR)
BUILD_DIR_LINK := $(BUILD_DIR)
ifeq ($(RELEASE_BUILD_DIR),)
	RELEASE_BUILD_DIR := .$(BUILD_DIR)_release
endif
ifeq ($(DEBUG_BUILD_DIR),)
	DEBUG_BUILD_DIR := .$(BUILD_DIR)_debug
endif

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	BUILD_DIR := $(DEBUG_BUILD_DIR)
	OTHER_BUILD_DIR := $(RELEASE_BUILD_DIR)
else
	BUILD_DIR := $(RELEASE_BUILD_DIR)
	OTHER_BUILD_DIR := $(DEBUG_BUILD_DIR)
endif

LIBRARY_NAME := $(PROJECT)
LIB_BUILD_DIR := $(BUILD_DIR)/lib
STATIC_NAME := $(LIB_BUILD_DIR)/lib$(LIBRARY_NAME).a
DYNAMIC_VERSION_MAJOR 		:= 1
DYNAMIC_VERSION_MINOR 		:= 0
DYNAMIC_VERSION_REVISION 	:= 0
DYNAMIC_NAME_SHORT := lib$(LIBRARY_NAME).so
DYNAMIC_VERSIONED_NAME_SHORT := $(DYNAMIC_NAME_SHORT).$(DYNAMIC_VERSION_MAJOR).$(DYNAMIC_VERSION_MINOR).$(DYNAMIC_VERSION_REVISION)
DYNAMIC_NAME := $(LIB_BUILD_DIR)/$(DYNAMIC_VERSIONED_NAME_SHORT)
COMMON_FLAGS += -DCFR_VERSION=$(DYNAMIC_VERSION_MAJOR).$(DYNAMIC_VERSION_MINOR).$(DYNAMIC_VERSION_REVISION)

ORIGIN := \$$ORIGIN
VERSIONFLAGS += -Wl,-soname,$(DYNAMIC_VERSIONED_NAME_SHORT) -Wl,-rpath,$(ORIGIN)/../lib


CXX_SRCS := $(shell find src/ ! -name "test_*.cpp" -name "*.cpp")
CXX_OBJS := $(addprefix $(BUILD_DIR)/, ${CXX_SRCS:.cpp=.o})
TEST_SRCS := $(shell find tests/ -name "test_*.cpp")
TEST_OBJS := $(addprefix $(BUILD_DIR)/, ${TEST_SRCS:.cpp=.o})
TEST_BUILD_DIR := $(BUILD_DIR)/tests
TEST_BINS := ${TEST_OBJS:.o=.bin}
TEST_BIN_LINKS := ${TEST_BINS:.bin=}
OBJS :=  $(CXX_OBJS)

WARNS_EXT := warnings.txt
CXX_WARNS := $(addprefix $(BUILD_DIR)/, ${CXX_SRCS:.cpp=.o.$(WARNS_EXT)})
TEST_WARNS := $(addprefix $(BUILD_DIR)/, ${TEST_SRCS:.cpp=.o.$(WARNS_EXT)})
WARNINGS := -Wall -Wno-sign-compare
ALL_WARNS := $(CXX_WARNS) $(TEST_WARNS)

SRC_DIRS := $(shell find * -type d -exec bash -c "find {} -maxdepth 1 \
		\( -name '*.cpp'  \) | grep -q ." \; -print)
INCLUDE_DIRS += $(BUILD_INCLUDE_DIR) ./src ./include
LIBRARY_DIRS += $(LIB_BUILD_DIR)
ALL_BUILD_DIRS := $(sort $(BUILD_DIR) $(addprefix $(BUILD_DIR)/, $(SRC_DIRS))) \
	$(LIB_BUILD_DIR)

COMMON_FLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
COMMON_FLAGS += -DDEBUG -g -O0
LINKFLAGS += -pthread -fPIC $(COMMON_FLAGS) $(WARNINGS)
CXXFLAGS += -pthread -fPIC $(COMMON_FLAGS) $(WARNINGS)
# Automatic dependency generation
CXXFLAGS += -MMD -MP
LDFLAGS += $(foreach librarydir,$(LIBRARY_DIRS),-L$(librarydir))

PYTHON_LIBRARIES ?= python2.7
PYTHON_LDFLAGS := $(LDFLAGS) $(foreach library,$(PYTHON_LIBRARIES),-l$(library))

PYCPP$(PROJECT)_SRC := python/pycpp$(PROJECT).cpp
PYCPP$(PROJECT)_SO := python/pycpp$(PROJECT).so

.PHONY: all lib test runtest

all: lib test

lib: $(STATIC_NAME) $(DYNAMIC_NAME)

test: $(TEST_BINS) $(TEST_BIN_LINKS)

runtest: $(TEST_BUILD_DIR)

warn: $(EMPTY_WARN_REPORT)

pycpp$(PROJECT): $(PYCPP$(PROJECT)_SO)


$(EMPTY_WARN_REPORT): $(ALL_WARNS) | $(BUILD_DIR)
	@ cat $(ALL_WARNS) > $@
	@ if [ -s "$@" ]; then \
		  cat $@; \
		  mv $@ $(NONEMPTY_WARN_REPORT); \
		  echo "Compiler produced one or more warnings."; \
		  exit 1; \
	  fi; \
	  $(RM) $(NONEMPTY_WARN_REPORT); \
	  echo "No compiler warnings!";

$(ALL_WARNS): %.o.$(WARNS_EXT) : %.o

$(BUILD_DIR_LINK): $(BUILD_DIR)/.linked

$(BUILD_DIR)/.linked:
		@ mkdir -p $(BUILD_DIR)
		@ $(RM) $(OTHER_BUILD_DIR)/.linked
		@ $(RM) -r $(BUILD_DIR_LINK)
		@ ln -s $(BUILD_DIR) $(BUILD_DIR_LINK)
		@ touch $@

$(ALL_BUILD_DIRS): | $(BUILD_DIR_LINK)
		@ mkdir -p $@

$(DYNAMIC_NAME): $(OBJS) | $(LIB_BUILD_DIR)
		@ echo LD -o $@
		$(Q)$(CXX) -shared -o $@ $(OBJS) $(VERSIONFLAGS) $(LINKFLAGS) $(LDFLAGS)
		@ cd $(BUILD_DIR)/lib; rm -f $(DYNAMIC_NAME_SHORT);   ln -s $(DYNAMIC_VERSIONED_NAME_SHORT) $(DYNAMIC_NAME_SHORT)


$(STATIC_NAME): $(OBJS) | $(LIB_BUILD_DIR)
		@ echo AR -o $@
		$(Q)ar rcs $@ $(OBJS)

$(BUILD_DIR)/%.o: %.cpp  $(ALL_BUILD_DIRS)
		@ echo CXX $<
		$(Q)$(CXX) $< $(CXXFLAGS) -c -o $@ 2> $@.$(WARNS_EXT) \
				|| (cat $@.$(WARNS_EXT); exit 1)
		@ cat $@.$(WARNS_EXT)

$(TEST_BUILD_DIR)/%: $(TEST_BUILD_DIR)/%.bin | $(TEST_BUILD_DIR)
		@ $(RM) $@
		@ ln -s $(notdir $<) $@

$(TEST_BINS): %.bin : %.o | $(DYNAMIC_NAME)
		@ echo CXX/LD -o $@
		$(Q)$(CXX) $< -o $@ $(LINKFLAGS) -l$(LIBRARY_NAME) $(LDFLAGS) \
				-Wl,-rpath,$(ORIGIN)/../lib

LINKFLAGS += `python -m pybind11 --includes`
$(PYCPP$(PROJECT)_SO):$(PYCPP$(PROJECT)_SRC) | $(DYNAMIC_NAME)
	    @ echo CXX/LD -o $@ $<
		$(Q)$(CXX) -O3 -shared -std=c++11 -o $@ $(PYCPP$(PROJECT)_SRC) \
				-o $@ $(LINKFLAGS) -l$(LIBRARY_NAME) $(PYTHON_LDFLAGS) \
			    -Wl,-rpath,$(ORIGIN)/../../build/lib

clean:
	@- $(RM) -rf $(ALL_BUILD_DIRS)
	@- $(RM) -rf $(OTHER_BUILD_DIR)
	@- $(RM) -rf $(BUILD_DIR_LINK)

