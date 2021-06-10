#include "mySetControlBlock.h"

ISetControlBlock::~ISetControlBlock() = default;

mySetControlBlock::mySetControlBlock(mySet* parent) {
    my_set = parent;
}

RC mySetControlBlock::getNext(IVector* const& vec, size_t& index, size_t indexInc) const {
    if (!my_set) {
        mySet::myLogger->warning(RC::SOURCE_SET_DESTROYED, __FILE__, __FUNCTION__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    return my_set->getNextVec(vec, index, indexInc);
}

RC mySetControlBlock::getPrevious(IVector* const& vec, size_t& index, size_t indexInc) const {
    if (!my_set) {
        mySet::myLogger->warning(RC::SOURCE_SET_DESTROYED, __FILE__, __FUNCTION__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    return my_set->getPrevVec(vec, index, indexInc);
}

RC mySetControlBlock::getBegin(IVector* const& vec, size_t& index) const {
    if (!my_set) {
        mySet::myLogger->warning(RC::SOURCE_SET_DESTROYED, __FILE__, __FUNCTION__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    return my_set->getBeginVec(vec, index);
}

RC mySetControlBlock::getEnd(IVector* const& vec, size_t& index) const {
    if (!my_set) {
        mySet::myLogger->warning(RC::SOURCE_SET_DESTROYED, __FILE__, __FUNCTION__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    return my_set->getEndVec(vec, index);
}

void mySetControlBlock::free_set() {
    my_set = nullptr;
}

mySetControlBlock::~mySetControlBlock() = default;
