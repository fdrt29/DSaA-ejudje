import math
import time


def is_prime(prime_list, multiple_list, candidate):
    limit = int(math.sqrt(candidate))
    result = True
    for pm, mul in zip(prime_list, multiple_list):
        if pm > limit:
            break
        while mul < candidate:
            mul += pm
        if mul == candidate:
            result = False
            break
    return result


def get_prime_list(list_length):
    if "primes" not in get_prime_list.__dict__:  # caching, analog of a static variable of function
        get_prime_list.primes = [3]
        get_prime_list.multiples = [3]  # multiple of the corresponding prime closest to the candidate
    primes = get_prime_list.primes
    multiples = get_prime_list.multiples
    list_length -= 1  # 2 is not in the list of prime numbers
    if list_length < len(primes):
        return [2] + primes[:list_length]
    candidate = 5
    while len(primes) < list_length:
        if is_prime(primes, multiples, candidate):
            primes.append(candidate)
            multiples.append(candidate)
        candidate += 2
    return [2] + primes


start = time.time()
ls = get_prime_list(20)
end = time.time()
print(end - start)
print(ls)

start = time.time()
ls = get_prime_list(10)
end = time.time()
print(end - start)
print(ls)
# print(get_prime_list(20))
