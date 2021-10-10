import libcore

class Boolean:
    def __init__(self, value):
        self.value = value

    def __bool__(self):
        return self.value
    def __and__(self, other):
        return Boolean(self.value and other.value)
    def str(self):
        return String(str(self.value))


class String:
    def __init__(self, value):
        self.value = value

    def len(self):
        return Integer(len(self.value))

    def list(self):
        return List([String(c) for c in self.value])

    @staticmethod
    def __add__(a, b):
        return String(a.value + b.value)

    def __hash__(self):
        return self.value.__hash__()

    def __eq__(self, o):
        return self.value == o.value


class List:
    def __init__(self, elems):
        self.elems = elems

    def len(self):
        return Integer(len(self.elems))
    def push(self, x):
        self.elems.append(x)
    def __get_item__(self, i):
        return self.elems[i.value]
    def slice(self, s, e):
        return List(self.elems[s.value:e.value])

class Dict:
    def __init__(self, d):
        self.d = d
    def __get_item__(self, i):
        return self.d[i]
    def __set_item__(self, k, v):
        self.d[k] = v
    def has(self, k):
        return Boolean(k in self.d)
    def keys(self):
        return List(list(self.d.keys()))

class Double:
    pass

class Float:
    pass

class Option:
    pass

class Integer:
    def __init__(self, value):
        self.value = value

    @staticmethod
    def __gt__(a, b):
        return Boolean(a.value > b.value)

    @staticmethod
    def __lt__(a, b):
        return Boolean(a.value < b.value)

    @staticmethod
    def eq(a, b):
        return Boolean(a.value == b.value)

    def __eq__(self, o):
        return Integer.eq(self, o)

    @staticmethod
    def __ne__(a, b):
        return Boolean(a.value != b.value)

    @staticmethod
    def __ge__(a, b):
        return Boolean(a.value >= b.value)

    @staticmethod
    def __le__(a, b):
        return Boolean(a.value <= b.value)

    @staticmethod
    def __add__(a, b):
        return Integer(a.value + b.value)

    @staticmethod
    def __div__(a, b):
        return Integer(a.value // b.value)

    @staticmethod
    def __sub__(a, b):
        return Integer(a.value - b.value)

    def str(o):
        return String(str(o.value))

    def __hash__(self):
        return self.value


_print = print
print = lambda s: _print(s.value)
_input = input
input = lambda :String(_input())
