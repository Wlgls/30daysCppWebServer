TCP=$(wildcard tcp/*.cpp)

server:
	g++ -std=c++14 -pthread -g \
	$(TCP) \
	echo_server.cpp \
	-o server
	
th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

clean:
	rm server 
