#pragma once

#include "ui_ClientWidget.h"

#include "ValueClient.h"
#include "AttachmentClient.h"

#include <Subscription.h>

#include <memory>

class ClientWidget : public QWidget {
    Q_OBJECT

public:
    ClientWidget(int idx, QWidget* parent = Q_NULLPTR);
    ~ClientWidget() override;

    void setServer(const std::string& address);

private:
    Ui::ClientWidget ui;
    std::unique_ptr<ValueClient> value_client_;
    tirx::Subscription value_subscription_;

    std::unique_ptr<AttachmentClient> attachment_client_;
    tirx::Subscription attachment_operation_subscription_;

    void openConnection();
    void updateControls();
    void getInitialValue();
    void updateCurValue(const std::string& value);

    void uploadFile();
};

