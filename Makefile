#
# Makefile for c-short
# Gnu gcc Version
# Zhang626 2016/04/10
#

CC 			= gcc
INCLUDE 	= include
CFLAGS 		= -I $(INCLUDE)  -DCS_DEBUG_PTR    
TARGET 		= cshort
OBJDIR 		= obj
TESTDIR 	= test
SRC			= src
FILES 		= cslex.o cshash.o csutil.o cssymbol.o cstype.o csenv.o cstemp.o  \
	csparser.o csast.o cscode.o csframe.o csopcodes.o csbytec.o csformat.o
OBJFILES 	= $(patsubst %.o,$(OBJDIR)/%.o,$(FILES))
DEPS 		= $(patsubst %.o,%.d,$(OBJFILES))

TESTFILE	= hash.o lex.o sym.o parser.o code.o
TESTOBJFILES= $(patsubst %.o,$(OBJDIR)/%.o,$(TESTFILE))
TESTDEPS 	= $(patsubst %.o,%.d,$(TESTOBJFILES))

.PHONY : all
all:$(DEPS) $(TARGET) 

#依赖文件
$(DEPS):$(OBJDIR)/%.d:$(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MT $(patsubst %.d,%.o,$@)  -MM $< > $@

$(TESTDEPS):$(OBJDIR)/%.d:$(TESTDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MT $(patsubst %.d,%.o,$@)  -MM $< > $@


-include $(DEPS)
-include $(TESTDEPS)

$(TARGET):$(OBJFILES) $(TESTOBJFILES) $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ 

$(OBJDIR)/main.o:main.c $(INCLUDE)/csglobal.h $(INCLUDE)/cslex.h $(INCLUDE)/csparser.h
	$(CC) $(CFLAGS) -c $< -o $@ 

$(OBJDIR)/%.o:$(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o:$(TESTDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean:
	rm -rf obj/* $(TARGET)
