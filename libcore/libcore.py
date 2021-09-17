class Boolean:
    def __init__(self, value):
        self.value = value

class String:
    def __init__(self, value):
        self.value = value
class Integer:
    def __init__(self, value):
        self.value = value
    @staticmethod
    def __gt__(a, b):
        return a.value > b.value
    @staticmethod
    def __lt__(a, b):
        return a.value < b.value
    @staticmethod
    def str(o):
        return String(str(o))
_print = print
print = lambda s: _print(s.value)