# # максимальная стоимость предметов,
# # которые можно уложить в рюкзак вместимости s,
# # если можно использовать только первые k предметов
# def A(k, s):
#     if k == 0 or s == 0:
#         return 0
class Item:
    def __init__(self, cost, weight):
        self.cost = cost
        self.weight = weight


def alg(items: list, w_limit):
    table = [[0] * (len(items) + 1)] * w_limit
    for i in items:
        for j in range(w_limit):

            pass
