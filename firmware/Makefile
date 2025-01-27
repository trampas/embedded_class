include make_jobs.mk #set the number of parallel compiles we can do 


COMPILER_PATH = $(CURDIR)/xpacks/@xpack-dev-tools/arm-none-eabi-gcc/.content/bin
TOOLS_PATH = $(CURDIR)/xpacks/@xpack-dev-tools/windows-build-tools/.content/bin
COMPILER_PREFIX = $(COMPILER_PATH)/arm-none-eabi-

SRC_DIR   = src
BUILD_DIR = build
OUTPUT = example
ELF		  = $(OUTPUT).elf
LD_SCRIPT  =  $(SRC_DIR)/samd51j19a_flash.ld
LDLIBS = 

DEFINES = __ATSAMD51J19A__  __SAMD51__ ARM_MATH_CM4 ENABLE_CACHE
INCULDE_PATHS= $(SRC_DIR) $(SRC_DIR)/CMSIS/samd51a/include  $(SRC_DIR)/CMSIS/Include $(SRC_DIR)/startup


# Make does not offer a rec
ursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

C_SRCS := $(strip $(call rwildcard,$(SRC_DIR)/,*.c))
CXX_SRCS = $(strip $(call rwildcard,$(SRC_DIR)/,*.cpp))


INC_PARAMS=$(foreach d, $(INCULDE_PATHS), -I$d)
DEFINE_PARAMS =$(foreach d, $(DEFINES), -D$d)

#this will remove unused functions only used on release builds
REMOVE_UNUSED_FUNC_FLAGS= -ffunction-sections -fdata-sections -Wl,--gc-sections -fmessage-length=0


COMMON_FLAGS =  $(DEFINE_PARAMS)  -Wall -mthumb -mcpu=cortex-m4 -fsigned-char  
COMMON_FLAGS += -Wextra -Wpointer-arith -Wlogical-op -Wfloat-equal -Wno-unused-parameter
#COMMON_FLAGS +=-Wconversion -Wmissing-declarations -Wunused -Wuninitialized

LD_FLAGS= -T$(LD_SCRIPT)  -mthumb -mcpu=cortex-m4 -fsigned-char 
LD_FLAGS +=  --specs=nano.specs  -Wl,--cref  --specs=nosys.specs
# -static-libgcc -static-libstdc++
C_FLAGS=$(COMMON_FLAGS) $(INC_PARAMS) $(CFLAGS)
CXX_FLAGS=$(COMMON_FLAGS) $(INC_PARAMS) $(CXXFLAGS) -fno-rtti -fno-exceptions -Wno-register


.DEFAULT_GOAL := release

.PHONY: debug release clean  all

all: release debug 

C_OBJS=$(patsubst %.c,%.o,$(C_SRCS)) 
CXX_OBJS=$(patsubst %.cpp,%.o,$(CXX_SRCS))

dir_guard=@$(TOOLS_PATH)/mkdir -p $(@D)


CC = $(COMPILER_PREFIX)gcc
CXX = $(COMPILER_PREFIX)g++
OBJ_DUMP = $(COMPILER_PREFIX)objdump
OBJ_COPY = $(COMPILER_PREFIX)objcopy
SIZE = $(COMPILER_PREFIX)size
#
# Debug build settings
#
DBGDIR = $(BUILD_DIR)/debug
DBGEXE = $(DBGDIR)/$(ELF)
DBGC_OBJS = $(addprefix $(DBGDIR)/, $(C_OBJS))
DBGCXX_OBJS = $(addprefix $(DBGDIR)/, $(CXX_OBJS))
DBGC_FLAGS = $(C_FLAGS) -g -O1 -DDEBUG=1
DBGCXX_FLAGS = $(CXX_FLAGS) -g -O1 -DDEBUG=1


#
# Debug build settings
#
RELDIR = $(BUILD_DIR)/release
RELEXE = $(RELDIR)/$(ELF)
RELC_OBJS = $(addprefix $(RELDIR)/, $(C_OBJS))
RELCXX_OBJS = $(addprefix $(RELDIR)/, $(CXX_OBJS))
RELC_FLAGS = $(C_FLAGS) $(REMOVE_UNUSED_FUNC_FLAGS) -Os -DNDEBUG
RELCXX_FLAGS = $(CXX_FLAGS) $(REMOVE_UNUSED_FUNC_FLAGS) -Os -DNDEBUG

-include $(DBGC_OBJS:%.o=%.d)
-include $(DBGCXX_OBJS:%.o=%.d)
-include $(RELC_OBJS:%.o=%.d)
-include $(RELCXX_OBJS:%.o=%.d)


debug: $(DBGEXE)


$(DBGEXE):$(DBGC_OBJS) $(DBGCXX_OBJS) $(LD_SCRIPT)
	$(info    LD $(DBGEXE))
	@$(CXX) $(LD_FLAGS) $(DBGCXX_FLAGS) -o $(DBGEXE)  $(DBGC_OBJS) $(DBGCXX_OBJS) $(LDLIBS) -Wl,-Map="$(DBGDIR)/$(OUTPUT).map"
#	# $(OBJ_COPY) --strip-unneeded $(DBGDIR)/$(OUTPUT)_tmp.elf $(DBGEXE) 
	@$(OBJ_DUMP) -dlr $(DBGEXE) >$(DBGDIR)/$(OUTPUT).lst
	@$(OBJ_COPY) -O ihex  $(DBGEXE) $(DBGDIR)/$(OUTPUT).hex
	$(SIZE) $(DBGEXE)


$(DBGC_OBJS):$(DBGDIR)/%.o: %.c
	$(dir_guard)
	$(info    CC $<)
	@$(CC) -MMD -MP -MF $(@:.o=.d) $(DBGC_FLAGS) -c $< -o $@  

$(DBGCXX_OBJS):$(DBGDIR)/%.o: %.cpp
	$(dir_guard)
	$(info    CCX $<)
	@$(CXX) -MMD -MP -MF $(@:.o=.d) $(DBGCXX_FLAGS) -c $< -o $@  

release: $(RELEXE) 

$(RELEXE) : $(RELC_OBJS) $(RELCXX_OBJS) $(LD_SCRIPT)
	$(dir_guard)
	$(info    LD $(RELEXE))
	@$(CXX) $(LD_FLAGS) $(RELCXX_FLAGS) -o $(RELEXE) $(RELC_OBJS) $(RELCXX_OBJS) $(LDLIBS) -Wl,-Map="$(RELDIR)/$(OUTPUT).map"

$(RELC_OBJS):$(RELDIR)/%.o: %.c
	$(dir_guard)
	$(info    CC $<)
	@$(CC) -MMD -MP -MF $(@:.o=.d) $(RELC_FLAGS) -c $< -o $@  

$(RELCXX_OBJS):$(RELDIR)/%.o: %.cpp
	$(dir_guard)
	$(info    CCX $<)
	@$(CXX) -MMD -MP -MF $(@:.o=.d) $(RELCXX_FLAGS) -c $< -o $@  

clean:
	$(TOOLS_PATH)/rm -rf $(BUILD_DIR)

#make sure every target is dependent on the make file and included makefiles
.EXTRA_PREREQS+=Makefile make_jobs.mk