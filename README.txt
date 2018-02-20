
	FLOW

	1. Establish a TCP connection	(address, port)

		- server: listens for connections;
		- client: performs the connection initiation.


	2. Perform the HTTP transaction

		> GET /file.txt HTTP/1.1
		> Host: localhost:8000
		> User-Agent: curl/7.56.1
		> Accept: */*
		>

		< HTTP/1.0 200 OK
		< Server: SimpleHTTP/0.6 Python/2.7.10
		< Date: Tue, 20 Feb 2018 21:44:42 GMT
		< Content-type: text/plain
		< Content-Length: 8
		< Last-Modified: Tue, 20 Feb 2018 21:44:23 GMT
		<

		- parse the request;
		- check if the file exists or not;
			- if so, return it (sendfile).


