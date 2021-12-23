
import math


def is_prime(prime_list, candidate):
    limit = int(math.sqrt(candidate))
    is_prm = True
    for pm in prime_list:
        if pm[0] > limit:
            break
        while pm[1] < candidate:
            pm[1] += pm[0]
        if pm[1] == candidate:
            is_prm = False
            break
    return is_prm


def get_prime_list(list_length):
    if "primes" not in get_prime_list.__dict__:
        get_prime_list.primes = [[3, 3]]  # analog of a static variable of function
        # get_prime_list.multiples = [3]  # analog of a static variable of function
    # TODO хранить делители в отдельном списке
    primes = get_prime_list.primes
    # multiples = get_prime_list.multiples
    if len(primes) < list_length:
        return primes[:list_length]
    candidate = 5
    while len(primes) < list_length:
        if is_prime(primes, candidate):
            primes.append([candidate, candidate])
            # multiples.append(candidate)
        candidate += 2
    return primes


get_prime_list(1000)

print(get_prime_list(20))
