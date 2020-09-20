fun join(l: List[String], n: Integer, c: String) -> String {
    var i = 0;
    var r = "";
    while(i<n - 1){
        r = r + l[i] + c;
        i = i + 1;
    }
    r = r + l[i];
    return r;
}

fun main()->Integer{
    var r = join(["1","2","3","4"],4,", ");
    print(r);
    return 0;
}