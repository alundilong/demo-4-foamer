#ifndef __RTSMACRO_H__
#define __RTSMACRO_H__

#define declareTypeName\
	static std::string m_typeName;\
	static std::string typeName() {return m_typeName;};\
	std::string objName() {return m_objName;}; \
	const std::string objName() const {return m_objName;}; \
	std::string objMemo(){return m_objMemo;}; \
	const std::string objMemo() const {return m_objMemo;}; \
	std::string m_objName; \
	std::string m_objMemo = std::string("no memo");

#define defineTypeName(classname)\
	std::string classname::m_typeName = std::string(#classname);

#include <memory>
#include <unordered_map>
#include <iostream>
#include <utility>
#include <functional>
	
#define declareRunTimeSelectionTable(unique_ptr, baseType, \
    argNames, argList, parList)\
    \
    typedef unique_ptr<baseType> (*argNames##ConstructorPtr)argList;\
    \
    typedef std::unordered_map<std::string,argNames##ConstructorPtr> \
    argNames##ConstructorTable;\
    \
    static argNames##ConstructorTable* argNames##ConstructorTablePtr_; \
    static void construct##argNames##ConstructorTables();\
    static void destroy##argNames##ConstructorTables();\
    \
    \
    /* derived class baseType##Type */\
    template<class baseType##Type>\
    class add##argNames##ConstructorToTable\
    {\
    public:\
    \
        static unique_ptr<baseType> New argList\
        {\
            return unique_ptr<baseType>(new baseType##Type parList);\
        }\
        \
        \
        /* constructor of this subclass */\
        add##argNames##ConstructorToTable \
        (\
            const std::string & lookup = baseType##Type::m_typeName\
        )\
        {\
           construct##argNames##ConstructorTables();\
           argNames##ConstructorTablePtr_->insert\
           (\
               std::make_pair(lookup, New)\
           );\
           if(0)\
           {\
                std::cerr << "Duplicate entry " << lookup\
                          << " in runtime selection table " << #baseType\
                          << std::endl;\
                /* error handle : better to use exception */\
                /* std::cout << std::cerr << std::endl;*/\
           }\
        }\
        \
        ~add##argNames##ConstructorToTable()\
        {\
            destroy##argNames##ConstructorTables();\
        }\
    };\
\

#define declareRunTimeNewSelectionTable(unique_ptr, baseType, \
    argNames, argList, parList)\
    \
    typedef unique_ptr<baseType> (*argNames##ConstructorPtr)argList;\
    \
    typedef std::unordered_map<std::string,argNames##ConstructorPtr> \
    argNames##ConstructorTable;\
    \
    static argNames##ConstructorTable* argNames##ConstructorTablePtr_; \
    static void construct##argNames##ConstructorTables();\
    static void destroy##argNames##ConstructorTables();\
    \
    \
    /* derived class baseType##Type */\
    template<class baseType##Type>\
    class add##argNames##ConstructorToTable\
    {\
    public:\
    \
        static unique_ptr<baseType> New##baseType argList\
        {\
            return unique_ptr<baseType>((baseType##Type::New parList));\
            \
        }\
        \
        \
        /* constructor of this subclass */\
        add##argNames##ConstructorToTable \
        (\
            const std::string & lookup = baseType##Type::m_typeName\
        )\
        {\
           construct##argNames##ConstructorTables();\
           argNames##ConstructorTablePtr_->insert\
           (\
                std::make_pair(lookup, New##baseType)\
           );\
           if(\
                0\
             )\
           {\
                std::cerr << "Duplicate entry " << lookup\
                          << " in runtime selection table " << #baseType\
                          << std::endl;\
                /* error handle : better to use exception */\
                /* std::cout << std::cerr << std::endl;*/\
           }\
        }\
        \
        ~add##argNames##ConstructorToTable()\
        {\
            destroy##argNames##ConstructorTables();\
        }\
    };\

/* constructor aid */
#define defineRunTimeSelectionTableConstructor(baseType, argNames) \
    \
    void baseType::construct##argNames##ConstructorTables()\
    {\
        static bool constructed = false;\
        if(!constructed) {\
            constructed = true;\
            baseType::argNames##ConstructorTablePtr_ \
                = new baseType::argNames##ConstructorTable;\
        }\
    }\

/* destructor aid */
#define defineRunTimeSelectionTableDestructor(baseType, argNames) \
    \
    void baseType::destroy##argNames##ConstructorTables()\
    {\
        if(baseType::argNames##ConstructorTablePtr_) {\
            delete baseType::argNames##ConstructorTablePtr_; \
            baseType::argNames##ConstructorTablePtr_ = NULL;\
        }\
    }\

/* create pointer to hash-table of functions */
#define defineRunTimeSelectionTablePtr(baseType, argNames)\
    \
    baseType::argNames##ConstructorTable *\
        baseType::argNames##ConstructorTablePtr_ = NULL \

/* interface to the classes */

#define defineRunTimeSelectionTable(baseType, argNames) \
    \
    defineRunTimeSelectionTablePtr(baseType, argNames);\
    defineRunTimeSelectionTableConstructor(baseType, argNames);\
    defineRunTimeSelectionTableDestructor(baseType, argNames)

#define addElementToRunTimeSelectionTable(SS, childClassType, baseClassType)\
    \
    baseClassType::add##SS##ConstructorToTable<childClassType>\
    \
    add##childClassType##SS##ConstructorTo##baseClassType##Table_;

#define makeElement(SS, childClassType, baseClassType) \
    defineTypeName(childClassType)\
    \
    addElementToRunTimeSelectionTable(SS, childClassType, baseClassType)

#endif
