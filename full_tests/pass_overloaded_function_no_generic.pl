fun call(x: Integer, f: fun(Integer)->String)->String{
    return f(x);
}

fun foo(i: Integer)->String{
    return "A number";
}

fun foo(i: String)->String{
    return "A string";
}

fun main()->Integer{
    print(call(5, foo));
    return 0;
}