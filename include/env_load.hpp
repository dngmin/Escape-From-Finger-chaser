#pragma once
#include <string>
#include <stdexcept>

class envError : public std::runtime_error
{
public:
    enum class Type 
    {
        FILE_NOT_FOUND = -1,
        PORT_NUMBER_NOT_NONNEGATIVE_INTEGER = -2,
        PORT_NUMBER_NOT_FOUND = -3
    };
    explicit envError(Type type, const std::string& msg);
    
    Type getType() const noexcept {return error_type;}

private:
    Type error_type;
};

int get_port_number(const std::string& filename = "../.env");