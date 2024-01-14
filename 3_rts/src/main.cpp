#include "TopA.h"
// #include <unordered_map>
#include "TypeDef.h"

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

	// object manager

	const TopA* p = childA.release();
	EsTopAObjectManager es_obj_manager(std::string("ChildA"),&p);

	es_obj_manager.toc();

	// const TopA* p2 = (*(es_obj_manager.get(p->objName())));
	std::cout << p->objName() << " " << p << std::endl;
	// std::cout << p2->objName() << " " << p2 << std::endl;

    return 0;
}
