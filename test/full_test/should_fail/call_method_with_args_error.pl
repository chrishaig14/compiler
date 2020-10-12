class Person {
    name: String;
    age: Integer;
    fun foo(x: Integer)->String{
        return "Person";
    }
}

fun main()->Integer{
    var p = #Person{name: "Christian", age: 26};
    print(p.foo(54,"Hello"));
    return 0;
}