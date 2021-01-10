fun map(l: List[a], n: Integer, f: fun(a)->b) -> List[b] {
    var i = 0;
    var r = []::List[b];
    while(i<n){
        r = r + [f(l[i])];
        i = i + 1;
    }
    return r;
}

fun double(i: Integer) -> Integer {
    return 2*i;
}

fun main()->Integer{
    var r = map([1,2,3,4], 4, double);
    print(str(r));
    return 0;
}