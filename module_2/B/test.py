import sys

from m2_taskB import main

if __name__ == '__main__':
    sys.stdout = open("output.txt", 'w')
    with open("input.txt") as sys.stdin:
        main()
