#pragma once

#include <memory>
#include <iostream>
#include <cmath>
#include <limits>
#include "../src/mySetControlBlock.h"
#include "../include/ISet.h"

using namespace std;

class mySetControlBlock;

class mySet : public ISet {
public:
    static ILogger* myLogger;
    static size_t const initial_quantity = 100;
    static size_t const multiple = 2;

public:
    class myIterator : public IIterator {
    public:
        myIterator(const shared_ptr<mySetControlBlock>& pBlock_, IVector* iVector_, size_t index_);

    public:
        static ILogger* LoggerIterator;

    public:
        IIterator* getNext(size_t indexInc) const override;

        IIterator* getPrevious(size_t indexInc) const override;

        IIterator* clone() const override;

        RC next(size_t indexInc) override;

        RC previous(size_t indexInc) override;

        bool isValid() const override;

        RC makeBegin() override;

        RC makeEnd() override;

        RC getVectorCopy(IVector*& val) const override;

        RC getVectorCoords(IVector* const& val) const override;

    public:
        ~myIterator() override;

    private:
        size_t index = 0;
        IVector* iVector = nullptr;
        shared_ptr<mySetControlBlock> pBlock;
    };

public:
    mySet();

    static RC isValid(double d);

    ISet* clone() const override;

    size_t getDim() const override;

    size_t getSize() const override;

    RC getCopy(size_t index, IVector*& val) const override;

    RC findFirstAndCopy(const IVector* const& pat, IVector::NORM n, double tol, IVector*& val) const override;

    RC getCoords(size_t index, IVector* const& val) const override;

    RC findFirstAndCopyCoords(const IVector* const& pat, IVector::NORM n, double tol, IVector* const& val) const override;

    RC insert(const IVector* const& val, IVector::NORM n, double tol) override;

    RC remove(size_t index) override;

    RC remove(const IVector* const& pat, IVector::NORM n, double tol) override;

    IIterator* getIterator(size_t index) const override;

    IIterator* getBegin() const override;

    IIterator* getEnd() const override;

    RC getNextVec(IVector* const& vec, size_t& index, size_t indexInc);

    RC getPrevVec(IVector* const& vec, size_t& index, size_t indexInc);

    RC getBeginVec(IVector* const& vec, size_t& index);

    RC getEndVec(IVector* const& vec, size_t& index);

    ~mySet() override;

private:
    size_t size;
    size_t dim;
    size_t nextIndex;
    size_t capacity;
    size_t* indexArr;
    double* data;
    shared_ptr<mySetControlBlock> pBlock;
};