ifndef RELEASE
 RELEASE=true
endif

ifndef ARCH
 ARCH=x86
endif

DIR_SRC = src/
DIR_INC = inc/
DIR_EXE = exe/
DIR_DEP = dep/
DIR_TMP = tmp/

CC     = ${CROSS_COMPILE}g++
LD     = ${CROSS_COMPILE}g++
DBG_PROG = gdb --ex run --args
MKDIR	= mkdir -p
RM	= rm -f

INCLUDES = -I$(DIR_INC)
OPTFLAGS = -mtune=generic

ifeq ($(RELEASE),false) #The variable Release is set to false? then build with debug stuff
#Enable highest debuglevel, profiling, and display every single warning
Releasename=dbg
CFLAGS = -O2 -g3 -Wall -D_DEBUG32
DIR_OBJ = obj_d/
else
#turn on some optimization
Releasename=
CFLAGS = -O2 -g -Wall
DIR_OBJ = obj_r/
endif

#Name of the executable:
EXE_NAME = cgravsim$(Releasename)_${ARCH}

SRC_FILES := $(wildcard $(DIR_SRC)*.cpp)
OBJ_FILES_TMP := $(SRC_FILES:%.cpp=%.o)
DEP_FILES_TMP := $(SRC_FILES:%.cpp=%.d)
DEP_FILES := $(DEP_FILES_TMP:$(DIR_SRC)%=$(DIR_DEP)%)
SRC_FILES_NOEXT := $(notdir $(basename $(SRC_FILES)))
OBJ_FILES := $(OBJ_FILES_TMP:$(DIR_SRC)%=$(DIR_OBJ)%)

ifeq ($(ARCH),x86)
CFLAGS += -m32
endif

ifeq ($(ARCH),amd64)
CFLAGS += -m64
endif

ifeq ($(ARCH),sparc)
CFLAGS += -m32
endif

ifeq ($(ARCH),sparc64)
CFLAGS += -m64
endif

LDFLAGS  = $(CFLAGS) $(INCLUDES) $(LIBARIES) $(OPTFLAGS)

all: $(DEP_FILES) $(OBJ_FILES)
	@echo "# Linking to $(DIR_EXE)$(EXE_NAME)"
	$(LD) $(LDFLAGS) -o $(DIR_EXE)$(EXE_NAME) $(OBJ_FILES)
	@echo "Done!"
	@echo ""

$(DIR_OBJ)%.o : $(DIR_SRC)%.cpp
	@echo "# Compiling for ${ARCH} $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@echo ""

#Automatic dependency generation and update
$(DIR_DEP)%.d : $(DIR_SRC)%.cpp 
	@echo "Update dependencies for $<"
	@$(CC) -MM $< $(INCLUDES) -MT ${DIR_OBJ}$(basename $(notdir $<)).o > $@

run: all
	@echo "Executing $(DIR_EXE)$(EXE_NAME)"
	$(DIR_EXE)$(EXE_NAME) 

rundebug: all
	@echo "Executing $(DBG_PROG) $(DIR_EXE)$(EXE_NAME)" 
	$(DBG_PROG) $(DIR_EXE)$(EXE_NAME)

arch_x86:
	RELEASE=false ARCH=amd64 make
	RELEASE=true ARCH=amd64 make
	make cleanarch
	RELEASE=false ARCH=x86 make
	RELEASE=true ARCH=x86 make

arch_sparc:
	RELEASE=false ARCH=sparc64 make cleanarch
	RELEASE=true ARCH=sparc64 make cleanarch
	RELEASE=false ARCH=sparc make cleanarch
	RELEASE=true ARCH=sparc make cleanarch

cleanarch:
	@echo "Doing only some architecture cleanup..."
	-${RM} $(DIR_OBJ)*
	-${RM} $(DIR_DEP)*
	-${RM} $(DIR_TMP)*
	-${MKDIR} $(DIR_OBJ)
	-${MKDIR} $(DIR_DEP)
	-${MKDIR} $(DIR_TMP)
	make all
	@echo "Cleanup done"

clean:
	@echo "Doing some cleanup..."
	-${RM} $(DIR_OBJ)*
	-${RM} $(DIR_EXE)*
	-${RM} $(DIR_DEP)*
	-${RM} $(DIR_TMP)*
	-${MKDIR} $(DIR_OBJ)
	-${MKDIR} $(DIR_EXE)
	-${MKDIR} $(DIR_DEP)
	-${MKDIR} $(DIR_TMP)
	@echo "Cleanup done"
	-${MKDIR} $(DIR_DEP)
	-${MKDIR} $(DIR_OBJ)

stats:
	@echo "Stats of Sourcefiles"
	@echo " Lines Words Size"
	@cd $(DIR_SRC) ; wc *
	@echo "Stats of Includefiles"
	@echo " Lines Words Size"
	@cd $(DIR_INC) ; wc *

tar:
	tar cfvz jgravsim_backend_`date +%Y%m%d%H%M%S`.tar.gz Makefile ${DIR_SRC}* ${DIR_INC}* Gravity_Simulation_Backend.*

docs:
	-${MKDIR} doc/
	doxygen doc/Doxyfile

help:
	@echo -e "\
USAGE: ARCH=[x86|amd64|sparc|sparc64] RELEASE=[true|false] make [COMMAND]\n\
(If RELEASE is not given a release version is build)\n\n\
Available COMMANDs:\n\
 Build:\n\
  all		: Build up dependencies and the executable					\n\
  arch_x86	: Build the project with ARCH=x86 and afterwards ARCH=amd64			\n\
  arch_sparc	: Build the project with ARCH=sparc and afterwards ARCH=sparc64			\n\
  cleanarch	: Cleanup parts of the project (obj and dep folders) and build 'all'		\n\
 Run:\n\
  run		: Build all and execute the program						\n\
  rundebug	: Build all and execute the program with gdb					\n\
 Clean:\n\
  clean		: Cleanup the whole project (obj, dep and exe folders will be fully purged)	\n\
		\t  all output files will also be deleted (including reports, logs etc)		\n\
 Others:\n\
  stats		: Calculate wc-stats of src/ and inc/						\n\
  tar		: Create a tar of inc/ src/ Makefile and Gravity_Simulation_Backend.*		\n"

-include $(DEP_FILES)
