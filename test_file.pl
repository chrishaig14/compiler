fun main() -> Integer {
    var l = ["Hello","Bye","World"];
    for w @ l {
        print(w);
    }
    var i = 0;
    while (i < 7) {
        print(str(i+100));
        i = i + 1;
    }
    if len(l) + 1 == 3 {
        print("The length is, in fact, 3!");
    }
    print(str(len(l)));
    for i @ range(0,5,100){
        print("THE NUMBER IS " + str(i));
    }
    return 0;
}