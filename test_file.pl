fun double(x: Integer) -> Integer {
    return 2*x;
}

fun double(x: String) -> String {
    return "471";
}

fun main() -> Integer {
    print(join(":",map(map(["1","5","7","9"],double),str)));
    print(str(len("Hello!!!")));
    return 0;
}