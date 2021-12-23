from collections import deque
import sys


class SplayTree:
    class SplayTreeNode:
        def __init__(self, key, data, parent=None):
            self.key = key
            self.data = data
            self.parent = parent
            self.left = None
            self.right = None

        def __str__(self):
            if self.parent != None:
                return f"[{self.key} {self.data} {self.parent.key}]"
            return f"[{self.key} {self.data}]"

    def __init__(self):
        self.root = None

    def left_rotate(self, x):
        y = x.right
        x.right = y.left
        if y.left != None:
            y.left.parent = x
        y.parent = x.parent
        if x.parent == None:
            self.root = y
        elif x == x.parent.left:
            x.parent.left = y
        else:
            x.parent.right = y
        y.left = x
        x.parent = y

    def right_rotate(self, x):
        y = x.left
        x.left = y.right
        if y.right != None:
            y.right.parent = x
        y.parent = x.parent
        if x.parent == None:
            self.root = y
        elif x == x.parent.right:
            x.parent.right = y
        else:
            x.parent.left = y
        y.right = x
        x.parent = y

    def splay(self, n):
        while n.parent != None:
            if n.parent.parent == None:
                if n == n.parent.left:
                    self.right_rotate(n.parent)
                else:
                    self.left_rotate(n.parent)
                return
            p = n.parent
            g = n.parent.parent
            if n == p.left and p == g.left:
                self.right_rotate(g)
                self.right_rotate(p)
            elif n == p.right and p == g.right:
                self.left_rotate(g)
                self.left_rotate(p)
            elif n == p.right and p == g.left:
                self.left_rotate(p)
                self.right_rotate(g)
            else:
                self.right_rotate(p)
                self.left_rotate(g)

    def minimum(self, x):
        while x.left != None:
            x = x.left
        return x

    def maximum(self, x):
        while x.right != None:
            x = x.right
        return x

    def upper_bound(self, key):
        n = self.root
        while True:
            if key < n.key:
                if n.left == None:
                    return n
                n = n.left
            elif key > n.key:
                if n.right == None:
                    return n
                n = n.right
            else:
                return n

    def add(self, key, data):
        if self.root == None:
            self.root = self.SplayTreeNode(key, data)
            return True
        p = self.upper_bound(key)
        if p.key == key:
            self.splay(p)
            return False
        else:
            n = self.SplayTreeNode(key, data, p)
            if n.key < p.key:
                p.left = n
            else:
                p.right = n
            self.splay(n)
        return True

    def set(self, key, data):
        if self.root == None:
            return False
        n = self.upper_bound(key)
        self.splay(n)
        if n.key == key:
            n.data = data
            return True
        else:
            return False

    def delete(self, key):
        if self.root == None:
            return False
        n = self.upper_bound(key)
        self.splay(n)
        if n.key != key:
            return False
        if n.left == None:
            self.root = n.right
            if self.root != None:
                self.root.parent = None
        elif n.right == None:
            self.root = n.left
            self.root.parent = None
        else:
            self.root = self.maximum(n.left)
            self.splay(self.root)
            self.root.right = n.right
            n.right.parent = self.root
        return True

    def search(self, key):
        if self.root == None:
            return None
        n = self.upper_bound(key)
        self.splay(n)
        if key == n.key:
            return n.data
        return None

    def max(self):
        if self.root == None:
            return None
        n = self.maximum(self.root)
        self.splay(n)
        return n.key, n.data

    def min(self):
        if self.root == None:
            return None
        n = self.minimum(self.root)
        self.splay(n)
        return n.key, n.data

    def print(self, cout):
        if self.root == None:
            cout.write("_\n")
            return
        nodesOnLevel = 0
        maxNodesOnLevel = 1
        notNone = 1
        queue = deque([self.root])
        levelNodes = []
        while len(queue) > 0:
            n = queue.popleft()
            if isinstance(n, str):
                nodesOnLevel += len(n) // 2 + 1
                levelNodes.append(n)
                queue.append(n + " " + n)
            else:
                notNone -= 1
                nodesOnLevel += 1
                levelNodes.append(str(n))
                if n.left != None:
                    queue.append(n.left)
                    notNone += 1
                else:
                    queue.append("_")
                if n.right != None:
                    queue.append(n.right)
                    notNone += 1
                else:
                    queue.append("_")
            if nodesOnLevel == maxNodesOnLevel:
                cout.write(" ".join(levelNodes))
                cout.write("\n")
                levelNodes.clear()
                if notNone == 0:
                    break
                nodesOnLevel = 0
                maxNodesOnLevel *= 2


def getCommand(inputBuffer: str):
    return inputBuffer[:inputBuffer.find(" ")]


def getArguments(inputBuffer: str):
    i1 = inputBuffer.find(" ")
    i2 = inputBuffer.find(" ", i1 + 1)
    return (inputBuffer[i1 + 1:i2], inputBuffer[i2 + 1:].strip())


def main():
    cin = sys.stdin
    cout = sys.stdout

    splaytree = SplayTree()
    errorMessage = "error"

    for inputBuffer in cin:
        command = getCommand(inputBuffer)
        argv = getArguments(inputBuffer)

        if command == "add":
            if not splaytree.add(int(argv[0]), argv[1]):
                cout.write(f"{errorMessage}\n")
        elif command == "set":
            if not splaytree.set(int(argv[0]), argv[1]):
                cout.write(f"{errorMessage}\n")
        elif command == "delete":
            if not splaytree.delete(int(argv[0])):
                cout.write(f"{errorMessage}\n")
        elif command == "search":
            data = splaytree.search(int(argv[0]))
            cout.write(f"1 {data}\n" if data else "0\n")
        elif command == "max":
            result = splaytree.max()
            cout.write(f"{result[0]} {result[1]}\n" if result else f"{errorMessage}\n")
        elif command == "min":
            result = splaytree.min()
            cout.write(f"{result[0]} {result[1]}\n" if result else f"{errorMessage}\n")
        elif command == "print":
            splaytree.print(cout)


if __name__ == '__main__':
    main()
