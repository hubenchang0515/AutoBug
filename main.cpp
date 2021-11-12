#include <cstdio>
#include "DimMap.h"
#include "Text.h"
#include "DataLoader.h"
#include "Kmeans.h"
#include "Accelerator.h"

using namespace AutoBug;

int main()
{
    setlocale(LC_ALL, "");
    if (Accelerator::instance().available())
    {
        printf("Use GPU: %s\n", Accelerator::instance().deviceName().c_str());
        printf("Work Size: %zu\n", Accelerator::instance().workSize());
    }
    auto dataset = DataLoader::load("train.txt", DimMap::instance());
    auto kmeans = Kmeans(dataset, 10);
    kmeans.learn(0.1f);
    kmeans.print();
}