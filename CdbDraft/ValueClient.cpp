#include "ValueClient.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;

ValueClient::ValueClient(const ValueClientProperties& properties) {
    db_client_ = std::make_unique<CdbClient>(properties.server_, properties.username_, properties.password_);
    db_client_->setDatabase(properties.database_);

    doc_name_ = properties.document_;
    value_name_ = properties.value_name_;
}

std::string ValueClient::getValue() const {
    const auto response = db_client_->getDocument(doc_name_);
    const auto json = json::parse(response);
    return json[value_name_];
}

void ValueClient::setValue(const std::string& value) {
    const auto response = db_client_->getDocument(doc_name_);
    auto json = json::parse(response);
    json[value_name_] = value;
    db_client_->writeDocument(doc_name_, json.dump());
}

tirx::Observable<std::string>& ValueClient::getObservable() const {
    return db_client_->getNewDataObservable();
}

//////////////////////////////////////

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_server(const std::string& server) {
    properties_.server_ = server;
    return *this;
}

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_username(const std::string& username) {
    properties_.username_ = username;
    return *this;
}

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_password(const std::string& password) {
    properties_.password_ = password;
    return *this;
}

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_database(const std::string& database) {
    properties_.database_ = database;
    return *this;
}

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_document(const std::string& document) {
    properties_.document_ = document;
    return *this;
}

ValueClient::ValueClientBuilder& ValueClient::ValueClientBuilder::with_value_name(const std::string& value_name) {
    properties_.value_name_ = value_name;
    return *this;
}

ValueClient ValueClient::ValueClientBuilder::build() const {
    return ValueClient(properties_);
}
