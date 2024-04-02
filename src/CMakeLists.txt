cmake_minimum_required(VERSION 3.10)

# プロジェクト名と使用する言語を設定
project(SlackBot CXX)

# C++標準の設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# jsoncppのインクルードディレクトリを追加
include_directories(/usr/include/jsoncpp)
# websocketppのインクルードディレクトリを追加（必要に応じて）
include_directories(/usr/include/websocketpp)

# 実行ファイルを生成するためのソースファイルを指定
add_executable(slack_bot main.cpp)

# 必要なライブラリをリンク
target_link_libraries(slack_bot
    curl
    jsoncpp
    boost_system # websocketppが依存するBoostライブラリ
    pthread
)
