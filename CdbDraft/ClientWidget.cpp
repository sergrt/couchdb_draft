#include "ClientWidget.h"

#include <QTimer>
#include <QFontDatabase>

#include <string>

namespace {

std::string timestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream sstream;
    struct tm tm;
    if (localtime_s(&tm, &tt) == 0) {
        sstream << std::put_time(&tm, "%T") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    } else {
        static constexpr char invalid_timestamp[] = "XX:XX:XX.XXX";
        sstream << invalid_timestamp;
    }

    return sstream.str();
}

std::string log_string(const std::string& message) {
    return "[" + timestamp() + "] " + message + "\n";
}

}

ClientWidget::ClientWidget(int idx, QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    ui.groupBox->setTitle(QString("Client %1").arg(idx));
    ui.monitor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    connect(ui.open_connection_button, &QPushButton::clicked, this, [this] {
        openConnection();
        updateControls();
        if (!value_client_)
            return;

        getInitialValue();

        // Subscribe on changes
        value_subscription_ = value_client_->getObservable().subscribe([this](std::string value) {
            // "value" contains technical information, we just need new value

            // perform task in ui thread
            QTimer::singleShot(0, this, [this] {
                const auto new_value = value_client_->getValue();
                ui.monitor->insertPlainText(QString::fromStdString(log_string("<notification> " + new_value)));
                updateCurValue(new_value);
            });
        });
    });

    connect(ui.update_value_button, &QPushButton::clicked, this, [this]() {
        const auto value = ui.value->text().toStdString();
        ui.monitor->insertPlainText(QString::fromStdString(log_string("<set> " + value)));
        value_client_->setValue(value);
    });
}

ClientWidget::~ClientWidget() {
    value_subscription_.unsubscribe();
    value_client_.reset();
}

void ClientWidget::openConnection() {
    const auto server = ui.server->text().toStdString();
    const auto username = ui.username->text().toStdString();
    const auto password = ui.password->text().toStdString();
    const auto db_name = ui.db_name->text().toStdString();
    const auto doc_name = ui.doc_name->text().toStdString();
    const auto value_name = ui.value_name->text().toStdString();

    value_client_ = std::make_unique<ValueClient>(ValueClient::ValueClientBuilder()
                                                      .with_server(server)
                                                      .with_username(username)
                                                      .with_password(password)
                                                      .with_database(db_name)
                                                      .with_document(doc_name)
                                                      .with_value_name(value_name)
                                                      .build());
}

void ClientWidget::updateControls() {
    ui.update_value_button->setEnabled(value_client_ != nullptr);
    ui.open_connection_button->setEnabled(value_client_ == nullptr);
}

void ClientWidget::setServer(const std::string& address) {
    ui.server->setText(address.data());
}

void ClientWidget::getInitialValue() {
    const auto init_value = value_client_->getValue();
    ui.monitor->insertPlainText(QString::fromStdString(log_string("<initial> " + init_value)));
    updateCurValue(init_value);
}

void ClientWidget::updateCurValue(const std::string& value) {
    ui.cur_value->setText(QString::fromStdString(value));
}

