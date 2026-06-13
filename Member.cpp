#include "Member.hpp"

#include "Book.hpp"
#include "Transaction.hpp"

#include <algorithm>
#include <iostream>

namespace lms {

Member::Member(int memberId, std::string name, std::string email)
    : memberId_(memberId), name_(std::move(name)), email_(std::move(email)) {}

int Member::getId() const {
    return memberId_;
}

const std::string& Member::getName() const {
    return name_;
}

const std::string& Member::getEmail() const {
    return email_;
}

const std::vector<Book*>& Member::getBorrowedBooks() const {
    return borrowedBooks_;
}

const std::vector<Transaction*>& Member::getHistory() const {
    return history_;
}

bool Member::borrowBook(Book& book) {
    if (!book.isAvailable()) {
        return false;
    }

    book.markBorrowed();
    borrowedBooks_.push_back(&book);
    return true;
}

bool Member::returnBook(Book& book) {
    const auto iterator = std::find(borrowedBooks_.begin(), borrowedBooks_.end(), &book);
    if (iterator == borrowedBooks_.end()) {
        return false;
    }

    book.markReturned();
    borrowedBooks_.erase(iterator);
    return true;
}

void Member::addToHistory(Transaction* transaction) {
    if (transaction != nullptr) {
        history_.push_back(transaction);
    }
}

void Member::viewHistory() const {
    std::cout << "\nBorrowing history for " << name_ << " (ID: " << memberId_ << ")\n";
    std::cout << std::string(52, '-') << '\n';

    if (history_.empty()) {
        std::cout << "  No transactions recorded.\n";
        return;
    }

    for (const Transaction* transaction : history_) {
        transaction->display();
        std::cout << '\n';
    }
}

std::string Member::getRole() const {
    return "Member";
}

}  // namespace lms
