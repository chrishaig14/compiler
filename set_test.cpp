#include <iostream>
#include <unordered_set>
int main(){
	std::unordered_set<int> v;
	for(int i = 0; i < 100000; i++){
		v.insert(i);
	}
	for(auto o: v){
		std::cout << o << std::endl;
	}
	
}
