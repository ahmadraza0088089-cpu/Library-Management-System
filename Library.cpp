#include "Library.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

namespace lms {

namespace {

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return value;
}

bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}

void printHeader(const std::string& title) {
    std::cout << '\n' << title << '\n' << std::string(title.size(), '=') << '\n';
}

}  // namespace

Library::Library(std::string name)
    : name_(std::move(name)), nextMemberId_(1001), nextTransactionId_(5001) {}

const std::string& Library::getName() const {
    return name_;
}

bool Library::addBook(Book book) {
    if (findBookByISBNMutable(book.getISBN()) != nullptr) {
        std::cout << "  Error: A book with ISBN " << book.getISBN() << " already exists.\n";
        return false;
    }

    books_.push_back(std::make_unique<Book>(std::move(book)));
    std::cout << "  Book added successfully.\n";
    return true;
}

bool Library::removeBook(const std::string& isbn) {
    const auto iterator = std::find_if(books_.begin(), books_.end(),
                                       [&](const std::unique_ptr<Book>& book) { return book->getISBN() == isbn; });

    if (iterator == books_.end()) {
        std::cout << "  Error: Book with ISBN " << isbn << " not found.\n";
        return false;
    }

    if (!(*iterator)->isAvailable()) {
        std::cout << "  Error: Cannot remove a book that is currently borrowed.\n";
        return false;
    }

    books_.erase(iterator);
    std::cout << "  Book removed successfully.\n";
    return true;
}

std::vector<const Book*> Library::searchBook(const std::string& query) const {
    std::vector<const Book*> results;

    for (const auto& book : books_) {
        if (containsIgnoreCase(book->getTitle(), query) || containsIgnoreCase(book->getAuthor(), query) ||
            containsIgnoreCase(book->getISBN(), query) || containsIgnoreCase(book->getGenre(), query)) {
            results.push_back(book.get());
        }
    }

    return results;
}

const Book* Library::findBookByISBN(const std::string& isbn) const {
    for (const auto& book : books_) {
        if (book->getISBN() == isbn) {
            return book.get();
        }
    }
    return nullptr;
}

void Library::displayCatalogue() const {
    printHeader("Book Catalogue — " + name_);

    if (books_.empty()) {
        std::cout << "  No books in catalogue.\n";
        return;
    }

    for (const auto& book : books_) {
        book->displayInfo();
        std::cout << std::string(40, '-') << '\n';
    }
}

void Library::checkAvailability(const std::string& isbn) const {
    const Book* book = findBookByISBN(isbn);
    if (book == nullptr) {
        std::cout << "  Book with ISBN " << isbn << " was not found.\n";
        return;
    }

    std::cout << "\nAvailability Check\n";
    std::cout << std::string(40, '-') << '\n';
    book->displayInfo();
}

Member& Library::registerMember(std::string name, std::string email) {
    const int memberId = nextMemberId_++;
    auto member = std::make_unique<Member>(memberId, std::move(name), std::move(email));
    Member& reference = *member;
    members_.push_back(std::move(member));

    std::cout << "  Member registered successfully. Assigned ID: " << memberId << '\n';
    return reference;
}

Librarian& Library::registerLibrarian(std::string name, std::string email, std::string staffId) {
    const int memberId = nextMemberId_++;
    auto librarian = std::make_unique<Librarian>(memberId, std::move(name), std::move(email), std::move(staffId));
    Librarian& reference = *librarian;
    members_.push_back(std::move(librarian));

    std::cout << "  Librarian registered successfully. Assigned ID: " << memberId << '\n';
    return reference;
}

const std::vector<std::unique_ptr<Member>>& Library::getMembers() const {
    return members_;
}

Member* Library::findMemberById(int memberId) {
    const auto iterator = std::find_if(members_.begin(), members_.end(),
                                       [memberId](const std::unique_ptr<Member>& member) {
                                           return member->getId() == memberId;
                                       });
    return iterator != members_.end() ? iterator->get() : nullptr;
}

const Member* Library::findMemberById(int memberId) const {
    const auto iterator = std::find_if(members_.begin(), members_.end(),
                                       [memberId](const std::unique_ptr<Member>& member) {
                                           return member->getId() == memberId;
                                       });
    return iterator != members_.end() ? iterator->get() : nullptr;
}

bool Library::issueBook(int memberId, const std::string& isbn) {
    Member* member = findMemberById(memberId);
    if (member == nullptr) {
        std::cout << "  Error: Member ID " << memberId << " not found.\n";
        return false;
    }

    Book* book = findBookByISBNMutable(isbn);
    if (book == nullptr) {
        std::cout << "  Error: Book with ISBN " << isbn << " not found.\n";
        return false;
    }

    if (!member->borrowBook(*book)) {
        std::cout << "  Error: Book is not available for borrowing.\n";
        return false;
    }

    auto transaction = std::make_unique<Transaction>(nextTransactionId_++, *book, *member, Date::today());
    Transaction* transactionPointer = transaction.get();
    member->addToHistory(transactionPointer);
    transactions_.push_back(std::move(transaction));

    std::cout << "  Book issued successfully.\n"
              << "  Due date: " << transactionPointer->getDueDate().toString() << " ("
              << Transaction::LOAN_PERIOD_DAYS << " days from today)\n";
    return true;
}

double Library::acceptReturn(int memberId, const std::string& isbn) {
    Member* member = findMemberById(memberId);
    if (member == nullptr) {
        std::cout << "  Error: Member ID " << memberId << " not found.\n";
        return 0.0;
    }

    Book* book = findBookByISBNMutable(isbn);
    if (book == nullptr) {
        std::cout << "  Error: Book with ISBN " << isbn << " not found.\n";
        return 0.0;
    }

    Transaction* transaction = findActiveTransaction(memberId, isbn);
    if (transaction == nullptr) {
        std::cout << "  Error: No active loan found for this member and book.\n";
        return 0.0;
    }

    const Date returnDate = Date::today();
    const double fine = transaction->calculateFine(returnDate);
    transaction->markReturned(returnDate);

    if (!member->returnBook(*book)) {
        std::cout << "  Error: Failed to process return.\n";
        return 0.0;
    }

    std::cout << "  Book returned successfully.\n";
    if (fine > 0.0) {
        std::cout << "  Overdue fine: Rs. " << fine << " (Rs. " << Transaction::FINE_PER_DAY
                  << " per day)\n";
    } else {
        std::cout << "  No overdue fine.\n";
    }

    return fine;
}

void Library::displayBorrowedBooks() const {
    printHeader("Currently Borrowed Books");

    bool found = false;
    for (const auto& transaction : transactions_) {
        if (!transaction->isReturned()) {
            found = true;
            transaction->display();
            std::cout << std::string(40, '-') << '\n';
        }
    }

    if (!found) {
        std::cout << "  No books are currently borrowed.\n";
    }
}

void Library::displayPendingReturns() const {
    printHeader("Pending Returns (Active & Overdue)");

    bool found = false;
    for (const auto& transaction : transactions_) {
        if (!transaction->isReturned()) {
            found = true;
            transaction->display();
            if (transaction->getStatus() == TransactionStatus::Overdue) {
                std::cout << "  >> OVERDUE — Estimated fine: Rs. " << transaction->calculateFine() << '\n';
            }
            std::cout << std::string(40, '-') << '\n';
        }
    }

    if (!found) {
        std::cout << "  No pending returns.\n";
    }
}

void Library::generateReport() const {
    printHeader("Library Report — " + name_);

    const int totalBooks = static_cast<int>(books_.size());
    const int availableBooks =
        static_cast<int>(std::count_if(books_.begin(), books_.end(),
                                       [](const std::unique_ptr<Book>& book) { return book->isAvailable(); }));
    const int activeLoans = static_cast<int>(std::count_if(
        transactions_.begin(), transactions_.end(),
        [](const std::unique_ptr<Transaction>& transaction) { return !transaction->isReturned(); }));
    const int overdueLoans = static_cast<int>(std::count_if(
        transactions_.begin(), transactions_.end(), [](const std::unique_ptr<Transaction>& transaction) {
            return !transaction->isReturned() && transaction->getStatus() == TransactionStatus::Overdue;
        }));

    std::cout << "  Total Books in Catalogue : " << totalBooks << '\n'
              << "  Available Books          : " << availableBooks << '\n'
              << "  Borrowed Books           : " << (totalBooks - availableBooks) << '\n'
              << "  Registered Members       : " << members_.size() << '\n'
              << "  Active Loans             : " << activeLoans << '\n'
              << "  Overdue Loans            : " << overdueLoans << '\n'
              << "  Total Transactions       : " << transactions_.size() << '\n';
}

void Library::seedSampleData() {
    addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", "978-0743273565", "Fiction"));
    addBook(Book("To Kill a Mockingbird", "Harper Lee", "978-0061120084", "Fiction"));
    addBook(Book("1984", "George Orwell", "978-0451524935", "Dystopian"));
    addBook(Book("Clean Code", "Robert C. Martin", "978-0132350884", "Technology"));
    addBook(Book("The Pragmatic Programmer", "David Thomas", "978-0201616224", "Technology"));

    registerMember("Alice Johnson", "alice@email.com");
    registerMember("Bob Smith", "bob@email.com");
    registerLibrarian("Sarah Williams", "sarah@library.gov", "LIB-001");
}

Book* Library::findBookByISBNMutable(const std::string& isbn) {
    const auto iterator = std::find_if(books_.begin(), books_.end(),
                                       [&](const std::unique_ptr<Book>& book) { return book->getISBN() == isbn; });
    return iterator != books_.end() ? iterator->get() : nullptr;
}

Transaction* Library::findActiveTransaction(int memberId, const std::string& isbn) {
    for (const auto& transaction : transactions_) {
        if (!transaction->isReturned() && transaction->getMember().getId() == memberId &&
            transaction->getBook().getISBN() == isbn) {
            return transaction.get();
        }
    }
    return nullptr;
}

}  // namespace lms
