fun map(l: List[a], n:Integer, f: fun(a)->b)->List[b]{
    var i = 0;
    var r = []::List[b];
    while(i<n){
        r = r + [f(l[i])];
        i = i+1;
    }
    return r;
}

fun join(l: List[String], n:Integer, c: String)->String{
    var i = 0;
    var r = "";
    while(i<n-1){
        r = r + l[i] + c;
        i = i+1;
    }
    r = r + l[i];
    return r;
}


fun double(i: Integer)->Integer{
    return 2*i;
}

fun main()->Integer{
    var l = map(map(["1","2","3","4","5"], 5, double),5,str);
    print(join(l,5,","));
    return 0;
}