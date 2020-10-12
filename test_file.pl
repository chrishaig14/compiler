fun main()->Integer{
    for e @ [3,1,4,1,5,9,2,6,5] {
        print(e.str());
    }
    print("The length of the list is " + [3,1,4,1,5,9,2,6,5].len().str());
    print("And the length of 'Hello world' is " + "Hello world".len().str());
    print("The third element of the list is " + [3,1,4,1,5,9,2,6,5][2].str());
    return 0;
}