#include "../lib/processing/processing.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct Student {
    uint64_t group_id;
    std::string name;

    bool operator==(const Student& other) const {
        return group_id == other.group_id && name == other.name;
    }

};

std::ostream& operator<<(std::ostream& out, const Student& val) {
    out << "{" << val.group_id << ", " << val.name << "}"; 
    return out;
}

struct Group {
    uint64_t id;
    std::string name;

    bool operator==(const Group& other) const {
        return id == other.id && name == other.name;
    }
};

std::ostream& operator<<(std::ostream& out, const Group& val) {
    out << "{" << val.id << ", " << val.name << "}"; 
    return out;
}


TEST(SimpleTest, JoinKV) {
    std::vector<KV<int, std::string>> left = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {1, "e"}};
    std::vector<KV<int, std::string>> right = {{0, "f"}, {1, "g"}, {3, "i"}};

    auto left_flow = AsDataFlow(left);
    auto right_flow = AsDataFlow(right);
    auto result = left_flow | Join(right_flow) | AsVector();

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"a", "f"},
            JoinResult<std::string, std::string>{"b", "g"},
            JoinResult<std::string, std::string>{"c", std::nullopt},
            JoinResult<std::string, std::string>{"d", "i"},
            JoinResult<std::string, std::string>{"e", "g"}
        )
    );
}

TEST(SimpleTest, JoinComparators) {
    std::vector<Student> students = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {1, "e"}};
    std::vector<Group> groups = {{0, "f"}, {1, "g"}, {3, "i"}};

    auto students_flow = AsDataFlow(students);
    auto groups_flow = AsDataFlow(groups);

    auto result =
        students_flow
            | Join(
                groups_flow,
                [](const Student& student) { return student.group_id; },
                [](const Group& group) { return group.id; })
            | AsVector()
    ;

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<Student, Group>{Student{0, "a"}, Group{0, "f"}},
            JoinResult<Student, Group>{Student{1, "b"}, Group{1, "g"}},
            JoinResult<Student, Group>{Student{2, "c"}, std::nullopt},
            JoinResult<Student, Group>{Student{3, "d"}, Group{3, "i"}},
            JoinResult<Student, Group>{Student{1, "e"}, Group{1, "g"}}
        )
    );
}
