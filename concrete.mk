CXX		:= $(CCACHE) $(SDK)/toolchain/linux_x86/bin/nacl-g++

CPPFLAGS	+= -I$(SDK)/toolchain/linux_x86/include \
		   -I$(SDK)/toolchain/linux_x86/nacl64/include \
		   -DCONCRETE_PORTABLE_BYTEORDER=true

CFLAGS		+= -pthread \
		   -fno-stack-protector \
		   -fdiagnostics-show-option
