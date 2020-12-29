#ifndef __Lock__
#define __Lock__ 
#include <Windows.h>
#include "HardCodedData.h"
#include "Functions.h"
typedef struct {
	int readers; 
	HANDLE readers_mutex; 
	HANDLE asset_in_use;
	HANDLE turnstile;
	int ErrorValue;
}Lock ;

//init lock object ant it's synch elements if failed put the error_value in the field ErrorValue.
int InitializeLock(uli num_of_threads,Lock ** OUT lock);
//Receives a pointer to the structure of a lock and performs a lock for reading.
//Capture the lock for re-reading if the lock is not occupied for writing
//If the lock is busy writing then wait until Timeout and return Failure.
void read_lock(Lock* lock);
/*Gets a pointer to the struct of a lock and  releases the lock from catch to read
must be Made by the same thread*/ 
void release_read(Lock* lock);

void lock_write(Lock* lock);
void release_write(Lock* lock);
int DestroyLock(Lock** lock);

#endif