#include "../include/ISet.h"
#include "../src/mySet.h"

RC ISet::setLogger(ILogger* const logger) {
    if (logger == nullptr) {
        return RC::NULLPTR_ERROR;
    }
    mySet::myLogger = logger;
    return RC::SUCCESS;
}

ISet* ISet::createSet() {
    ISet* res = new(std::nothrow) mySet();
    if (res == nullptr)
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
    return res;
}

ISet* ISet::makeIntersection(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    IVector* v1 = nullptr;
    IVector* v2 = nullptr;
    ISet* res = createSet();
    for (size_t i = 0; i < op1->getSize(); ++i) {
        if (op1->getCopy(i, v1) != RC::SUCCESS) {
            mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            delete res;
            return nullptr;
        }
        for (size_t j = 0; j < op2->getSize(); ++j) {
            if (op2->getCopy(j, v2) != RC::SUCCESS){
                mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
                delete v1;
                v1 = nullptr;
                delete res;
                return nullptr;
            }
            if (IVector::equals(v1, v2, n, tol)) {
                res->insert(v1, n, tol);
                delete v2;
                v2 = nullptr;
                break;
            }
            delete v2;
            v2 = nullptr;
        }
        delete v1;
        v1 = nullptr;
    }
    return res;
}

ISet* ISet::makeUnion(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning((RC::MISMATCHING_DIMENSIONS), __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    ISet* res = op1->clone();
    IVector* v2 = nullptr;
    for (size_t i = 0; i < op2->getSize(); ++i) {
        if (op2->getCopy(i, v2) != RC::SUCCESS) {
            delete res;
            return nullptr;
        }
        res->insert(v2, n, tol);
        delete v2;
        v2 = nullptr;
    }
    return res;
}

ISet* ISet::sub(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning((RC::MISMATCHING_DIMENSIONS), __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    ISet* res = op1->clone();
    IVector* v2 = nullptr;
    for (size_t i = 0; i < op2->getSize(); ++i) {
        if (op2->getCopy(i, v2) != RC::SUCCESS) {
            delete res;
            return nullptr;
        }
        res->remove(v2, n, tol);
        delete v2;
        v2 = nullptr;
    }
    return res;
}

ISet* ISet::symSub(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning((RC::MISMATCHING_DIMENSIONS), __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    ISet* Union = makeUnion(op1, op2, n, tol);
    if (Union == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    ISet* intersection = makeIntersection(op1, op2, n, tol);
    if (intersection == nullptr) {
        mySet::myLogger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        delete Union;
        return nullptr;
    }
    ISet* res = sub(Union, intersection, n, tol);
    delete Union;
    delete intersection;
    return res;
}

bool ISet::equals(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning((RC::MISMATCHING_DIMENSIONS), __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    return (ISet::subSet(op1, op2, n, tol) && ISet::subSet(op2, op1, n, tol));
}

bool ISet::subSet(const ISet* const& op1, const ISet* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        mySet::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (n == IVector::NORM::AMOUNT || tol < 0 || mySet::isValid(tol) != RC::SUCCESS) {
        mySet::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (op1->getDim() != op2->getDim()) {
        mySet::myLogger->warning((RC::MISMATCHING_DIMENSIONS), __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    IVector* v1 = nullptr;
    IVector* v2 = nullptr;
    for (size_t i = 0; i < op1->getSize(); ++i) {
        if (op1->getCopy(i, v1) != RC::SUCCESS) {
            return false;
        }
        if (op2->findFirstAndCopy(v1, n, tol, v2) != RC::SUCCESS) {
            delete v1;
            return false;
        } else {
            delete v2;
            v2 = nullptr;
        }
        delete v1;
        v1 = nullptr;
    }
    return true;
}

RC ISet::IIterator::setLogger(ILogger* const pLogger) {
    if (pLogger == nullptr) {
        return RC::NULLPTR_ERROR;
    }
    mySet::myIterator::LoggerIterator = pLogger;
    return RC::SUCCESS;
}

ISet::~ISet() = default;
