#include <iostream>
#include <pthread.h>
#include <unistd.h>

// Thread-safe lossy buffer class.  This is a class which buffers data 
// between producer and consumer threads. The buffer always holds the 
// latest data generated by the producer. There is no requirement that
// the consumer get every item generated. Instead, it just wants the 
// most recent data for each Read() call.
template <typename T>
class TSBuffer 
{
   // Mutex (mutual-exclusion) is a variable used to control access to
   // shared data. A lock() call on a mutex will only allow one caller through
   // at once. Any subsequent call to lock() by another thread before the
   // first code in calls unlock() will wait.  Once unlock() is called, another
   // thread will be allowed through. This guarantees that only one 
   // thread is operating on the shared data at a given time.
   pthread_mutex_t mutex;
   
   // Condition code - used to wait on / communicate the state of a given 
   // condition between various threads. In this case, it is used to wait
   // in the case where a read call takes control of a mutex but there
   // is no valid data to read. In that situation, the code waits() on
   // the condition to change.  This unlocks the mutex and allows other
   // code to pass into the critical section.  Once that other thread
   // fills in the buffer, it signals that valid data exists and the
   // read() thread retakes the mutex.
   pthread_cond_t  cond_valid;

   T               buffer; // the buffer to share between threads
   bool            valid;  // is this data valid - buffer has data not yet seen by reader

   public:
      TSBuffer(void)
      {
         pthread_mutex_init(&mutex, NULL);      // Initialize mutex
         pthread_cond_init(&cond_valid, NULL);  // Initialize valid data condition var
         valid = false;                         // initial state is no valid data
      }
      ~TSBuffer()
      {
         pthread_mutex_destroy(&mutex);       // Free up mutex
         pthread_cond_destroy(&cond_valid);   // Free up data-valid condition variable
      }

      // Update buffer with most recently read data
      void Update(const T &data) 
      {
         pthread_mutex_lock(&mutex);

         // If the lock() call has returned, we're guaranteed to
         // be the only thread inside the sections of code protected
         // by the lock (the code below plus the section reading data and valid
         // in the Read() code below).
         // Don't worry about the state of data in buffer - since we
         // want the newest data no matter what, overwriting old but unread / 
         // valid data is fine.  If we wanted to make sure the reader
         // got every data entry, we'd need to add a cond_not_valid
         // pthread_cond_t and loop while data is valid on that cond code,
         // and also add a matching pthread_cond_signal() in the Read
         // code
         // Update the buffer and set valid to true since data is new to reader
         // Since we have exclusive accesses to these two variables, this thread
         // can be sure that the update is complete, and e.g. Read won't set valid
         // to false just after we set it to true here and accidentally mark new data
         // as invalid.  You also won't e.g. have Read going off the end of the
         // buffer if that code read the length just before this code shrunk it.
         buffer = data;
         valid  = true;

         pthread_cond_signal(&cond_valid);  // wake up reader if it is waiting for valid data
         pthread_mutex_unlock(&mutex);      // release the buffer, allow another thread into
      }                                     // the critical sections of code

      void Read(T &data) 
      {
         pthread_mutex_lock(&mutex);      
         // This thread is now the only code in a section protected by
         // lock(). That's great except for the case where the buffer
         // isn't valid. If the buffer isn't valid, we need to yield to
         // another thread which will add valid data.
         // Just looping until valid changes isn't enough - since the lock()
         // call above succeeded no other code will be allowed in to 
         // modify buffer in Update().  That's called a "deadlock" -
         // a mutex which is needed to update data has been locked by a reader
         // which itself is waiting for the writer to update the data.
         // Use the call to cond_wait to give up the lock until cond_valid
         // is set. That will allow other code to get into the critical
         // section of Update().
         // Once Update() has signaled the condition has changed and
         // released the mutex, this thread can continue on to read the new data.
         while (!valid)                    
            pthread_cond_wait(&cond_valid, &mutex);

         // At this point there is only one thread accessing 
         // data and valid AND we're sure that data is valid
         // That makes it safe to read the buffer without worrying
         // about interference from Update (e.g. having only 
         // half of the buffer set).
         data  = buffer;                        // copy data from buffer 
         valid = false;                         // mark data as invalid - prevent repeat reads of same data
         pthread_mutex_unlock(&mutex);          // release the lock() so other code can acquire it
         // Nothing to signal here since the 
      }
};

void *producer(void *data)
{
   TSBuffer <int> *tsBuffer = (TSBuffer <int> *)data;
   for (int i = 0; i < 1000; i ++)
   {
      tsBuffer->Update(i);
      usleep(100);
   }
}

void *consumer(void *data)
{
   TSBuffer <int> *tsBuffer = (TSBuffer <int> *)data;
   int i;
   while (1)
   {
      tsBuffer->Read(i);
      std::cout << i << std::endl;
      usleep(500);
   }
}


int main(int argc, char **argv) {
  pthread_t pro, con;
  TSBuffer <int> tsBuffer;
  int rc;

  // Producer and consumer both need to use the same buffer -
  // pass a pointer to it into both threads
  rc = pthread_create(&pro, NULL, producer, &tsBuffer);
  if (rc != 0)
  {
     perror("pthread_create for producer");
     return -1;
  }
  pthread_create(&con, NULL, consumer, &tsBuffer);
  if (rc != 0)
  {
     perror("pthread_create for consumer");
     return -1;
  }

  // Wait for the threads to finish
  // Otherwise main might run to the end
  // and kill the entire process when it exits.
  //pthread_join(con, NULL);
  rc = pthread_join(pro, NULL);
  if (rc != 0)
  {
     perror("pthread_join for producer");
     return -1;
  }
}