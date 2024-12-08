#include "lazy_sort.h"
#include "colors.h"

// global lock
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// FileEntry* global_files;
int total_files = 0;
int global_array[MAX_SORT_RANGE] = {0};

// calculate required number of threads based on data size and partition size
int calculate_req_threads(int total_size, int partition_size){
    return (total_size + partition_size - 1) / partition_size; // celing division
}

// unique hashing ensuring lexicographical order is maintained
int hash_name(char* name) {
    // get first character's ASCII value to create base range (a-z: 97-122)
    int first_char = (name[0] >= 'a' && name[0] <= 'z') ? name[0] - 'a' : 0;
    
    // each character range gets a section of MAX_SORT_RANGE
    // divide MAX_SORT_RANGE into 26 sections for each starting letter
    int section_size = MAX_SORT_RANGE / 26;
    int base = first_char * section_size;
    
    // within each section, use remaining characters to create offset
    int offset = 0;
    for(int i = 1; i < strlen(name) && i < 3; i++) {
        offset = offset * 26 + (name[i] - 'a');
    }
    
    // scale offset to fit within section_size
    offset = (offset * section_size) / (26 * 26);
    
    return base + offset;
}
int hash_id(int id){
    return id % MAX_SORT_RANGE;
}

int hash_timestamp(char* timestamp){
    int year = (timestamp[0] - '0') * 1000 + (timestamp[1] - '0') * 100 + (timestamp[2] - '0') * 10 + (timestamp[3] - '0');
    int month = (timestamp[5] - '0') * 10 + (timestamp[6] - '0');
    int day = (timestamp[8] - '0') * 10 + (timestamp[9] - '0');
    int hour = (timestamp[11] - '0') * 10 + (timestamp[12] - '0');
    int minute = (timestamp[14] - '0') * 10 + (timestamp[15] - '0');
    int second = (timestamp[17] - '0') * 10 + (timestamp[18] - '0');
    
    // considering year would be only in range 2023-2024
    year = year - 2023;  // Will be 0 for 2023, 1 for 2024
    return ((year * 12 * 31 * 24 * 60 * 60) + (month-1) * (31 * 24 * 60 * 60) + (day-1) * (24 * 60 * 60) + hour * (60 * 60) + minute * 60 + second) % MAX_SORT_RANGE;
}

int get_hash_value(FileEntry* entry, int column) {
    switch(column) {
        case 0: // Name
            return hash_name(entry->name);
        case 1: // ID
            return hash_id(entry->id);
        case 2: // Timestamp
            return hash_timestamp(entry->time_stamp);
        default:
            return 0;
    }
}

void* count_sort_routine(void* args){
    PartitionArgs* partition_args = (PartitionArgs*)args;
    FileEntry* local_data = partition_args->partition_data;
    int size = partition_args->size;
    int column = partition_args->column;

    int b = MAX_SORT_RANGE;
    int* local_count = (int*)calloc(sizeof(int), b);

    for(int i = 0; i < size; i++){
        int val;
        if(column == 0){
            // based on Name
            val = hash_name(local_data[i].name);
        } else if(column == 1){
            // based on ID
            val = hash_id(local_data[i].id);
        } else if(column == 2){
            // based on Timestamp
            val = hash_timestamp(local_data[i].time_stamp);
        }
        // printf("start: %d\tend: %d\tval: %d\n", start, end, val);
        local_count[val]++;
    }

    // global count array update
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < b; i++){
        global_array[i] += local_count[i];
    }
    pthread_mutex_unlock(&mutex);

    free(local_count);
    return NULL;

}


void* merge_sort_routine(void* args){
    PartitionArgs* partition_args = (PartitionArgs*)args;
    FileEntry* local_data = partition_args->partition_data;
    int size = partition_args->size;
    int column = partition_args->column;
    
    merge_sort_normal(local_data, 0, size - 1, column);
    return NULL;
}

void* merge_sort_normal(FileEntry* files, int start, int end, int column){
    if(start < end){
        int mid = start + (end - start) / 2;
        // printf("start: %d\tmid: %d\tend: %d\n", start, mid, end);

        merge_sort_normal(files, start, mid, column);
        merge_sort_normal(files, mid + 1, end, column);
        merge(files, start, mid, end, column);
    }
    return NULL;
}

int comparator(FileEntry* entry1, FileEntry* entry2, int column){
    if(column == 0){
        // based on name
        return strcmp(entry1->name, entry2->name);
    } else if(column == 1){
        // based on id
        return (entry1->id - entry2->id);
    } else if(column == 2){
        // based on time_stamp
        return strcmp(entry1->time_stamp, entry2->time_stamp);
    }
}

void merge(FileEntry* files, int l, int mid, int r, int column){
    int n1 = mid - l + 1;
    int n2 = r - (mid + 1) + 1;
    
    FileEntry* entry1 = (FileEntry*)malloc(sizeof(FileEntry) * (n1));
    FileEntry* entry2 = (FileEntry*)malloc(sizeof(FileEntry) * (n2));

    // copy entries
    for(int i = 0; i < n1; i++){
        entry1[i] = files[i + l];
    }
    for(int i = 0; i < n2; i++){
        entry2[i] = files[i + mid + 1];
    }

    int i = 0, j = 0, k = l;

    while(i < n1 && j < n2){
        if(comparator(&entry1[i], &entry2[j], column) <= 0){
            files[k++] = entry1[i++];
        } else{
            files[k++] = entry2[j++];
        }
    }

    while(i < n1){
        files[k++] = entry1[i++];
    }
    while(j < n2){
        files[k++] = entry2[j++];
    }
    free(entry1);
    free(entry2);
}

int main(){
    // <input>
    int n = 0;
    scanf("%d", &n);
    if(n > MAX_FILES){
        fprintf(stderr, BRED "exceeded number of files limit. keep it below %d\n" CRESET, MAX_FILES);
        exit(EXIT_FAILURE);
    }

    FileEntry* files = (FileEntry*)malloc(sizeof(FileEntry) * n);
    for(int i = 0; i < n; i++){
        scanf("%s %d %s", files[i].name, &files[i].id, files[i].time_stamp);
    }

    char sort_column[MAX_SORT_COLUMN_LEN + 1];
    scanf("%s", sort_column); // <- criterion

    // map string to a number for easification
    int column = -1;
    if(strcmp(sort_column, "Name") == 0){
        column = 0;
    } else if(strcmp(sort_column, "ID") == 0){
        column = 1;
    } else if(strcmp(sort_column, "Timestamp") == 0){
        column = 2;
    } else{
        fprintf(stderr, BRED "invalid sort column\n" CRESET);
        exit(EXIT_FAILURE);
    }
    // printf("Using column: %s for sorting...\n", sort_column);
    if(n < THRESHOLD){
        // printf("Using count sort...\n");
    } else{
        // printf("Using merge sort...\n");
    }
    // </input>

    // calculate required number of threads and partition sizes
    int req_threads = calculate_req_threads(n, PARTITION_DATA_SIZE);
    int active_threads = (req_threads <= NUM_THREADS) ? req_threads : NUM_THREADS;
    // printf("Active threads in use: %d\n", active_threads);

    // allocate thread and partition arguments arrays
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * active_threads);
    PartitionArgs* partition_args = (PartitionArgs*)malloc(sizeof(PartitionArgs) * active_threads);

    // calculate actual partition sizes
    int base_partition_size = n / active_threads;
    int extra_elements = n % active_threads;

    // printf("Base partition size: %d\n", base_partition_size);
    // printf("extra elements: %d\n", extra_elements);

    if (!threads || !partition_args) {
        if (threads) free(threads);
        if (partition_args) free(partition_args);
        free(files);
        fprintf(stderr, "Memory allocation failed for threads or partition_args\n");
        exit(EXIT_FAILURE);
    }

    // starting position for data distribution
    int current_pos = 0;

    // create and initialize partitions
    for(int i = 0; i < active_threads; i++){
        // calculate this partition's size
        int partition_size = base_partition_size + ((i < extra_elements) ? 1 : 0);
        // printf("Thread %d: partition size: %d\n", i + 1, partition_size);
        FileEntry* partition_data = (FileEntry*)malloc(sizeof(FileEntry) * partition_size);
        if (!partition_data) {
            // Clean up previously allocated memory
            for(int j = 0; j < i; j++) {
                free(partition_args[j].partition_data);
            }
            free(partition_args);
            free(threads);
            free(files);
            fprintf(stderr, "Memory allocation failed for partition data\n");
            exit(EXIT_FAILURE);
        }
        memcpy(partition_data, &files[current_pos], sizeof(FileEntry) * partition_size);

        partition_args[i].partition_data = partition_data;
        partition_args[i].size = partition_size;
        partition_args[i].column = column;
        partition_args[i].partition_id = i;

        if(n < THRESHOLD){
            // count sort
            pthread_create(&threads[i], NULL, count_sort_routine, &partition_args[i]);

        } else{
            // merge sort
            pthread_create(&threads[i], NULL, merge_sort_routine, &partition_args[i]);
        }
        current_pos += partition_size;
    }

    // Wait for all threads to complete
    for(int i = 0; i < active_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // printf("All threads done....\n");
    // printf("Sorting sorted partitions...\n");

    if(n >= THRESHOLD){
        // merge sorted partitions
        // printf("Performing final merge operation on sorted partitions...\n");
        int current_pos = 0;
        for(int i = 0; i < active_threads; i++){
            memcpy(&files[current_pos], partition_args[i].partition_data, sizeof(FileEntry) * partition_args[i].size);
            current_pos += partition_args[i].size;
        }

        ChunkBoundary* chunks = (ChunkBoundary*)malloc(sizeof(ChunkBoundary) * active_threads);

        // initialize chunk boundaries
        current_pos = 0;
        for(int i = 0; i < active_threads; i++){
            chunks[i].start = current_pos;
            current_pos += partition_args[i].size;
            chunks[i].end = current_pos - 1;
            // printf("Thread %d\n", i + 1);
            // printf("chunk start: %d\n", chunks[i].start);
            // printf("chunk end: %d\n", chunks[i].end);
        }

        // merge chunks until only one remains
        int n_chunks = active_threads;
        while(n_chunks > 1){
            int new_size = 0; // track the new number of chunks after merging

            // handle pairs of chunks
            for(int i = 0; i < n_chunks - 1; i += 2){
                int start = chunks[i].start;
                int mid = chunks[i].end;
                int right = chunks[i + 1].end;

                // perform the merge
                merge(files, start, mid, right, column);

                // store the merged chunk's boundaries
                chunks[new_size].start = start;
                chunks[new_size].end = right;
                new_size++;
            }

            // handle the last chunk if n_chunks is odd
            if(n_chunks % 2 == 1){
                chunks[new_size].start = chunks[n_chunks - 1].start;
                chunks[new_size].end = chunks[n_chunks - 1].end;
                new_size++;
            }

            // update the number of chunks
            n_chunks = new_size;
        }
        free(chunks);

    } else{
        // printf("Decoding count sort global array for final sorted array...\n");
        for(int i = 1; i < MAX_SORT_RANGE; i++) {
            global_array[i] += global_array[i - 1];
        }
        // printf("\nComputed cumulative sum array...\n");

        FileEntry* output_entries = (FileEntry*)malloc(sizeof(FileEntry) * n);
        if (!output_entries) {
            // Clean up
            for(int i = 0; i < active_threads; i++) {
                free(partition_args[i].partition_data);
            }
            free(partition_args);
            free(threads);
            free(files);
            fprintf(stderr, "Memory allocation failed for output entries\n");
            exit(EXIT_FAILURE);
        }
        // Build sorted array using the correct hash value
        for(int i = n - 1; i >= 0; i--) {
            int hash_val = get_hash_value(&files[i], column);
            output_entries[global_array[hash_val] - 1] = files[i];
            global_array[hash_val]--;
        }

        memcpy(files, output_entries, sizeof(FileEntry) * n);
        free(output_entries);
    }

    for(int i = 0; i < active_threads; i++) {
        free(partition_args[i].partition_data);
    }
    free(threads);
    free(partition_args);

    // Output:
    printf("%s\n", sort_column);
    for(int i = 0; i < n; i++) {
        printf("%s %d %s\n", files[i].name, files[i].id, files[i].time_stamp);
    }

    free(files);
    return 0;
}