
abcdefghij: abcdefghij.cpp
	g++ -std=c++14 -O3 $^ -o $@

clean:
	rm -rf abcdefghij abcdefghij.dSYM

