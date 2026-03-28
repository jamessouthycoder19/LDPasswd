import os
import json

files = [
    "passwords.txt",
    "english_wikipedia.txt",
    "us_tv_and_film.txt",
    "surnames.txt",
    "male_names.txt",
    "female_names.txt",
    ]

json_file = "data.txt"
header_file = "data.h"

# Clear the json file before writing to it
with open ("../data/" + json_file, "w") as f:
    f.write("")

json_data = {}

for file in files:
    split = False
    if file == "english_wikipedia.txt" or file == "passwords.txt" or file == "us_tv_and_film.txt":
        split = True
    with open("../data/original-wordlists/" + file) as f:
        lines = f.read().splitlines()
        for line in lines:
            if split:
                line = line.split()[0]

            # make sure we only have characters a-z and A-Z
            line = ''.join(c for c in line if c.isalpha())
            if len(line) == 0:
                continue

            chunks = [line[i:i+2] for i in range(0, len(line), 2)]

            if chunks[0] not in json_data:
                json_data[chunks[0]] = None if len(chunks) == 1 else {}
                

            current_dict = json_data[chunks[0]]
            if current_dict is None and len(chunks) > 1:
                current_dict = {}
                json_data[chunks[0]] = current_dict
            
            try:
                for i in range(1, len(chunks)):
                    chunk = chunks[i]
                    if chunk not in current_dict:
                        current_dict[chunk] = None if len(chunks) == i+1 else {}
                    elif current_dict[chunk] is None:
                        current_dict[chunk] = {}
                    current_dict = current_dict[chunk]
            except Exception as e:
                print(f"Error processing line: {line}")
                print(f"Chunks: {chunks}")
                print(f"Current dict: {current_dict}")
                raise e
            
with open("../data/" + json_file, "w") as f:
    f.write(json.dumps(json_data) + "\n")

with open("data", "w") as f:
    f.write(json.dumps(json_data) + "\n")

os.system(f"xxd -i data > ../include/ldpasswd/{header_file}")