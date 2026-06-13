/*
 * Library Management System - Single File Version
 * All classes + all program logic inside main()
 * OOP: Book, Member, Librarian, Transaction, Library, Date
 */

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ============================================================================
// CLASS: Date
// ============================================================================
class Date {
public:
    Date() : daysSinceEpoch_(toDays(std::chrono::system_clock::now())) {}
    explicit Date(const std::chrono::system_clock::time_point& tp) : daysSinceEpoch_(toDays(tp)) {}

    static Date today() { return Date(); }
    static Date fromDaysSinceEpoch(int days) {
        Date d;
        d.daysSinceEpoch_ = days;
        return d;
    }

    Date addDays(int days) const { return fromDaysSinceEpoch(daysSinceEpoch_ + days); }
    int daysBetween(const Date& other) const { return daysSinceEpoch_ - other.daysSinceEpoch_; }
    std::string toString() const {
        const auto tp = std::chrono::system_clock::from_time_t(
            static_cast<std::time_t>(daysSinceEpoch_) * 24 * 60 * 60);
        std::tm localTime = toLocalTm(tp);
        std::ostringstream stream;
        stream << std::put_time(&localTime, "%Y-%m-%d");
        return stream.str();
    }

    bool operator<(const Date& o) const { return daysSinceEpoch_ < o.daysSinceEpoch_; }
    bool operator<=(const Date& o) const { return daysSinceEpoch_ <= o.daysSinceEpoch_; }
    bool operator>(const Date& o) const { return daysSinceEpoch_ > o.daysSinceEpoch_; }
    bool operator==(const Date& o) const { return daysSinceEpoch_ == o.daysSinceEpoch_; }

private:
    int daysSinceEpoch_;

    static std::tm toLocalTm(const std::chrono::system_clock::time_point& tp) {
        const std::time_t time = std::chrono::system_clock::to_time_t(tp);
        std::tm localTime{};
#if defined(_MSC_VER)
        localtime_s(&localTime, &time);
#else
        const std::tm* result = std::localtime(&time);
        if (result) localTime = *result;
#endif
        return localTime;
    }

    static int toDays(const std::chrono::system_clock::time_point& tp) {
        std::tm localTime = toLocalTm(tp);
        std::tm midnight = localTime;
        midnight.tm_hour = 0;
        midnight.tm_min = 0;
        midnight.tm_sec = 0;
        const std::time_t midnightTime = std::mktime(&midnight);
        if (midnightTime == static_cast<std::time_t>(-1))
            throw std::runtime_error("Failed to convert date.");
        return static_cast<int>(midnightTime / (24 * 60 * 60));
    }
};

// ============================================================================
// CLASS: Book
// ============================================================================
class Book {
public:
    Book(std::string title, std::string author, std::string isbn, std::string genre = "General")
        : title_(std::move(title)), author_(std::move(author)), isbn_(std::move(isbn)),
          genre_(std::move(genre)), isAvailable_(true) {}

    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    const std::string& getISBN() const { return isbn_; }
    const std::string& getGenre() const { return genre_; }
    bool isAvailable() const { return isAvailable_; }

    void markBorrowed() { isAvailable_ = false; }
    void markReturned() { isAvailable_ = true; }

    void displayInfo() const {
        std::cout << "  Title    : " << title_ << '\n'
                  << "  Author   : " << author_ << '\n'
                  << "  ISBN     : " << isbn_ << '\n'
                  << "  Genre    : " << genre_ << '\n'
                  << "  Status   : " << (isAvailable_ ? "Available" : "Borrowed") << '\n';
    }

private:
    std::string title_;
    std::string author_;
    std::string isbn_;
    std::string genre_;
    bool isAvailable_;
};

// Forward declarations
class Transaction;

// ============================================================================
// CLASS: Member
// ============================================================================
class Member {
public:
    Member(int memberId, std::string name, std::string email)
        : memberId_(memberId), name_(std::move(name)), email_(std::move(email)) {}
    virtual ~Member() {}

    int getId() const { return memberId_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    const std::vector<Book*>& getBorrowedBooks() const { return borrowedBooks_; }
    const std::vector<Transaction*>& getHistory() const { return history_; }

    bool borrowBook(Book& book) {
        if (!book.isAvailable()) return false;
        book.markBorrowed();
        borrowedBooks_.push_back(&book);
        return true;
    }

    bool returnBook(Book& book) {
        auto it = std::find(borrowedBooks_.begin(), borrowedBooks_.end(), &book);
        if (it == borrowedBooks_.end()) return false;
        book.markReturned();
        borrowedBooks_.erase(it);
        return true;
    }

    void addToHistory(Transaction* t) {
        if (t) history_.push_back(t);
    }

    void viewHistory() const;

    virtual std::string getRole() const { return "Member"; }

protected:
    int memberId_;
    std::string name_;
    std::string email_;
    std::vector<Book*> borrowedBooks_;
    std::vector<Transaction*> history_;
};

// ============================================================================
// CLASS: Transaction
// ============================================================================
enum class TransactionStatus { Active, Returned, Overdue };

class Transaction {
public:
    static const int LOAN_PERIOD_DAYS = 14;
    static const double FINE_PER_DAY;

    Transaction(int id, Book& book, Member& member, const Date& issueDate)
        : transactionId_(id), book_(book), member_(member),
          dateIssued_(issueDate), dueDate_(issueDate.addDays(LOAN_PERIOD_DAYS)),
          returnDate_(issueDate), returned_(false) {}

    int getId() const { return transactionId_; }
    Book& getBook() const { return book_; }
    Member& getMember() const { return member_; }
    const Date& getDueDate() const { return dueDate_; }
    bool isReturned() const { return returned_; }

    TransactionStatus getStatus() const {
        if (returned_) return TransactionStatus::Returned;
        if (Date::today() > dueDate_) return TransactionStatus::Overdue;
        return TransactionStatus::Active;
    }

    double calculateFine(const Date& returnDate = Date::today()) const {
        if (!returned_ && returnDate <= dueDate_) return 0.0;
        const Date effective = returned_ ? returnDate_ : returnDate;
        int overdue = effective.daysBetween(dueDate_);
        return overdue > 0 ? overdue * FINE_PER_DAY : 0.0;
    }

    void markReturned(const Date& returnDate = Date::today()) {
        returned_ = true;
        returnDate_ = returnDate;
    }

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

const double Transaction::FINE_PER_DAY = 5.0;

void Transaction::display() const {
    std::string status;
    switch (getStatus()) {
        case TransactionStatus::Active:   status = "Active"; break;
        case TransactionStatus::Returned: status = "Returned"; break;
        case TransactionStatus::Overdue:  status = "Overdue"; break;
    }
    std::cout << "  Transaction ID : " << transactionId_ << '\n'
              << "  Member         : " << member_.getName() << " (ID: " << member_.getId() << ")\n"
              << "  Book           : " << book_.getTitle() << " [" << book_.getISBN() << "]\n"
              << "  Issued On      : " << dateIssued_.toString() << '\n'
              << "  Due Date       : " << dueDate_.toString() << '\n';
    if (returned_) std::cout << "  Returned On    : " << returnDate_.toString() << '\n';
    std::cout << "  Status         : " << status << '\n'
              << "  Fine (Rs.)     : " << calculateFine() << '\n';
}

void Member::viewHistory() const {
    std::cout << "\nBorrowing history for " << name_ << " (ID: " << memberId_ << ")\n";
    std::cout << std::string(52, '-') << '\n';
    if (history_.empty()) {
        std::cout << "  No transactions recorded.\n";
        return;
    }
    for (const Transaction* t : history_) {
        t->display();
        std::cout << '\n';
    }
}

// Forward declaration
class Library;

// ============================================================================
// CLASS: Librarian (inherits Member)
// ============================================================================
class Librarian : public Member {
public:
    Librarian(int memberId, std::string name, std::string email, std::string staffId)
        : Member(memberId, std::move(name), std::move(email)), staffId_(std::move(staffId)) {}

    const std::string& getStaffId() const { return staffId_; }
    std::string getRole() const override { return "Librarian"; }

    bool addBook(Library& library, Book book);
    bool removeBook(Library& library, const std::string& isbn);
    std::vector<const Book*> searchBook(const Library& library, const std::string& query) const;
    void viewAllMembers(const Library& library) const;

private:
    std::string staffId_;
};

// ============================================================================
// CLASS: Library
// ============================================================================
class Library {
public:
    Library(std::string name) : name_(std::move(name)), nextMemberId_(1001), nextTransactionId_(5001) {}

    const std::string& getName() const { return name_; }

    bool addBook(Book book) {
        if (findBookByISBNMutable(book.getISBN())) {
            std::cout << "  Error: A book with ISBN " << book.getISBN() << " already exists.\n";
            return false;
        }
        books_.push_back(std::unique_ptr<Book>(new Book(std::move(book))));
        std::cout << "  Book added successfully.\n";
        return true;
    }

    bool removeBook(const std::string& isbn) {
        auto it = std::find_if(books_.begin(), books_.end(),
            [&](const std::unique_ptr<Book>& b) { return b->getISBN() == isbn; });
        if (it == books_.end()) {
            std::cout << "  Error: Book with ISBN " << isbn << " not found.\n";
            return false;
        }
        if (!(*it)->isAvailable()) {
            std::cout << "  Error: Cannot remove a borrowed book.\n";
            return false;
        }
        books_.erase(it);
        std::cout << "  Book removed successfully.\n";
        return true;
    }

    std::vector<const Book*> searchBook(const std::string& query) const {
        std::vector<const Book*> results;
        for (const auto& book : books_) {
            if (containsIgnoreCase(book->getTitle(), query) ||
                containsIgnoreCase(book->getAuthor(), query) ||
                containsIgnoreCase(book->getISBN(), query) ||
                containsIgnoreCase(book->getGenre(), query))
                results.push_back(book.get());
        }
        return results;
    }

    const Book* findBookByISBN(const std::string& isbn) const {
        for (const auto& book : books_)
            if (book->getISBN() == isbn) return book.get();
        return nullptr;
    }

    void displayCatalogue() const {
        std::cout << "\nBook Catalogue - " << name_ << '\n' << std::string(40, '=') << '\n';
        if (books_.empty()) { std::cout << "  No books.\n"; return; }
        for (const auto& book : books_) {
            book->displayInfo();
            std::cout << std::string(40, '-') << '\n';
        }
    }

    void checkAvailability(const std::string& isbn) const {
        const Book* book = findBookByISBN(isbn);
        if (!book) { std::cout << "  Book not found.\n"; return; }
        std::cout << "\nAvailability Check\n" << std::string(40, '-') << '\n';
        book->displayInfo();
    }

    Member& registerMember(std::string name, std::string email) {
        int id = nextMemberId_++;
        auto m = std::unique_ptr<Member>(new Member(id, std::move(name), std::move(email)));
        Member& ref = *m;
        members_.push_back(std::move(m));
        std::cout << "  Member registered. ID: " << id << '\n';
        return ref;
    }

    Librarian& registerLibrarian(std::string name, std::string email, std::string staffId) {
        int id = nextMemberId_++;
        auto lib = std::unique_ptr<Librarian>(new Librarian(id, std::move(name), std::move(email), std::move(staffId)));
        Librarian& ref = *lib;
        members_.push_back(std::move(lib));
        std::cout << "  Librarian registered. ID: " << id << '\n';
        return ref;
    }

    const std::vector<std::unique_ptr<Member>>& getMembers() const { return members_; }

    Member* findMemberById(int id) {
        for (auto& m : members_) if (m->getId() == id) return m.get();
        return nullptr;
    }

    const Member* findMemberById(int id) const {
        for (const auto& m : members_) if (m->getId() == id) return m.get();
        return nullptr;
    }

    bool issueBook(int memberId, const std::string& isbn) {
        Member* member = findMemberById(memberId);
        if (!member) { std::cout << "  Error: Member not found.\n"; return false; }
        Book* book = findBookByISBNMutable(isbn);
        if (!book) { std::cout << "  Error: Book not found.\n"; return false; }
        if (!member->borrowBook(*book)) {
            std::cout << "  Error: Book not available.\n";
            return false;
        }
        auto t = std::unique_ptr<Transaction>(new Transaction(nextTransactionId_++, *book, *member, Date::today()));
        Transaction* tp = t.get();
        member->addToHistory(tp);
        transactions_.push_back(std::move(t));
        std::cout << "  Book issued. Due: " << tp->getDueDate().toString() << '\n';
        return true;
    }

    double acceptReturn(int memberId, const std::string& isbn) {
        Member* member = findMemberById(memberId);
        if (!member) { std::cout << "  Error: Member not found.\n"; return 0.0; }
        Book* book = findBookByISBNMutable(isbn);
        if (!book) { std::cout << "  Error: Book not found.\n"; return 0.0; }
        Transaction* t = findActiveTransaction(memberId, isbn);
        if (!t) { std::cout << "  Error: No active loan found.\n"; return 0.0; }
        Date returnDate = Date::today();
        double fine = t->calculateFine(returnDate);
        t->markReturned(returnDate);
        member->returnBook(*book);
        std::cout << "  Book returned.";
        if (fine > 0) std::cout << " Fine: Rs. " << fine;
        else std::cout << " No fine.";
        std::cout << '\n';
        return fine;
    }

    void displayBorrowedBooks() const {
        std::cout << "\nCurrently Borrowed Books\n" << std::string(40, '=') << '\n';
        bool found = false;
        for (const auto& t : transactions_) {
            if (!t->isReturned()) { found = true; t->display(); std::cout << std::string(40, '-') << '\n'; }
        }
        if (!found) std::cout << "  None.\n";
    }

    void displayPendingReturns() const {
        std::cout << "\nPending Returns\n" << std::string(40, '=') << '\n';
        bool found = false;
        for (const auto& t : transactions_) {
            if (!t->isReturned()) {
                found = true;
                t->display();
                if (t->getStatus() == TransactionStatus::Overdue)
                    std::cout << "  >> OVERDUE - Fine: Rs. " << t->calculateFine() << '\n';
                std::cout << std::string(40, '-') << '\n';
            }
        }
        if (!found) std::cout << "  None.\n";
    }

    void generateReport() const {
        int available = 0, active = 0, overdue = 0;
        for (const auto& b : books_) if (b->isAvailable()) ++available;
        for (const auto& t : transactions_) {
            if (!t->isReturned()) {
                ++active;
                if (t->getStatus() == TransactionStatus::Overdue) ++overdue;
            }
        }
        std::cout << "\nLibrary Report - " << name_ << '\n' << std::string(40, '=') << '\n'
                  << "  Total Books      : " << books_.size() << '\n'
                  << "  Available        : " << available << '\n'
                  << "  Borrowed         : " << (books_.size() - available) << '\n'
                  << "  Members          : " << members_.size() << '\n'
                  << "  Active Loans     : " << active << '\n'
                  << "  Overdue Loans    : " << overdue << '\n'
                  << "  Total Transactions: " << transactions_.size() << '\n';
    }

    void loadSampleData() {
        Book book1("The Great Gatsby", "F. Scott Fitzgerald", "978-0743273565", "Fiction");
        Book book2("To Kill a Mockingbird", "Harper Lee", "978-0061120084", "Fiction");
        Book book3("1984", "George Orwell", "978-0451524935", "Dystopian");
        Book book4("Clean Code", "Robert C. Martin", "978-0132350884", "Technology");
        Book book5("The Pragmatic Programmer", "David Thomas", "978-0201616224", "Technology");
        addBook(book1);
        addBook(book2);
        addBook(book3);
        addBook(book4);
        addBook(book5);
        registerMember("Alice Johnson", "alice@email.com");
        registerMember("Bob Smith", "bob@email.com");
        registerLibrarian("Sarah Williams", "sarah@library.gov", "LIB-001");
    }

private:
    std::string name_;
    std::vector<std::unique_ptr<Book>> books_;
    std::vector<std::unique_ptr<Member>> members_;
    std::vector<std::unique_ptr<Transaction>> transactions_;
    int nextMemberId_;
    int nextTransactionId_;

    static std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    static bool containsIgnoreCase(const std::string& hay, const std::string& needle) {
        return toLower(hay).find(toLower(needle)) != std::string::npos;
    }

    Book* findBookByISBNMutable(const std::string& isbn) {
        for (auto& b : books_) if (b->getISBN() == isbn) return b.get();
        return nullptr;
    }

    Transaction* findActiveTransaction(int memberId, const std::string& isbn) {
        for (auto& t : transactions_)
            if (!t->isReturned() && t->getMember().getId() == memberId && t->getBook().getISBN() == isbn)
                return t.get();
        return nullptr;
    }
};

// Librarian methods (need Library defined first)
bool Librarian::addBook(Library& library, Book book) { return library.addBook(std::move(book)); }
bool Librarian::removeBook(Library& library, const std::string& isbn) { return library.removeBook(isbn); }
std::vector<const Book*> Librarian::searchBook(const Library& library, const std::string& query) const {
    return library.searchBook(query);
}
void Librarian::viewAllMembers(const Library& library) const {
    std::cout << "\nRegistered Members\n" << std::string(52, '-') << '\n';
    for (const auto& m : library.getMembers())
        std::cout << "  ID: " << m->getId() << " | " << m->getName()
                  << " | " << m->getEmail() << " | " << m->getRole() << '\n';
}

// ============================================================================
// MAIN — All objects created here, all menu logic here
// ============================================================================
int main() {
    std::cout << "\n  ================================================\n"
              << "       LIBRARY MANAGEMENT SYSTEM  (C++ OOP)\n"
              << "  ================================================\n";

    // ---- CREATE OBJECTS OF EVERY CLASS ----
    Date today = Date::today();
    Library library("Central Public Library");
    Librarian chiefLibrarian(1000, "Sarah Williams", "sarah@library.gov", "LIB-001");

    std::cout << "\n  Objects created:\n"
              << "    Date         -> today = " << today.toString() << '\n'
              << "    Library      -> \"" << library.getName() << "\"\n"
              << "    Librarian    -> " << chiefLibrarian.getName()
              << " (Staff: " << chiefLibrarian.getStaffId() << ")\n";

    // Helper lambdas (inside main only)
    auto clearInput = []() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    };
    auto readLine = [&](const std::string& prompt) -> std::string {
        std::cout << prompt;
        std::string v;
        std::getline(std::cin, v);
        return v;
    };
    auto readInt = [&](const std::string& prompt) -> int {
        while (true) {
            std::cout << prompt;
            int v = 0;
            if (std::cin >> v) { clearInput(); return v; }
            std::cout << "  Invalid number.\n";
            clearInput();
        }
    };
    auto pause = [&]() { readLine("\nPress Enter to continue..."); };

    bool running = true;
    while (running) {
        std::cout << "\n  -------- MAIN MENU --------\n"
                  << "  1. Book Management\n"
                  << "  2. Member Management\n"
                  << "  3. Issue / Return Books\n"
                  << "  4. Reports & Search\n"
                  << "  5. Load Sample Data\n"
                  << "  0. Exit\n";

        int choice = readInt("  Choice: ");

        // ---- BOOK MANAGEMENT ----
        if (choice == 1) {
            bool back = false;
            while (!back) {
                std::cout << "\n  [Book Management]\n"
                          << "  1.Add  2.Remove  3.Catalogue  4.Availability  5.Search  0.Back\n";
                int c = readInt("  Choice: ");
                if (c == 1) {
                    Book newBook(readLine("  Title  : "), readLine("  Author : "),
                                 readLine("  ISBN   : "), readLine("  Genre  : "));
                    chiefLibrarian.addBook(library, newBook);
                } else if (c == 2) {
                    chiefLibrarian.removeBook(library, readLine("  ISBN: "));
                } else if (c == 3) {
                    library.displayCatalogue();
                } else if (c == 4) {
                    library.checkAvailability(readLine("  ISBN: "));
                } else if (c == 5) {
                    auto results = chiefLibrarian.searchBook(library, readLine("  Search: "));
                    std::cout << "\n  Results: " << results.size() << " book(s)\n";
                    for (const Book* b : results) { b->displayInfo(); std::cout << "---\n"; }
                } else if (c == 0) {
                    back = true;
                } else {
                    std::cout << "  Invalid.\n";
                }
                if (!back) pause();
            }

        // ---- MEMBER MANAGEMENT ----
        } else if (choice == 2) {
            bool back = false;
            while (!back) {
                std::cout << "\n  [Member Management]\n"
                          << "  1.Register Member  2.Register Librarian  3.View All  4.History  0.Back\n";
                int c = readInt("  Choice: ");
                if (c == 1) {
                    Member& m = library.registerMember(readLine("  Name: "), readLine("  Email: "));
                    std::cout << "  Member object -> " << m.getName() << " (ID: " << m.getId() << ")\n";
                } else if (c == 2) {
                    Librarian& lib = library.registerLibrarian(
                        readLine("  Name: "), readLine("  Email: "), readLine("  Staff ID: "));
                    std::cout << "  Librarian object -> " << lib.getName() << " (Staff: " << lib.getStaffId() << ")\n";
                } else if (c == 3) {
                    chiefLibrarian.viewAllMembers(library);
                } else if (c == 4) {
                    const Member* m = library.findMemberById(readInt("  Member ID: "));
                    if (m) m->viewHistory();
                    else std::cout << "  Not found.\n";
                } else if (c == 0) {
                    back = true;
                } else {
                    std::cout << "  Invalid.\n";
                }
                if (!back) pause();
            }

        // ---- ISSUE / RETURN ----
        } else if (choice == 3) {
            bool back = false;
            while (!back) {
                std::cout << "\n  [Issue / Return]\n  1.Issue Book  2.Accept Return  0.Back\n";
                int c = readInt("  Choice: ");
                if (c == 1) {
                    int mid = readInt("  Member ID: ");
                    std::string isbn = readLine("  ISBN: ");
                    if (library.issueBook(mid, isbn)) {
                        std::cout << "  Transaction object created for loan.\n";
                    }
                } else if (c == 2) {
                    int mid = readInt("  Member ID: ");
                    std::string isbn = readLine("  ISBN: ");
                    library.acceptReturn(mid, isbn);
                } else if (c == 0) {
                    back = true;
                } else {
                    std::cout << "  Invalid.\n";
                }
                if (!back) pause();
            }

        // ---- REPORTS ----
        } else if (choice == 4) {
            bool back = false;
            while (!back) {
                std::cout << "\n  [Reports]\n"
                          << "  1.Borrowed  2.Pending  3.Report  4.Search  0.Back\n";
                int c = readInt("  Choice: ");
                if (c == 1) library.displayBorrowedBooks();
                else if (c == 2) library.displayPendingReturns();
                else if (c == 3) library.generateReport();
                else if (c == 4) {
                    auto results = library.searchBook(readLine("  Search: "));
                    for (const Book* b : results) { b->displayInfo(); std::cout << "---\n"; }
                } else if (c == 0) back = true;
                else std::cout << "  Invalid.\n";
                if (!back) pause();
            }

        // ---- SAMPLE DATA ----
        } else if (choice == 5) {
            library.loadSampleData();
            std::cout << "\n  Sample Book, Member, Librarian & Transaction objects loaded.\n";
            pause();

        // ---- EXIT ----
        } else if (choice == 0) {
            std::cout << "\n  Goodbye!\n\n";
            running = false;
        } else {
            std::cout << "  Invalid choice.\n";
            pause();
        }
    }

    return 0;
}
