# RDMAxWiki

This repo aims to create a basic Wikipidia using RDMA technology. The main goal is to create a simple and efficient way to store and retrieve data using RDMA. 

## what is RDMA?

RDMA stands for Remote Direct Memory Access. It is a technology that allows data to be transferred directly from the memory of one computer to the memory of another computer without involving either one's operating system. This can be done with very little CPU overhead, and very low latency.
for more information about RDMA you can check this [link](https://en.wikipedia.org/wiki/Remote_direct_memory_access)

## Whats the code support?

The code is able to support multiple clients which can read and write data to the server on the same time. The server is able to handle multiple clients at the same time.

## How to run the code?

To run the code you need to have a linux machine with RDMA support. Then use the make file to compile the code. 


## Future work 

- Use a more efficient way to store the data.
  - store the data in a hash table.
- Enable not predefined amount of clients to connect to the server.
  - use UDP connection to connect to the server before using RDMA.
- Use a more efficient implmnetation data transfer using RDMA.
  - Use more low level API function in the client-server communication protcol.
- Build a simple web interface to interact with the server.


## Report

You can find the report of the project in the following [link](https://drive.google.com/file/d/1SQdhuHIPX8Ni0VYwQ5mGddxKZo-4hlvW/view?usp=sharing)
