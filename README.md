# simpleServer
This is a basic HTTP server for Linux that handles GET requests. It use IP version 4 and handles these requests concurrently with a fixed number of threads in a pool. It was made to hint at what server design entails and is not meant for industrial use.

Set up this server with the following steps: create a public directory named "public" and fill it with html, css and javascript files you wish to access; create a server executable with the "make" utility in the same directory where the public one from the last step resides; run the server in a terminal and access files of your choosing via a web browser at 127.0.0.1.
