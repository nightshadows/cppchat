This is a simple 1-1 chat app

To build the project, run `make` in the root directory.
To run the server, use `./chat --server [--port <port>]`
To run the client, use `./chat --client <server_address> [--port <port>]`

By default, both the server and client listen on port 14000.
The message size is limited to 4096 bytes, including the header (9 bytes).
