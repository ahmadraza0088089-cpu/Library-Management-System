#pragma once

#include "Book.hpp"
#include "Librarian.hpp"
#include "Member.hpp"
#include "Transaction.hpp"

#include <memory>
#include <string>
#include <vector>

namespace lms {

class Library {
public:
    Library(std::string name);

    const std::string& getName() const;

    bool addBook(Book book);
    bool removeBook(const std::string& isbn);
    std::vector<const Book*> searchBook(const std::string& query) const;
    const Book* findBookByISBN(const std::string& isbn) const;
    void displayCatalogue() const;
    void checkAvailability(const std::string& isbn) const;

    Member& registerMember(std::string name, std::string email);
    Librarian& registerLibrarian(std::string name, std::string email, std::string staffId);
    const std::vector<std::unique_ptr<Member>>& getMembers() const;
    Member* findMemberById(int memberId);
    const Member* findMemberById(int memberId) const;

    bool issueBook(int memberId, const std::string& isbn);
    double acceptReturn(int memberId, const std::string& isbn);
    void displayBorrowedBooks() const;
    void displayPendingReturns() const;
    void generateReport() const;

    void seedSampleData();

private:
    std::string name_;
    std::vector<std::unique_ptr<Book>> books_;
    std::vector<std::unique_ptr<Member>> members_;
    std::vector<std::unique_ptr<Transaction>> transactions_;

    int nextMemberId_;
    int nextTransactionId_;

    Book* findBookByISBNMutable(const std::string& isbn);
    Transaction* findActiveTransaction(int memberId, const std::string& isbn);
};

}  // namespace lms
