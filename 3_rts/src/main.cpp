#include "TopA.h"
// #include <unordered_map>

int main(int argc, char *argv[])
{
	//- create obj1
    TopA hi("TopA","hi","for construction demo");
	//- create obj2 of topA's child class
	std::unique_ptr<TopA> childA = TopA::New("ChildA","objA","no reason");

	std::unordered_map<std::string, std::string> lut = {};
	lut["a"] = "aa";
	lut["b"] = "bb";
	lut.insert(std::make_pair<std::string,std::string>("c","cc"));
	for (auto const& i : lut) {
		std::cout << i.first << " " << i.second << std::endl;
    }
	lut.clear();

    return 0;
}
