class BinaryTree:
    def __init__(self, key=None, value=None, parent=None, left=None, right=None):
        self.key = key
        self.value = value
        self.parent = parent
        self.left = left
        self.right = right

    def __str__(self):
        if not self.parent:
            return '[{0} {1}]'.format(self.key, self.value)
        return '[{0} {1} {2}]'.format(self.key, self.value, self.parent.key)

    def insert(self, key, value):
        if self.empty():
            self.key = key
            self.value = value
            return True, self, None
        cur, prev = self._search(self, key)
        if cur:
            return False, cur, prev
        new = BinaryTree(key, value, parent=prev)
        if key < prev.key:
            prev.left = new
        else:
            prev.right = new
        return True, new, prev

    def empty(self):
        return self.key is None

    @staticmethod
    def _search(x: 'BinaryTree', key):
        prev = None
        while x and key != x.key:
            prev = x
            if key < x.key:
                x = x.left
            else:
                x = x.right
        return x, prev

    @staticmethod
    def _min(x: 'BinaryTree'):
        while x.left:
            x = x.left
        return x

    @staticmethod
    def _max(x: 'BinaryTree'):
        while x.right:
            x = x.right
        return x


class SplayTree(BinaryTree):
    def __init__(self, key=None, value=None, parent=None, left=None, right=None):
        super().__init__(key, value, parent, left, right)

    @staticmethod
    def rotate_right(p: 'BinaryTree'):
        x = p.left
        p.left = x.right
        if x.right:
            x.right.parent = p
        SplayTree.transplant(x, p)
        x.right = p
        x.right.parent = x

    @staticmethod
    def rotate_left(p: 'BinaryTree'):
        x = p.right
        p.right = x.left
        if x.left:
            x.left.parent = p
        SplayTree.transplant(x, p)
        x.left = p
        x.left.parent = x

    @staticmethod
    def transplant(x: 'BinaryTree', p: 'BinaryTree'):
        if not p.parent:
            x.parent = None
            return
        if p == p.parent.left:
            p.parent.left = x
        elif p == p.parent.right:
            p.parent.right = x

        if x:
            x.parent = p.parent

    @staticmethod
    def zig(x: 'BinaryTree'):
        if x.parent.left == x:
            SplayTree.rotate_right(x.parent)
        else:
            SplayTree.rotate_left(x.parent)

    @staticmethod
    def zig_zig_l(x: 'BinaryTree'):
        SplayTree.rotate_right(x.parent.parent)
        SplayTree.rotate_right(x.parent)

    @staticmethod
    def zig_zig_r(x: 'BinaryTree'):
        SplayTree.rotate_left(x.parent.parent)
        SplayTree.rotate_left(x.parent)

    @staticmethod
    def zig_zag_l(x: 'BinaryTree'):
        SplayTree.rotate_right(x.parent)
        SplayTree.rotate_left(x.parent)

    @staticmethod
    def zig_zag_r(x: 'BinaryTree'):
        SplayTree.rotate_left(x.parent)
        SplayTree.rotate_right(x.parent)

    @staticmethod
    def splay(x: 'BinaryTree'):
        while x.parent:
            p = x.parent
            if not x.parent.parent:
                if x == x.parent.left:
                    SplayTree.rotate_right(p)
                else:
                    SplayTree.rotate_left(p)
                return
            g = p.parent
            if x == p.left and p == g.left:
                SplayTree.zig_zig_l(x)
            elif x == p.right and p == g.right:
                SplayTree.zig_zig_r(x)
            elif x == p.left and p == g.right:
                SplayTree.zig_zag_l(x)
            elif x == p.right and p == g.left:
                SplayTree.zig_zag_r(x)

    def max(self):
        x = super()._max(self)
        self.splay(x)
        return x.key, x.value

    def min(self):
        x = super()._min(self)
        self.splay(x)
        return x.key, x.value

    def insert(self, key, value):
        inserted, cur, prev = super().insert(key, value)
        if inserted or cur is not None:
            self.splay(cur)
        return inserted, cur, prev


tree = SplayTree()
tree.insert(8, 10)
tree.insert(4, 14)
tree.insert(7, 15)
# tree.set(8, 11)
# tree.insert(3, 13)
# tree.insert(5, 16)
# tree.search(88)
# tree.search(7)
# tree.delete(5)
# tree.print_to(sys.stdout)
