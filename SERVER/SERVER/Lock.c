#include "Lock.h"

int InitializeLock(uli num_of_threads,Lock** OUT my_lock)
{
	int ret_val = 0;
	 (*my_lock) = calloc(1, sizeof(Lock));
	 ret_val = CheckAlocation((*my_lock));
	 if (ret_val != SUCCESS)
		 return ret_val;
	 (*my_lock)->readers = 0;

	 ret_val = CreateMutexWrap(FALSE, &(*my_lock)->readers_mutex);
	 if (ret_val != SUCCESS)
		 return ret_val;
	 ret_val = CreateSemphoreWrap(num_of_threads, &(*my_lock)->asset_in_use, 1);
	 if (ret_val != SUCCESS)
		 return ret_val;
	 ret_val = CreateMutexWrap(FALSE, &(*my_lock)->turnstile);
	 if (ret_val != SUCCESS)
		 return ret_val;
	return SUCCESS;
}

void read_lock(Lock* lock)
{
	lock->ErrorValue = WaitForSingleObjectWrap(lock->turnstile, TIMEOUT_IN_MILLISECONDS);
	if (lock->ErrorValue != SUCCESS)
		return;

	lock->ErrorValue = ReleaseMutexeWrap(lock->turnstile);
	if (lock->ErrorValue != SUCCESS)
		return;
	lock->ErrorValue = WaitForSingleObjectWrap(lock->readers_mutex, TIMEOUT_IN_MILLISECONDS);
	if (lock->ErrorValue != SUCCESS)
		return;
	lock->readers += 1; 
	if (lock->readers == 1)
	{
		lock->ErrorValue = WaitForSingleObjectWrap(lock->asset_in_use, TIMEOUT_IN_MILLISECONDS);
		if (lock->ErrorValue != SUCCESS)
			return;
	}
	lock->ErrorValue = ReleaseMutexeWrap(lock->readers_mutex);
	if (lock->ErrorValue != SUCCESS)
		return;
}

void release_read(Lock* lock)
{
	lock->ErrorValue = WaitForSingleObjectWrap(lock->readers_mutex, TIMEOUT_IN_MILLISECONDS);
	if (lock->ErrorValue != SUCCESS)
		return;
	lock->readers -= 1; 
	if (lock->readers == 0)
	{
		lock->ErrorValue = ReleaseSemphoreWrap(lock->asset_in_use, 1);
		if (lock->ErrorValue != SUCCESS)
			return;
	}
	lock->ErrorValue = ReleaseMutexeWrap(lock->readers_mutex);
	if (lock->ErrorValue != SUCCESS)
		return;

}

void lock_write(Lock* lock)
{
	// only the thread that lock this mutex for write can realse it .
	lock->ErrorValue = WaitForSingleObjectWrap(lock->turnstile, TIMEOUT_IN_MILLISECONDS);
	if (lock->ErrorValue != SUCCESS)
		return;
	lock->ErrorValue = WaitForSingleObjectWrap(lock->asset_in_use, TIMEOUT_IN_MILLISECONDS);
	if (lock->ErrorValue != SUCCESS)
		return;

}
void release_write(Lock* lock)
{
	lock->ErrorValue = ReleaseMutexeWrap(lock->turnstile);
	if (lock->ErrorValue != SUCCESS)
		return;
	lock->ErrorValue = ReleaseSemphoreWrap(lock->asset_in_use, 1);
	if (lock->ErrorValue != SUCCESS)
		return;
}
int DestroyLock(Lock** lock)
{
	int ret_val = 0; 
	ret_val= CloseHandleWrap((*lock)->asset_in_use);
	if (ret_val != SUCCESS)
	{
		return ret_val;
	}
	ret_val = CloseHandleWrap((*lock)->readers_mutex);
	if (ret_val != SUCCESS)
	{
		return ret_val;
	}
	ret_val = CloseHandleWrap((*lock)->turnstile);
	if (ret_val != SUCCESS)
	{
		return ret_val;
	}
	free(*lock); 
	*lock = NULL;
	return SUCCESS;
}

