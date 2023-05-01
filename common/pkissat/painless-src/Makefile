SRCS = $(shell find . -name "*.cpp")

OBJS = $(addsuffix .o, $(basename $(SRCS)))

EXEC = painless

LIBS = -lkissat -L../kissat_mab/build \
       -lmapleCOMSPS -L../mapleCOMSPS/build/release/lib/ \
       -lm4ri -L../mapleCOMSPS/m4ri-20140914/.libs \
       -lpthread -lz -lm -static

CXXFLAGS = -I. \
           -I../mapleCOMSPS -I../mapleCOMSPS/m4ri-20140914 \
           -I../kissat_mab \
           -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS \
           -fopenmp -std=c++17 -O3 -D NDEBUG

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)
