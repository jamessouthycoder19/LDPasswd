import os
import struct

def generate_binary_table(wordlist_paths):
    root = {}
    # Build the Trie in memory first
    for path in wordlist_paths:
        with open(path, 'r') as f:
            for line in f:
                # Get rid of non-alphabetic characters and convert to lowercase
                word = ''.join(c for c in line.strip() if c.isalpha()).lower()
                if not word: continue
                
                # Split into chunks of 2 characters
                chunks = [word[i:i+2] for i in range(0, len(word), 2)]
                curr = root
                for i, chunk in enumerate(chunks):
                    if chunk not in curr:
                        curr[chunk] = {"_is_word": False, "children": {}}
                    if i == len(chunks) - 1:
                        curr[chunk]["_is_word"] = True
                    curr = curr[chunk]["children"]

    # Flatten the Trie into a list of nodes
    flat_table = []
    
    def flatten(node_dict):
        if not node_dict:
            return -1, 0
        
        start_index = len(flat_table)
        sorted_keys = sorted(node_dict.keys())
        
        # Reserve space for these keys
        for key in sorted_keys:
            flat_table.append({'token': key, 'is_word': node_dict[key]['_is_word'], 'child_idx': -1, 'child_cnt': 0})
            
        # Fill in children recursively
        for i, key in enumerate(sorted_keys):
            c_idx, c_cnt = flatten(node_dict[key]['children'])
            flat_table[start_index + i]['child_idx'] = c_idx
            flat_table[start_index + i]['child_cnt'] = c_cnt
            
        return start_index, len(sorted_keys)

    root_idx, root_cnt = flatten(root)

    # Write to binary file
    # Struct Format: 2s (token), ? (bool is_word), i (int child_idx), i (int child_cnt)
    # Alignment might add padding; '2s ? x i i' (12 bytes total)
    with open("data.bin", "wb") as f:
        for entry in flat_table:
            token_bytes = entry['token'].ljust(2, '\0').encode('ascii')
            f.write(struct.pack('2s?Bii', token_bytes, entry['is_word'], 0, entry['child_idx'], entry['child_cnt']))
    
    return root_idx, root_cnt

def main():
    wordlist_paths = [
        # "../data/original-wordlists/passwords.txt",
        "../data/original-wordlists/english_wikipedia.txt",
        "../data/original-wordlists/us_tv_and_film.txt",
        "../data/original-wordlists/surnames.txt",
        "../data/original-wordlists/male_names.txt",
        "../data/original-wordlists/female_names.txt",
    ]
    root_idx, root_cnt = generate_binary_table(wordlist_paths)
    print(root_idx, root_cnt)
    os.system(f"xxd -i data.bin > ../include/ldpasswd/data.h")

# Run this and then use: xxd -i data.bin > table_data.h

main()