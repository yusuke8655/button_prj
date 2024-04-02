#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <jsoncpp/json/json.h>
#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std::placeholders;
typedef websocketpp::client<websocketpp::config::asio_client> client;

// cURLのレスポンスデータを受け取るためのコールバック関数
size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Slack APIトークン
const std::string SLACK_APP_TOKEN = "";

// cURLライブラリを使用してSlack APIにリクエストを送信し、WebSocket URLを取得する関数
std::string getWebSocketUrl() {
    CURL* curl = curl_easy_init();
    std::string response_data;
    std::string websocket_url;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + SLACK_APP_TOKEN).c_str());

        // cURLオプションを設定
        curl_easy_setopt(curl, CURLOPT_URL, "https://slack.com/api/apps.connections.open");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // HTTPリクエストを実行
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        } else {
            // レスポンスデータを解析してWebSocket URLを取得
            Json::Value json;
            Json::Reader reader;
            if (reader.parse(response_data, json)) {
                websocket_url = json["url"].asString();
            }
        }

        // リソースを解放
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return websocket_url;
}

// cURLライブラリを使用してSlackのresponse_urlにメッセージを投稿する関数
void postInteractiveMessageResponse(const std::string& response_url, const std::string& message) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // JSON形式の投稿データを作成
        std::string data = "{\"text\":\"" + message + "\"}";

        // cURLオプションを設定
        curl_easy_setopt(curl, CURLOPT_URL, response_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        // HTTPリクエストを実行
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        // リソースを解放
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}

// WebSocketメッセージハンドラ
void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    // 受信したメッセージを解析
    Json::Value json;
    Json::Reader reader;
    if (!reader.parse(msg->get_payload(), json)) {
        std::cerr << "Failed to parse message\n";
        return;
    }

    // Slackからのイベントを処理
    std::string type = json["type"].asString();
    if (type == "interactive_message") {
        // ボタンが押されたイベントを処理
        std::string response_url = json["response_url"].asString();

        // 応答メッセージを投稿
        postInteractiveMessageResponse(response_url, "ボタンが押されました！");
    }
}

int main() {
    // WebSocket URLを取得
    std::string slack_ws_url = getWebSocketUrl();
    std::cout << "WebSocket URL: " << slack_ws_url << std::endl;

    // WebSocketクライアントの初期化
    client c;

    try {
        // メッセージハンドラの設定
        c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

        // WebSocket接続の開始
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(slack_ws_url, ec);
        if (ec) {
            std::cerr << "Could not create connection because: " << ec.message() << "\n";
            return 0;
        }

        c.connect(con);

        // イベントループの開始
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
