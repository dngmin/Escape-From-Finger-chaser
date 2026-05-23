#pragma once
#include <string>
#include <stdexcept>

// 例外処理
class envError : public std::runtime_error
{
public:
    enum class Error 
    {
        FILE_NOT_FOUND = -1,
        PORT_NUMBER_NOT_NONNEGATIVE_INTEGER = -2,
        PORT_NUMBER_NOT_FOUND = -3
    };

    // 初期化
    explicit envError(Error error, const std::string& msg);
    
    // エラーコード取得
    Error getError() const noexcept {return errorCode;}

private:
    Error errorCode;
};

// port number取得
int get_port_number(const std::string& filename = "../.env");