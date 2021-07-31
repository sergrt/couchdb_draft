#pragma once

#include "CdbClient.h"

#include <string>

class ValueClient {

public:
    std::string getValue() const;
    void setValue(const std::string& value);
    tirx::Observable<std::string>& getObservable() const;

 private:
    struct ValueClientProperties {
        std::string server_;
        std::string username_;
        std::string password_;
        std::string database_;
        std::string document_;
        std::string value_name_;
    };

public:
    class ValueClientBuilder {
    public:
        ValueClientBuilder& with_server(const std::string& server);
        ValueClientBuilder& with_username(const std::string& username);
        ValueClientBuilder& with_password(const std::string& password);
        ValueClientBuilder& with_database(const std::string& database);
        ValueClientBuilder& with_document(const std::string& document);
        ValueClientBuilder& with_value_name(const std::string& value_name);
        ValueClient build() const;
    private:
        ValueClientProperties properties_;
    };

private:
    ValueClient(const ValueClientProperties& properties);

    std::unique_ptr<CdbClient> db_client_;
    std::string doc_name_;
    std::string value_name_;
};
