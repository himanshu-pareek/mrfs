a.out:	mrfs_41.cpp
	g++ mrfs_41.cpp -std=c++11 -lpthread

clean:	a.out
	rm a.out

