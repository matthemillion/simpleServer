# simpleServer
This is a basic HTTP server for Linux that handles GET requests. It use IP version 4 and handles these requests concurrently with a fixed number of threads in a pool. It was made to hint at what server design entails and is not meant for industrial use.

After making the server executable, run it in a terminal and access public files of your choosing via a web browser at 127.0.0.1. A public directory for html, css and javascript files must reside within the directory where you make the server executable in order to correctly serve them.
