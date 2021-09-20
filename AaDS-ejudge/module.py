import sys
import re


def main():
    is_program_start = False
    for line in sys.stdin:
        if re.match(r"set_size \d+"):
            pass





if __name__ == '__main__':
    main()
