class Something[t]{
    value: t;
    l: List[t];
}

fun main()->Integer{
    var something : Something[Integer] = #Something[Integer]{value: 7, l: [1,2,3]};
    print(something.l[0].str());
    return something.l[0];
}