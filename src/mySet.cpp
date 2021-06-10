#include "mySet.h"

static bool binarySearch(const size_t* arr, size_t length, const size_t target, size_t& index) {
    size_t left = 0;
    size_t right = length - 1;
    size_t mid;
    while (left < right) {
        mid = (left + right) / 2;
        if (arr[mid] < target) {
            left = mid + 1;
        } else if (arr[mid] > target) {
            right = mid;
        } else {
            index = mid;
            return true;
        }
    }
    return false;
}

RC mySet::isValid(double d) {
    if (std::isnan(d))
        return RC::NOT_NUMBER;
    if (d == INFINITY || -d == INFINITY)
        return RC::INFINITY_OVERFLOW;
    return RC::SUCCESS;
}

mySet::mySet() {
    size = 0;
    dim = 0;
    nextIndex = 0;
    capacity = 0;
    indexArr = nullptr;
    data = nullptr;
    pBlock = shared_ptr<mySetControlBlock>(new(nothrow) mySetControlBlock(this));
}

ILogger* mySet::myLogger = nullptr;

ILogger* mySet::myIterator::LoggerIterator = nullptr;

mySet::myIterator::myIterator(const shared_ptr<mySetControlBlock>& pBlock_, IVector* iVector_, size_t index_) {
    pBlock = pBlock_;
    iVector = iVector_;
    index = index_;
}

ISet* mySet::clone() const {
    mySet* set = new(nothrow) mySet();
    if (set == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    set->dim = dim;
    set->size = size;
    set->capacity = capacity;
    set->nextIndex = nextIndex;
    set->data = new(nothrow) double[dim * capacity];
    if (set->data == nullptr){
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        delete set;
        return nullptr;
    }
    set->indexArr = new(nothrow) size_t[capacity];
    if (set->indexArr == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        delete[] set->data;
        delete set;
        return nullptr;
    }
    memcpy(set->data, data, dim * capacity * sizeof(double));
    memcpy(set->indexArr, indexArr, capacity * sizeof(size_t));
    return set;
}

size_t mySet::getDim() const {
    return dim;
}

size_t mySet::getSize() const {
    return size;
}

RC mySet::getCopy(size_t index, IVector*& val) const {
    if (index >= size) {
        myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        val = nullptr;
        return RC::INDEX_OUT_OF_BOUND;
    }
    val = IVector::createVector(dim, data + index * dim);
    if (val == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    return RC::SUCCESS;
}

RC mySet::findFirstAndCopy(const IVector* const& pat, IVector::NORM n, double tol, IVector*& val) const {
    val = nullptr;
    if (pat == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    if (pat->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    for (size_t i = 0; i < size; ++i) {
        IVector* iVector = IVector::createVector(dim, data + i * dim);
        if (iVector == nullptr) {
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            val = nullptr;
            return RC::ALLOCATION_ERROR;
        }
        if (IVector::equals(pat, iVector, n, tol)) {
            getCopy(i, val);
            delete iVector;
            return RC::SUCCESS;
        }
        delete iVector;
    }
    mySet::myLogger->info(RC::VECTOR_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
    return RC::VECTOR_NOT_FOUND;
}

RC mySet::getCoords(size_t index, IVector* const& val) const {
    if (index >= size) {
        myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    if (val->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    val->setData(dim,data + dim * index);
    return RC::SUCCESS;
}

RC mySet::findFirstAndCopyCoords(const IVector* const& pat, IVector::NORM n, double tol, IVector* const& val) const {
    if (pat == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    if (pat->getDim() != dim || val->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS);
        return RC::MISMATCHING_DIMENSIONS;
    }
    for (size_t i = 0; i < size; ++i) {
        IVector* iVector = IVector::createVector(dim, data + i * dim);
        if (IVector::equals(pat, iVector, n, tol)) {
            getCoords(i, val);
            delete iVector;
            return RC::SUCCESS;
        }
        delete iVector;
    }
    mySet::myLogger->info(RC::VECTOR_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
    return RC::VECTOR_NOT_FOUND;
}

RC mySet::insert(const IVector* const& val, IVector::NORM n, double tol) {
    if (val == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    if (dim != 0 && val->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    //Если множество только что родилось. т.е. ничего в нем нет.
    if (nextIndex == 0) {
        dim = val->getDim();
        data = new(nothrow) double[dim * initial_quantity];
        if (data == nullptr){
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            dim = 0;
            return RC::ALLOCATION_ERROR;
        }
        indexArr = new(nothrow) size_t[initial_quantity];
        if (indexArr == nullptr) {
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            dim = 0;
            delete[] data;
            return RC::ALLOCATION_ERROR;
        }
        memcpy(data, val->getData(), dim * sizeof(double));
        indexArr[size] = nextIndex;
        nextIndex++;
        capacity = initial_quantity;
        size++;
        return RC::SUCCESS;
    }
    //Проверить существование добавляемого вектора
    for (size_t i = 0; i < size; ++i) {
        IVector* iVector = IVector::createVector(dim, data + i * dim);
        if (IVector::equals(val, iVector, n, tol)) {
            delete iVector;
            mySet::myLogger->info(RC::VECTOR_ALREADY_EXIST, __FILE__, __FUNCTION__, __LINE__);
            return RC::VECTOR_ALREADY_EXIST;
        }
        delete iVector;
    }
    //Если множество уже полно.
    if (size == capacity) {
        double* dataCopy = new(nothrow) double[capacity * multiple * dim];
        if (dataCopy == nullptr){
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            return RC::ALLOCATION_ERROR;
        }
        size_t* indexArrCopy = new(nothrow) size_t[capacity * multiple];
        if (indexArrCopy == nullptr) {
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            delete[] dataCopy;
            return RC::ALLOCATION_ERROR;
        }
        capacity *= multiple;
        memcpy(dataCopy, data, size * dim * sizeof(double));
        memcpy(indexArrCopy, indexArr, size * sizeof(size_t));
        delete[] data;
        delete[] indexArr;
        data = dataCopy;
        indexArr = indexArrCopy;
    }

    memcpy(data + size * dim, val->getData(), dim * sizeof(double));
    indexArr[size] = nextIndex;
    nextIndex++;
    size++;
    return RC::SUCCESS;
}

RC mySet::remove(size_t index) {
    if (index >= size) {
        myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    memmove(data + index * dim, data + (index + 1) * dim, dim * (size - index - 1) * sizeof(double));
    memmove(indexArr + index, indexArr + (index + 1), (size - index - 1) * sizeof(size_t));
    size--;
    return RC::SUCCESS;
}

RC mySet::remove(const IVector* const& pat, IVector::NORM n, double tol) {
    if (pat == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    if (pat->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS);
        return RC::MISMATCHING_DIMENSIONS;
    }
    for (size_t i = 0; i < size; ++i) {
        IVector* iVector = IVector::createVector(dim, data + i * dim);
        if (IVector::equals(pat, iVector, n, tol)) {
            delete iVector;
            return remove(i);
        }
        delete iVector;
    }
    mySet::myLogger->info(RC::VECTOR_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
    return RC::VECTOR_NOT_FOUND;
}

RC mySet::getNextVec(IVector* const& vec, size_t& index, size_t indexInc) {
    if (vec == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (index >= nextIndex) {
        mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    if (vec->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    size_t res;
    if (binarySearch(indexArr, size, index, res)) {
        if (res + indexInc >= size) {
            mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
            index = indexArr[size - 1];
            return RC::INDEX_OUT_OF_BOUND;
        }
        vec->setData(dim, data + dim * (res + indexInc));
        index = indexArr[res + indexInc];
        return RC::SUCCESS;
    }
    mySet::myLogger->info(RC::VECTOR_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
    return RC::VECTOR_NOT_FOUND;
}

RC mySet::getPrevVec(IVector* const& vec, size_t& index, size_t indexInc) {
    if (vec == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (index >= nextIndex) {
        mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    if (vec->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    size_t res;
    if (binarySearch(indexArr, size, index, res)) {
        if (res < indexInc) {
            mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
            index = indexArr[size - 1];
            return RC::INDEX_OUT_OF_BOUND;
        }
        vec->setData(dim, data + dim * (res - indexInc));
        return RC::SUCCESS;
    }
    mySet::myLogger->info(RC::VECTOR_NOT_FOUND, __FILE__, __FUNCTION__, __LINE__);
    return RC::VECTOR_NOT_FOUND;
}

RC mySet::getBeginVec(IVector* const& vec, size_t& index) {
    if (vec == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (vec->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    if (size == 0) {
        return RC::SOURCE_SET_EMPTY;
    }
    vec->setData(dim, data);
    index = indexArr[0];
    return RC::SUCCESS;
}

RC mySet::getEndVec(IVector* const& vec, size_t& index) {
    if (vec == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (vec->getDim() != dim) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    if (size == 0) {
        return RC::SOURCE_SET_EMPTY;
    }
    vec->setData(dim, data + dim * (size - 1));
    index = (indexArr)[size - 1];
    return RC::SUCCESS;
}

ISet::IIterator* mySet::getIterator(size_t index) const {
    if (index >= size) {
        mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    IVector* vec = IVector::createVector(dim, data + dim * index);
    if (vec == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    IIterator* iterator = new(nothrow) mySet::myIterator(pBlock, vec, indexArr[index]);
    if (iterator == nullptr) {
        mySet::myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        delete vec;
        return nullptr;
    }
    return iterator;
}

ISet::IIterator* mySet::getBegin() const {
    return getIterator(0);
}

ISet::IIterator* mySet::getEnd() const {
    return getIterator(size - 1);
}

mySet::~mySet() {
    pBlock->free_set();
    delete[] indexArr;
    delete[] data;
}

ISet::IIterator* mySet::myIterator::getNext(size_t indexInc) const {
    IIterator* iIterator = clone();
    if (iIterator == nullptr) {
        LoggerIterator->warning(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    iIterator->next(indexInc);
    return iIterator;
}

ISet::IIterator* mySet::myIterator::getPrevious(size_t indexInc) const {
    IIterator* iIterator = clone();
    if (iIterator == nullptr) {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    iIterator->previous(indexInc);
    return iIterator;
}

ISet::IIterator* mySet::myIterator::clone() const {
    IVector* vec = iVector->clone();
    if (vec == nullptr) {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    IIterator* iterator = new(nothrow) mySet::myIterator(pBlock, vec, index);
    if (iterator == nullptr) {
        delete vec;
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    return iterator;
}

RC mySet::myIterator::next(size_t indexInc) {
    return pBlock->getNext(iVector, index, indexInc);
}

RC mySet::myIterator::previous(size_t indexInc) {
    return pBlock->getPrevious(iVector, index, indexInc);
}

bool mySet::myIterator::isValid() const {
    return index != numeric_limits<size_t>::max();
}

RC mySet::myIterator::makeBegin() {
    return pBlock->getBegin(iVector, index);
}

RC mySet::myIterator::makeEnd() {
    return pBlock->getEnd(iVector, index);
}

RC mySet::myIterator::getVectorCopy(IVector*& val) const {
    IVector* vec = iVector->clone();
    if (vec == nullptr) {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        val = nullptr;
        return RC::ALLOCATION_ERROR;
    }
    val = vec;
    return RC::SUCCESS;
}

RC mySet::myIterator::getVectorCoords(IVector* const& val) const {
    if (val == nullptr) {
        LoggerIterator->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (val->getDim() != iVector->getDim()) {
        LoggerIterator->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    return val->setData(iVector->getDim(), iVector->getData());
}

mySet::myIterator::~myIterator() {
    delete iVector;
}

ISet::IIterator::~IIterator() = default;