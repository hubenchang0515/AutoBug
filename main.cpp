#include <cstdio>
#include "DimMap.h"
#include "Text.h"
#include "DataLoader.h"
#include "Kmeans.h"

using namespace AutoBug;

int main()
{
    setlocale(LC_ALL, "");
    auto dataset = DataLoader::load("train.txt", DimMap::instance());
    auto kmeans = Kmeans(dataset, 10);
    kmeans.learn();
    kmeans.print();
}