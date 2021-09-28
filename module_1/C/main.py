import sys

import module

if __name__ == '__main__':
    # sys.stdout = open('output.txt', 'w')  # Something here that provides a write method.
    with open('input.txt') as sys.stdin:
        module.main()
    # module.main()
