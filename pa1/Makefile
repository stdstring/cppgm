CFLAGS=-g -std=gnu++14 -Wall

all: pptoken

SOURCES=Utf8.cpp \
        Utf8Processor.cpp \
        Utf32Processors.cpp \
        TokenParsers.cpp

# build pptoken application
pptoken:
	g++ $(CFLAGS) -o pptoken pptoken.cpp $(SOURCES)

# build & execute unit tests

TEST-LIBS-DIR=../gtest/lib

TEST-LFLAGS=-Wl,-rpath,$(TEST-LIBS-DIR)

TEST-SOURCES= Utf8DecodeUtf32Tests.cpp \
              Utf8EncodeUtf32Tests.cpp \
              Utf8OctetCountTests.cpp \
              Utf8ProcessorTests.cpp \
              Utf32ProcessorsTests.cpp \
              TokenParsersTests.cpp


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

