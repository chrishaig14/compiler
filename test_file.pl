fun main()->Integer{
    var y : Option[Integer] = 7
    var y_val = y? it: 0;
    if y == none {

                print("y is equal to " + y_val.str())
    }
    else {
         var x = "Hello"
         x = y_val.str()
         print("y is none")
    }
    return 0
}