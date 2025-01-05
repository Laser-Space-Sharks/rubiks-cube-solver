CXX := clang

pi?=
sysroot?=

CXXFLAGS  := -O2 -Wall -Wno-missing-braces -Wno-unused-function -Wno-unused-variable -std=c23 --debug

ifeq ($(pi), true)
ifeq ($(sysroot),)
sysroot_not_specified::
	@echo "sysroot wasn't provided. Exiting..."
	@exit 1
endif

ifeq ($(sysroot), default)
SYSROOT += $$HOME/raspbian_sysroot/
else
SYSROOT += $(sysroot)
endif

CXXFLAGS  += --target=armv6-linux-gnueabihf --sysroot=$(SYSROOT)
CXXFLAGS  += -march=armv6
CXXFLAGS  += -fuse-ld=lld
endif

COBJFLAGS := $(CXXFLAGS) -c

OBJ_PATH := obj
SRC_PATH := src


TARGET := solver

ifeq ($(pi), true)
TARGET := solverpi
endif

SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

CLEANLIST := $(TARGET) \
			 $(OBJ) \
			 $(OBJ_PATH)/

# default rule
default: makedir all

# non phony targets
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(COBJFLAGS) -o $@ $<

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(OBJ_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	@echo Cleaning: $(CLEANLIST)
	@rm -rf $(CLEANLIST)
