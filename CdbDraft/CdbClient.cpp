#include "CdbClient.h"

CdbClient::CdbClient(const std::string& server, const std::string& username, const std::string& password) {
    http_client_ = std::make_unique<httplib::Client>(server.c_str());
    http_client_->set_connection_timeout(3, 0);
    http_client_->set_basic_auth(username.c_str(), password.c_str());
    
    // prepare feed client
    feed_client_ = std::make_unique<httplib::Client>(server.c_str());
    feed_client_->set_basic_auth(username.c_str(), password.c_str());
    feed_client_->set_connection_timeout(3, 0);
    const time_t very_long_timeout = 99999;
    feed_client_->set_read_timeout(very_long_timeout);
    feed_thread_ = std::thread(&CdbClient::feedFunc, this);
}

CdbClient::~CdbClient() {
    http_client_.reset();
    feed_stop_ = true;
    feed_thread_.join();
    feed_client_.reset();
}

void CdbClient::setDatabase(const std::string& db_name) {
    db_name_ = db_name;
}

std::string CdbClient::getDocument(const std::string& doc_name) const {
    const auto path = "/" + db_name_ + "/" + doc_name + "?conflicts=true";
    const auto res = http_client_->Get(path.c_str());
    return res->body;
}

void CdbClient::writeDocument(const std::string& doc_name, const std::string& body) {
    const auto path = "/" + db_name_ + "/" + doc_name;
    const auto res = http_client_->Put(path.c_str(), body, "application/json");
    //check res for write result if needed
}

void CdbClient::feedFunc() {
    const auto path = "/" + db_name_ + "/_changes?feed=continuous&heartbeat=5&since=now";

    httplib::ResponseHandler response_handler = [](const httplib::Response& r) {
        // handle connection response here
        return true;
    };

    httplib::ContentReceiver content_rcv = [this](const char* data, size_t len) {
        if (feed_stop_)
            return false;

        static const int heartbeat_len = 1;
        if (len > heartbeat_len)
            new_data_observable_.next(std::string(data, len));

        return true;
    };

    while (!feed_stop_) {
        auto res = feed_client_->Get(path.c_str(), response_handler, content_rcv);
    }
}

tirx::Observable<std::string>& CdbClient::getNewDataObservable() {
    return new_data_observable_;
}
