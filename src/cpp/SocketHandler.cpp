#include "SocketHandler.hpp"

// 例外処理
SocketError::SocketError(Error error, const std::string& msg)
    : std::runtime_error("\033[31m" + msg + "\033[0m"), errorCode(error) {}

SocketHandler::SocketHandler()
    : port_number(LoadPortNumber())
{
    // 受信待機遅延をなくすためノンブロッキング設定
    socket.setBlocking(false);
    if (socket.bind(port_number)!= sf::Socket::Status::Done) throw SocketError(SocketError::Error::FAILED_BINDING,"[Error] ポートバインドに失敗しました");
}

bool SocketHandler::receive()
{
    sf::Socket::Status status = socket.receive(&received_pos, sizeof(received_pos), received, sender, port);

    if (status == sf::Socket::Status::Done) return true;
    else if (status == sf::Socket::Status::Error) throw SocketError(SocketError::Error::SOCKET_NOT_VALID, "[Error] Socketは無効です");
    else return false;
}