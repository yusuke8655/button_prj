# Ubuntu 20.04をベースイメージとして使用
FROM ubuntu:20.04

# タイムゾーンの環境変数を設定
ENV TZ=Asia/Tokyo

# タイムゾーンを設定
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 以降は以前のDockerfileの内容を続ける
# 必要なパッケージのインストール
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    gdb \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    libboost-all-dev \
    libwebsocketpp-dev \
    libjsoncpp-dev \
    && rm -rf /var/lib/apt/lists/*

# 作業ディレクトリの設定
# WORKDIR /usr/src/app
WORKDIR /usr/src

# ホストマシンのソースコードとCMakeLists.txtをコンテナ内にコピー
COPY ./src .

# CMakeを使用してビルドプロセスを実行
# RUN cmake . && make

# コンテナ起動時にプログラムを実行
# CMD ["./slack_bot"]
