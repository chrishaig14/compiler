fun range(a: Integer, s: Integer, b: Integer) -> List[Integer] {
    var r = []::List[Integer];
    while(a < b){
        r = r + [a];
        a = a + s;
    }
    return r;
}

fun main() -> Integer {
    for (i @ range (0,5,100)) {
        print(str(i));
    }
    print(str(length(range(0, 5, 100))));
    return 0;
}