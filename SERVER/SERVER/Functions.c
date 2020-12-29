#include "Functions.h"
#include <assert.h>
#include < stdlib.h >



// ******** valiation of data and parms ************ 

	void CheakArgs(int argc,int excpted_num_of_args)
{
	if (argc < excpted_num_of_args)
	{
		printf("ERROR: Not enough input arguments");
		exit(ERR_CODE_NOT_ENOUGH_ARGUMENTS);
	}
	if (argc > excpted_num_of_args)
	{
		printf("ERROR: Too many input arguments");
		exit(ERR_CODE_TOO_MANY_ARGUMENTS);
	}
}

	int valid_PTR(void* ptr )
	{
		if (!ptr)
		{
			printf(" invalid input pointer parm");
			return INVALID_INPUT_PARM_EMPTY_POINTER;
		}
		return SUCCESS;
	}

	int FreeHandelsArray(HANDLE* handels, int len)
	{
		int ret_val = 0;
		ret_val=valid_PTR(handels);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}
for (int i = 0; i < len; i++)
{
	if (handels[i] != 0)
		CloseHandleWrap(handels[i]);
}
if (handels != 0)
free(handels);
return SUCCESS;
	}

	int WaitForSingleObjectWrap(HANDLE handle, uli time_ms)
	{
		int wait_code = WaitForSingleObject(handle, time_ms);
		if (wait_code != WAIT_OBJECT_0)
		{
			printf("problem with WaitForSingleObject ,error code is %d \n\n", GetLastError());
			return TIME_OUT_THREAD;
		}
		return SUCCESS;
	}

	int WaitForMultipleObjectsWrap(uli num_of_threads, HANDLE* handle_arr, uli time_ms, BOOL bWaitAll)
	{

		int wait_code = WaitForMultipleObjects(num_of_threads, handle_arr, TRUE, INFINITE);
		if (WAIT_OBJECT_0 != wait_code)
		{
			printf("problem with WaitForMultipleObject ,error code is %d \n\n", GetLastError());
			return TIME_OUT_THREAD;
		}
		return SUCCESS;
	}

	int  CheckAlocation(void* p_arr)
	{
		if (p_arr == NULL) {
			printf_s("MEMORY_ALLOCATION_FAILURE.\n");
			return MEMORY_ALLOCATION_FAILURE;
		}
		return SUCCESS;
	}
	int CheakHandle(HANDLE my_handle)
	{
		if (my_handle == INVALID_HANDLE_VALUE)
		{
			printf_s("INVALID_HANDLE. error code %d", GetLastError());
			return  GetLastError();
		}
		return SUCCESS;
	}

	int CheakIsAnumber(char* str)
	{
		int ret_val = 0;
		ret_val = valid_PTR(str);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}
		/*This function cheak if string is a number - used for validate the value of key in this code */
		for (int i = 0; str[i] != '\0'; i++)
		{
			if (!isdigit(str[i]))
			{
				return FALSE;
			}
		}
		return SUCCESS;
	}
	//****************File methods**************

	int ReadLine(HANDLE input_file, char** OUT line)
	{
		BOOL eof_found = FALSE;
		uli expand_factor = 1;
		uli curser_index = 0;
		int ret_val = 0;
		int start_pos;
		ret_val = SetFilePointerWrap(input_file, 0, FILE_CURRENT, &start_pos);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}
		char* line_temp = calloc(NUM_OF_BYTES_TO_READ+1, sizeof(char));
		ret_val = CheckAlocation(line_temp);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}

		while (!eof_found)
		{
			DWORD num_of_bytes_read;
			ret_val = ReadFileWrap(NUM_OF_BYTES_TO_READ, input_file, &line_temp[curser_index], &num_of_bytes_read);
			if (ret_val != SUCCESS)
				return ret_val;
			if (num_of_bytes_read < NUM_OF_BYTES_TO_READ)
				eof_found = TRUE;
			// if not found in the for we need to read more "NUM_OF_BYTES_TO_READ" bytes
			for (; line_temp[curser_index] != 0 && line_temp[curser_index] != '\n'; curser_index++);
			if (line_temp[curser_index] == '\n')
			{
				ret_val = SetFilePointerWrap(input_file, start_pos+curser_index+1, FILE_BEGIN,NULL);
				if (ret_val != SUCCESS)
				{
					return ret_val;
				}

				line_temp[curser_index - 1] = '\0';
				*line = (char*)calloc(curser_index , sizeof(char));
				ret_val = CheckAlocation(line_temp);
				if (ret_val != SUCCESS)
				{
					return ret_val;
				}
				memcpy(*line, line_temp, curser_index);
				free(line_temp);
				return SUCCESS;
			}
			expand_factor++;
			char* line_temp_alloc=(char*)realloc(line_temp, expand_factor * NUM_OF_BYTES_TO_READ+1);
			ret_val = CheckAlocation(line_temp_alloc);
			if (ret_val != SUCCESS)
			{
				free(line_temp);
				return ret_val;
			}

			else {
				line_temp = line_temp_alloc;
				memset(line_temp + curser_index, 0, NUM_OF_BYTES_TO_READ + 1);
			}
		}
			return NO_NEW_LINE;
	}

	int OpenFileWrap( LPCSTR str, DWORD mode,HANDLE * OUT hFile)
	{
		//CreateFileA wrap 
		int ret_val = 0;
		*hFile = CreateFileA(str, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, mode, FILE_ATTRIBUTE_NORMAL, NULL);
		return CheakHandle(*hFile);
	}
	
	int CloseHandleWrap(HANDLE file)
	{
		//CloseHandle wrap 
		BOOL  file_status = CloseHandle(file);
		if (!file_status)
		{
			printf("Failed to close file.error code %d", GetLastError());
			return FAILAD_TO_CLOSE_FILE;
			//not exit couse try best effort to close more files. 
		}
		return SUCCESS;
	}
	int FreeArray(void** arr, int len)
	{
		int ret_val = 0;
		ret_val = valid_PTR(arr);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}
		for (int i = 0; i < len; i++)
		{
			if (arr[i] != NULL)
				free(arr[i]);
		}
		free(arr);
		return SUCCESS;
	}
	int CreateSemphoreWrap(int max_count, HANDLE* OUT semphore,int initialcount)
	{
		*semphore = CreateSemaphoreA(
			NULL,	/* Default security attributes */
			initialcount,		/* Initial Count - all slots are empty */
			max_count,		/* Maximum Count */
			NULL); 

		if (*semphore == NULL)
		{
			printf("problem with create semphore,error code %d", GetLastError());
			return PROBLEM_CREATE_SEMPHORE;
		}
		return SUCCESS;
	}

	int CreateMutexWrap(BOOL bInitialOwner, HANDLE* OUT mutex )
	{

		*mutex = CreateMutexA(
			NULL,	/* Default security attributes */
			bInitialOwner,		/* Set if the creator of the mutex is lock it after it create it.  */
			NULL);

		if (*mutex == NULL)
		{
			printf("problem with create mutex,error code %d", GetLastError());
			return PROBLEM_CREATE_MUTEX;
		}
		return SUCCESS;
	}

	int OpenSemphoreWrap(HANDLE* OUT semphore, const char* name)
	{
		*semphore = OpenSemaphoreA(
			SYNCHRONIZE,
			FALSE,		
			name);

		if (*semphore == NULL)
		{
			printf("problem with OPEN semphore,error code %d", GetLastError());
			return PROBLEM_OPEN_SEMPHORE;
		}
		return SUCCESS;
	}

	int ReleaseSemphoreWrap(HANDLE semphore, int lReleaseCount)
	{
		int wait_code = ReleaseSemaphore(semphore, lReleaseCount, NULL);
		if (wait_code == 0)
		{
			printf("problem with realease semphore ,error code%d", GetLastError());
			return ERROR_RELEASE_SEMPHORE;
		}
		return SUCCESS;
	}

	int ReleaseMutexeWrap(HANDLE mutex)
	{
		int wait_code = ReleaseMutex(mutex);
		if (wait_code == 0)
		{
			printf("problem with realease Mutex,error code  %d", GetLastError());
			return ERROR_RELEASE_MUTEX;
		}
		return SUCCESS;
	}
	
	int  ReadFileWrap(DWORD len, HANDLE file,char* my_file_buff, DWORD* NumberOfBytesRead )
	{
		//WRAP TO ReadFile 
		if (ReadFile(file, (LPVOID)my_file_buff, len, NumberOfBytesRead, NULL) == 0)
		{
			printf("error read file . error code %d", GetLastError());
			return ERROR_READ_FILE;
		}
		return SUCCESS;
	}

	//handle ,len  
	int SetEndOfFileWarp(HANDLE  input_file, uli offset_len,int mode )
	{
		int ret_val = 0;
		uli position_in_file = 0;
		// set EOF at the end of the input file 
		ret_val = SetFilePointer(input_file, offset_len,&position_in_file, mode);
		if (ret_val == INVALID_SET_FILE_POINTER)
		{
			printf("problem with set file-pointer %d \n", GetLastError());
			return ret_val;
		}
		ret_val = SetEndOfFile(input_file);
		if (ret_val==0)
		{
			printf("error with set eof ,error code %d", GetLastError());
			return ret_val;
		}
		return SUCCESS;
	}

	int WriteFileWrap(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
	{
		DWORD lpNumberOfBytesRead = 0;
		if (WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &lpNumberOfBytesRead, NULL) == 0)
		{
			printf("error write file . error code %d", GetLastError());
			return FAILED_WRITE_TO_FILE;
		}
		return SUCCESS;
	}


	//-------------------strings 
	int  convert_int_to_str(int num, char** OUT str)
	{
		int ret_val = 0;
		int counter = find_len_number(num);
		char* my_str = (char*)calloc(counter + 1, sizeof(char));
		ret_val = CheckAlocation(my_str);
		if (ret_val != SUCCESS)
		{
			return ret_val;
		}
		my_str[counter] = '\0';
		for (int i = 1;i <= counter; i++)
		{
			my_str[counter - i] = num % 10 + '0';
			num /= 10;
		}
		*str = my_str;
		return SUCCESS;
	}
	int find_len_number(int num)
	{
		int counter = 0;
		while (num != 0)
		{
			num /= 10;
			counter++;
		}
		return counter;
	}



	//int find_dest_path(const char* source_path,OUT char ** dest_out,int opreation  )
	//{
		/* find the dest path of where to save the decrypted.txt  ootput file rather source path is absulte or realative
put the result in dest ptr TO-do  free dest outside */
		//char* dest;
		//int ret_val = 0;
		//ret_val = 0;
		//char* p_abs_path = strrchr(source_path, '\\');
		//char* p_explicit_file;
		//if (opreation == ENCRYPT)
		//{
		//	p_explicit_file = "encrypted.txt";
		//}
		//else
		//{
		//	p_explicit_file = "decrypted.txt";
		//}
		//size_t explicit_file_len = strlen(p_explicit_file);
		//size_t abs_path_len = p_abs_path == NULL ? 0 : p_abs_path - source_path;
		//dest = calloc( explicit_file_len + abs_path_len + ADDITION_LEN_TO_PATH, sizeof(char));
		//ret_val=CheckAlocation((void*)dest);
		//if (ret_val != SUCCESS)
		//	return ret_val;
		//if (p_abs_path)
		//{
		//	memcpy(dest, source_path, abs_path_len);
		//	dest[abs_path_len] = '\\';
		//}
		//strcat_s(dest, explicit_file_len + abs_path_len + ADDITION_LEN_TO_PATH, p_explicit_file);
		//*dest_out = dest;
		//return SUCCESS;
	//}

	//int CheckOperation(char* operation,int* modeflag)
	//{	char enc[3] = "-e";
	//	char dec[3] = "-d";
	//	int ret_val = 0;
	//	ret_val = valid_PTR(operation);
	//	if (ret_val != SUCCESS)
	//	{
	//		return ret_val;
	//	}
	//	if (strchr(operation, 'e') != NULL && strchr(operation, 'd') != NULL)
	//	{
	//		printf("INVALID_NUMBER_OF_PARAMS");
	//		return INVALID_NUMBER_OF_PARAMS;
	//	}
	//	if (strcmp(operation, enc) == 0)
	//	{
	//		*modeflag = ENCRYPT;
	//
	//	}
	//	else if (strcmp(operation, dec) == 0)
	//	{
	//		*modeflag = DECRYPT;
	//	}
	//	else
	//		return NOT_VALID_OPREATION;
	//
	//	return SUCCESS;
	//}
	int SetFilePointerWrap(HANDLE input_file, uli DistanceToMove, DWORD FromWhereToMove, DWORD* OUT PositionAfterSet)
	{
		DWORD retval;
		if (PositionAfterSet != NULL)
		{
			retval = SetFilePointer(input_file, DistanceToMove, NULL, FromWhereToMove);
			*PositionAfterSet = retval;
		}
		else 
			retval = SetFilePointer(input_file, DistanceToMove, NULL, FromWhereToMove);
		if (retval == INVALID_SET_FILE_POINTER)
		{
			printf("INVALID_SET_FILE_POINTER");
			return INVALID_SET_FILE_POINTER;
		}
		return SUCCESS;
	}


	//int setup_memory_menagment(MemoryTracker* OUT MemTracker)
	//{
	//	MemTracker->max_size = START_ALLOCATION_SIZE;
	//	MemTracker->count = 0;
	//	MemTracker->array_tracker = ((MemoryTrackerElement*)calloc(START_ALLOCATION_SIZE, sizeof(MemoryTrackerElement)));
	//	return CheakAlocation(MemTracker->array_tracker);

	//}

	//int push_element_memory_mangment(MemoryTracker* MemTracker, MemoryTracker element)
	//{
	//	int ret_val = 0;
	//	ret_val = valid_PTR(MemTracker);
	//	if (ret_val != SUCCESS)
	//	{
	//		return ret_val;
	//	}
	//	if (MemTracker->count < MemTracker->max_size)
	//	{
	//		MemTracker->max_size *= 2;
	//		MemTracker->array_tracker = (MemoryTracker*)realloc(MemTracker->array_tracker, sizeof(MemoryTracker) * sizeof(MemTracker->max_size));
	//		ret_val = CheakAlocation(MemTracker->array_tracker);
	//		if (ret_val != SUCCESS)
	//		{
	//			return ret_val;
	//		}
	//	}
	//	MemTracker->array_tracker[MemTracker->count] = element;
	//	MemTracker->count++;
	//}

	//void FreeAll(MemoryTracker* MemTracker)
	//{
	//	int ret_val = 0;
	//	ret_val = valid_PTR(MemTracker);
	//	if (ret_val != SUCCESS)
	//	{
	//		return ret_val;
	//	}
	//	for (int i = 0; i < MemTracker->count; i++)
	//	{
	//		if (MemTracker->array_tracker[i].array_handle == NULL)
	//		{
	//			continue;
	//		}
	//		if (MemTracker->array_tracker[i].type == ARRAY)
	//		{
	//			if (MemTracker->array_tracker[i].count == 1)
	//				free(MemTracker->array_tracker[i].array_handle);
	//			else
	//			{
	//				for (int j = 0; j < MemTracker->array_tracker[i].count; j++)
	//					free(MemTracker->array_tracker[i].array_handle[i]);
	//			}
	//		}

	//	}
	//}