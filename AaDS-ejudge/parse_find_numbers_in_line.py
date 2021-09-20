import sys
import re
from enum import Enum


class State(Enum):
    Junk = 1
    Sign = 2
    Digit = 3


def is_sign(ch):
    return ch == '+' or ch == '-'


def ints_from_line(line: str):
    state = State.Junk
    num = ''
    for e in line:
        if is_sign(e):
            if state == State.Digit:
                yield int(num)
                num = ''
            state = State.Sign
            num = e
        elif e.isdigit():
            if state == State.Junk or state == State.Sign:
                state = State.Digit
            # if previous sign, digit or junk
            num += e
        else:
            if state == State.Digit:
                state = State.Junk
                yield int(num)
                num = ''


def main():
    res = 0
    for line in sys.stdin:
        # line = re.findall(r'[+\-]?(\d+)', line)
        for n in ints_from_line(line):
            res += n
    print(res)


if __name__ == '__main__':
    main()
