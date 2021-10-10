import libcore

class Boolean:
    def __init__(self, value):
        self.value = value

    def __bool__(self):
        return self.value


class String:
    def __init__(self, value):
        self.value = value

    def len(self):
        return Integer(len(self.value))

    @staticmethod
    def __add__(a, b):
        return String(a.value + b.value)


class List:
    def __init__(self, elems):
        self.elems = elems

    def len(self):
        return Integer(len(self.elems))
    def push(self, x):
        self.elems.append(x)
    def __get_item__(self, i):
        return self.elems[i.value]

class Dict:
    def __init__(self, d):
        self.d = d
    def __get_item__(self, i):
        return self.d[i]
    def __set_item__(self, k, v):
        self.d[k] = v

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
    def __sub__(a, b):
        return Integer(a.value - b.value)

    def str(o):
        return String(str(o.value))

    def __hash__(self):
        return self.value


_print = print
print = lambda s: _print(s.value)
