GPP      = g++ -std=gnu++17 -g -O0 -Wall -Wextra
MKDEP    = g++ -std=gnu++17 -MM
MKFILE   = Makefile
DEPFILE  = Makefile.dep
SOURCES  = auxlib.cpp string_set.cpp main.cpp
HEADERS  = string_set.h auxlib.h
OBJECTS  = ${SOURCES:.cpp=.o}
EXECBIN  = oc
SRCFILES = ${HEADERS} ${SOURCES} ${MKFILE}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GPP} ${OBJECTS} -o ${EXECBIN}

%.o : %.cpp
	${GPP} -c $<

ci :
	cid + ${SRCFILES}

clean :
	-rm ${OBJECTS} ${DEPFILE}
	-rm core *.str *~

spotless : clean
	- rm ${EXECBIN} Listing.ps Listing.pdf test.out

${DEPFILE} :
	${MKDEP} ${SOURCES} >${DEPFILE}

dep :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

include ${DEPFILE}

test : ${EXECBIN}
	${EXECBIN} * * * >test.out 2>&1

lis : test
	mkpspdf Listing.ps ${SRCFILES} ${DEPFILE} test.out

again : ${SRCFILES}
	make --no-print-directory spotless dep ci test lis
