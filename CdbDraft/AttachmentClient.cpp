#include "AttachmentClient.h"

#include <nlohmann/json.hpp>

#include <filesystem>

using namespace nlohmann;

AttachmentClient::AttachmentClient(const AttachmentClientProperties& properties) {
    db_client_ = std::make_unique<CdbClient>(properties.server_, properties.username_, properties.password_);
    db_client_->setDatabase(properties.database_);

    doc_name_ = properties.document_;
    attachment_name_ = properties.attachment_name_;
}
/*
std::vector<char> AttachmentClient::getAttachment() const {
    const auto response = db_client_->getDocumentAttachment(doc_name_, attachment_name_);
    //const auto json = json::parse(response);
    //return json[value_name_];
    return {};
}
*/


void AttachmentClient::uploadAttachment(const std::string& path) {
    

    if (std::filesystem::is_directory(path)) {
        using namespace std::chrono_literals;
        auto func = [this, path] {
            int cur_idx = 0;

            std::chrono::high_resolution_clock clock;
            std::chrono::duration<float> file_op_total;
            const auto op_begin = clock.now();
            for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator{path}) {
                auto f = dir_entry.path().filename().string();
                //operation_observable_.next(f);

                const auto doc_name = f;

                const auto response = db_client_->putDocument(doc_name);
                const auto json = json::parse(response);
                const auto rev = json["rev"];

                
                const auto file_op_start = clock.now();

                const auto file_size = std::filesystem::file_size(dir_entry);
                std::ifstream ifs(dir_entry.path().string(), std::ios::binary);
                std::vector<char> data((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
                const auto file_op_end = clock.now();

                const auto file_op_delta = file_op_end - file_op_start;
                file_op_total += file_op_delta;

                db_client_->putAttachment(doc_name, f, data, rev);
                //operation_observable_.next("End upload file " + f + " [" + std::to_string(++cur_idx) + "]");
                //std::this_thread::sleep_for(1s);
            }

            auto s_op = std::chrono::duration_cast<std::chrono::seconds>(clock.now() - op_begin);
            auto s_file = std::chrono::duration_cast<std::chrono::seconds>(file_op_total);
            std::string status = "Total time: " + std::to_string(s_op.count() - /*cur_idx*/0) +
                                 " sec, file op time: " + std::to_string(s_file.count()) + " sec";
            operation_observable_.next(status);
            
        };
        std::thread t(func);
        t.detach();
        
        /*
        for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator{file_name}) {
            auto f = dir_entry.path().filename().string();
            operation_observable_.next(f);

            const auto response = db_client_->getDocument(doc_name_);
            const auto json = json::parse(response);
            const auto rev = json["_rev"];

            const auto file_size = std::filesystem::file_size(file_name);
            std::ifstream ifs(file_name, std::ios::binary);
            std::vector<char> data((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            operation_observable_.next("Start upload file " + f);
            db_client_->putAttachment(doc_name_, f, data, rev);
            operation_observable_.next("End upload file " + f);
        }
        */
    } else {
        const auto response = db_client_->getDocument(doc_name_);
        const auto json = json::parse(response);
        const auto rev = json["_rev"];

        const auto file_size = std::filesystem::file_size(path);
        std::ifstream ifs(path, std::ios::binary);
        std::vector<char> data((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        operation_observable_.next("Start upload file");
        db_client_->putAttachment(doc_name_, attachment_name_, data, rev);
        operation_observable_.next("End upload file");
    }
    /*
    const auto file_size = std::filesystem::file_size(file_name);
    
    const int parts_count = static_cast<int>(std::ceil(file_size / part_size));

    // Write info about attachment parts count
    const auto response = db_client_->getDocument(doc_name_);
    auto json = json::parse(response);
    json[attachment_name_] = parts_count;
    db_client_->writeDocument(doc_name_, json.dump());

    // Upload parts
    for (int i = 0; i < parts_count; ++i) {
        const auto attachment_part_name = attachment_name_ + std::to_string(i);
        const auto response = db_client_->putDocumentAttachment(doc_name_, attachment_name_, attachment);
    }
    */
    /*
    const auto response = db_client_->putDocumentAttachment(doc_name_, attachment_name_, attachment);
    auto json = json::parse(response);
    json[value_name_] = attachment;
    db_client_->writeDocument(doc_name_, json.dump());
    */
}

tirx::Observable<std::string>& AttachmentClient::getObservable() {
    return operation_observable_;
}

//////////////////////////////////////

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_server(
    const std::string& server) {
    properties_.server_ = server;
    return *this;
}

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_username(
    const std::string& username) {
    properties_.username_ = username;
    return *this;
}

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_password(
    const std::string& password) {
    properties_.password_ = password;
    return *this;
}

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_database(
    const std::string& database) {
    properties_.database_ = database;
    return *this;
}

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_document(
    const std::string& document) {
    properties_.document_ = document;
    return *this;
}

AttachmentClient::AttachmentClientBuilder& AttachmentClient::AttachmentClientBuilder::with_attachment_name(
    const std::string& attachment_name) {
    properties_.attachment_name_ = attachment_name;
    return *this;
}

AttachmentClient AttachmentClient::AttachmentClientBuilder::build() const {
    return AttachmentClient(properties_);
}
