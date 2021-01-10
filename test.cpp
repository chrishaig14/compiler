#include<vector>
#include<iostream>

int main(){
	std::vector<int> v;
	long total = 0;
	for(int i = 0; i < 1000000; i++){
		v.push_back(i);
	}
	for(int i = 0; i < 1000000; i++){
	std::cout << v[i] << std::endl;
	}
}
