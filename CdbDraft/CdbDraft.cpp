#include "CdbDraft.h"

#include "ClientWidget.h"

constexpr int widgets_count = 2;
//const std::vector<std::string> predefined_servers = {"http://20.124.115.25:5984", "http://20.106.201.122:5984"};
const std::vector<std::string> predefined_servers = {"http://192.168.1.53:5984", "http://192.168.1.152:5984"};

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
