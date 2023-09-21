# day30-WebBench的测试

关于WebBench的话，网络有许多教程，[官网](http://home.tiscali.cz/~cz210552/webbench.html)

测试时，运行当前服务器，并使用webbench命令即可。

```shell
➜  ~ webbench -c 10000 -t 5 http://127.0.0.1:1236/
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:1236/
10000 clients, running 5 sec.

Speed=3010200 pages/min, 57294136 bytes/sec.
Requests: 250850 susceed, 0 failed.
```

大概就这样，更改客户端数量和时间和连接来测试不同的情况即可。