#pragma once
#include <vector>
using namespace std;


class IDGenerator
{
public:
    ~IDGenerator();
    static IDGenerator* GetInstance();
    int GenerateFreeIDForControlOf(CWnd*);
private:
    IDGenerator();
};
