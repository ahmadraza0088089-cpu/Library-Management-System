#include "Transaction.hpp"

#include "Book.hpp"
#include "Member.hpp"

#include <iostream>
#include <string>

namespace lms {

namespace {

std::string statusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::Active:
            return "Active";
        case TransactionStatus::Returned:
            return "Returned";
        case TransactionStatus::Overdue:
            return "Overdue";
    }
    return "Unknown";
}

}  // namespace

Transaction::Transaction(int transactionId, Book& book, Member& member, const Date& issueDate)
    : transactionId_(transactionId),
      book_(book),
      member_(member),
      dateIssued_(issueDate),
      dueDate_(issueDate.addDays(LOAN_PERIOD_DAYS)),
      returnDate_(issueDate),
      returned_(false) {}

int Transaction::getId() const {
    return transactionId_;
}

Book& Transaction::getBook() const {
    return book_;
}

Member& Transaction::getMember() const {
    return member_;
}

const Date& Transaction::getIssueDate() const {
    return dateIssued_;
}

const Date& Transaction::getDueDate() const {
    return dueDate_;
}

const Date& Transaction::getReturnDate() const {
    return returnDate_;
}

bool Transaction::isReturned() const {
    return returned_;
}

TransactionStatus Transaction::getStatus() const {
    if (returned_) {
        return TransactionStatus::Returned;
    }
    if (Date::today() > dueDate_) {
        return TransactionStatus::Overdue;
    }
    return TransactionStatus::Active;
}

double Transaction::calculateFine(const Date& returnDate) const {
    if (!returned_ && returnDate <= dueDate_) {
        return 0.0;
    }

    const Date effectiveReturnDate = returned_ ? returnDate_ : returnDate;
    const int overdueDays = effectiveReturnDate.daysBetween(dueDate_);
    if (overdueDays <= 0) {
        return 0.0;
    }

    return static_cast<double>(overdueDays) * FINE_PER_DAY;
}

void Transaction::markReturned(const Date& returnDate) {
    returned_ = true;
    returnDate_ = returnDate;
}

void Transaction::display() const {
    std::cout << "  Transaction ID : " << transactionId_ << '\n'
              << "  Member         : " << member_.getName() << " (ID: " << member_.getId() << ")\n"
              << "  Book           : " << book_.getTitle() << " [" << book_.getISBN() << "]\n"
              << "  Issued On      : " << dateIssued_.toString() << '\n'
              << "  Due Date       : " << dueDate_.toString() << '\n';

    if (returned_) {
        std::cout << "  Returned On    : " << returnDate_.toString() << '\n';
    }

    std::cout << "  Status         : " << statusToString(getStatus()) << '\n'
              << "  Fine (Rs.)     : " << calculateFine() << '\n';
}

}  // namespace lms
