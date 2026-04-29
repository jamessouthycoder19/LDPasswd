import os
import struct
import math

# Important words are words that can be customized per red team environement, to make sure that words that
# operators are pretty confident will be used in the target environment are included in the trie with a very 
# high frequency, to ensure that they are tokenized correctly. For example IRSeC is not an actual common word,
# but for our yearly IRSeC compeitition at RITSEC, people are probably going to put irsec in their password. 
IMPORTANT_WORDS = ["password", "red", "blue", "irsec", "lockdown"]

DATA_LOCATION = "../data.bin"
DATA_HEADER_LOCATION = "../../src/data.c"

ORIGNIAL_DATA_LOCATION = "../original-wordlists/"

def generate_binary_table(wordlist_paths):
    root = {}
    total_frequency = 0
    DEFAULT_FREQUENCY = 25

    # Build the Trie
    for path in wordlist_paths:
        print(f"Processing: {path}")
        path = f"{ORIGNIAL_DATA_LOCATION}{path}"
        if not os.path.exists(path):
            continue
            
        with open(path, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                parts = line.strip().split()
                if not parts: continue
                
                raw_word = parts[0]
                word = ''.join(c for c in raw_word if c.isalpha()).lower()
                if not word: continue

                frequency = int(parts[1]) if len(parts) > 1 and parts[1].isdigit() else DEFAULT_FREQUENCY
                total_frequency += frequency

                # Boost the frequency of a some important words to ensure that they are tokenized correctly, 
                # even if they don't appear in the wordlists with a high frequency.
                if word in IMPORTANT_WORDS:
                    frequency += 1000000 
                    total_frequency += 1000000
                
                # Insert into Trie, add vars for it's children, and if it's a word or not, and the frequency of the word (if it is a word)
                curr = root
                for i, char in enumerate(word):
                    if char not in curr:
                        curr[char] = {"_is_word": False, "freq": 0, "children": {}}
                    if i == len(word) - 1:
                        curr[char]["_is_word"] = True
                        curr[char]["freq"] += frequency
                    curr = curr[char]["children"]

    for word in IMPORTANT_WORDS:
        if word not in root:
            curr = root
            for i, char in enumerate(word):
                if char not in curr:
                    curr[char] = {"_is_word": False, "freq": 0, "children": {}}
                if i == len(word) - 1:
                    curr[char]["_is_word"] = True
                    curr[char]["freq"] += 1000000
                    total_frequency += 1000000
                curr = curr[char]["children"]

    # Calculate log total frequency for cost calculations
    log_total = math.log2(total_frequency) if total_frequency > 0 else 0
    flat_table = []

    # Recursively flatten the Trie into a list. Each node in the Trie is represented as a dictionary with keys:
    # - '_is_word': boolean indicating if the node represents the end of a valid word
    # - 'freq': frequency of the word if '_is_word' is True
    # - 'children': a dictionary of child nodes keyed by the next character
    def flatten(node_dict, current_depth):
        if not node_dict:
            return -1, 0
        
        start_index = len(flat_table)
        sorted_keys = sorted(node_dict.keys())
        
        for key in sorted_keys:
            node = node_dict[key]
            word_freq = node['freq']
            cost = 0.0
            
            if node['_is_word'] and word_freq > 0:
                # Base cost from frequency
                cost = log_total - math.log2(word_freq)
                
                # Apply Length Penalties. We want short words to not be prioritized as much
                if current_depth == 1:
                    cost += 15.0
                elif current_depth == 2:
                    cost += 10.0
                elif current_depth == 3:
                    cost += 5.0
                
                if cost < 0.0001: cost = 0.0001

            flat_table.append({
                'token': key, 
                'is_word': node['_is_word'], 
                'cost': cost,
                'child_idx': -1, 
                'child_cnt': 0
            })
            
        for i, key in enumerate(sorted_keys):
            # Recurse and increment depth for children
            c_idx, c_cnt = flatten(node_dict[key]['children'], current_depth + 1)
            flat_table[start_index + i]['child_idx'] = c_idx
            flat_table[start_index + i]['child_cnt'] = c_cnt
            
        return start_index, len(sorted_keys)

    # Start flattening at depth 1
    root_idx, root_cnt = flatten(root, 1)

    # Write all of this to to a binary file
    with open(DATA_LOCATION, "wb") as f:
        for entry in flat_table:
            token_byte = entry['token'].encode('ascii')
            f.write(struct.pack('c?2xfii', 
                                token_byte, 
                                entry['is_word'], 
                                entry['cost'], 
                                entry['child_idx'], 
                                entry['child_cnt']))
    
    return total_frequency

def main():
    wordlist_paths = [
        # "passwords.txt",
        "english_wikipedia.txt",
        "us_tv_and_film.txt",
        "surnames.txt",
        "male_names.txt",
        "female_names.txt",
    ]
    total = generate_binary_table(wordlist_paths)
    print(f"Total Frequency: {total}")
    print(f"Flattening complete")
    os.system(f"xxd -i {DATA_LOCATION} > {DATA_HEADER_LOCATION}")
    print("Header file generated")

if __name__ == "__main__":
    main()