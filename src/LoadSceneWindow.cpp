#include "LoadSceneWindow.h"

LoadSceneWindow::LoadSceneWindow(std::function<void()> b1, 
                                 std::function<void()> b2, 
                                 std::function<void()> b3,
                                 std::function<void()> b4, 
                                 std::function<void()> b5,
                                 std::function<void()> bh) 
    : backHook(bh) {
    
    btn_1 = std::make_shared<PrimaryButton>("场景1", b1);
    btn_2 = std::make_shared<PrimaryButton>("场景2", b2);
    btn_3 = std::make_shared<PrimaryButton>("场景3", b3);
    btn_4 = std::make_shared<PrimaryButton>("场景4", b4);
    btn_5 = std::make_shared<PrimaryButton>("场景5", b5);

    btn_1->SetPosition(40, 600);
    btn_1->SetSize(120, 40);
    btn_1->SetFontSize(24);

    btn_2->SetPosition(180, 600);
    btn_2->SetSize(120, 40);
    btn_2->SetFontSize(24);

    btn_3->SetPosition(320, 600);
    btn_3->SetSize(120, 40);
    btn_3->SetFontSize(24);

    btn_4->SetPosition(460, 600);
    btn_4->SetSize(120, 40);
    btn_4->SetFontSize(24);

    btn_5->SetPosition(600, 600);
    btn_5->SetSize(120, 40);
    btn_5->SetFontSize(24);

    auto btn_bk = std::make_shared<PrimaryButton>("返回", [this]() {
        if (backHook) backHook(); 
    });
    btn_bk->SetPosition(40, 800);
    btn_bk->SetSize(200, 50);
    btn_bk->SetFontSize(25);
    
    tbk1 = std::make_shared<TextBlock>(
        "还没有写好 5 个场景，所以先用这些按钮凑数，应应急的吧", 
        30, 
        SkColorSetARGB(255, 0, 191, 255)
    );
    tbk1->SetRect(SkRect::MakeWH(1600, 900));
    
    AddControl(tbk1);
    AddControl(btn_1);
    AddControl(btn_2);
    AddControl(btn_3);
    AddControl(btn_4);
    AddControl(btn_5);
    AddControl(btn_bk);

    OnMouseRBUPHook = [this]() { 
        if (backHook) backHook(); 
    };
    
    OnKeyDownHook = [this](SDL_Keycode key) {
        if (key == SDLK_ESCAPE) {
            if (backHook) backHook();
        }
    };
}
