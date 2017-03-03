#include <stdio.h>
#include <stdlib.h>

#include "TestSuite.h"

size_t unfreedMemory = 0;
MemoryRef *MemTrackerHead = NULL;

/**
 * Adds a MemoryRef to the list of refs currently being tracked.
 *
 * @param newRef (MemoryRef *) - a pointer to the new MemoryRef to track with the ptr and
 *  bytes fields prepopulated.
 */
void addMemoryRef(MemoryRef *newRef) {
  MemoryRef *previous = MemTrackerHead;
  MemoryRef *current = MemTrackerHead->next;
  while(current != NULL) {
    previous = current;
    current = current->next;
  }

  newRef->next = NULL;
  newRef->prev = previous;

  previous->next = newRef;
}

/**
 * Searches through the list of tracked MemoryRefs for the one that tracks the specified
 * memory location.  If the MemoryRef is found, a pointer to it is returned; otherwise,
 * NULL is returned.
 *
 * @param memoryLoc (void *) - the pointer to the memory location that should be found
 *
 * @return (MemoryRef *) a pointer to the MemoryRef that tracks the specified memory
 *  location
 */
MemoryRef *getMemoryRef(void *memoryLoc) {
  MemoryRef *previous = MemTrackerHead;
  MemoryRef *current = MemTrackerHead->next;
  while(current != NULL && current->ptr != memoryLoc) {
    previous = current;
    current = current->next;
  }

  return current;
}

/**
 * Reinitializes the memory tracking system and frees all memory locations that have not
 * already been freed.
 */
void resetMemoryTracking() {
  unfreedMemory = 0;

  MemoryRef *current = MemTrackerHead->next;
  while(current->next) {
    current = current->next;

    __real_free(current->prev->ptr);
    __real_free(current->prev);
  }
}

/**
 * Returns the total memory in bytes that has been allocated, but not yet freed.
 *
 * @return (int) the total bytes of memory that has been allocated on the heap, but not yet
 *  freed
 */
int totalUnfreedMemory() {
  return unfreedMemory;
}

/**
 * A wrapper for malloc() that allows the testing suite to track memory leaks.  The params
 * and return value mirror that of the real malloc() function.
 */
void *__wrap_malloc(size_t bytes) {
  if(!MemTrackerHead) {
    MemTrackerHead = (MemoryRef *) __real_calloc(1, sizeof(MemoryRef));
  }

  unfreedMemory += bytes;

  void *tempPtr = __real_malloc(bytes);

  MemoryRef *newRef = (MemoryRef *) __real_malloc(sizeof(MemoryRef));
  *newRef = (MemoryRef) {
    .ptr    = tempPtr,
    .bytes  = bytes
  };

  addMemoryRef(newRef);

  return tempPtr;
}

/**
 * A wrapper for calloc() that allows the testing suite to track memory leaks.  The params
 * and return value mirror that of the real calloc() function.
 */
void *__wrap_calloc(size_t nitems, size_t size) {
  if(!MemTrackerHead) {
    MemTrackerHead = (MemoryRef *) __real_calloc(1, sizeof(MemoryRef));
  }

  size_t totalBytes = nitems * size;

  unfreedMemory += totalBytes;

  void *tempPtr = __real_calloc(nitems, size);

  MemoryRef *newRef = (MemoryRef *) __real_malloc(sizeof(MemoryRef));
  *newRef = (MemoryRef) {
    .ptr    = tempPtr,
    .bytes  = totalBytes
  };

  addMemoryRef(newRef);

  return tempPtr;
}

/**
 * A wrapper for realloc() that allows the testing suite to track memory leaks.  The params
 * and return value mirror that of the real realloc() function.
 */
void *__wrap_realloc(void *oldPtr, size_t bytes) {
  MemoryRef *oldRef = getMemoryRef(oldPtr);

  if(oldRef == NULL) {
    fprintf(stderr, "Could not find requested memory location.  Calling realloc anyways.\n");

    return __real_realloc(oldPtr, bytes);
  }

  unfreedMemory = unfreedMemory - oldRef->bytes + bytes;

  void *tempPtr = __real_realloc(oldPtr, bytes);

  oldRef->ptr = tempPtr;
  oldRef->bytes = bytes;

  return tempPtr;
}

/**
 * A wrapper for free() that allows the testing suite to track memory leaks.  The params and
 * return value mirror that of the real free() function.
 */
void __wrap_free(void *ptr) {
  MemoryRef *oldRef = getMemoryRef(ptr);

  if(oldRef == NULL) {
    fprintf(stderr, "Could not find memory location to free.  Calling free anyways.\n");

    return __real_free(ptr);
  }

  unfreedMemory -= oldRef->bytes;

  oldRef->prev->next = oldRef->next;

  __real_free(oldRef);
  __real_free(ptr);
}