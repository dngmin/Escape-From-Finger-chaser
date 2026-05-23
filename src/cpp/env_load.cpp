#include "env_load.hpp"
#include <fstream>
#include <sstream>

// 例外処理
envError::envError(Error error, const std::string& msg)
    : std::runtime_error("\033[31m" + msg + "\033[0m"), errorCode(error) {}


int get_port_number(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) throw envError(envError::Error::FILE_NOT_FOUND,"[Error] .envファイルを開くことができません");

    // 読み込みアルゴリズム
    // .envを一行ずつ読み込む。
    // 空白や'#'で始まるコメントであれば無視する。
    // '='があるか確認し、ないと無視する。
    // '='を空白に変換し、分かれている二つの文字列を取得
    // このアルゴリズムより書き方によるデータ読み込み失敗を防ぐ
    std::string line;
    while (std::getline(file,line))
    {
        if (line.empty() || line[0] == '#') continue;

        size_t split_point = line.find('=');
        if (split_point != std::string::npos)
        {
            line[split_point] = ' ';
            std::stringstream ss(line);
            std::string key, value;
            ss >> key >> value;
            if (key == "port_number")
            {
                size_t processed_length;
                int port_number;
                try
                {
                    port_number = std::stoi(value, &processed_length);
                }
                catch (const std::exception& e)
                {
                    throw envError(envError::Error::PORT_NUMBER_NOT_NONNEGATIVE_INTEGER,"[Error] port numberは非負整数である必要があります");
                }
                if (processed_length != value.length() || port_number <= 0)
                {
                    throw envError(envError::Error::PORT_NUMBER_NOT_NONNEGATIVE_INTEGER,"[Error] port numberは非負整数である必要があります");
                }
                return port_number;
            }
        }
    }
    throw envError(envError::Error::PORT_NUMBER_NOT_FOUND, "[Error] port numberが見つかりません");
}