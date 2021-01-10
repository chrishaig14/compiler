fun sort(l: List[Integer]) -> List[Integer]{
    if l.len() == 0 {
        return l
    }
    var pivot = l[0]
    var smaller = []::List[Integer]
    var greater = []::List[Integer]
    var equal = [pivot]
    for i @ range(1,1,l.len()) {
        if l[i] < pivot {
            smaller = smaller + [l[i]]
        } else {
            if l[i] > pivot {
                greater = greater + [l[i]]
            } else {
                equal = equal + [pivot]
            }
        }
    }
    return sort(smaller) + equal + sort(greater)
}
