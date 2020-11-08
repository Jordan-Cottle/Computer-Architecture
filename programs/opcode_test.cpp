#include "test.h"
using namespace Simulation;

#include "opcodes.h"

int main()
{
    assert(getImmediateS(0xfe000f80) == 0xFFF);

    for (int i = 0; i < 32; i++)
    {
        assert(setBit(0, i, 1) == 1u << i);
    }

    assert(getImmediateSB(0xfe000f80) == (0xFFF << 1));

    assert(getImmediateU(0xABCDEF12) == 0xABCDE000);
    assert(getImmediateUB(0xABCDEF12) == 0x001DE2BC);

    assert(sign_extend(1024, 11) == 1024);
    assert((int)sign_extend(1, 0) == -1);
    assert(sign_extend(2048, 11) == 0xFFFFF800);
    assert(sign_extend(2048, 11) + (2048 << 1) == 0x00000800);
    assert((int)sign_extend(4, 2) == -4);
    assert((int)sign_extend(-4, 2) == -4);

    return 0;
}