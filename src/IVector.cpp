#include "../include/IVector.h"
#include "myVector.h"
#include "myLogger.h"

IVector* IVector::createVector(size_t dim, const double* const& ptr_data) {
    if (ptr_data == nullptr) {
        myVector::myLogger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (dim == 0) {
        myVector::myLogger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    RC rc = myVector::isValidArr(ptr_data, dim);
    if (rc != RC::SUCCESS) {
        myVector::myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    size_t size = sizeof(myVector) + dim * sizeof(double);
    uint8_t* p = new(nothrow) uint8_t[size];
    if(p == nullptr){
        myVector::myLogger->severe(RC::ALLOCATION_ERROR,__FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    myVector* res = new(p) myVector(dim);
    memcpy(p + sizeof(myVector), ptr_data, dim * sizeof(double));
    return res;
}

RC IVector::copyInstance(IVector* const dest, const IVector* const& src) {
    if (dest == nullptr) {
        myVector::myLogger->severe(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    const uint8_t* ds = reinterpret_cast<const uint8_t*>(dest);
    const uint8_t* rc = reinterpret_cast<const uint8_t*>(src);
    size_t diff = rc > ds ? rc - ds : ds - rc;
    if (diff < src->sizeAllocated()) {
        myVector::myLogger->severe(RC::MEMORY_INTERSECTION, __FILE__, __FUNCTION__, __LINE__);
        return RC::MEMORY_INTERSECTION;
    }
    memcpy((void*)dest, (void*)src, src->sizeAllocated());
    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src) {
    if (dest == nullptr) {
        myVector::myLogger->severe(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    memmove((void*)dest, (void*)src, src->sizeAllocated());
    delete src;
    src = nullptr;
    return RC::SUCCESS;
}

RC IVector::setLogger(ILogger* const logger) {
    if (logger == nullptr) {
        return RC::NULLPTR_ERROR;
    }
    myVector::myLogger = logger;
    return RC::SUCCESS;
}

IVector* IVector::add(const IVector* const& op1, const IVector* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        myVector::myLogger->severe(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        myVector::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    double* ptr_data = new(nothrow) double[op1->getDim()];
    if(ptr_data == nullptr){
        myVector::myLogger->severe(RC::ALLOCATION_ERROR,__FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    for (size_t i = 0; i < op1->getDim(); ++i) {
        ptr_data[i] = op1->getData()[i] + op2->getData()[i];
    }
    RC rc = myVector::isValidArr(ptr_data, op1->getDim());
    if (rc != RC::SUCCESS) {
        delete[] ptr_data;
        myVector::myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* res = IVector::createVector(op1->getDim(), ptr_data);
    delete[] ptr_data;
    return res;
}

IVector* IVector::sub(const IVector* const& op1, const IVector* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        myVector::myLogger->severe(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        myVector::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    double* ptr_data = new(nothrow) double[op1->getDim()];
    if(ptr_data == nullptr){
        myVector::myLogger->severe(RC::ALLOCATION_ERROR,__FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    for (size_t i = 0; i < op1->getDim(); ++i) {
        ptr_data[i] = op1->getData()[i] - op2->getData()[i];
    }
    RC rc = myVector::isValidArr(ptr_data, op1->getDim());
    if (rc != RC::SUCCESS) {
        delete[] ptr_data;
        myVector::myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* res = IVector::createVector(op1->getDim(), ptr_data);
    delete[] ptr_data;
    return res;
}

double IVector::dot(const IVector* const& op1, const IVector* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        myVector::myLogger->severe(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return NAN;
    }
    if (op1->getDim() != op2->getDim()) {
        myVector::myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return NAN;
    }
    if (op1->getDim() != op2->getDim()) {
        return NAN;
    }
    double res = 0.0;
    for (size_t i = 0; i < op1->getDim(); ++i) {
        res += op1->getData()[i] * op2->getData()[i];
    }
    if (myVector::isValid(res) != RC::SUCCESS) {
        myVector::myLogger->warning(myVector::isValid(res), __FILE__, __FUNCTION__, __LINE__);
        return NAN;
    }
    return res;
}

bool IVector::equals(const IVector* const& op1, const IVector* const& op2, IVector::NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        myVector::myLogger->info(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (op1->getDim() != op2->getDim()) {
        myVector::myLogger->info(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if (n == IVector::NORM::AMOUNT) {
        myVector::myLogger->info(RC::AMOUNT, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    if(tol < 0 || myVector::isValid(tol) != RC::SUCCESS){
        myVector::myLogger->info(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    IVector* op3 = IVector::sub(op1, op2);
    if (op3 == nullptr)
        return false;
    double sum = op3->norm(n);
    delete op3;
    return sum < tol;
}

IVector::~IVector() = default;
