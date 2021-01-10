class Thing[t]{
    value: t;
}

fun main()->Integer{
    var thing = #Thing[Integer]{value: 7};
    return thing.value;
}