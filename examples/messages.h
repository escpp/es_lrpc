#pragma once
#include <sstream>
#include <string>
#include <string.h>
#include <stdint.h>

class People {
public:
    uint8_t m_gender = -1;
    uint8_t m_age = 0;
    char m_name[32];

public:
    People() = default;
    People(const char* name, uint8_t age, uint8_t gender)
    {
        init(name, age, gender);
    }

    void init(const char* name, uint8_t age, uint8_t gender)
    {
        m_age = age;
        m_gender = gender;
        snprintf(m_name, sizeof(m_name), "%s", name?name:"");
    }

    People& operator=(const People& other)
    {
        init(other.m_name, other.m_age, other.m_gender);
        return *this;
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "people:" << std::endl
            << "    name:" << m_name << std::endl
            << "    age:" << (int)m_age << std::endl
            << "    gender:" << (int)m_gender << std::endl
            ;

        return ss.str();
    }
};

class PeopleRequest {
public:
    uint8_t m_command = -1;
    People m_people;

public:
    PeopleRequest() = default;
    PeopleRequest(uint8_t command, const People &&people)
    {
        m_people = people;
        m_command = command;
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "PeopleRequest:" << std::endl
            << "    command:" << (int)m_command << std::endl
            << m_people.to_string()
            ;

        return ss.str();
    }
};

class PeopleResponse {
public:
    int m_code = -1;
    char m_message[128];
    People m_people;

public:
    PeopleResponse() = default;
    PeopleResponse(int code, const char* message, const People &people)
    {
        m_code = code;
        snprintf(m_message, sizeof(m_message), "%s", message?message:"");
        m_people = people;
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "PeopleResponse:" << std::endl
            << "    code:" << m_code << std::endl
            << "    message:" << m_message << std::endl
            << m_people.to_string()
            ;

        return ss.str();
    }
};