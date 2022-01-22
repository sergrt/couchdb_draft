#pragma once

#include "CdbClient.h"

class AttachmentClient {
 public:
    //std::vector<char> getAttachment() const;
    void uploadAttachment(const std::string& path);
    tirx::Observable<std::string>& getObservable();

 private:
    struct AttachmentClientProperties {
        std::string server_;
        std::string username_;
        std::string password_;
        std::string database_;
        std::string document_;
        std::string attachment_name_;
    };

 public:
    class AttachmentClientBuilder {
     public:
        AttachmentClientBuilder& with_server(const std::string& server);
        AttachmentClientBuilder& with_username(const std::string& username);
        AttachmentClientBuilder& with_password(const std::string& password);
        AttachmentClientBuilder& with_database(const std::string& database);
        AttachmentClientBuilder& with_document(const std::string& document);
        AttachmentClientBuilder& with_attachment_name(const std::string& attachment_name);
        AttachmentClient build() const;

     private:
        AttachmentClientProperties properties_;
    };

 private:
    AttachmentClient(const AttachmentClientProperties& properties);

    std::unique_ptr<CdbClient> db_client_;
    std::string doc_name_;
    std::string attachment_name_;
    tirx::Observable<std::string> operation_observable_;
};
