class Person {
    name: String;
    age: Integer;
    fun get_name()->String{
        return this.name + "foo";
    }
}

fun main()->Integer{
    var p = #Person{name: "Christian", age: 26};
    print(p.get_name());
    return 0;
}