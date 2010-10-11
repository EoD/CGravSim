ifndef RELEASE
 RELEASE=true
endif

ifndef ARCH
 ARCH=x86
else
 ifeq ($(ARCH),x86_64)
  ARCH=amd64
 endif
 ifeq ($(ARCH),i686)
  ARCH=x86
 endif
endif

DIR_SRC = src/
DIR_INC = inc/
DIR_EXE = exe/
DIR_DEP = dep_${ARCH}/

DBG_PROG = gdb --ex run --args
MKDIR	= mkdir -p
RM	= rm -rf

#######
##GCC - default compiler
#######
#GCC 4.6: (-Ofast) == (-O3 -ffast-math) maybe also -D_GLIBCXX_PARALLEL
CFLAGS	= -Wall -fopenmp -mtune=generic
CFLAGS_RELEASE	= -g -O3 -ffast-math -funroll-loops
CFLAGS_DEBUG	= -g3 -pedantic -Wno-long-long -Wextra
CC	= ${CROSS_COMPILE}g++
LD	= $(CC)
DFLAGS	= -MM $< $(INCLUDES) -MT ${DIR_OBJ}$(basename $(notdir $<)).o

#######
##CC - Sun Studio 12 compiler
#######
#CC 5.9: no -xopenmp; OpenMP standard too old
#CC	= CC
#LD	= $(CC)
#CFLAGS	= -m64
#CFLAGS_RELEASE	= +w -fast -xarch=generic -xautopar
#CFLAGS_DEBUG	= +w2
#DFLAGS	= -xM1 $< $(INCLUDES) 

#######
##ICC - Intel C++ Compiler
#######
#CC	= icc
#LD	= icpc
#CFLAGS	= -openmp -parallel
#CFLAGS_RELEASE	= -fast -xSSE2 -funroll-loops
#CFLAGS_DEBUG	= -Wall -static-intel
#DFLAGS	= -MM $< $(INCLUDES) -MT ${DIR_OBJ}$(basename $(notdir $<)).o

INCLUDES = -I$(DIR_INC)

ifeq ($(RELEASE),false) #The variable Release is set to false? then build with debug stuff
POSTFIX=dbg
CFLAGS += -D_DEBUG $(CFLAGS_DEBUG)
DIR_OBJ = obj_${ARCH}_d/
else
POSTFIX=
CFLAGS += $(CFLAGS_RELEASE)
DIR_OBJ = obj_${ARCH}_r/
endif

#Name of the executable:
ifdef WINDOWS
 EXE_NAME = cgravsim$(POSTFIX)_${ARCH}.exe
 LD = ${CROSS_COMPILE}${GCC} -static-libgcc
else
 EXE_NAME = cgravsim$(POSTFIX)_${ARCH}
endif

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


all: $(DEP_FILES) $(OBJ_FILES)
	@echo "# Linking to $(DIR_EXE)$(EXE_NAME)"
	$(LD) $(CFLAGS) $(INCLUDE) -o $(DIR_EXE)$(EXE_NAME) $(OBJ_FILES)
	@echo "Done!"
	@echo ""

$(DIR_OBJ)%.o : $(DIR_SRC)%.cpp
	-@${MKDIR} ${DIR_OBJ}
	@echo "# Compiling for ${ARCH} $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@echo ""

#Automatic dependency generation and update
$(DIR_DEP)%.d : $(DIR_SRC)%.cpp 
#	@echo "Update dependencies for ${ARCH} $<"
	-@${MKDIR} ${DIR_DEP}
	@$(CC) $(DFLAGS) > $@


run: all
	@echo "Executing $(DIR_EXE)$(EXE_NAME)"
	$(DIR_EXE)$(EXE_NAME) 

rundebug: all
	@echo "Executing $(DBG_PROG) $(DIR_EXE)$(EXE_NAME)" 
	$(DBG_PROG) $(DIR_EXE)$(EXE_NAME)

debug:
	@RELEASE=false make auto

auto:
	@ARCH=`(uname -m 2>/dev/null) || echo x86` make all

arch_auto: auto

arch_win32:
	@WINDOWS=true make arch_x86

arch_i686: arch_x86
arch_x86:
	@(RELEASE=false ARCH=x86 make) && (RELEASE=true ARCH=x86 make)
	
arch_x86_64: arch_amd64
arch_amd64:
	@(RELEASE=false ARCH=amd64 make) && (RELEASE=true ARCH=amd64 make)
	
x86_64:	amd64
amd64:
	@(make arch_x86) && (make arch_amd64)

arch_sparc:
	@(RELEASE=false ARCH=sparc make) && (RELEASE=true ARCH=sparc make)

arch_sparc64:
	@(RELEASE=false ARCH=sparc64 make) && (RELEASE=true ARCH=sparc64 make)

sparc:
	@(make arch_sparc) && (make arch_sparc64)

cleanarch:
	@echo "Doing only some ${ARCH} architecture cleanup..."
	-@${RM} $(DIR_OBJ)
	-@${RM} $(DIR_DEP)
	@echo "Cleanup done"

cleantmp:
	@echo "Doing obj and dep cleanup..."
	-@${RM} obj_*
	-@${RM} dep_*
	@echo "Cleanup done"

clean:
	@echo "Doing some cleanup..."
	-@${RM} obj_*
	-@${RM} $(DIR_EXE)*
	-@${RM} dep_*
	-@${MKDIR} $(DIR_EXE)
	@echo "Cleanup done"

stats:
	@echo "Stats of Sourcefiles"
	@echo " Lines Words Size"
	@cd $(DIR_SRC) ; wc *
	@echo "Stats of Includefiles"
	@echo " Lines Words Size"
	@cd $(DIR_INC) ; wc *

tar:
	@echo "Creating tar jgravsim_backend_`date +%Y%m%d%H%M%S`.tar.gz of important files:"
	@tar cfvz jgravsim_backend_`date +%Y%m%d%H%M%S`.tar.gz Makefile ${DIR_SRC}* ${DIR_INC}* Gravity_Simulation_Backend.*

docs:
	-${MKDIR} doc/
	doxygen doc/Doxyfile

help:
	@echo -e "\
USAGE: ARCH=[x86|amd64|sparc|sparc64|win32] RELEASE=[true|false] make [COMMAND]			\n\
 If RELEASE is not given a release version is build.						\n\
 If ARCH is not defined, x86 is assumed.							\n\
 If you want to use the Intel C++ compiler or the Sun Studio compiler instead of GCC		\n\
 uncomment the lines in the Makefile.								\n\
												\n\
Available COMMANDs:\n\
 Build:\n\
  all		: Build up dependencies and the executable					\n\
  auto		: Build up dependencies and the executable with automatic ARCH detection	\n\
  debug		: Build the executable with automatic ARCH detection and RELEASE=false		\n\
  arch_ARCH	: Build the project with ARCH (x86,amd64,sparc,sparc64,win32)			\n\
  amd64		: Build the project for arch_x86 and arch_amd64					\n\
  sparc		: Build the project for arch_sparc and arch_sparc64				\n\
 Run:\n\
  run		: Build all and execute the program						\n\
  rundebug	: Build all and execute the program with gdb					\n\
 Clean:\n\
  clean		: Cleanup the whole project (obj, dep and exe folders will be fully purged)	\n\
  cleantmp	: Cleanup all temporary folders (obj and dep folders will be purged)		\n\
  cleanarch	: Cleanup parts of the project (obj_ARCH and dep_ARCH folders will be purged)	\n\
 Others:\n\
  stats		: Calculate wc-stats of src/ and inc/						\n\
  tar		: Create a tar of inc/ src/ Makefile and Gravity_Simulation_Backend.*		\n"

-include $(DEP_FILES)
