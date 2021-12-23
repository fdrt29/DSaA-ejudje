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
    if "primes" not in get_prime_list.__dict__:
        get_prime_list.primes = [3]  # analog of a static variable of function
        get_prime_list.multiples = [3]
    primes = get_prime_list.primes
    multiples = get_prime_list.multiples
    if list_length < len(primes):
        return primes[:list_length]
    candidate = 5
    while len(primes) < list_length:
        if is_prime(primes, multiples, candidate):
            primes.append(candidate)
            multiples.append(candidate)
        candidate += 2
    return primes


start = time.time()
ls = get_prime_list(1000)
end = time.time()
print(end - start)


start = time.time()
ls = get_prime_list(20)
end = time.time()
print(end - start)
# print(get_prime_list(20))
