CXX := clang
CXXFLAGS  := -O2 -Wall -Wno-missing-braces -std=gnu23
COBJFLAGS := $(CXXFLAGS) -c


OBJ_PATH := obj
SRC_PATH := src


TARGET := solver

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
