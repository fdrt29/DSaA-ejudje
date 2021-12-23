import re
import math
import sys

try:
    from collections.abc import Sequence
except ImportError:
    from collections import Sequence


# ----- Bitset { -------------------------------------------------------
class Bitset(Sequence):
    value = 0
    length = 0

    def __len__(self):
        return self.length

    def __init__(self, length):
        self.value = 0
        self.length = length

    def __getitem__(self, s):
        if isinstance(s, slice):
            start, stop, step = s.indices(len(self))
            results = []
            for position in range(start, stop, step):
                pos = len(self) - position - 1
                results.append(bool(self.value & (1 << pos)))
            return results
        elif isinstance(s, int):
            pos = len(self) - s - 1
            return bool(self.value & (1 << pos))
        else:
            raise TypeError("Invalid argument type.")

    def __setitem__(self, s, value):
        if isinstance(s, slice):
            start, stop, step = s.indices(len(self))
            for position in range(start, stop, step):
                pos = len(self) - position - 1
                if value:
                    self.value |= (1 << pos)
                else:
                    self.value &= ~(1 << pos)
            maximum_position = max((start + 1, stop, len(self)))
            self.length = maximum_position
        elif isinstance(s, int):
            pos = len(self) - s - 1
            if value:
                self.value |= (1 << pos)
            else:
                self.value &= ~(1 << pos)
            if len(self) < pos:
                self.length = pos
        else:
            raise TypeError("Invalid argument type.")
        return self

    def __str__(self):
        s = ""
        for i in self[:]:
            s += "1" if i else "0"
        return s

    def __repr__(self):
        return "Bitset(%s)" % str(self)


# ----- } End of Bitset -------------------------------------------------------


# ----- Bloom Filter { -------------------------------------------------------
class BloomFilterError(RuntimeError):
    pass


class BloomFilter:
    mersenne_31_n = 2147483647

    def __init__(self, n, p):
        self.size = round(-n * (math.log2(p)) / math.log(2))
        self.hash_count = round(-math.log2(p))
        if not self.hash_count:
            raise BloomFilterError
        self.bitset = Bitset(self.size)
        self.primes = get_prime_list(self.hash_count)
        self._indexes = self

    def f_hash(self, seed, key):
        return (((seed + 1) * key + self.primes[seed]) % BloomFilter.mersenne_31_n) % self.size

    def __contains__(self, key):
        for i in range(self.hash_count):
            if not self.bitset[self.f_hash(i, key)]:
                return False
        return True

    def add(self, key):
        for i in range(self.hash_count):
            self.bitset[self.f_hash(i, key)] = True

    def __str__(self):
        return str(self.bitset)


# ----- } End of Bloom Filter -------------------------------------------------

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


# ----- Input handle: ----------------------------------------------------

def main():
    bl_filter = None

    set_size_pattern = re.compile(r"^set\s([+]?[1-9]\d*)\s([+]?\d+\.\d+)\n")  # 1st group - int, 2nd - float
    add_pattern = re.compile(r"^add\s([^ ]+)\n")  # non-space chars in group
    search_pattern = re.compile(r"^search\s([^ ]+)\n")
    print_pattern = re.compile(r"^print\n")

    for line in sys.stdin:
        if line == '\n' or line == '':
            continue

        if bl_filter is None:
            match = set_size_pattern.match(line)
            if match:
                appr_el_num = int(match.group(1))
                probability = float(match.group(2))
                try:
                    bl_filter = BloomFilter(appr_el_num, probability)
                except BloomFilterError:
                    print("error")
                    continue
                print(bl_filter.size, bl_filter.hash_count)
                continue
        else:
            match = add_pattern.match(line)
            if match:
                key = int(match.group(1))
                bl_filter.add(key)
                continue

            match = search_pattern.match(line)
            if match:
                key = int(match.group(1))
                res = 1 if key in bl_filter else 0
                print(res)
                continue

            match = print_pattern.match(line)
            if match:
                print(str(bl_filter))
                continue

        # Else
        print("error")


if __name__ == '__main__':
    main()
