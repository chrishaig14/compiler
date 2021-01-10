class Structure[t]{
    value: t;
    l: List[t];
    fun get_first_of_list()->t {
        return this.l[0];
    }
}

fun main()->Integer{
    var something : Something[Integer] = #Something[Integer]{value: 7, l: [1,2,3]};
    print(something.l[0].str());
    return something.l[0];
}