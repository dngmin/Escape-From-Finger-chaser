#pragma once
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "env_load.hpp"

// 例外処理
class SocketError : public std::runtime_error
{
public:
    enum class Error
    {
        FAILED_BINDING = -1,
        SOCKET_NOT_VALID = -2
    };

    // 初期化
    explicit SocketError(Error error, const std::string& msg);
    
    // エラーコード取得
    Error getError() const noexcept {return errorCode;}

private:
    Error errorCode;
};

class SocketSetting
{
public:
    void init();

    sf::Vector2f get_received_pos() const
    {
        return received_pos;
    }

    // 受信したらtrueをreturn
    bool receive();

private:
    const int port_number = get_port_number();
    sf::UdpSocket socket;
    std::size_t received;
    std::optional<sf::IpAddress> sender;
    unsigned short port;
    sf::Vector2f received_pos;
};