class Person {
    name: String;
    age: Integer;
}

fun return_person_name(p: Person) -> String {
    return p.name;
}

fun main() -> Integer {
    var p = #Person{name: "John", age: 7};
    return 0;
}