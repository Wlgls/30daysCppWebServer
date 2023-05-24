TCP=$(wildcard TCP/*.cpp)

server:
	g++ -std=c++14 -pthread -g \
	$(TCP) \
	echo_server.cpp \
	-o server

context_test:
	g++ -std=c++14 \
	HTTP/HttpRequest.cpp HTTP/HttpContext.cpp test_httpcontext.cpp -o test_httpcontext

th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

clean:
	rm server 
