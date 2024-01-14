#ifndef __TOPA_H__
#define __TOPA_H__

#include <iostream>
#include <memory>
#include "rtsmacro.h"

class TopA
{
public:

	//static std::string m_typeName;
	declareTypeName

    TopA(const std::string &classTypeName, const std::string &objName, const std::string &memo);
	static std::unique_ptr<TopA> New(const std::string &classTypeName, const std::string &objName ,const std::string &memo);

    declareRunTimeSelectionTable
    (
        std::unique_ptr,
        TopA,
        YourAvatar,
        (
            const std::string & classTypeName,
            const std::string & objName,
			const std::string & objMemo
        ),
        (
            classTypeName,
            objName,
			objMemo
        )
    )

    virtual void print();

private:

	// std::string m_objName;
};

#endif
