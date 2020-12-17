
treesearch: treesearch.cpp
	g++ -std=c++14 \
	-I . \
	-I /usr/local/Cellar/eigen/3.3.9/include/eigen3 \
	-O3 -g $^ -o $@

basenum: basenum.cpp
	g++ -std=c++14 -O3 -g $^ -o $@
	#g++ -std=c++14 -g $^ -o $@

abcdefghij: abcdefghij.cpp
	g++ -std=c++14 -O3 $^ -o $@

clean:
	rm -rf abcdefghij abcdefghij.dSYM basenum treesearch

