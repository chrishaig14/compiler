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


_print = print
print = lambda s: _print(s.value)
