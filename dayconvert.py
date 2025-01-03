#! python3

# Milliseconds since epoch
milliseconds = 1735927379837

# Convert milliseconds to seconds and then to a datetime object
epoch = datetime(1970, 1, 1)
converted_time = epoch + timedelta(milliseconds=milliseconds)

# Format the datetime to display time to the second
formatted_time = converted_time.strftime("%Y-%m-%d %H:%M:%S")
print(formatted_time)
