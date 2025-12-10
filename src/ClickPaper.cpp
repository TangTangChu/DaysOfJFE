#include "ClickPaper.h"

void ClickPaper::OnMouseUp(int x, int y) {
    if (OnMouseUpPreHook) {
        OnMouseUpPreHook();
    }
}
