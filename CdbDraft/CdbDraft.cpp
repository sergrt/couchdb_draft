#include "CdbDraft.h"

#include "ClientWidget.h"

constexpr int widgets_count = 2;
const std::vector<std::string> predefined_servers = {};

CdbDraft::CdbDraft(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);

    auto* layout = new QGridLayout{ui.frame};
    for (int i = 0; i < widgets_count; ++i) {
        auto* w = new ClientWidget{i + 1, this};
        if (i < predefined_servers.size())
            w->setServer(predefined_servers[i]);
        layout->addWidget(w, 0, i);
    }
    ui.frame->setLayout(layout);
}
