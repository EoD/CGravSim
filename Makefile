RELEASE=false

DIR_SRC = src/
DIR_INC = inc/
DIR_OBJDEBUG = obj_d/
DIR_OBJRELEASE = obj_r/
DIR_EXE = exe/
DIR_DEP = dep/
DIR_TMP = tmp/

CC     = g++
LD     = g++
DBG_PROG=xxgdb

INCLUDES = -I$(DIR_INC)
#LIBARIES = -lsmpeg -lpthread -lid3
#OPTFLAGS = -march=athlon-xp -mtune=athlon-xp 
OPTFLAGS =
#"opteron" for K8 64bit cpu's

SRC_FILES := $(wildcard $(DIR_SRC)*.cpp)
OBJ_FILES_TMP := $(SRC_FILES:%.cpp=%.o)
DEP_FILES_TMP := $(SRC_FILES:%.cpp=%.d)
DEP_FILES := $(DEP_FILES_TMP:$(DIR_SRC)%=$(DIR_DEP)%)
SRC_FILES_NOEXT := $(notdir $(basename $(SRC_FILES)))

ifeq ($(RELEASE),false) #The variable Release is set to false? then build with debug stuff
#No optimization! Enable highest debuglevel, profiling, and display every single warning
Releasename=debug
CFLAGS = -O0 -g3 -Wall -D_DEBUG32
OBJ_FILES := $(OBJ_FILES_TMP:$(DIR_SRC)%=$(DIR_OBJDEBUG)%)
else
#turn on some optimization
Releasename=release
CFLAGS = -O3 -g -Wall
OBJ_FILES := $(OBJ_FILES_TMP:$(DIR_SRC)%=$(DIR_OBJRELEASE)%)
endif

#Name of the executable:
EXE_NAME = gravitysim_$(Releasename)

LDFLAGS  = $(CFLAGS) $(INCLUDES) $(LIBARIES) $(OPTFLAGS)

all: $(DEP_FILES) $(OBJ_FILES)
	@echo "# Linking to $(DIR_EXE)$(EXE_NAME)"
	$(LD) $(LDFLAGS) -o $(DIR_EXE)$(EXE_NAME) $(OBJ_FILES)
	@echo "Done!"

$(DIR_OBJDEBUG)%.o : $(DIR_SRC)%.cpp
	@echo "# Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@echo ""

$(DIR_OBJRELEASE)%.o : $(DIR_SRC)%.cpp
	@echo "# Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 
	@echo ""

#Automatic dependency generation and update
$(DIR_DEP)%.d : $(DIR_SRC)%.cpp 
	@echo "Update dependencies for $<"
	@$(CC) -MM $< $(INCLUDES) -MT obj/$(basename $(notdir $<)).o > $@

run: all
	@echo "Executing $(DIR_EXE)$(EXE_NAME)"
	$(DIR_EXE)$(EXE_NAME) 

rundebug: all
	@echo "Executing $(DBG_PROG) $(DIR_EXE)$(EXE_NAME)" 
	$(DBG_PROG) $(DIR_EXE)$(EXE_NAME)

clean:
	@echo "Doing some cleanup..."
	-rm -f $(DIR_OBJDEBUG)*
	-rm -f $(DIR_OBJRELEASE)*
	-rm -f $(DIR_EXE)*
	-rm -f $(DIR_DEP)*
	-rm -f $(DIR_TMP)*
	mkdir -p $(DIR_OBJDEBUG)
	mkdir -p $(DIR_OBJRELEASE)
	mkdir -p $(DIR_EXE)
	mkdir -p $(DIR_DEP)
	mkdir -p $(DIR_TMP)
	-rm -rf gmon.out core *.tar.gz
	@echo "Cleanup done"

stats:
	@echo "Stats of Sourcefiles"
	@echo "  Lines Words Size"
	@cd $(DIR_SRC) ; wc *
	@echo "Stats of Includefiles"
	@echo "  Lines Words Size"
	@cd $(DIR_INC) ; wc *

tar:
	tar cfvz LanJukeBoxServer.tar.gz Makefile jukecfg.txt src/ inc/

docs:
	doxygen doc/Doxyfile

-include $(DEP_FILES)

help:
	@echo "\
make <target> Releasename=<releasename> out of:\n\
(If Releasename is not given a debug version is build)\n\
######################################################################################\n\
 all   : Build up dependencies and the executable                                   \n\
 run   : Build "all" and execute the program                                        \n\
 clean : Cleanup the whole projekt (obj, dep and exe folders will be fully purged)  \n\
         all output files will also be deleted (including reports, logs etc)        \n\
######################################################################################\n"
