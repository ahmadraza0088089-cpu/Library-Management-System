#pragma once

#include "DateUtils.hpp"

#include <string>

namespace lms {

class Book;
class Member;

enum class TransactionStatus {
    Active,
    Returned,
    Overdue
};

class Transaction {
public:
    static constexpr int LOAN_PERIOD_DAYS = 14;
    static constexpr double FINE_PER_DAY = 5.0;

    Transaction(int transactionId, Book& book, Member& member, const Date& issueDate);

    int getId() const;
    Book& getBook() const;
    Member& getMember() const;
    const Date& getIssueDate() const;
    const Date& getDueDate() const;
    const Date& getReturnDate() const;
    bool isReturned() const;
    TransactionStatus getStatus() const;

    double calculateFine(const Date& returnDate = Date::today()) const;
    void markReturned(const Date& returnDate = Date::today());
    void display() const;

private:
    int transactionId_;
    Book& book_;
    Member& member_;
    Date dateIssued_;
    Date dueDate_;
    Date returnDate_;
    bool returned_;
};

}  // namespace lms
