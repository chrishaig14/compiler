fun imap(l: List[t], f: fun(t)->b) -> List[b] {
    return [f(l[0])];
}

fun foo(x: String) -> String {
    return "Hello";
}

fun main()->Integer{
    var f : List[Integer] = imap([5,4,6],foo);
    print(f[0]);
    return 0;
}