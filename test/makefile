DIR_TEST = $(shell pwd)
DIR_RUN = $(DIR_TEST)/run

TEST_SRC = $(wildcard test_*.cpp)
TEST_DEP = $(TEST_SRC:%.cpp=%.d)
TEST_OBJ = $(TEST_SRC:%.cpp=%.o)
TEST_EXEC = DPCB.test

LDFLAG += -lgtest_main -lgtest -lpthread


.PHONY : all clean

all : $(DIR_RUN)/$(TEST_EXEC)
	$<

clean:
	-rm $(DIR_RUN)/*

$(DIR_RUN)/$(TEST_EXEC) : $(TEST_OBJ:%=$(DIR_RUN)/%)
	$(CXX) $^ $(LDFLAG) -o $@

$(DIR_RUN)/%.o : %.cpp
	@if [ ! -d $(DIR_RUN) ]; then mkdir -p $(DIR_RUN); fi;
	$(CXX) $< $(CXXFLAG) -c -o $@

$(DIR_RUN)/%.d : %.cpp
	@$(CXX) $< $(CXXFLAG) -MM -MT '$$(DIR_RUN)/$(<:%.cpp=%.o)' > $@

sinclude $(TEST_DEP:%=$(DIR_RUN)/%)