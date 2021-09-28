import re
import sys


class Queue(object):
    def __init__(self, size):
        self.size_ = size
        self.array_ = [None] * size
        self.head_index_ = 0
        self.tail_index_ = 0

    def push(self, value: str):
        # В случае переполнения выводится "overflow".
        if (self.tail_index_ + 1) % self.size_ == self.head_index_:
            print("overflow")
            return
        if not self.empty():
            self.tail_index_ = (self.tail_index_ + 1) % self.size_

        self.array_[self.tail_index_] = value

    def pop(self):
        # Команда pop выводит элемент или "underflow", если пуст.
        if self.empty():
            print("underflow")
            return
        print(self.array_[self.head_index_])
        self.array_[self.head_index_] = None

        if not self.empty():
            self.head_index_ = (self.head_index_ + 1) % self.size_

    def print(self):
        # Команда print выводит содержимое очередь (от головы к хвосту) одной строкой, значения разделяются пробелами.
        # Если очередь пуста, то выводится "empty".
        if self.empty():
            print("empty")
            return

        itr = self.head_index_
        while itr != self.tail_index_:
            print(self.array_[itr], end=" ")
            itr = (itr + 1) % self.size_
        print(self.array_[itr], end="\n")

    def empty(self):
        return self.head_index_ == self.tail_index_ and self.array_[self.head_index_] is None


def main():
    stack = None

    set_size_pattern = re.compile(r"^set_size ([+]?\d+)\n")  # positive nums in group
    push_pattern = re.compile(r"^push ([^ ]+)\n")  # non-space chars in group
    pop_pattern = re.compile(r"^pop\n")
    print_pattern = re.compile(r"^print\n")

    sys.stdin = open(sys.argv[1])
    sys.stdout = open(sys.argv[2], 'w')
    for line in sys.stdin:
        if line == '\n' or line == '':
            continue

        if stack is None:
            match = set_size_pattern.match(line)
            if match:
                # Get size from match group
                # 0 is whole match, 1 - first matching group
                size = int(match.group(1))
                stack = Queue(size)
                continue
        else:
            match = push_pattern.match(line)
            if match:
                stack.push(match.group(1))
                continue

            match = pop_pattern.match(line)
            if match:
                stack.pop()
                continue

            match = print_pattern.match(line)
            if match:
                stack.print()
                continue

        # Else
        print("error")


if __name__ == '__main__':
    try:
        main()
    except BaseException:
        print("error")
    else:
        pass
