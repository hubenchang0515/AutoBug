#include <cstdio>
#include "DimMap.h"
#include "Text.h"

using namespace AutoBug;

int main()
{
    setlocale(LC_ALL, "");
    DimMap dimMap;
    Text text1, text2, text3;
    text1.setText(u8"开启无密码登录，密码过期后密码输入框文案为英文", dimMap);
    text2.setText(u8"在系统内打开控制中心-电源管理-通用，设置为节能模式，快速修改自动降低亮度处的百分比，出现屏幕闪烁", dimMap);
    text3.setText(u8"拖拽控制中心-电源管理-降低亮度按钮，发现屏幕长时间闪烁", dimMap);
    printf("1-2 %f\n", text1.distance(text2));
    printf("1-3 %f\n", text1.distance(text3));
    printf("2-3 %f\n", text2.distance(text3));
}