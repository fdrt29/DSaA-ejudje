import sys

from m3_taskD import main

if __name__ == '__main__':
    sys.stdout = open("output.txt", 'w')
    with open("input.txt") as sys.stdin:
        main()
