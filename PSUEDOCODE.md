## perturb_password() main public function
```py
def perturb_password(password: str, eps: float) -> str:
    unleeted_password = unleet_password(password)
    tokens, token_types = tokenize_password(unleeted_password)

    # Divy up Privacy Budget
    semantic_budget = eps / (len(tokens) + 1)
    diction_budget = (len(tokens) * eps) / (len(tokens) + 1)
    budget_per_token = diction_budget / len(tokens)

    # Diction Perturbment
    for i in range(len(tokens)):
        token = tokens[i]
        token_type = token_types[i]
        if token_type == "word":
            tokens[i] = perturb_word(token, budget_per_token)
        if token_type == "number":
            tokens[i] = perturb_number(token, budget_per_token)
        if token_type == "special":
            tokens[i] = perturb_special(token, budget_per_token)

    # Semantic Perturbment
    perturb_semantics(password, semantic_budget)

    perturbed_password = fix_leet_password(password, ''.join(tokens))

    return perturbed_password
```

## tokenize_password
```py
def tokenize_password(password: str) -> int:
    lower_password = password.lower()
    unleeted_variants = generate_unleet(lower_password)

    token_count = 0
    i = 0

    while i < len(password) and token_count < 20:
        best_len = 0
        best_total_cost = INF
        best_unleeted_source = None

        for variant in unleeted_variants:
            costs = find_word_costs(variant[i:])
            for len_candidate in valid_word_lengths(costs):
                if len_candidate <= 2 and not password[i].isalpha():
                    continue

                word_cost = costs[len_candidate - 1]
                follow_cost = best_followup_word_cost(unleeted_variants, i + len_candidate)
                total_cost = word_cost + follow_cost + SPLIT_PENALTY

                if total_cost < best_total_cost:
                    best_total_cost = total_cost
                    best_len = len_candidate
                    best_unleeted_source = variant[i:]

        start_of_token_indices[token_count] = i

        if best_len > 0 and is_valid_word_token(password, i, best_len):
            token_types[token_count] = 'w'
            copy_unleeted_segment(unleeted_pw, i, best_unleeted_source, best_len)
            i += best_len
        elif password[i].isdigit():
            token_types[token_count] = 'n'
            while i < len(password) and password[i].isdigit() and digits_collected < 6:
                unleeted_pw[i] = password[i]
                i += 1
        elif password[i].ispunct():
            token_types[token_count] = 's'
            unleeted_pw[i] = password[i]
            i += 1
        else:
            token_types[token_count] = 'o'
            unleeted_pw[i] = password[i]
            i += 1

        token_count += 1

    cleanup_unleeted_variants(unleeted_variants)
    return token_count
```

## perturb_word
```py
def perturb_word(token: str, budget: float) -> str:
    current_idx = 0
    current_cnt = 26
    table = trie_table
    current_depth = 0
    nodes_visited = []

    while current_depth < len(token):
        key = Node(token=token[current_depth])
        match = binary_search(key, table[current_idx:current_idx+current_cnt])
        if match:
            nodes_visited.append(match)
            current_depth += 1
            if match.child_idx == -1 or match.child_cnt == 0:
                break
            current_idx = match.child_idx
            current_cnt = match.child_cnt
        else:
            break

    candidates = []
    candidate_costs = []
    candidate_distances = []
    min_cost = INF

    if current_depth == len(token) and nodes_visited[-1].is_word:
        candidates.append(token)
        candidate_costs.append(nodes_visited[-1].cost)
        min_cost = min(min_cost, candidate_costs[-1])
        candidate_distances.append(0)

    # Find sibling and child candidates
    for depth in range(current_depth - 1, -1, -1):
        if len(candidates) >= 10:
            break
        level_start = 0 if depth == 0 else nodes_visited[depth-1].child_idx
        level_count = 26 if depth == 0 else nodes_visited[depth-1].child_cnt

        for i in range(level_count):
            if len(candidates) >= 10:
                break
            sibling = table[level_start + i]
            if sibling == nodes_visited[depth]:
                continue

            if sibling.is_word:
                temp_word = build_word_from_nodes(nodes_visited[:depth]) + sibling.token
                if temp_word not in candidates:
                    candidates.append(temp_word)
                    candidate_costs.append(sibling.cost)
                    min_cost = min(min_cost, candidate_costs[-1])
                    candidate_distances.append((len(token) - depth) + 1)

            if sibling.child_idx != -1:
                for j in range(sibling.child_cnt):
                    if len(candidates) >= 10:
                        break
                    child = table[sibling.child_idx + j]
                    if child.is_word:
                        temp_word = build_word_from_nodes(nodes_visited[:depth]) + sibling.token + child.token
                        if temp_word not in candidates:
                            candidates.append(temp_word)
                            candidate_costs.append(child.cost)
                            min_cost = min(min_cost, candidate_costs[-1])
                            candidate_distances.append((len(token) - depth) + 1)

    if candidates:
        candidate_costs[0] = min_cost

    utilities = [min_cost - cost - dist for cost, dist in zip(candidate_costs, candidate_distances)]

    # Select using exponential mechanism
    selected_idx = exponential_mechanism(utilities, budget)
    return candidates[selected_idx]
```