#pragma once
#include "../include/ISetControlBlock.h"
#include "../src/mySet.h"

class mySet;

class mySetControlBlock: public ISetControlBlock{
public:
    mySetControlBlock(mySet* parent);

public:
    RC getNext(IVector* const& vec, size_t& index, size_t indexInc) const override;

    RC getPrevious(IVector* const& vec, size_t& index, size_t indexInc) const override;

    RC getBegin(IVector* const& vec, size_t& index) const override;

    RC getEnd(IVector* const& vec, size_t& index) const override;

    ~mySetControlBlock() override;

    void free_set();

private:
    mySet* my_set;
};