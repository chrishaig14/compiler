class Tree[t] {
    value: t
    left: Option[Tree[t]]
    right: Option[Tree[t]]
    fun inorder()->List[t]{
        var l = this.left ? it.inorder() : []::List[t]
        var r = this.right ? it.inorder() : []::List[t]
        return l + [this.value] + r
    }
    fun foo()->Tree[t]{
        return this
    }
}

fun main()->Integer{
    var t1 = #Tree[Integer]{value:1,left:none,right:none}
    var t3 = #Tree[Integer]{value:3,left:none,right:none}
    var t5 = #Tree[Integer]{value:5,left:none,right:none}
    var t8 = #Tree[Integer]{value:8,left:none,right:none}
    var t10 = #Tree[Integer]{value:10,left:none,right:none}

    var t9 = #Tree[Integer]{value:9,left:t8,right:t10}

    var t7 = #Tree[Integer]{7,none,"Hello"}

    var t2 = #Tree[Integer]{value:2,left:t1,right:t3}
    var t6 = #Tree[Integer]{value:6,left:t5,right:t7}

    var t4 = #Tree[Integer]{value:4,left:t2,right:t6}
    print(join(t4.inorder().map(Integer.str), " , "))
    return 0
}