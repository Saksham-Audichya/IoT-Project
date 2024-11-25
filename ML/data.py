import csv
import re

# File paths
input_file_path = 'esp32_results_1.txt'  # Replace with the actual input file path
output_file_path = 'SSID_grid_1.csv'  # Output CSV file

# Initialize the data structure to hold grid data
grid_data = {}

# Function to parse lines in the input file
def parse_line(line):
    # Regular expression to match the relevant information
    match = re.match(r"\[Grid: (\w+)\] SSID: CSG518-(\d) RSSI: (-\d+ dBm)", line)
    if match:
        grid = match.group(1)
        ssid = f"CSG518-{match.group(2)}"
        rssi = match.group(3)
        return grid, ssid, rssi
    return None

# Read the input file and organize data by grid and SSID
with open(input_file_path, 'r') as file:
    for line in file:
        if "Scanning stopped" not in line:  # Skip lines with scanning stopped results
            result = parse_line(line)
            if result:
                grid, ssid, rssi = result
                if grid not in grid_data:
                    grid_data[grid] = {f"CSG518-{i}": [] for i in range(1, 9)}  # Initialize empty lists for each SSID
                grid_data[grid][ssid].append(rssi)

# Prepare the rows for the CSV output
csv_rows = []

# Write headers
headers = ['Grid', 'CSG518-1', 'CSG518-2', 'CSG518-3', 'CSG518-4', 'CSG518-5', 'CSG518-6', 'CSG518-7', 'CSG518-8']
csv_rows.append(headers)

# Generate rows for each grid
for grid, ssid_data in grid_data.items():
    # Find the maximum number of rows needed for this grid (the longest list for each SSID)
    max_rows = max(len(rssi_list) for rssi_list in ssid_data.values())
    
    # Iterate and create rows
    for i in range(max_rows):
        row = [grid]
        for ssid in range(1, 9):
            ssid_key = f"CSG518-{ssid}"
            if i < len(ssid_data[ssid_key]):
                row.append(ssid_data[ssid_key][i])
            else:
                row.append('0')  # Missing value, insert 0
        csv_rows.append(row)

# Write the rows to the CSV file
with open(output_file_path, 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    csvwriter.writerows(csv_rows)

print(f"Data successfully written to {output_file_path}")
