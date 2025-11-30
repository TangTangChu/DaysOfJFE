#include "ui/ClickPaper.h"

void ClickPaper::OnMouseUp(int, int) {
    if (!IsEnabled())
        return;
    Controls::OnMouseUp(0, 0);
}
