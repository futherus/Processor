export CXX      ?= gcc
export CXXFLAGS ?= -O2 -mavx -mavx2 -g --static-pie -std=c++14 -fmax-errors=100 -Wall -Wextra   \
    			   -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat 	   					\
    			   -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual 	   					\
    			   -Wchar-subscripts -Wconditionally-supported -Wconversion        				\
    			   -Wctor-dtor-privacy -Wempty-body -Wfloat-equal 		   						\
    			   -Wformat-nonliteral -Wformat-security -Wformat-signedness       				\
    			   -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op 	           				\
    			   -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd 	   					\
    			   -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls 				\
    			   -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192      				\
    			   -Wstrict-null-sentinel -Wstrict-overflow=2 			   						\
    			   -Wsuggest-attribute=noreturn -Wsuggest-final-methods 	   					\
    			   -Wsuggest-final-types -Wsuggest-override -Wswitch-default 	   				\
    			   -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused   				\
    			   -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix 	   					\
    			   -Wno-missing-field-initializers -Wno-narrowing 	           					\
    			   -Wno-old-style-cast -Wno-varargs -fcheck-new 		   						\
    			   -fsized-deallocation -fstack-check -fstack-protector            				\
    			   -fstrict-overflow -flto-odr-type-merging 	   		   						\
    			   -fno-omit-frame-pointer                                         				\
    			   -fPIE                                                           				\
				   -fsanitize=address 	                                           				\
				   -fsanitize=alignment                                            				\
				   -fsanitize=bool                                                 				\
				   -fsanitize=bounds                                               				\
				   -fsanitize=enum                                                 				\
				   -fsanitize=float-cast-overflow 	                           					\
				   -fsanitize=float-divide-by-zero 			           							\
				   -fsanitize=integer-divide-by-zero                               				\
				   -fsanitize=leak 	                                           					\
				   -fsanitize=nonnull-attribute                                    				\
				   -fsanitize=null 	                                           					\
				   -fsanitize=object-size                                          				\
				   -fsanitize=return 		                                   					\
				   -fsanitize=returns-nonnull-attribute                            				\
				   -fsanitize=shift                                                				\
				   -fsanitize=signed-integer-overflow                              				\
				   -fsanitize=undefined                                            				\
				   -fsanitize=unreachable                                          				\
				   -fsanitize=vla-bound                                            				\
				   -fsanitize=vptr                                                 				\
				   -lm -pie 					 

# not overwrite DESTDIR if recursive
export DESTDIR 	 ?= $(CURDIR)/bin
ASSEMBLER_TARGET := $(DESTDIR)/assembler
CPU_TARGET 		 := $(DESTDIR)/cpu

export OBJDIR 	 := $(CURDIR)/obj
ASSEMBLER_OBJ    := assembler.o common.o
CPU_OBJ          := cpu.o common.o

#------------------------------------------------------------------------------
all: assembler cpu

assembler: common | $(OBJDIR) $(DESTDIR)
	@ cd source/assembler && $(MAKE)
	@ echo ======== Linking $(notdir $@) ========
	@ $(CXX) $(addprefix $(OBJDIR)/, $(ASSEMBLER_OBJ)) -o $(ASSEMBLER_TARGET) $(CXXFLAGS)

cpu: common | $(OBJDIR) $(DESTDIR)
	@ cd source/cpu && $(MAKE)
	@ echo ======== Linking $(notdir $@) ========
	@ $(CXX) $(addprefix $(OBJDIR)/, $(CPU_OBJ)) -o $(CPU_TARGET) $(CXXFLAGS)

clean:
	rm -rf $(OBJDIR)

distclean:
	rm -rf $(OBJDIR) $(DESTDIR)
#------------------------------------------------------------------------------

common: | $(OBJDIR)
	@ cd source/common && $(MAKE)
	
$(OBJDIR):
	mkdir $(OBJDIR)

$(DESTDIR):
	mkdir $(DESTDIR)

.PHONY: assembler cpu common