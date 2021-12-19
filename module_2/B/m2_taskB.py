import sys
from collections import deque


class BinaryTree:
    # ------- Node { ------------------------------------------------------
    class Node:
        def __init__(self, key, value, parent=None, left=None, right=None):
            self.key = key
            self.value = value
            self.parent = parent
            self.left = left
            self.right = right

        def __str__(self):
            if self.parent is None:
                return '[{0} {1}]'.format(self.key, self.value)
            return '[{0} {1} {2}]'.format(self.key, self.value, self.parent.key)

    # ------- } Node ------------------------------------------------------
    def __init__(self):
        self.root = None

    def _search(self, key):
        cur = self.root
        prev = None
        while cur and key != cur.key:
            # if key == cur.key:
            #     break
            prev = cur
            if key < cur.key:
                cur = cur.left
            else:
                cur = cur.right
        return cur, prev

    def _insert(self, new: Node):
        cur, prev = self._search(new.key)
        if cur is not None:
            return False, cur, prev
        new.parent = prev
        if prev is None:
            self.root = new
            cur = self.root
        elif new.key < prev.key:
            prev.left = new
            cur = new
        else:
            prev.right = new
            cur = new
        return True, cur, prev

    @staticmethod
    def _min(x: Node):
        while not x.left:
            x = x.left
        return x

    @staticmethod
    def _max(x: Node):
        while not x.right:
            x = x.right
        return x


# ------- } BinaryTree ----------------------------------------------------
class SplayTree(BinaryTree):
    def __init__(self):
        super().__init__()

    def rotate_right(self, p: BinaryTree.Node):
        x = p.left
        p.left = x.right
        if x.right is not None:
            x.right.parent = p

        self.setup_parent(x, p)

        x.right = p
        x.right.parent = x

    def rotate_left(self, p: BinaryTree.Node):
        x = p.right
        p.right = x.left
        if x.left is not None:
            x.left.parent = p
        self.setup_parent(x, p)
        x.left = p
        x.left.parent = x

    def setup_parent(self, x: BinaryTree.Node, p: BinaryTree.Node):
        if p.parent is None:
            self.root = x
        elif p == p.parent.left:
            p.parent.left = x
        elif p == p.parent.right:
            p.parent.right = x

        if x is not None:
            x.parent = p.parent

    def zig(self, x: BinaryTree.Node):
        if x.parent.left == x:
            self.rotate_right(x.parent)
        else:
            self.rotate_left(x.parent)

    def zig_zig_l(self, x: BinaryTree.Node):
        self.rotate_right(x.parent.parent)
        self.rotate_right(x.parent)

    def zig_zig_r(self, x: BinaryTree.Node):
        self.rotate_left(x.parent.parent)
        self.rotate_left(x.parent)

    def zig_zag_l(self, x: BinaryTree.Node):
        self.rotate_right(x.parent)
        self.rotate_left(x.parent)

    def zig_zag_r(self, x: BinaryTree.Node):
        self.rotate_left(x.parent)
        self.rotate_right(x.parent)

    def splay(self, x: BinaryTree.Node):
        while x != self.root:
            p = x.parent
            if p == self.root:
                self.zig(x)
                continue
            g = p.parent
            if x == p.left and p == g.left:
                self.zig_zig_l(x)
            elif x == p.right and p == g.right:
                self.zig_zig_r(x)
            elif x == p.left and p == g.right:
                self.zig_zag_l(x)
            elif x == p.right and p == g.left:
                self.zig_zag_r(x)

    def max(self):
        if self.root is None:
            return None
        x = super()._max(self.root)
        self.splay(x)
        return x.key, x.value

    def min(self):
        if self.root is None:
            return None
        x = super()._min(self.root)
        self.splay(x)
        return x.key, x.value

    def search(self, key):
        (x, prev) = super()._search(key)
        if x is not None:
            self.splay(x)
        else:
            self.splay(prev)
        return x, prev

    def _insert(self, new: BinaryTree.Node):
        inserted, cur, prev = super()._insert(new)
        if inserted or cur is not None:
            self.splay(cur)
        return inserted, cur, prev

    def insert(self, key, value):
        inserted, _, _ = self._insert(BinaryTree.Node(key, value))
        return inserted

    # TODO check
    def delete(self, key):
        if self.root is None:
            return False
        (x, prev) = self.search(key)
        if x is None:
            return False
        if x.left is None:
            self.root = x.right
            if self.root is not None:
                self.root.parent = None
        elif x.right is None:
            self.root = x.left
            self.root.parent = x.left
        else:
            self.root = super()._max(x.left)
            self.splay(self.root)
            self.root.right = x.right
            x.right.parent = self.root
        return True

    def set(self, key, value):
        if self.root is None:
            return False
        x, prev = self.search(key)
        if x is not None:
            x.value = value
            return True
        return False

    def print_to(self, stream):
        def visit_callback(node: BinaryTree.Node, level: int, is_next_level):
            if level != 0:
                if is_next_level:
                    print(file=stream)
                    out << std::endl
                else:
                    out << " "

            if node is not None:
                out << *node
            else:
                out << "_"
            # print(str(node), file=stream)



        # TODO проверить на лишний перенос строки
        levelorder_traversal(self.root, visit_callback)

    # TODO delete
    #
    # def print_to(self, stream):
    #     if self.root is not None:
    #         self._print_to(self.root, None, stream)
    #
    # def _print_to(self, node, parent_key, stream):
    #     print(node.to_str(parent_key), file=stream)
    #     self._print_to(node.left, parent_key)


def levelorder_traversal(root, visit_callback):
    queue = deque()
    queue.append(root)

    level = 0
    is_next_level = True
    while queue:
        n = len(queue)  # There is only one whole level in queue now
        for i in range(0, n):
            node = queue.pop()
            visit_callback(node, level, is_next_level)
            if node:
                queue.append(node.left)  # Also if == nullptr
                queue.append(node.right)
            is_next_level = False
        level += 1
        is_next_level = True

    # while q:
    #     node = q.popleft()
    #     visit_callback(node)
    #     if node.left:
    #         q.append(node.left)
    #     if node.right:
    #         q.append(node.right)


if __name__ == '__main__':
    tree = SplayTree()
    tree.insert(8, 10)
    tree.insert(4, 14)
    tree.insert(7, 15)
    tree.set(8, 11)
    tree.insert(3, 13)
    tree.insert(5, 16)
    tree.search(88)
    tree.search(7)
    tree.delete(5)
    tree.print_to(sys.stdout)
