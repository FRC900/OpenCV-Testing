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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/* Create a TCP/IP socket and wait for a connection
 * Returns the new socket file descriptor to use
 * for subsequent read and write calls.
 */
int make_connection(int portno, int *sockfd)
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
   fprintf (stderr, "Waiting for connection on port %d\n", portno);
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

/* Simple code to fill 4 bytes in a char * buffer from an int */
void fill_buffer(unsigned char *buf, unsigned int val)
{
   buf[0] = val >> 24;
   buf[1] = val >> 16;
   buf[2] = val >>  8;
   buf[3] = val      ;
}

int main(int argc, char *argv[])
{
     int sockfd = -1, newsockfd;
     unsigned char buffer[16];
     unsigned int value = 1;
     ssize_t buf_idx;
     ssize_t rc;
     int stop_server = 0;
     int close_connection = 0;
     if (argc < 2) 
     {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     /* TODO = add a command to shut down the server compeletly */
     /* Maybe add another to restart the server from scratch */
     while (!stop_server)
     {
           newsockfd = make_connection(atoi(argv[1]), &sockfd);
           if (newsockfd < 0)
           {
              /* This should never happen (famous last words) */
              perror("make_connection failed");
              continue;
           }
           fprintf(stderr, "Connected\n");
           /* Loop reading and responding to commands from the client */
           for (close_connection = 0; !close_connection; )
           {
              /* Loop to read 4 bytes. A read should never break up such
               * a small read, but better safe than sorry */
              for (buf_idx = 0; buf_idx < 4; )
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
                 timeout.tv_sec  = 10;
                 timeout.tv_usec = 0;
                 /* select() waits for the socket fd in the input list
                  * to have data show up.  It will also return if no
                  * data is seen for timeout seconds.  The latter case
                  * most likely means the client has died so close
                  * the current connection and wait for the next one */
                 rc = select(newsockfd + 1, &input, NULL, NULL, &timeout);
                 if (rc <= 0)
                 {
                    /* 0 == timeout, -1 is an error. In either case, 
                     * assume timeout or remote end closed connection */
                    if (rc < 0)
                       perror("select");
                    close_connection = 1;
                    break;
                 }

                 rc = read(newsockfd, &buffer[buf_idx], sizeof(buffer) - buf_idx - 1);
                 if (rc <= 0)
                 {
                    /* Same as above - assume errors mean timeout or 
                     * remote end closed connection unexpectedly */
                    if (rc < 0)
                       perror("read");
                    close_connection = 1;
                    break;
                 }
                 /* if rc > 0 , rc is the number of bytes read.
                  * Hopefully this will be 4 */
                 buf_idx += rc;
              }
              /* If something happend and the server timed out
               * or saw another error, process that immediately
               * since there's nothing valid in buffer[] */
              if (close_connection)
              {
                 fprintf(stderr, "Closing connection due to error or timeout waiting for data\n");
              }
              else
              {
                 fprintf(stderr,"received %2.2x %2.2x %2.2x %2.2x\n", 
                       buffer[0], buffer[1], buffer[2], buffer[3]);
                 if (buffer[3] == 0)
                 {
                    /* Orderly disconnect. Close the connection then loop back and
                     * start waiting for the next one */
                    close_connection = 1;
                    fprintf(stderr, "Closing connection due to request from client\n");
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
                    fill_buffer(buffer  , value++);
                    fill_buffer(buffer+4, value++);
                    fill_buffer(buffer+8, value++);
                    rc = write(newsockfd, buffer, 12);
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
