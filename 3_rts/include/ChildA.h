#ifndef __CHILDA_H__
#define __CHILDA_H__

#include <iostream>
#include <memory>

class TopA;

class ChildA : public TopA
{
public:

	declareTypeName

    ChildA(const std::string &classTypeName, const std::string &objName, const std::string &objMemo);

private:

	// std::string m_objName;
	// std::string m_memo;

    virtual void print() override;
};

#endif
