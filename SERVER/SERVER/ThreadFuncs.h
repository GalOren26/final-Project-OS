#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "HardCodedData.h"
#include <Windows.h>
#include "Lock.h"
#include "queue.h"

typedef struct
{
	Queue* fifo;
	LPCSTR input_path;
	uli number_of_lines;
	Lock* lock;
} parssing_data;



int Createmultiplethreads(parssing_data* p_params,uli num_of_threads);

//took from the Recritation, the wrap of create threads function.
DWORD WINAPI read(LPVOID lpParam);
 int CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id, HANDLE* OUT thread_handle);
 //inputs:prime - the number that we want to get it's prime factors
//	prime_component is an int* array with size 30-
//(max prime factor is 30  couse  num <2^30)
 int FindPrimeComponets(int prime, int* OUT prime_components);
 //format thr string that print the prime factors. 
 int FormatNumberString(int* prime_components, char** OUT prime_factors_by_format, int number_of_components,int number);
