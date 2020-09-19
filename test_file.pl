fun apply(i: Integer, f: fun(Integer)->Integer)->Integer {
    return f(i);
}

fun double(i: Integer)->Integer{
    return 2*i;
}

fun main()->Integer{
    var r = apply(7, double);
    print(str(r));
    return 0;
}