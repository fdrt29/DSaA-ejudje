import sys

from m2_taskB import main

if __name__ == '__main__':
    sys.stdout = open("output.txt", 'w')
    with open("input.txt") as sys.stdin:
        main()

#     def print_to(self, stream):
#         def visit_callback(node: BinaryTree.Node, level: int, is_next_level):
#             st = ''
#             if level != 0:
#                 if is_next_level:
#                     st = '\n'
#                 else:
#                     st = ' '
#
#             if node is not None:
#                 st += str(node)
#             else:
#                 st += "_"
#             print(st, file=stream, end='')
#         levelorder_traversal(self.root, visit_callback)
