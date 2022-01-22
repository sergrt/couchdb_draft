#pragma once

#include <httplib.h>

#include <tirx.h>

#include <string>
#include <memory>
#include <atomic>

class CdbClient {
public:
    CdbClient(const std::string& server, const std::string& username, const std::string& password);

    CdbClient(const CdbClient&) = delete;
    CdbClient(CdbClient&&) = delete;
    CdbClient& operator=(const CdbClient&) = delete;
    CdbClient& operator=(CdbClient&&) = delete;

    ~CdbClient();

    void setDatabase(const std::string& db_name);
    std::string getDocument(const std::string& doc_name) const;
    void writeDocument(const std::string& doc_name, const std::string& body);
    void feedFunc();

    std::vector<char> getDocumentAttachment(const std::string& doc_name, const std::string& attachment_name) const;
    void putAttachment(const std::string& doc_name, const std::string& attachment_name, const std::vector<char>& data, const std::string& rev);
    std::string putDocument(const std::string& doc_name);

    tirx::Observable<std::string>& getNewDataObservable();

private:
    std::unique_ptr<httplib::Client> http_client_;
    std::unique_ptr<httplib::Client> feed_client_;
    std::thread feed_thread_;
    std::atomic_bool feed_stop_{false};
    std::string db_name_;
    tirx::Observable<std::string> new_data_observable_;
};
