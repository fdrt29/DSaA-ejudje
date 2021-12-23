import re
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
            return False, cur
        new.parent = prev
        if prev is None:
            self.root = new
        elif new.key < prev.key:
            prev.left = new
        else:
            prev.right = new
        return True, new

    @staticmethod
    def _min(x: Node):
        while x.left:
            x = x.left
        return x

    @staticmethod
    def _max(x: Node):
        while x.right:
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
        while x.parent is not None:
            p = x.parent
            if p.parent is None:
                if x == x.parent.left:
                    self.rotate_right(p)
                else:
                    self.rotate_left(p)
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

    def _splay_search(self, key):
        (x, prev) = super()._search(key)
        if x is not None:
            self.splay(x)
        else:
            self.splay(prev)
        return x

    def search(self, key):
        x = self._splay_search(key)
        return x.value if x else None

    def _insert(self, new: BinaryTree.Node):
        success, inserted = super()._insert(new)
        if success or inserted is not None:
            self.splay(inserted)
        return success, inserted

    def insert(self, key, value):
        inserted, _ = self._insert(BinaryTree.Node(key, value))
        return inserted

    # def join(self, t: BinaryTree):
    #     if t is None:
    #         return self
    #     x = super()._max(self.root)
    #     self.splay(x)
    #     self.root.right = t.root
    #     t.root.parent = self.root
    #     t.root = None
    #
    # def split(self, x: BinaryTree.Node):
    #     self.splay(x)
    #     if x.right:
    #         tree2 = x.right
    #         tree2.parent = None
    #     else:
    #         tree2 = None
    #     tree1 = x
    #     tree1.right = None
    #     return tree1, tree2

    # TODO check
    def delete(self, key):
        if self.root is None:
            return False
        x = self._splay_search(key)
        if x is None:
            return False
        if x.left is None:
            self.root = x.right
            if self.root is not None:
                self.root.parent = None
        elif x.right is None:
            self.root = x.left
            self.root.parent = None
        else:
            self.root = super()._max(x.left)
            self.splay(self.root)
            self.root.right = x.right
            x.right.parent = self.root
        return True

    def set(self, key, value):
        if self.root is None:
            return False
        x = self._splay_search(key)
        if x is not None:
            x.value = value
            return True
        return False

    def print_to(self, stream):
        def visit_callback(node: BinaryTree.Node, level: int, is_next_level):
            st = ''
            if level != 0:
                if is_next_level:
                    st = '\n'
                else:
                    st = ' '

            if node is not None:
                st += str(node)
            else:
                st += "_"
            print(st, file=stream, end='')
        levelorder_traversal(self.root, visit_callback)


def levelorder_traversal(root, visit_callback):
    queue = deque()
    queue.append(root)

    level = 0
    is_next_level = True
    while queue:
        n = len(queue)  # There is only one whole level in queue now
        if not [x for x in queue if x is not None]:
            return
        for i in range(0, n):
            node = queue.popleft()
            visit_callback(node, level, is_next_level)
            if node:
                queue.append(node.left)
                queue.append(node.right)
            else:
                queue.append(None)
                queue.append(None)
            is_next_level = False
        level += 1
        is_next_level = True


def main():
    tree = SplayTree()

    add_pattern = re.compile(r"^add\s([+]?[1-9]\d*)\s([^ ]+)\n")  # 1st group - int, 2nd - non-space chars
    set_pattern = re.compile(r"^set\s([+]?[1-9]\d*)\s([^ ]+)\n")
    delete_pattern = re.compile(r"^delete\s([+]?[1-9]\d*)\n")
    search_pattern = re.compile(r"^search\s([+]?[1-9]\d*)\n")

    for line in sys.stdin:
        if line == '\n' or line == '':
            continue
        match = add_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not tree.insert(key, match.group(2)):
                print("error")
            continue

        match = set_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not tree.set(key, match.group(2)):
                print("error")
            continue

        match = delete_pattern.match(line)
        if match:
            key = int(match.group(1))
            if not tree.delete(key):
                print("error")
            continue

        match = search_pattern.match(line)
        if match:
            key = int(match.group(1))
            x = tree.search(key)
            st = '1 ' + x if x else '0'
            print(st)
            continue

        if line == 'min\n':
            x = tree.min()
            if x:
                print(x[0], x[1])
            else:
                print("error")
        elif line == 'max\n':
            x = tree.max()
            if x:
                print(x[0], x[1])
            else:
                print("error")
        elif line == "print\n":
            tree.print_to(sys.stdout)


if __name__ == '__main__':
    main()
