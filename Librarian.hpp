#pragma once

#include "Member.hpp"

#include <string>
#include <vector>

namespace lms {

class Book;
class Library;

class Librarian : public Member {
public:
    Librarian(int memberId, std::string name, std::string email, std::string staffId);

    const std::string& getStaffId() const;
    std::string getRole() const override;

    bool addBook(Library& library, Book book);
    bool removeBook(Library& library, const std::string& isbn);
    std::vector<const Book*> searchBook(const Library& library, const std::string& query) const;
    void viewAllMembers(const Library& library) const;

private:
    std::string staffId_;
};

}  // namespace lms
