/**
 * ȫ��Ψһ��UUID���ɹ����࣬Windows��ʵ��ʹ�õ���GUID, UUIDGenerator.cpp
 */

#include "UUIDGenerator.h"

#include <uuid/uuid.h>

std::string UUIDGenerator::generate()
{
    uuid_t uuid;
    char str[40] = { 0 };

    uuid_generate(uuid);
    uuid_unparse(uuid, str);
    return std::string(str, 36);
}
