fun main()->Integer{
    var p : Option[Integer] = 5
     if p != none {
        var x = p.str()
         p = 9
         var z = p.str()
    }
    var z = 8
    var y = p.str()
    return 0
}