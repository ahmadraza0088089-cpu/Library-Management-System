#pragma once

#include <string>
#include <vector>

namespace lms {

class Book;
class Transaction;

class Member {
public:
    Member(int memberId, std::string name, std::string email);
    virtual ~Member() = default;

    int getId() const;
    const std::string& getName() const;
    const std::string& getEmail() const;
    const std::vector<Book*>& getBorrowedBooks() const;
    const std::vector<Transaction*>& getHistory() const;

    bool borrowBook(Book& book);
    bool returnBook(Book& book);
    void addToHistory(Transaction* transaction);
    void viewHistory() const;

    virtual std::string getRole() const;

protected:
    int memberId_;
    std::string name_;
    std::string email_;
    std::vector<Book*> borrowedBooks_;
    std::vector<Transaction*> history_;
};

}  // namespace lms
