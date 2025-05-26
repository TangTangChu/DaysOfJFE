#include "stdafx.h"
#include "ClickPaper.h"

void ClickPaper::OnMouseUp(int x, int y) {
	if (onClick) {
		onClick();
	}
}