#include "stdafx.h"
#include "IDGenerator.h"


IDGenerator::IDGenerator()
{
}


IDGenerator::~IDGenerator()
{
}


IDGenerator* IDGenerator::GetInstance()
{
    static IDGenerator gen;
    return &gen;
}


int IDGenerator::GenerateFreeIDForControlOf(CWnd* wnd)
{
    int id;
    do
    {
        id = MIN_ID + rand() % (MAX_ID - MIN_ID);
    } while(GetDlgItem(wnd->m_hWnd, id) != 0);
    return id;
}
