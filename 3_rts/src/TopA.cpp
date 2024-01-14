#include <iostream>

#include "TopA.h"

// std::string TopA::m_typeName = std::string("TopA");
defineTypeName(TopA)

TopA::TopA(const std::string& classTypeName, const std::string & objName, const std::string & objMemo)
	:
	m_objName(objName),
	m_objMemo(objMemo)
{
	this->print();
}

void TopA::print()
{
    std::cout <<__FILE__<<":"<<__LINE__<<":"<< this->typeName() << " Constructor()!" << " objName: " << this->objName() << std::endl;
}

std::unique_ptr<TopA> TopA::New(const std::string &classTypeName, const std::string &objName, const std::string &objMemo)
{
    typename YourAvatarConstructorTable::iterator cstrIter =
    YourAvatarConstructorTablePtr_->find(classTypeName);

    if(cstrIter == YourAvatarConstructorTablePtr_->end()) {
        std::cout << "Error : Failure to find " \
                  << classTypeName \
                  << " in CstrTable " << std::endl;
        std::cout << "Valid are as follows :" << std::endl;

        for (cstrIter = YourAvatarConstructorTablePtr_->begin(); \
             cstrIter != YourAvatarConstructorTablePtr_->end();\
             ++cstrIter) {
            std::cout << cstrIter->first << std::endl;
        }
		return std::unique_ptr<TopA>(nullptr);
    }

    std::unique_ptr<TopA> ptr = \
        (
            ((cstrIter->second))(classTypeName,objName,objMemo)
        );
    return ptr;
}

defineRunTimeSelectionTable(TopA, YourAvatar)

