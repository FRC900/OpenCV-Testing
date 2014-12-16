#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "threadsafe_buffer.hpp"
#include "vision_thread.hpp"

using namespace std;

/* Basic shell of a TCP/IP server which can run on Linux (or Windows
 * with Cygwin or similar).  At this point it only accepts two
 * commands = 0x00000000 closes the current connection, 0x00000001
 * requests a read of data. That data is 3 4-byte values
 *
 * The server is single-threaded, which means it can only handle one
 * connection. That's not going to be a problem unless the client
 * disconnects unexpectedly and leaves the server hung. To fix this
 * the server is coded to time out after 10 seconds of inactivity.
 * I'd expect that our use will be polling for data way more frequently
 * than that, so this is long enough to indicate a real problem.
 */

/* Create a TCP/IP socket and wait for a connection
 * Returns the new socket file descriptor to use
 * for subsequent read and write calls.
 */
int makeConnection(int portno, int *sockfd)
{
   struct sockaddr_in serv_addr;
   int newsockfd;
   int fcntl_val;

   /* sockfd is the socket used to listen for connections
    * If that socket hasn't been created and bound to the
    * specified port number, do so here */
   if (*sockfd < 0)
   {
      /* Create a TCP/IP socket */
      *sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (*sockfd < 0)
      {
	 perror ("socket");
	 return -1;
      }
      /* Set the socket up to listen for INET (tcp/ip) connections
       * from any remote address using the specified port */
      memset(&serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(portno);
      if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
	 perror ("bind");
	 return -1;
      }
   }
   cerr << "Waiting for connection on port " << portno << endl;

   /* Listen for a connection. When one arrives, accept it.
    * Accepting creates the actual port for communication
    * for the rest of the program - save the port descriptor
    * as newsockfd.  */
   /* Note that sockfd will still be open to accept connections
    * the next time through this function, so don't reinit it each
    * time */
   if (listen(*sockfd,5) < 0)
   {
      perror ("listen");
      return -1;
   }
   newsockfd = accept(*sockfd, NULL, NULL);
   if (newsockfd < 0)
   {
      perror ("accept");
      return -1;
   }

   /* This might not be needed later.
    * Set the port up for non-blocking IO.  Non-blocking
    * means that reads from the port will always return immediately
    * even if there's no data. We'll use other tricks to see
    * if there is data available before calling read, and those tricks
    * will allow us to add a timeout. That timeout will help detect
    * cases where the client has disconnected without telling the
    * server, allowing the server to reset itself and handle those
    * types of errors */
   fcntl_val = fcntl(newsockfd, F_GETFL);
   if (fcntl(newsockfd, F_SETFL, fcntl_val | O_NONBLOCK) < 0)
   {
      perror ("fcntl for non-blocking");
      close (newsockfd);
      return -1;
   }

   return newsockfd;
}

int main(int argc, char *argv[])
{
   int sockfd = -1, newsockfd;
   unsigned char buffer[2048];
   ssize_t buf_idx;
   ssize_t rc;
   int stop_server = 0;
   int close_connection = 0;

   TSBuffer< VisionBuf > tsBuffer;
   pthread_t visionThreadId;
   bool startedGraphicsThread = false;

   if (argc < 2)
   {
      cerr << "ERROR, need to provide a port number on the command line" << endl;
      exit(1);
   }

   // Start graphics thread right away - let GPU code initialize ASAP
   if (!startedGraphicsThread)
   {
      pthread_create(&visionThreadId, NULL, visionThread, &tsBuffer);
      startedGraphicsThread = true;
   }

   /* TODO = add a command to shut down the server compeletly */
   /* Maybe add another to restart the server from scratch */
   while (!stop_server)
   {
      newsockfd = makeConnection(atoi(argv[1]), &sockfd);
      if (newsockfd < 0)
      {
	 /* This should never happen (famous last words) */
	 perror("makeConnection failed");
	 return -1;
      }
      cerr << "Connected" << endl;
      /* Loop reading and responding to commands from the client */
      for (close_connection = 0; !close_connection; )
      {
	 /* Loop to read 4 bytes. A read(*) should never break up such
	  * a small read, but better safe than sorry */
	 for (buf_idx = 0; !close_connection && buf_idx < 4; )
	 {
	    /* Ugly code to wait for data on newsockfd for
	     * timeout seconds.  
	     * The FD_ZERO/FD_SET is making a list of sockets 
	     * to wait for. Here we're only waiting for the one, 
	     * but select() can be programmed to wait for data
	     * on multiple sockets - the code is a bit overly-
	     * complex for simple cases like this */
	    fd_set input;
	    FD_ZERO(&input);
	    FD_SET (newsockfd, &input);
	    /* Set the timeout for 10 seconds */
	    struct timeval timeout;
	    timeout.tv_sec  = 1000;
	    timeout.tv_usec = 0;
	    rc = select(newsockfd + 1, &input, NULL, NULL, &timeout);

	    if (rc <= 0)
	    {
	       /* 0 == timeout, -1 is an error. In either case, 
		* assume timeout or remote end closed connection
		*/
	       if (rc < 0)
		  perror("select\n");
	       close_connection = 1;
	    }
	    else 
	    {
	       rc = read(newsockfd, &buffer[buf_idx], sizeof(buffer) - buf_idx - 1);
	       if (rc <= 0)
	       {
		  /* Same as above - assume errors mean timeout or
		   * remote end closed connection unexpectedly */
		  if (rc < 0)
		     perror("read");
		  close_connection = 1;
	       }
	       else
	       {
		  /* if rc > 0 , rc is the number of bytes read.
		   * Hopefully this will be 4 */
		  buf_idx += rc;
	       }
	    }
	 }
	 /* If something happend and the server timed out
	  * or saw another error, process that immediately
	  * since there's nothing valid in buffer[] */
	 if (close_connection)
	 {
	    cerr << "Closing connection due to error or timeout waiting for data" << endl;
	 }
	 else
	 {
	    // cerr << "Received " << hex << (unsigned int)buffer[0] << " " << (unsigned int)buffer[1] << " " << (unsigned int)buffer[2] << " " << (unsigned int)buffer[3] << dec << endl;
	    if (buffer[3] == 0)
	    {
	       /* Orderly disconnect. Close the connection then loop back and
		* start waiting for the next one */
	       close_connection = 1;
	       cerr << "Closing connection due to request from client" << endl;
	    }
	    else if (buffer[3] == 1)
	    {
	       /* Fill in dummy data.  This will be replaced by
		* actual processed image (or whatever) data from the
		* Jetson board.
		* Initially, probably do an image capture/process
		* each time the client requests it.
		* For better performance, kick off a thread when
		* the client connects that is constantly reading/processing
		* When the client requests data return the most recent
		* completely processed data from that separate thread.
		* This will be similar to a typical multi-threaded
		* consumer/producer problem, but where the consumer/
		* producer pattern has the producer wait if the consumer isn't
		* ready here we want to just overwrite old data and
		* constantly replace it with newer values.
		*/
	       VisionBuf buf;
	       tsBuffer.Read(buf);
	       for (size_t j = 0; j < buf.size(); j++)
		  cout << (unsigned)buf[j] << " ";
	       cout << endl;
	       rc = write(newsockfd, &buf[0], buf.size());
	       if (rc < 0)
	       {
		  /* Assume timeout or remote end closed connection */
		  perror("ERROR writing to socket");
		  close_connection = 1;
	       }
	    }
	 }
      }
      /* Close the connection. Note this only closes the one
       * created by accept().  The original socket is still
       * listening for new connections to pass off to listen/accept
       * next time around */
      close(newsockfd);
   }
   close(sockfd);
   return 0;
}



