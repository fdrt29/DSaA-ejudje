import re
from collections import Sequence

import math
import sys


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
        try:
            start, stop, step = s.indices(len(self))
            results = []
            for position in range(start, stop, step):
                pos = len(self) - position - 1
                results.append(bool(self.value & (1 << pos)))
            return results
        except Exception:
            pos = len(self) - s - 1
            return bool(self.value & (1 << pos))

    def __setitem__(self, s, value):
        try:
            start, stop, step = s.indices(len(self))
            for position in range(start, stop, step):
                pos = len(self) - position - 1
                if value:
                    self.value |= (1 << pos)
                else:
                    self.value &= ~(1 << pos)
            maximum_position = max((start + 1, stop, len(self)))
            self.length = maximum_position
        except Exception:
            pos = len(self) - s - 1
            if value:
                self.value |= (1 << pos)
            else:
                self.value &= ~(1 << pos)
            if len(self) < pos:
                self.length = pos
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
        self.primes = []
        prime = prime_gen()
        while len(self.primes) < self.hash_count:
            self.primes.append(next(prime))

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

    def print(self, stream=sys.stdout):
        print(str(self.bitset), file=stream)


# ----- } End of Bloom Filter -------------------------------------------------

def prime_gen():
    dct = {}
    a = 2
    while True:
        if a not in dct:
            yield a
            dct[a * a] = [a]
        else:
            for p in dct[a]:
                dct.setdefault(p + a, []).append(p)
            del dct[a]
        a += 1


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
            # if line == "end":
            #     break

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
                # bl_filter.print()
                continue

        # Else
        print("error")


if __name__ == '__main__':
    main()
