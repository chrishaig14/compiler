fun call(x: a, f: fun(a)->b)->b{
    return f(x);
}

fun foo(i: Integer)->String{
    return "A number";
}

fun foo(s: String)->String {
    return "A string";
}

fun main()->Integer{
    print(call(5, foo));
    return 0;
}