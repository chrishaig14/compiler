struct Tree[t]{ 
    v: t;
    l: Option[Tree[t]];
    r: Option[Tree[t]];
}

fun sum(t : Tree[Integer]) -> Integer {
    var l = t.l?sum(it):0;
    var r = t.r?sum(it):0;
    return t.v + l + r;
}

fun main() -> Integer {
    var t = Tree[Integer]{l:Tree[Integer]{l:none, r:none, v:5}, r:Tree[Integer]{l:none, r:none, v:7}, v:9};
    var result = sum(t);
    print(str(result));
    return 0;
}

