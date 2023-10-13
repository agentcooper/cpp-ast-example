CXX=g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O3

all: ast_inheritance ast_variant_visit

ast_inheritance: ast_inheritance.cpp
	$(CXX) $(CXXFLAGS) ast_inheritance.cpp -o ast_inheritance

ast_variant_visit: ast_variant_visit.cpp
	$(CXX) $(CXXFLAGS) ast_variant_visit.cpp -o ast_variant_visit

.PHONY: clean
clean:
	rm -rf ast_inheritance ast_variant_visit