#pragma once

#include "ui_CdbDraft.h"

#include <QtWidgets/QMainWindow>

class CdbDraft : public QMainWindow {
    Q_OBJECT

public:
    CdbDraft(QWidget *parent = Q_NULLPTR);

private:
    Ui::CdbDraftClass ui;
};
