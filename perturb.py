# perturb.py
import math
import random
import itertools

def tokenize(password: str) -> tuple[list[str], list[str]]:
    return ["Password", "123", "!"], ["w", "n", "s"]

def exponential_mech(output_space: list, utility_space: list[int], eps: float, sensitivity: float = 1.0):
    weights = []
    total_weight = 0
    for i in range(len(output_space)):
        weight = math.exp((eps * utility_space[i])/(2 * sensitivity))
        weights.append(weight)
        total_weight += weight

    probabilities = [x / total_weight for x in weights]

    # choose random number 0-1
    rand = random.uniform(0,1)
    selection = ""
    cumsum = 0
    # get cumulative sum to see which category our random number falls into
    for i in range(len(probabilities)):
        prob = probabilities[i]
        cumsum += prob
        if rand < cumsum:
            selection = output_space[i]
            break

    return selection

def get_swap_distance(original: list[str], permutation: list[str]) -> int:
    # leetcode type problem lol
    original_map = {element: index for index, element in enumerate(original)}

    permutation_indices = [original_map[element] for element in permutation]

    inversions = 0
    n = len(permutation_indices)
    
    # Count how many times a larger index appears before a smaller index
    for i in range(n):
        for j in range(i + 1, n):
            if permutation_indices[i] > permutation_indices[j]:
                inversions += 1
                
    return -1 * inversions

def semantically_perturb(tokens: list[str], token_types: list[str], eps: float):
    token_permutations = [list(p) for p in itertools.permutations(tokens)]
    token_type_permutations = [list(p) for p in itertools.permutations(token_types)]

    utility_space = [get_swap_distance(tokens, token_permutations[i]) for i in range(len(token_permutations))]

    selected_permutation = exponential_mech(token_permutations, utility_space, eps)

    permutation_index = token_permutations.index(selected_permutation)

    selected_token_type_permutation = token_type_permutations[permutation_index]

    return selected_permutation, selected_token_type_permutation

def perturb_word(word: str, eps: float) -> str:
    return word

def perturb_number(number: str, eps: float) -> str:
    if (not str.isdigit(number)):
        return number
    
    num_len = len(number)
    max_num = int('9' * num_len)

    output_space = [str(x) for x in range(max_num + 1)]
    utility_space = [-1 * abs(int(number) - int(x)) for x in output_space]

    selection = exponential_mech(output_space, utility_space, eps)

    return selection

def perturb_special(special: str, eps: float) -> str:
    return special

def perturb(password: str, eps: float = 1.0) -> str:
    tokens, token_types = tokenize(password)

    semantic_budget = eps / 2
    diction_budget = eps / 2

    tokens, token_types = semantically_perturb(tokens, token_types, semantic_budget)

    budget_per_token = diction_budget / len(tokens)
    for i in range(len(tokens)):
        token = tokens[i]
        token_type = token_types[i]

        if token_type == "w":
            tokens[i] = perturb_word(token, budget_per_token)
        elif token_type == "n":
            tokens[i] = perturb_number(token, budget_per_token)
        elif token_type == "s":
            tokens[i] = perturb_special(token, budget_per_token)
        
    return ''.join(tokens)

def main():
    perturbed_pass = perturb("Password123!", 2.0)
    print(perturbed_pass)

if (__name__ == '__main__'):
    main()