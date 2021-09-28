import re
import sys


class Stack(object):
    def __init__(self, size):
        self.size_ = size
        self.array_ = [None] * size
        self.head_index_ = -1

    def push(self, value: str):
        # В случае переполнения стека выводится "overflow".
        if self.head_index_ + 1 >= self.size_:
            print("overflow")
            return
        self.head_index_ += 1
        self.array_[self.head_index_] = value

    def pop(self):
        # Команда pop выводит элемент или "underflow", если стек пуст.
        if self.empty():
            print("underflow")
            return
        print(self.array_[self.head_index_])
        self.array_[self.head_index_] = None
        self.head_index_ -= 1

    def print(self):
        # Команда print выводит содержимое стека (снизу вверх) одной строкой, значения разделяются пробелами.
        # Если стек пуст, то выводится "empty".
        if self.empty():
            print("empty")
        end = " "
        for i in range(0, self.head_index_ + 1):
            if i == self.head_index_:
                end = "\n"
            print(self.array_[i], end=end)

    def empty(self):
        return self.head_index_ == -1


def main():
    stack = None

    set_size_pattern = re.compile(r"^set_size ([+]?\d+)\n")  # positive nums in group
    push_pattern = re.compile(r"^push ([^ ]+)\n")  # non-space chars in group
    pop_pattern = re.compile(r"^pop\n")
    print_pattern = re.compile(r"^print\n")

    for line in sys.stdin:
        if line == '\n' or line == '':
            continue

        if stack is None:
            match = set_size_pattern.match(line)
            if match:
                # Get size from match group
                # 0 is whole match, 1 - first matching group
                size = int(match.group(1))
                stack = Stack(size)
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
