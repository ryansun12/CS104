DEPSFILE  = Makefile.deps
CPP       = g++ -g -O0 -Wall -Wextra -std=gnu++17
MKDEPS    = g++ -MM -std=gnu++17
GRIND     = valgrind --leak-check=full --show-reachable=yes
FLEX      = flex --outfile=${LEXCPP}
BISON     = bison --defines=${PARSEHDR} --output=${PARSECPP}

MODULES   = astree emitter lyutils string_set auxlib sym 
HDRSRC    = ${MODULES:=.h}
CPPSRC    = ${MODULES:=.cpp} main.cpp
FLEXSRC   = scanner.l
BISONSRC  = parser.y
PARSEHDR  = yyparse.h
LEXCPP    = yylex.cpp
PARSECPP  = yyparse.cpp
CGENS     = ${LEXCPP} ${PARSECPP}
ALLGENS   = ${PARSEHDR} ${CGENS}
EXECBIN   = oc
ALLCSRC   = ${CPPSRC} ${CGENS}
OBJECTS   = ${ALLCSRC:.cpp=.o}
LEXOUT    = yylex.output
PARSEOUT  = yyparse.output

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS} ${LEXCPP}
	${CPP} -o ${EXECBIN} ${OBJECTS}

yylex.o : yylex.cpp
	@ # Suppress warning message from flex compilation.
	${CPP} -Wno-sign-compare -c $<

%.o : %.cpp
	${CPP} -c $<

${LEXCPP} : ${FLEXSRC}
	${FLEX} ${FLEXSRC}

${PARSECPP} ${PARSEHDR} : ${BISONSRC}
	${BISON} ${BISONSRC}

ci : ${GIT}

clean :
	- rm ${OBJECTS} ${ALLGENS} ${DEPSFILE} *.output *.tok *.str \
	*.ast *.sym *.err *.out *.lexyacctrace *.oil

spotless : clean
	- rm ${EXECBIN}

deps : ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

test: ${EXECBIN}
	./oc test.oc

grind:
	${GRIND} ./${EXECBIN} test.oc

remake: spotless ${EXECBIN}


${DEPSFILE} :
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif

