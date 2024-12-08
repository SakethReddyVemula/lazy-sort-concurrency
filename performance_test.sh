#!/bin/bash

# Compile the program
gcc -O2 lazy_sort.c -o lazy_sort.out -pthread

# Create results directory
mkdir -p results

# Function to run test and save results
run_test() {
    dataset=$1
    echo "Running test on dataset: $dataset"
    
    # Run the program with input file and measure performance
    ./measure.sh lazy_sort.out < ${dataset}.txt > results/result_${dataset}.txt
    
    # Save detailed performance metrics
    cat lazy_sort_usage.txt >> results/result_${dataset}.txt
    echo "----------------------------------------" >> results/result_${dataset}.txt
}

# Generate test data using Python script
python3 generate_test_data.py

# Run tests with different sizes
for dataset in small medium large; do
    run_test $dataset
done

# Summarize results
echo "Performance Summary:"
echo "==================="
for dataset in small medium large; do
    echo "Dataset: $dataset"
    grep "Total Runtime" results/result_${dataset}.txt
    grep "Max Memory Usage" results/result_${dataset}.txt
    echo "-----------------"
done