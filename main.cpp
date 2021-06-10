#include <iostream>
#include <cmath>
#include "include/ISet.h"
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
using namespace std;

void paintVector(const IVector* const& iVector) {
    if (iVector == nullptr)
        return;
    cout << "[";
    double d = 0.0;
    for (size_t i = 0; iVector->getDim() - 1 > i; ++i) {
        iVector->getCord(i, d);
        cout << d << ", ";
    }
    iVector->getCord(iVector->getDim() - 1, d);
    cout << d << "]" << endl;
}

void paintSet(const ISet* const& iSet) {
    if (iSet == nullptr) {
        cout << "This is nullptr";
        return;
    }
    IVector* iVector;
    cout << "[";
    for (size_t i = 0; i < iSet->getSize(); ++i) {
        iSet->getCopy(i, iVector);
        paintVector(iVector);
        delete iVector;
        iVector = nullptr;
    }
    cout << "]" << endl;
}

void testSet(ILogger* logger) {
    ISet* iSet1 = ISet::createSet();
    ISet* iSet2 = ISet::createSet();
    const double tol = 0.1;
    double data1[4] = {1, 2, 3, 4};
    double data2[4] = {2, 3, 4, 5};
    double data3[4] = {3, 4, 5, 6};
    double data4[4] = {4, 5, 6, 7};
    double data5[4] = {5, 6, 7, 8};
    double data6[4] = {6, 7, 8, 9};
    double data7[4] = {1, 3, 5, 7};
    double data8[4] = {2, 4, 6, 8};
    double data9[4] = {1.001, 2, 3, 4};
    double data10[5] = {3, 3, 3, 3, 4};

    IVector* vec1 = IVector::createVector(sizeof(data1) / sizeof(data1[0]), data1);
    IVector* vec2 = IVector::createVector(sizeof(data2) / sizeof(data2[0]), data2);
    IVector* vec3 = IVector::createVector(sizeof(data3) / sizeof(data3[0]), data3);
    IVector* vec4 = IVector::createVector(sizeof(data4) / sizeof(data4[0]), data4);
    IVector* vec5 = IVector::createVector(sizeof(data5) / sizeof(data5[0]), data5);
    IVector* vec6 = IVector::createVector(sizeof(data6) / sizeof(data6[0]), data6);
    IVector* vec7 = IVector::createVector(sizeof(data7) / sizeof(data7[0]), data7);
    IVector* vec8 = IVector::createVector(sizeof(data8) / sizeof(data8[0]), data8);
    IVector* vec9 = IVector::createVector(sizeof(data9) / sizeof(data9[0]), data9);
    IVector* vec10 = IVector::createVector(sizeof(data10) / sizeof(data10[0]), data10);

    IVector::setLogger(logger);
    ISet::setLogger(logger);

    iSet1->insert(vec1, IVector::NORM::FIRST, tol);
    iSet1->insert(vec2, IVector::NORM::FIRST, tol);
    iSet1->insert(vec3, IVector::NORM::FIRST, tol);
    iSet1->insert(vec4, IVector::NORM::FIRST, tol);
    iSet1->insert(vec5, IVector::NORM::FIRST, tol);
    iSet1->insert(vec9, IVector::NORM::FIRST, tol);
    iSet1->insert(vec10, IVector::NORM::FIRST, tol);


    iSet2->insert(vec9, IVector::NORM::FIRST, tol);
    iSet2->insert(vec8, IVector::NORM::FIRST, tol);
    iSet2->insert(vec7, IVector::NORM::FIRST, tol);
    iSet2->insert(vec6, IVector::NORM::FIRST, tol);
    iSet2->insert(vec5, IVector::NORM::FIRST, tol);
    iSet2->insert(vec4, IVector::NORM::FIRST, tol);

    cout << "iSet1: " << endl;
    paintSet(iSet1);
    cout << "iSet2: " << endl;
    paintSet(iSet2);


    ISet::equals(iSet1, iSet2, IVector::NORM::FIRST, tol);
    ISet* intersectionSet = ISet::makeIntersection(iSet1, iSet2, IVector::NORM::FIRST, tol);
    ISet* unionSet = ISet::makeUnion(iSet1, iSet2, IVector::NORM::FIRST, tol);
    ISet* subSet = ISet::sub(iSet1, iSet2, IVector::NORM::FIRST, tol);
    ISet* symSubSet = ISet::symSub(iSet1, iSet2, IVector::NORM::FIRST, tol);

    cout << "Intersection: " << endl;
    paintSet(intersectionSet);
    cout << "Union: " << endl;
    paintSet(unionSet);
    cout << "Sub: " << endl;
    paintSet(subSet);
    cout << "Symsub: " << endl;
    paintSet(symSubSet);

    ISet* iSet3 = ISet::createSet();
    ISet* iSet4 = ISet::createSet();
    iSet3->insert(vec1, IVector::NORM::FIRST, tol);
    iSet3->insert(vec2, IVector::NORM::FIRST, tol);
    iSet3->insert(vec3, IVector::NORM::FIRST, tol);
    iSet3->insert(vec4, IVector::NORM::FIRST, tol);
    iSet3->insert(vec5, IVector::NORM::FIRST, tol);

    iSet4->insert(vec5, IVector::NORM::FIRST, tol);
    iSet4->insert(vec4, IVector::NORM::FIRST, tol);
    iSet4->insert(vec3, IVector::NORM::FIRST, tol);
    iSet4->insert(vec2, IVector::NORM::FIRST, tol);
    iSet4->insert(vec9, IVector::NORM::FIRST, tol);

    cout << "iSet3 == iSet4 ?   " << ISet::equals(iSet3, iSet4, IVector::NORM::FIRST, tol) << endl;
    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete vec5;
    delete vec6;
    delete vec7;
    delete vec8;
    delete vec9;
    delete vec10;

    delete iSet1;
    delete iSet2;
    delete iSet3;
    delete iSet4;

    delete intersectionSet;
    delete unionSet;
    delete subSet;
    delete symSubSet;
}

void iteratorTests(ILogger* logger) {
    double data1[6] = {1, 2, 3, 4, 5, 6};
    double data2[6] = {1, 2, 1, 2, 1, 2};
    double data3[6] = {-1, -2, -3, -4, -5, -6};

    IVector* vec1 = IVector::createVector(sizeof(data1) / sizeof(data1[0]), data1);
    IVector* vec2 = IVector::createVector(sizeof(data2) / sizeof(data2[0]), data2);
    IVector* vec3 = IVector::createVector(sizeof(data3) / sizeof(data3[0]), data3);

    ISet* set = ISet::createSet();
    IVector::setLogger(logger);
    ISet::setLogger(logger);


    set->insert(vec1, IVector::NORM::SECOND, 0.1);
    set->insert(vec2, IVector::NORM::SECOND, 0.1);
    set->insert(vec3, IVector::NORM::SECOND, 0.1);

    IVector* vector;
    ISet::IIterator* iterator1 = set->getIterator(0);
    iterator1->getVectorCopy(vector);
    std::cout << "Iterator (first vector):" << std::endl;
    paintVector(vector);
    delete vector;
    vector = nullptr;
    ISet::IIterator* iterator2 = iterator1->getNext(2);
    std::cout << "Iterator (third vector)" << std::endl;
    iterator2->getVectorCopy(vector);
    paintVector(vector);

    delete vector;
    delete iterator1;
    delete iterator2;
    delete vec1;
    delete vec2;
    delete vec3;
    delete set;
}

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

    const char* filename = "error.txt";
    ILogger* logger = ILogger::createLogger(filename, true);

    cout << "Test static function" << endl;
    testSet(logger);

    cout << "Test iterator" << endl;
    iteratorTests(logger);

    delete logger;

    return 0;
}
