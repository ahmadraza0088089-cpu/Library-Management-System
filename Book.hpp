#pragma once

#include <string>

namespace lms {

class Book {
public:
    Book(std::string title, std::string author, std::string isbn, std::string genre = "General");

    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getISBN() const;
    const std::string& getGenre() const;
    bool isAvailable() const;

    void markBorrowed();
    void markReturned();
    void displayInfo() const;

private:
    std::string title_;
    std::string author_;
    std::string isbn_;
    std::string genre_;
    bool isAvailable_;
};

}  // namespace lms
