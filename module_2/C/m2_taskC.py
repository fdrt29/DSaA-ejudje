import re
import sys


class MinHeap:
    def __init__(self):
        self.array = []
        self.size = 0

    def sift_up(self, x):
        pass

    def sift_down(self):
        pass


def main():
    bin_heap = MinHeap()

    add_pattern = re.compile(r"^add\s([+-]?\d*)\s([^ ]+?|)\n")  # 1st group - int, 2nd - non-space chars
    set_pattern = re.compile(r"^set\s([+-]?\d*)\s([^ ]+?|)\n")
    delete_pattern = re.compile(r"^delete\s([+-]?\d*)\n")
    search_pattern = re.compile(r"^search\s([+-]?\d*)\n")

    for line in sys.stdin:
        if line == '\n' or line == '':
            continue
        match = add_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not bin_heap.Insert(key, match.group(2)):
                print("error")
            continue

        match = set_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not bin_heap.set(key, match.group(2)):
                print("error")
            continue

        match = delete_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not bin_heap.delete(key):
                print("error")
            continue

        match = search_pattern.match(line)
        if match:
            key = int(match.group(1))
            x = bin_heap.search(key)
            st = '1 ' + x if x is not None else '0'
            print(st)
            continue

        if line == 'min\n':
            x = bin_heap.min()
            if x:
                print(x[0], x[1])
            else:
                print("error")
            continue
        elif line == 'max\n':
            x = bin_heap.max()
            if x:
                print(x[0], x[1])
            else:
                print("error")
            continue
        elif line == "print\n" or line == "print":
            print(*bin_heap.level_str_generator(), sep='\n')
            continue

        # Else
        print("error")


if __name__ == '__main__':
    main()
