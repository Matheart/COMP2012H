OUT := gitlite
SRCS := Commit.cpp gitlite.cpp Repository.cpp Tester.cpp Utils.cpp
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

CXX := g++-10
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude

ifeq (Windows_NT, $(OS))
RM := del
else
RM := rm -f
endif

.PHONY: all
all: $(OUT)


$(OUT): $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(OUT) $(OBJS)
