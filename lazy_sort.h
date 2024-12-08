#ifndef __LAZY_SORT_H
#define __LAZY_SORT_H

// include libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/resource.h>
#include <stdint.h>

// define macros
#define MAX_FILE_NAME_LEN 128
#define MAX_TEXT_LEN 128
#define THRESHOLD 43 // 42
#define MAX_FILES 50000 // Assumption: Maximum number of file entries can be 1000
#define TIME_STAMP_LEN 20
#define MAX_SORT_COLUMN_LEN 20
#define NUM_THREADS 5 // ~ hyperparameter
#define MAX_SORT_RANGE 63072000 // greater and required for hashing timestamp
#define PARTITION_DATA_SIZE 1000 // ideally, each thread has these many files allocated

// file structure to store information of a file entry
/*
format:
name      id  time_stamp
fileA.txt 101 2023-10-01T14:30:00
fileB.txt 102 2023-10-01T12:15:00
fileC.txt 103 2023-09-29T09:45:00
fileD.txt 104 2023-10-02T17:05:00
fileE.txt 105 2023-09-30T10:20:00
*/
typedef struct{
    char name[MAX_FILE_NAME_LEN + 1];
    int id;
    char time_stamp[TIME_STAMP_LEN + 1];
} FileEntry;

// partition arguments
typedef struct{
    FileEntry* partition_data;
    int size;
    int column;
    int partition_id;
} PartitionArgs;

// for merging sorted partitions in MergeSort
typedef struct{
    int start;
    int end;
} ChunkBoundary;

void* count_sort_routine(void* args);
void* merge_sort_routine(void* args);
void merge(FileEntry* files, int left, int mid, int right, int column);
void* merge_sort_normal(FileEntry* files, int start, int end, int column);
int comparator(FileEntry* entry1, FileEntry* entry2, int column);
int hash_name(char* name);
int hash_timestamp(char* timestamp);

#endif