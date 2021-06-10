#ifndef UI_VECTOR_MYVECTOR_H
#define UI_VECTOR_MYVECTOR_H
#include <cmath>
#include "../include/IVector.h"
#include "myLogger.h"
using namespace std;

class myVector : public IVector{
public:
    static ILogger* myLogger;

    static RC isValidArr(const double* da, size_t size);

    static RC isValid(double d);

public:
    myVector(size_t dim);

public:
    IVector *clone() const override;

    const double* getData() const override;

    RC setData(size_t dim_, double const* const& ptr_data) override;

    RC getCord(size_t index, double& val) const override;

    RC setCord(size_t index, double val) override;

    RC scale(double multiplier) override;

    size_t getDim() const override;

    RC inc(const IVector* const& op) override;

    RC dec(const IVector* const& op) override;

    double norm(NORM n) const override;

    size_t sizeAllocated() const override;

    RC applyFunction(const std::function<double(double)>& fun) override;

    RC foreach(const std::function<void(double)>& fun) const override;

    ~myVector() override = default;

private:
    size_t dim;
};


#endif //UI_VECTOR_MYVECTOR_H
