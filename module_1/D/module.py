import re
import sys
from collections import defaultdict, deque


class SortedSet:
    def __init__(self):
        self.data = list()

    def add(self, value):
        if value not in self.data:
            self.data.append(value)
            self.data.sort()

    def __iter__(self):
        return iter(self.data)

    def __reversed__(self):
        return reversed(self.data)


class Graph:
    def __init__(self, directed=False):
        self.directed = directed
        self.data = defaultdict(SortedSet)

    def add(self, start, end):
        self.data[start].add(end)
        if not self.directed:
            self.data[end].add(start)

    def depth_graph_traversal(self, start_vertex_id):
        if start_vertex_id not in self.data:
            yield start_vertex_id
            return

        visited = set()
        visit_queue = [start_vertex_id]

        while visit_queue:
            current = visit_queue.pop()
            visited.add(current)
            yield current

            for neighbor in reversed(self.data[current]):
                if neighbor not in visited:
                    if neighbor in visit_queue:
                        visit_queue.remove(neighbor)
                    visit_queue.append(neighbor)

    def width_graph_traversal(self, start_vertex_id):
        if start_vertex_id not in self.data:
            yield start_vertex_id
            return

        visited = {start_vertex_id}
        visit_queue = deque()
        visit_queue.append(start_vertex_id)
        yield start_vertex_id

        while visit_queue:
            current = visit_queue.popleft()

            for neighbor in self.data[current]:
                if neighbor not in visited:
                    visit_queue.append(neighbor)
                    visited.add(neighbor)
                    yield neighbor


def main():
    first_line = input()
    match = re.match(r"^([du]) ([^ ]+) ([bd])$", first_line)

    is_directed = match.group(1) == 'd'
    gr = Graph(is_directed)

    start_vertex = match.group(2)
    search_type = match.group(3)
    searchers = {'b': gr.width_graph_traversal, 'd': gr.depth_graph_traversal}

    for line in sys.stdin:
        if line == '\n':
            break
        start, end = line.split()
        gr.add(start, end)

    print(*searchers[search_type](start_vertex), sep='\n')


if __name__ == '__main__':
    debug = False
    if not debug:
        main()
    else:
        sys.stdout = open("output.txt", 'w')
        with open("input.txt") as sys.stdin:
            main()
