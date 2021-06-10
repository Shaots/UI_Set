#include "myVector.h"
#define SHAOTS_GETDATA_NOT_CONST (reinterpret_cast<double*>(this + 1))

myVector::myVector(size_t dim) : dim(dim) {}

ILogger* myVector::myLogger = nullptr;

size_t myVector::sizeAllocated() const {
    return dim * sizeof(double) + sizeof(myVector);
}

IVector* myVector::clone() const {
    uint8_t* p = new(nothrow) uint8_t[myVector::sizeAllocated()];
    if(p == nullptr){
        myVector::myLogger->severe(RC::ALLOCATION_ERROR,__FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    myVector* copy = new(p) myVector(dim);
    memcpy(p + sizeof(myVector), this->getData(), dim * sizeof(double));
    return copy;
}

const double* myVector::getData() const {
    return reinterpret_cast<const double*>(this + 1);
}

RC myVector::setData(size_t dim_, const double* const& ptr_data) {
    if (this->dim != dim_) {
        myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    memcpy(SHAOTS_GETDATA_NOT_CONST, ptr_data, dim_ * sizeof(double));
    return RC::SUCCESS;
}

RC myVector::getCord(size_t index, double& val) const {
    if (index >= dim) {
        myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    } else
        val = getData()[index];
    return RC::SUCCESS;
}

RC myVector::setCord(size_t index, double val) {
    if (index >= dim) {
        myLogger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    if (myVector::isValid(val) != RC::SUCCESS) {
        myLogger->warning(myVector::isValid(val), __FILE__, __FUNCTION__, __LINE__);
        return myVector::isValid(val);
    }
    SHAOTS_GETDATA_NOT_CONST[index] = val;
    return RC::SUCCESS;
}

RC myVector::scale(double multiplier) {
    double* data = SHAOTS_GETDATA_NOT_CONST;
    double max = fabs(data[0]);
    RC rc;
    for (size_t i = 1; i < dim; ++i) {
        if (max < fabs(data[i]))
            max = fabs(data[i]);
    }
    rc = isValid(max * multiplier);
    if (rc != RC::SUCCESS) {
        myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return rc;
    }
    for (size_t i = 0; i < dim; ++i) {
        data[i] *= multiplier;
    }
    return RC::SUCCESS;
}

size_t myVector::getDim() const {
    return dim;
}

RC myVector::inc(const IVector* const& op) {
    if (op->getDim() != this->dim) {
        myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    double* data = SHAOTS_GETDATA_NOT_CONST;
    double max = fabs(data[0] + op->getData()[0]);
    RC rc;

    for (size_t i = 1; i < dim; ++i) {
        if (max < fabs(data[i] + op->getData()[i]))
            max = fabs(data[i] + op->getData()[i]);
    }
    rc = isValid(max);
    if (rc != RC::SUCCESS) {
        myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return rc;
    }
    for (size_t i = 0; i < dim; ++i) {
        data[i] += op->getData()[i];
    }
    return RC::SUCCESS;
}

RC myVector::dec(const IVector* const& op) {
    if (op->getDim() != this->dim) {
        myLogger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    double* data = SHAOTS_GETDATA_NOT_CONST;
    double max = fabs(data[0] - op->getData()[0]);
    RC rc;

    for (size_t i = 1; i < dim; ++i) {
        if (max < fabs(data[i] - op->getData()[i]))
            max = fabs(data[i] - op->getData()[i]);
    }
    rc = isValid(max);
    if (rc != RC::SUCCESS) {
        myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
        return rc;
    }
    for (size_t i = 0; i < dim; ++i) {
        data[i] -= op->getData()[i];
    }
    return RC::SUCCESS;
}

double myVector::norm(IVector::NORM n) const {
    const double* data = myVector::getData();
    switch (n) {
        case IVector::NORM::CHEBYSHEV: {
            double max = fabs(data[0]);
            for (size_t i = 1; i < dim; ++i) {
                max = fabs(data[i]) > max ? fabs(data[i]) : max;
            }
            return max;
        }
        case IVector::NORM::FIRST: {
            double sum = 0;
            for (size_t i = 0; i < dim; ++i) {
                sum += fabs(data[i]);
            }
            if (isValid(sum) != RC::SUCCESS) {
                myLogger->warning(isValid(sum), __FILE__, __FUNCTION__, __LINE__);
                return NAN;
            }
            return sum;
        }
        case IVector::NORM::SECOND: {
            double sum = 0;
            for (size_t i = 0; i < dim; ++i) {
                sum += data[i] * data[i];
            }
            if (isValid(sum) != RC::SUCCESS) {
                myLogger->warning(isValid(sum), __FILE__, __FUNCTION__, __LINE__);
                return NAN;
            }
            return sqrt(sum);
        }
        case IVector::NORM::AMOUNT:
        default:
            return NAN;
    }
}

RC myVector::applyFunction(const std::function<double(double)>& fun) {
    RC rc;
    double* copy = new(nothrow) double[dim];
    if(copy == nullptr){
        myVector::myLogger->severe(RC::ALLOCATION_ERROR,__FILE__, __FUNCTION__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    memcpy(copy, SHAOTS_GETDATA_NOT_CONST, dim * sizeof(double));
    for (size_t i = 0; i < dim; ++i) {
        SHAOTS_GETDATA_NOT_CONST[i] = fun(SHAOTS_GETDATA_NOT_CONST[i]);
        rc = isValid(SHAOTS_GETDATA_NOT_CONST[i]);
        if (rc != RC::SUCCESS) {
            myLogger->warning(rc, __FILE__, __FUNCTION__, __LINE__);
            memcpy(SHAOTS_GETDATA_NOT_CONST, copy, dim * sizeof(double));
            return rc;
        }
    }
    return RC::INDEX_OUT_OF_BOUND;
}

RC myVector::foreach(const std::function<void(double)>& fun) const {
    const double* data = getData();
    for (size_t i = 0; i < dim; ++i)
        fun(data[i]);
    return RC::SUCCESS;
}

RC myVector::isValid(double d) {
    if (std::isnan(d))
        return RC::NOT_NUMBER;
    if (d == INFINITY || -d == INFINITY)
        return RC::INFINITY_OVERFLOW;
    return RC::SUCCESS;
}

RC myVector::isValidArr(const double* da, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        RC rc = isValid(da[i]);
        if (rc != RC::SUCCESS)
            return rc;
    }
    return RC::SUCCESS;
}