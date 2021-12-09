import sys
from collections import deque


class Node:
    def __init__(self, key, value, left=None, right=None):
        self.key = key
        self.value = value
        self.parent_key = parent_key
        self.left = left
        self.right = right

    def to_str(self, parent_key=None):
        if parent_key is None:
            return '[{0} {1}]'.format(self.key, self.value)
        return '[{0} {1} {2}]'.format(self.key, self.value, parent_key)


def levelorder_traversal(root, visit_callback):
    q = deque()
    q.append(root)
    while q:
        node = q.popleft()
        visit_callback(node)
        if node.left:
            q.append(node.left)
        if node.right:
            q.append(node.right)


class SplayTree:
    def __init__(self):
        self.root = None

    def add(self, key, value):
        if self.root is None:
            self.root = Node(key, value)
        else:
            self._add(key, value, self.root)

    def _add(self, key, value, node):
        if key > node.key:
            if node.right is not None:
                self._add(key, value, node.right)
            else:
                node.right = Node(key, value)
        else:
            if node.left is not None:
                self._add(key, value, node.left)
            else:
                node.left = Node(key, value)

    #
    # def print_to(self, stream):
    #     if self.root is not None:
    #         self._print_to(self.root, None, stream)
    #
    # def _print_to(self, node, parent_key, stream):
    #     print(node.to_str(parent_key), file=stream)
    #     self._print_to(node.left, parent_key)

    def print_to(self, stream):
        def visit_callback(node): print(node, file=stream)

        levelorder_traversal(self.root, visit_callback)


if __name__ == '__main__':
    tree = SplayTree()
    tree.add(8, 10)
    tree.add(4, 14)
    tree.add(7, 15)
    tree.add(8, 11)
    tree.add(3, 13)
    tree.add(5, 16)
    tree.levelorder()
