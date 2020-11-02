fun factorial(n: Integer) -> Integer {
    var prev_factorial = 1
    var i_factorial = 1
    for i @ range(1, 1, n+1) {
        prev_factorial = i_factorial
        i_factorial = i_factorial*i
    }
    return i_factorial
}

fun print_hello(){
    print("Hello world")
}

fun foo()->Integer{
    print_hello()
    return 8
}

fun find(x: Integer, l: List[Integer])->Integer{
    for i @ range(0,1,l.len()){
        if l[i] == x {
            return i
        }
    }
    return 0-1
}

fun main()->Integer{
    print(factorial(5).str())
    return 0
}