#pragma once

#include <string>

class UUIDGenerator final
{
private:
    UUIDGenerator() = delete;
    ~UUIDGenerator() = delete;

    UUIDGenerator(const UUIDGenerator& rhs) = delete;
    UUIDGenerator& operator =(const UUIDGenerator& rhs) = delete;

public:
    static std::string generate();

};


