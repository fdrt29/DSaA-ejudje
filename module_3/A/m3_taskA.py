import re

import sys
from math import gcd


class Item:
    def __init__(self, weight: int, cost: int):
        self.weight = weight
        self.cost = cost


class BackpackAlgorithm:
    # Неявные условия. "Алгоритм должен быть инкапсулирован"? Расценено как просто изоляция алгоритма от обработки ввода
    # На всякий случай в класс запихнул...
    @staticmethod
    def execute(items: list, limit):
        n_limit, n_weights = BackpackAlgorithm.normalize_weights(items, limit)
        n = len(items)

        # меморизация максимальной стоимости предметов,
        # которые можно уложить в рюкзак вместимости limit,
        # если можно использовать только первые i предметов
        memo = [[0] * (n_limit + 1) for _ in range(n + 1)]
        for i in range(1, n + 1):
            for j in range(0, n_limit + 1):  # local limit
                if n_weights[i - 1] <= j:  # а влезает ли вообще предмет при данном лимите?
                    # стоимость больше если положить предмет или нет?
                    memo[i][j] = max(memo[i - 1][j], items[i - 1].cost + memo[i - 1][j - n_weights[i - 1]])
                else:
                    memo[i][j] = memo[i - 1][j]

        # Обратный ход
        res_ind = []
        res_weight = 0
        res_cost = memo[-1][-1]
        j = n_limit
        i = n
        while i > 0 or j > 0:
            if memo[i][j] == 0:
                break
            if memo[i][j] != memo[i - 1][j]:
                res_ind.append(i)  # индекс уменьшен, чтобы начинаться с 1
                res_weight += items[i - 1].weight
                j -= n_weights[i - 1]
            i -= 1
        return res_weight, res_cost, res_ind

    @staticmethod
    def normalize_weights(items, limit):
        n = len(items)
        gcd_ = gcd(limit, items[0].weight)
        for i in range(1, n):
            gcd_ = gcd(gcd_, items[i].weight)
        gcd_ = 1 if gcd_ == 0 else gcd_

        normalized = [items[i].weight // gcd_ for i in range(n)]
        return [limit // gcd_, normalized]


if __name__ == '__main__':
    w_limit = 0
    all_items = []

    # Handle input
    item_pattern = re.compile(r"^([+]?\d+)\s([+]?\d+)\n")  # positive nums in groups
    is_limit_initialized = False
    for line in sys.stdin:
        if line == '\n' or line == '':
            continue

        # if line == 'end\n':
        #     break

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
    weight, cost, indexes = BackpackAlgorithm.execute(all_items, w_limit)
    print(weight, cost)
    print(*sorted(indexes), sep='\n')
