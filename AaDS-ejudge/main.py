import sys
import module
import parse_find_numbers_in_line

# sys.stdout = open('output.txt', 'w')  # Something here that provides a write method.
with open('test_data.txt') as sys.stdin:
    parse_find_numbers_in_line.main()
# module.main()
