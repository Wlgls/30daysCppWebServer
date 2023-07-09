TCP=$(wildcard TCP/*.cpp)
HTTP=$(wildcard HTTP/*.cpp)
echo_server:
	g++ -std=c++14 -pthread -g \
	$(TCP) $(HTTP) \
	echo_server.cpp \
	-o echo_server

http_server:
	g++ -std=c++14 -pthread -g \
	$(TCP) $(HTTP) \
	http_server.cpp \
	-o http_server

context_test:
	g++ -std=c++14 \
	HTTP/HttpRequest.cpp HTTP/HttpContext.cpp test_httpcontext.cpp -o test_httpcontext


clean:
	rm echo_server && rm http_server && rm context_test 
