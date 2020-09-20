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

fun double(i: String) -> Integer {
    return 7;
}

fun main()->Integer{
    var r = map([1,2,3,4], 4, double);
    print(str(r));
    return 0;
}

fun bar(i:Integer)->String{}
fun buzz(i:Integer)->String{}

fun foo(f: fun(a)->b, g: fun(a)->b) -> List[fun(a)->b]{}

foo(bar, buzz) // ok they match!

fun buzz(i:Integer)->String // buzz.0
fun buzz(i:Integer)->Integer // buzz.1

foo(bar, buzz) // ok, it can decide to use buzz.1

fun bar(i:Integer)->Integer // bar.1

foo(bar, buzz) // don't know whether to call bar.0/buzz.0 or bar.1/buzz.1!