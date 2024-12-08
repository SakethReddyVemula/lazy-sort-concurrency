import random
import string
from datetime import datetime, timedelta

def generate_test_strings(num_cases=10, max_name_length=10):
    test_cases = []
    # Base datetime for generating timestamps
    base_date = datetime(2023, 1, 1)
    
    # Generate the specified number of test cases
    for i in range(num_cases):
        # Generate random filename with .txt extension
        length = random.randint(1, max_name_length - 4)  # -4 for '.txt'
        file_name = ''.join(random.choices(string.ascii_lowercase, k=length)) + '.txt'
        
        # Generate random ID (starting from 101)
        file_id = 101 + i
        
        # Generate random timestamp within a reasonable range
        random_days = random.randint(0, 365)
        random_hours = random.randint(0, 23)
        random_minutes = random.randint(0, 59)
        random_seconds = random.randint(0, 59)
        
        timestamp = base_date + timedelta(
            days=random_days,
            hours=random_hours,
            minutes=random_minutes,
            seconds=random_seconds
        )
        
        # Format timestamp in ISO 8601 format
        timestamp_str = timestamp.strftime("%Y-%m-%dT%H:%M:%S")
        
        test_cases.append((file_name, file_id, timestamp_str))
    
    return test_cases

def generate_test_file(num_cases=5):
    # Generate test cases
    test_cases = generate_test_strings(num_cases)
    
    # Create the complete test input
    result = [str(num_cases)]  # First line is the number of cases
    
    # Add each test case
    for file_name, file_id, timestamp in test_cases:
        result.append(f"{file_name} {file_id} {timestamp}")
    
    # Add random sorting column as the last line
    sort_columns = ["Name", "ID", "Timestamp"]
    result.append(random.choice(sort_columns))
    
    return result

# Example usage
test_input = generate_test_file(num_cases=40960)
print("\n".join(test_input))