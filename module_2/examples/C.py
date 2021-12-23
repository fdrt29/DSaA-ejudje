import sys
import math

class MinBinHeap:
    class BinHeapNode:
        def __init__(self, key, data):
            self.key = key
            self.data = data

    def __init__(self):
        self.keys = dict()
        self.heap = []

    def sift_down(self, i):
        while 2 * i + 1 < len(self.heap):
            l = 2 * i + 1
            r = 2 * i + 2
            s = l
            if r < len(self.heap) and self.heap[r].key < self.heap[l].key:
                s = r
            if self.heap[i].key < self.heap[s].key:
                break
            self.swap(i, s)
            i = s

    def sift_up(self, i):
        while i >= 1 and self.heap[i].key < self.heap[(i - 1) // 2].key:
            self.swap(i, (i - 1) // 2)
            i = (i - 1) // 2

    def swap(self, i1, i2):
        key1 = self.heap[i1].key
        key2 = self.heap[i2].key
        tmp = self.keys[key1]
        self.keys[key1] = self.keys[key2]
        self.keys[key2] = tmp
        tmp = self.heap[i1]
        self.heap[i1] = self.heap[i2]
        self.heap[i2] = tmp

    def add(self, key, data):
        if key in self.keys:
            return False
        self.keys[key] = len(self.heap)
        self.heap.append(self.BinHeapNode(key, data))
        self.sift_up(len(self.heap) - 1)
        return True

    def set(self, key, data):
        if key not in self.keys:
            return False
        self.heap[ self.keys[key] ].data = data
        return True
    
    def delete(self, key):
        if key not in self.keys:
            return False
        index = self.keys[key]
        self.swap(index, len(self.heap) - 1)
        self.keys.pop(key)
        self.heap.pop()
        if index < len(self.heap):
            self.sift_down(index)
            self.sift_up(index)
        return True

    def search(self, key):
        if key not in self.keys:
            return None
        return self.keys[key], self.heap[ self.keys[key] ].data
    
    def min(self):
        if not len(self.heap):
            return None
        return self.heap[0].key, 0, self.heap[0].data

    def max(self):
        if not len(self.heap):
            return None
        imax = 0
        for index in range(len(self.heap)):
            if self.heap[index].key > self.heap[imax].key:
                imax = index
        return (self.heap[imax].key, imax, self.heap[imax].data)

    def extract(self):
        if not len(self.heap):
            return None
        self.swap(0, len(self.heap) - 1)
        n = self.heap.pop()
        self.keys.pop(n.key)
        self.sift_down(0)
        return n.key, n.data

    def print(self, cout):
        if not len(self.heap):
            cout.write("_\n")
            return
        cout.write(f"[{self.heap[0].key} {self.heap[0].data}]\n")
        if len(self.heap) == 1:
            return
        lvl = int(math.log2(len(self.heap))) + 1
        l = 1
        r = 2
        buffer = []
        for _ in range(1, lvl-1):
            buffer.clear()
            for i in range(l, r + 1):
                k = self.heap[i].key
                d = self.heap[i].data
                kp = self.heap[(i - 1) // 2].key
                buffer.append(f"[{k} {d} {kp}]")
            cout.write(" ".join(buffer))
            cout.write("\n")
            l = 2 * l + 1
            r = 2 * r + 2
        buffer.clear()
        for i in range(l, len(self.heap)):
            k = self.heap[i].key
            d = self.heap[i].data
            kp = self.heap[(i - 1) // 2].key
            buffer.append(f"[{k} {d} {kp}]")
        if len(self.heap) <= r:
            empty = r - len(self.heap) + 1
            buffer.append(" ".join(("_" for _ in range(empty))))
        cout.write(" ".join(buffer))
        cout.write("\n")

def getCommand(inputBuffer: str):
    return inputBuffer[:inputBuffer.find(" ")]

def getArguments(inputBuffer: str):
    i1 = inputBuffer.find(" ")
    i2 = inputBuffer.find(" ", i1+1)
    return (inputBuffer[i1+1:i2], inputBuffer[i2+1:].strip())

cin = sys.stdin
cout = sys.stdout

heap = MinBinHeap()
errorMessage = "error"

for inputBuffer in cin:
    command = getCommand(inputBuffer)
    argv = getArguments(inputBuffer)

    if command == "add":
        if not heap.add(int(argv[0]), argv[1]):
            cout.write(f"{errorMessage}\n")
    elif command == "set":
        if not heap.set(int(argv[0]), argv[1]):
            cout.write(f"{errorMessage}\n")
    elif command == "delete":
        if not heap.delete(int(argv[0])):
            cout.write(f"{errorMessage}\n")
    elif command == "search":
        data = heap.search(int(argv[0]))
        cout.write(f"1 {data[0]} {data[1]}\n" if data else "0\n")
    elif command == "min":
        data = heap.min()
        cout.write(f"{data[0]} {data[1]} {data[2]}\n" if data else f"{errorMessage}\n")
    elif command == "max":
        data = heap.max()
        cout.write(f"{data[0]} {data[1]} {data[2]}\n" if data else f"{errorMessage}\n")
    elif command == "extract":
        data = heap.extract()
        cout.write(f"{data[0]} {data[1]}\n" if data else f"{errorMessage}\n")
    elif command == "print":
        heap.print(cout)