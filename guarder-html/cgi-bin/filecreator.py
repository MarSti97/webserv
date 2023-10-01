# Define the text you want to write to the file
text_to_write = "Success!!"

# Specify the file path (change to your desired file path)
file_path = "output.txt"

# Open the file in 'w' mode (write mode)
# If the file does not exist, it will be created. If it exists, its contents will be overwritten.
with open(file_path, 'w') as file:
    file.write(text_to_write)

# Inform the user that the operation is complete
print(f"Text has been written to {file_path}")