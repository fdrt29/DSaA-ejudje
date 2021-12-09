import re, sys


# # максимальная стоимость предметов,
# # которые можно уложить в рюкзак вместимости s,
# # если можно использовать только первые k предметов

class Item:
    def __init__(self, weight: int, cost: int):
        self.weight = weight
        self.cost = cost


def alg(items: list, limit):
    memo = [[0] * (limit + 1) for _ in range(len(items) + 1)]
    for i in range(1, len(items) + 1):
        for j in range(1, limit + 1):  # local limit
            if items[i - 1].weight <= j:
                memo[i][j] = max(memo[i - 1][j], items[i - 1].cost + memo[i - 1][j - items[i - 1].weight])
            else:
                memo[i][j] = memo[i - 1][j]

    # Обратный ход
    res_ind = []
    res_weight = 0
    res_cost = 0
    j = limit
    i = len(items)
    while i > 1 or j > 1:  # todo and?
        if memo[i][j] == 0:
            break
        if memo[i][j] != memo[i - 1][j]:
            res_ind.append(i)
            res_weight += items[i - 1].weight
            res_cost += items[i - 1].cost
            j -= items[i - 1].weight
        i -= 1
    return res_weight, res_cost, res_ind


if __name__ == '__main__':
    w_limit = 0
    all_items = []

    # handle input
    item_pattern = re.compile(r"^([+]?\d+)\s([+]?\d+)\n")  # positive nums in groups
    is_limit_initialized = False
    for line in sys.stdin:
        if line == 'end\n':  # TODO DELETE
            break
        if line == '\n' or line == '':
            continue

        if not is_limit_initialized:
            try:
                w_limit = int(line)
                is_limit_initialized = True
            except ValueError:
                print("error")
            continue

        match = item_pattern.match(line)
        if match:
            all_items.append(Item(int(match.group(1)), int(match.group(2))))
            continue
        # Else
        print("error")

    # Algorithm
    res_weight, res_cost, res_ind = alg(all_items, w_limit)
    print(res_weight, res_cost)
    print(*sorted(res_ind), sep='\n')
