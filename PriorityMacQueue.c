/**
 * This file represents a priority queue for MAC addresses.  Unlike a normal priority queue,
 * however, the priority of the MAC address is not specified when enqueuing the address.
 * Rather, the priority is determined by the number of times the MAC address is enqueued in
 * the list and the last time the MAC address was enqueued.  The priority is determined by
 * following formula:
 *
 *      P = n_P * n - t_P * t
 *
 * Where P is the priority of the given element, n is the number of packets received from that
 * MAC address, t is the amount of time that has passed since the last packet was received,
 * and _P (subscript P) means "the priority of".
 *
 * Given t_P > 0, the priority P will decrease as the time between transmissions from the
 * device increases.  A positive, nonzero value for t_P is appropriate when the fortified
 * server only allows access for a limited amount of time.  A value of 0 for t_P is
 * appropriate when the fortified server does not limit access based on time.  A negative
 * value for t_P is unlikely to be meaningful.  The value for t_P is given by the macro
 * NETFREE_TIMEDELTA_WEIGHT in PriorityMacQueue.h.
 *
 * Given n_P > 0, the priority P will increase with the number of times a message is received
 * while n_P < 0 will cause the priority P to decrease.  A positive value for n_P is
 * appropriate when it is unknown if the computers communicating with the fortified router
 * have access to the network (e.g. many users may try to connect to the network before
 * realizing access is not free).  This assumes that those computers sending a large number
 * of packets within the network likely have access to the Internet and are not just trying
 * to form a connection.  Of course, this approach has its down side as spoofing a heavy user
 * may cause additional delays.  A negative value for n_P is appropriate when it is known
 * that the users communicating with the fortified router have access to the network and you
 * just want to spoof the lightest user.  The value for n_P is given by the macro
 * NETFREE_REVCOUNT_WEIGHT in PriorityMacQueue.h.
 *
 * If both n_P and t_P are 0, the queue will act as a normal queue, which is unlikely to
 * result in favorable results.
 */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "PriorityMacQueue.h"
#include "mac.h"

#ifndef MAC_PRIORITY
  #define MAC_PRIORITY(timesReceived, lastReceived)  (NETFREE_REVCOUNT_WEIGHT * timesReceived) + (NETFREE_TIMEDELTA_WEIGHT * lastReceived)
#endif

typedef struct PriorityMacElementStruct {
  struct PriorityMacElementStruct *next;

  char   *macAddress;
  int     packetsReceived;
  double  lastUpdated;
  double  priority;
} PriorityMacElement;

PriorityMacElement *queueHead = NULL;
pthread_mutex_t queueMutex;

int length;

/**
 * Initializes the queue for use.
 */
void initMacQueue() {
  queueHead = (PriorityMacElement *) calloc(1, sizeof(PriorityMacElement));
  length = 0;

  pthread_mutex_init(&queueMutex, NULL);
}

/**
 * Destroys the queue and frees any memory allocated for it.
 */
void destroyMacQueue() {
  PriorityMacElement *current = queueHead;
  PriorityMacElement *previous;

  pthread_mutex_lock(&queueMutex);
  while(current->next) {
    previous = current;
    current = current->next;

    free(previous->macAddress);
    free(previous);
  }

  if(current != queueHead) {
    free(current->macAddress);
  }

  free(current);

  queueHead = NULL;
  pthread_mutex_unlock(&queueMutex);

  pthread_mutex_destroy(&queueMutex);
}

/**
 * Adds a new MAC address to the queue and assigns it an appropriate priority based on the
 * number of packets received by the given MAC address and the last time a transmission was
 * received by the device.
 *
 * @param macAddress (char *) - the 6 character MAC address for the device
 * @param timestamp (double) - the time at which the transmission was received.  If 0 or a
 *  negative number, the time will be calculated before the MAC is enqueued.
 */
void enqueueMac(char *macAddress, double timestamp) {
  PriorityMacElement *current;
  PriorityMacElement *previous;
  PriorityMacElement *oldPrevious;
  PriorityMacElement *last;
  int                 priority;
  double              timeReceived;

  timeReceived = timestamp;
  if(timestamp <= 0) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    timeReceived = (double) now.tv_sec + (1.0e-9 * now.tv_nsec);
  }

  priority = MAC_PRIORITY(1, timeReceived);     // Priority if the MAC address is new.

  pthread_mutex_lock(&queueMutex);
  previous = queueHead;
  for(current = queueHead; current->next && !macEquals(current->next->macAddress, macAddress); current = current->next) {
    // I cannot predict what the priority will be if this MAC address already exists, but I can for a new MAC address.
    // So, I'll determine what the index of a new MAC address will be here to reduce that case's complexity since for
    // this case memory must be allocated, which can be timely.
    if(current->next->priority < priority && !previous) {
      previous = current;
    }
  }

  if(!previous) {
    previous = current;
  }

  oldPrevious = current;
  current = current->next;

  if(current) {
    // Found the MAC address.
    current->packetsReceived++;
    current->lastUpdated = timeReceived;
    current->priority = MAC_PRIORITY(current->packetsReceived, timeReceived);

    // Find the appropriate location for this address and add it.
    for(previous = queueHead; previous->next && previous->next->priority >= current->priority; previous = previous->next);

    oldPrevious->next = current->next;
    current->next = previous->next;
    previous->next = current;
  } else {
    // This is a new MAC address.
    current = (PriorityMacElement *) calloc(1, sizeof(PriorityMacElement));

    current->macAddress = (char *) malloc(NETFREE_MAC_SIZE);
    strncpy(current->macAddress, macAddress, NETFREE_MAC_SIZE);

    current->packetsReceived = 1;
    current->lastUpdated = timeReceived;
    current->priority = priority;

    current->next = previous->next;
    previous->next = current;

    length++;
  }
  pthread_mutex_unlock(&queueMutex);
}

/**
 * Determines the first MAC address in the queue.  The MAC address is copied to macAddress,
 * which should have NETFREE_MAC_SIZE bytes.  If NETFREE_MAC_SIZE bytes are not allocated
 * to pointer, the behavior is undefined.  If there are no remaining MAC addresses, NULL is
 * returned and macAddress is left unmodified.  The string will not be NULL terminated.
 *
 * @param macAddress (char *) - a pointer to NETFREE_MAC_SIZE bytes of memory where the copy
 *  of the MAC address can be stored
 *
 * @return (char *) macAddress
 */
char *macQueuePeek(char *macAddress) {
  pthread_mutex_lock(&queueMutex);
  if(!queueHead->next || !queueHead->next->macAddress) {
    pthread_mutex_unlock(&queueMutex);

    return NULL;
  }

  strncpy(macAddress, queueHead->next->macAddress, NETFREE_MAC_SIZE);
  pthread_mutex_unlock(&queueMutex);

  return macAddress;
}

/**
 * Returns the length of the queue.
 *
 * @return (int) the length of the queue
 */
int macQueueLength() {
  return length;
}

/**
 * Removes the first MAC address in the queue and returns it.  The returned MAC address will
 * not be NULL terminated.
 *
 * @param macAddress (char *) - either NULL or a pointer to NETFREE_MAC_SIZE bytes of memory
 *  where a copy of the MAC address can be stored.  If macAddress is NULL, the MAC address
 *  will not be set
 *
 * @return (char *) macAddress
 */
char *dequeueMac(char *macAddress) {
  PriorityMacElement *top;

  if(macAddress) {
    macQueuePeek(macAddress);
  }

  pthread_mutex_lock(&queueMutex);
  if(queueHead->next) {
    top = queueHead->next;

    queueHead->next = top->next;

    strncpy(macAddress, top->macAddress, NETFREE_MAC_SIZE);

    free(top->macAddress);
    free(top);
  }
  pthread_mutex_unlock(&queueMutex);

  return macAddress;
}