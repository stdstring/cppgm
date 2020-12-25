CFLAGS=-g -std=gnu++11 -Wall

all: pptoken

SOURCES=

# build pptoken application
pptoken:
	g++ $(CFLAGS) -o pptoken pptoken.cpp $(SOURCES)

# build & execute unit tests

TEST-LIBS-DIR=../gtest/lib

TEST-LFLAGS=-Wl,-rpath,$(TEST-LIBS-DIR)

TEST-SOURCES=

TEST-INCLUDE=-I../gtest/include

TEST-LIBS=-L$(TEST-LIBS-DIR) -lgtest -lgtest_main -lpthread

unittest:
	g++ $(CFLAGS) $(TEST-LFLAGS) $(TEST-INCLUDE) -o pptoken-unittest $(SOURCES) $(TEST-SOURCES) $(TEST-LIBS)
	./pptoken-unittest

# test pptoken application
test: all
	scripts/run_all_tests.pl pptoken my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl pptoken-ref ref

