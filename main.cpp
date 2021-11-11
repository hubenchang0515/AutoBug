#include <cstdio>
#include "DimMap.h"
#include "Text.h"
#include "DataLoader.h"

using namespace AutoBug;

int main()
{
    setlocale(LC_ALL, "");
    auto dataset = DataLoader::load("train.txt", DimMap::instance());
    printf("%zu\n", dataset.size());
    for (auto& data : dataset)
    {
        printf("%ls\n", data.text().c_str());
        data.print(DimMap::instance());
    }
}