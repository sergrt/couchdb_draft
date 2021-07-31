#include "CdbDraft.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    CdbDraft w;
    w.show();
    return a.exec();
}
