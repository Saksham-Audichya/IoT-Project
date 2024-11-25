import pandas as pd

# Load the CSV file
input_file = 'SSID_grid_1.csv'  # Replace with your file name
df = pd.read_csv(input_file)

# Remove rows where all elements are NaN (blank)
df_cleaned = df.dropna(how='all')

# Save the cleaned DataFrame back to a new CSV file
output_file = 'SSID.csv'  # Replace with your desired output file name
df_cleaned.to_csv(output_file, index=False)

print(f"Blank rows removed and saved to {output_file}")
