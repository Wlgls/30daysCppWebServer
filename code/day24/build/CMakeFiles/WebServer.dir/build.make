# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/asher/Desktop/30daysCppWebServer/code/day24

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/asher/Desktop/30daysCppWebServer/code/day24/build

# Include any dependencies generated for this target.
include CMakeFiles/WebServer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/WebServer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/WebServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WebServer.dir/flags.make

CMakeFiles/WebServer.dir/http_server.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/http_server.cpp.o: ../http_server.cpp
CMakeFiles/WebServer.dir/http_server.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/WebServer.dir/http_server.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/http_server.cpp.o -MF CMakeFiles/WebServer.dir/http_server.cpp.o.d -o CMakeFiles/WebServer.dir/http_server.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/http_server.cpp

CMakeFiles/WebServer.dir/http_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/http_server.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/http_server.cpp > CMakeFiles/WebServer.dir/http_server.cpp.i

CMakeFiles/WebServer.dir/http_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/http_server.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/http_server.cpp -o CMakeFiles/WebServer.dir/http_server.cpp.s

CMakeFiles/WebServer.dir/base/currentthread.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/base/currentthread.cpp.o: ../base/currentthread.cpp
CMakeFiles/WebServer.dir/base/currentthread.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/WebServer.dir/base/currentthread.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/base/currentthread.cpp.o -MF CMakeFiles/WebServer.dir/base/currentthread.cpp.o.d -o CMakeFiles/WebServer.dir/base/currentthread.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/base/currentthread.cpp

CMakeFiles/WebServer.dir/base/currentthread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/base/currentthread.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/base/currentthread.cpp > CMakeFiles/WebServer.dir/base/currentthread.cpp.i

CMakeFiles/WebServer.dir/base/currentthread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/base/currentthread.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/base/currentthread.cpp -o CMakeFiles/WebServer.dir/base/currentthread.cpp.s

CMakeFiles/WebServer.dir/timer/timer.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/timer/timer.cpp.o: ../timer/timer.cpp
CMakeFiles/WebServer.dir/timer/timer.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/WebServer.dir/timer/timer.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/timer/timer.cpp.o -MF CMakeFiles/WebServer.dir/timer/timer.cpp.o.d -o CMakeFiles/WebServer.dir/timer/timer.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timer.cpp

CMakeFiles/WebServer.dir/timer/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/timer/timer.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timer.cpp > CMakeFiles/WebServer.dir/timer/timer.cpp.i

CMakeFiles/WebServer.dir/timer/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/timer/timer.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timer.cpp -o CMakeFiles/WebServer.dir/timer/timer.cpp.s

CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o: ../timer/timerqueue.cpp
CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o -MF CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o.d -o CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timerqueue.cpp

CMakeFiles/WebServer.dir/timer/timerqueue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/timer/timerqueue.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timerqueue.cpp > CMakeFiles/WebServer.dir/timer/timerqueue.cpp.i

CMakeFiles/WebServer.dir/timer/timerqueue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/timer/timerqueue.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/timer/timerqueue.cpp -o CMakeFiles/WebServer.dir/timer/timerqueue.cpp.s

CMakeFiles/WebServer.dir/log/LogStream.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/log/LogStream.cpp.o: ../log/LogStream.cpp
CMakeFiles/WebServer.dir/log/LogStream.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/WebServer.dir/log/LogStream.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/log/LogStream.cpp.o -MF CMakeFiles/WebServer.dir/log/LogStream.cpp.o.d -o CMakeFiles/WebServer.dir/log/LogStream.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/log/LogStream.cpp

CMakeFiles/WebServer.dir/log/LogStream.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/log/LogStream.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/log/LogStream.cpp > CMakeFiles/WebServer.dir/log/LogStream.cpp.i

CMakeFiles/WebServer.dir/log/LogStream.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/log/LogStream.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/log/LogStream.cpp -o CMakeFiles/WebServer.dir/log/LogStream.cpp.s

CMakeFiles/WebServer.dir/log/Logging.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/log/Logging.cpp.o: ../log/Logging.cpp
CMakeFiles/WebServer.dir/log/Logging.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/WebServer.dir/log/Logging.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/log/Logging.cpp.o -MF CMakeFiles/WebServer.dir/log/Logging.cpp.o.d -o CMakeFiles/WebServer.dir/log/Logging.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/log/Logging.cpp

CMakeFiles/WebServer.dir/log/Logging.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/log/Logging.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/log/Logging.cpp > CMakeFiles/WebServer.dir/log/Logging.cpp.i

CMakeFiles/WebServer.dir/log/Logging.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/log/Logging.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/log/Logging.cpp -o CMakeFiles/WebServer.dir/log/Logging.cpp.s

CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o: ../tcp/Acceptor.cpp
CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o -MF CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Acceptor.cpp

CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Acceptor.cpp > CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.i

CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Acceptor.cpp -o CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.s

CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o: ../tcp/Buffer.cpp
CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o -MF CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Buffer.cpp

CMakeFiles/WebServer.dir/tcp/Buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/Buffer.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Buffer.cpp > CMakeFiles/WebServer.dir/tcp/Buffer.cpp.i

CMakeFiles/WebServer.dir/tcp/Buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/Buffer.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Buffer.cpp -o CMakeFiles/WebServer.dir/tcp/Buffer.cpp.s

CMakeFiles/WebServer.dir/tcp/Channel.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/Channel.cpp.o: ../tcp/Channel.cpp
CMakeFiles/WebServer.dir/tcp/Channel.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/WebServer.dir/tcp/Channel.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/Channel.cpp.o -MF CMakeFiles/WebServer.dir/tcp/Channel.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/Channel.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Channel.cpp

CMakeFiles/WebServer.dir/tcp/Channel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/Channel.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Channel.cpp > CMakeFiles/WebServer.dir/tcp/Channel.cpp.i

CMakeFiles/WebServer.dir/tcp/Channel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/Channel.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Channel.cpp -o CMakeFiles/WebServer.dir/tcp/Channel.cpp.s

CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o: ../tcp/Epoller.cpp
CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o -MF CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Epoller.cpp

CMakeFiles/WebServer.dir/tcp/Epoller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/Epoller.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Epoller.cpp > CMakeFiles/WebServer.dir/tcp/Epoller.cpp.i

CMakeFiles/WebServer.dir/tcp/Epoller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/Epoller.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/Epoller.cpp -o CMakeFiles/WebServer.dir/tcp/Epoller.cpp.s

CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o: ../tcp/EventLoop.cpp
CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o -MF CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/EventLoop.cpp

CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/EventLoop.cpp > CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.i

CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/EventLoop.cpp -o CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.s

CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o: ../tcp/TcpConnection.cpp
CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o -MF CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpConnection.cpp

CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpConnection.cpp > CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.i

CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpConnection.cpp -o CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.s

CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o: ../tcp/TcpServer.cpp
CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o -MF CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpServer.cpp

CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpServer.cpp > CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.i

CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/TcpServer.cpp -o CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.s

CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o: ../tcp/ThreadPool.cpp
CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o -MF CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o.d -o CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/ThreadPool.cpp

CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/ThreadPool.cpp > CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.i

CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/tcp/ThreadPool.cpp -o CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.s

CMakeFiles/WebServer.dir/http/HttpContext.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/http/HttpContext.cpp.o: ../http/HttpContext.cpp
CMakeFiles/WebServer.dir/http/HttpContext.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/WebServer.dir/http/HttpContext.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/http/HttpContext.cpp.o -MF CMakeFiles/WebServer.dir/http/HttpContext.cpp.o.d -o CMakeFiles/WebServer.dir/http/HttpContext.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpContext.cpp

CMakeFiles/WebServer.dir/http/HttpContext.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/http/HttpContext.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpContext.cpp > CMakeFiles/WebServer.dir/http/HttpContext.cpp.i

CMakeFiles/WebServer.dir/http/HttpContext.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/http/HttpContext.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpContext.cpp -o CMakeFiles/WebServer.dir/http/HttpContext.cpp.s

CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o: ../http/HttpRequest.cpp
CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o -MF CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o.d -o CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpRequest.cpp

CMakeFiles/WebServer.dir/http/HttpRequest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/http/HttpRequest.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpRequest.cpp > CMakeFiles/WebServer.dir/http/HttpRequest.cpp.i

CMakeFiles/WebServer.dir/http/HttpRequest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/http/HttpRequest.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpRequest.cpp -o CMakeFiles/WebServer.dir/http/HttpRequest.cpp.s

CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o: ../http/HttpResponse.cpp
CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o -MF CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o.d -o CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpResponse.cpp

CMakeFiles/WebServer.dir/http/HttpResponse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/http/HttpResponse.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpResponse.cpp > CMakeFiles/WebServer.dir/http/HttpResponse.cpp.i

CMakeFiles/WebServer.dir/http/HttpResponse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/http/HttpResponse.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpResponse.cpp -o CMakeFiles/WebServer.dir/http/HttpResponse.cpp.s

CMakeFiles/WebServer.dir/http/HttpServer.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/http/HttpServer.cpp.o: ../http/HttpServer.cpp
CMakeFiles/WebServer.dir/http/HttpServer.cpp.o: CMakeFiles/WebServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/WebServer.dir/http/HttpServer.cpp.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/WebServer.dir/http/HttpServer.cpp.o -MF CMakeFiles/WebServer.dir/http/HttpServer.cpp.o.d -o CMakeFiles/WebServer.dir/http/HttpServer.cpp.o -c /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpServer.cpp

CMakeFiles/WebServer.dir/http/HttpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/http/HttpServer.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpServer.cpp > CMakeFiles/WebServer.dir/http/HttpServer.cpp.i

CMakeFiles/WebServer.dir/http/HttpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/http/HttpServer.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/asher/Desktop/30daysCppWebServer/code/day24/http/HttpServer.cpp -o CMakeFiles/WebServer.dir/http/HttpServer.cpp.s

# Object files for target WebServer
WebServer_OBJECTS = \
"CMakeFiles/WebServer.dir/http_server.cpp.o" \
"CMakeFiles/WebServer.dir/base/currentthread.cpp.o" \
"CMakeFiles/WebServer.dir/timer/timer.cpp.o" \
"CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o" \
"CMakeFiles/WebServer.dir/log/LogStream.cpp.o" \
"CMakeFiles/WebServer.dir/log/Logging.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/Channel.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o" \
"CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o" \
"CMakeFiles/WebServer.dir/http/HttpContext.cpp.o" \
"CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o" \
"CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o" \
"CMakeFiles/WebServer.dir/http/HttpServer.cpp.o"

# External object files for target WebServer
WebServer_EXTERNAL_OBJECTS =

test/WebServer: CMakeFiles/WebServer.dir/http_server.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/base/currentthread.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/timer/timer.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/timer/timerqueue.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/log/LogStream.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/log/Logging.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/Acceptor.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/Buffer.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/Channel.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/Epoller.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/EventLoop.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/TcpConnection.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/TcpServer.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/tcp/ThreadPool.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/http/HttpContext.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/http/HttpRequest.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/http/HttpResponse.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/http/HttpServer.cpp.o
test/WebServer: CMakeFiles/WebServer.dir/build.make
test/WebServer: CMakeFiles/WebServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Linking CXX executable test/WebServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WebServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WebServer.dir/build: test/WebServer
.PHONY : CMakeFiles/WebServer.dir/build

CMakeFiles/WebServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WebServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WebServer.dir/clean

CMakeFiles/WebServer.dir/depend:
	cd /home/asher/Desktop/30daysCppWebServer/code/day24/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/asher/Desktop/30daysCppWebServer/code/day24 /home/asher/Desktop/30daysCppWebServer/code/day24 /home/asher/Desktop/30daysCppWebServer/code/day24/build /home/asher/Desktop/30daysCppWebServer/code/day24/build /home/asher/Desktop/30daysCppWebServer/code/day24/build/CMakeFiles/WebServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WebServer.dir/depend
