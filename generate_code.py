import stringcase
import sys

template = 'std::string Transpiler::visit_{}({}&node){{}}'

for n in sys.stdin:
	n = n.rstrip()
	if n == "Node":
		continue
	snake = stringcase.snakecase(n)[:-5]
	camel = n
	print(template.format(snake, camel))
