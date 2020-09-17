fun foo(i: Integer)->String{
    return "This is an Integer";
}

fun foo(s: String) -> String {
    return s;
}

fun main()->Integer{
    print(foo(7));
    print(foo("Hello world!"));
    print(foo(false));
    return 0;
}