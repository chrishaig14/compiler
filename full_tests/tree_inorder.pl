struct Tree[a]{
    value: a;
    left: Option[Tree[a]];
    right: Option[Tree[a]];
}

fun inorder(t: Tree[a])->List[a]{
    return (t.left?inorder(it):[]::List[a]) + [t.value] + (t.right?inorder(it):[]::List[a]);
}

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

fun main()->Integer{
    var t = #Tree[Integer]{value:3,left:#Tree[Integer]{value:7,left:#Tree[Integer]{value:4,left:none,right:none},right:none}, right:#Tree[Integer]{value:9,left:none,right:none}};
    print(join(map(inorder(t),4,str),4,","));
    return 0;
}