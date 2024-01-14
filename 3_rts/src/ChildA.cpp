#include <iostream>

#include "TopA.h"
#include "ChildA.h"

ChildA::ChildA(const std::string & classTypeName,const std::string & objName, const std::string &objMemo)
	: 
	TopA(classTypeName,objName,objMemo),
	m_objName(objName),
	m_objMemo(objMemo)
{
	this->print();
}

void ChildA::print()
{
    std::cout <<__FILE__<<":"<<__LINE__<<":"<< this->typeName() << " Constructor()!" << " objName: " << this->objName() << std::endl;
}

makeElement(YourAvatar, ChildA, TopA)

