ARCH		:= x86_$(BITS)

NAME		:= concrete
SOURCES		:= $(wildcard nacl/*.cpp) \
		   $(wildcard nacl/*/*.cpp) \
		   $(wildcard $(O)/src/concrete/*.cpp) \
		   $(wildcard $(O)/src/concrete/*/*.cpp) \
		   $(wildcard $(O)/src/concrete/*/*/*.cpp)

CXX		:= $(CCACHE) $(TOOLCHAIN)/bin/x86_64-nacl-g++
CFLAGS		+= -pthread -m$(BITS)
LIBS		+= -lppapi -lppapi_cpp

include build/common.mk

$(O)/bin/$(NAME)_$(ARCH).nexe: $(SOURCES:%.cpp=$(O)/obj/$(ARCH)/%.o)
	$(call echo,Link,$@)
	$(QUIET) mkdir -p $(dir $@)
	$(QUIET) $(CXX) -o $@ $(CFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) $(LIBS)

$(O)/obj/$(ARCH)/%.o: %.cpp
	$(call echo,Compile,$@)
	$(QUIET) mkdir -p $(dir $@)
	$(QUIET) $(CXX) -o $@ $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -c $*.cpp \
		-MD -MF $(O)/obj/$(ARCH)/$(patsubst %,%.d,$*.cpp) -MT $(O)/obj/$(ARCH)/$*.o

-include $(SOURCES:%=$(O)/obj/$(ARCH)/%.d)
