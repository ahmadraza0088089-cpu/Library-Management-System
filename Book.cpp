#include "Book.hpp"

#include <iostream>

namespace lms {

Book::Book(std::string title, std::string author, std::string isbn, std::string genre)
    : title_(std::move(title)),
      author_(std::move(author)),
      isbn_(std::move(isbn)),
      genre_(std::move(genre)),
      isAvailable_(true) {}

const std::string& Book::getTitle() const {
    return title_;
}

const std::string& Book::getAuthor() const {
    return author_;
}

const std::string& Book::getISBN() const {
    return isbn_;
}

const std::string& Book::getGenre() const {
    return genre_;
}

bool Book::isAvailable() const {
    return isAvailable_;
}

void Book::markBorrowed() {
    isAvailable_ = false;
}

void Book::markReturned() {
    isAvailable_ = true;
}

void Book::displayInfo() const {
    std::cout << "  Title    : " << title_ << '\n'
              << "  Author   : " << author_ << '\n'
              << "  ISBN     : " << isbn_ << '\n'
              << "  Genre    : " << genre_ << '\n'
              << "  Status   : " << (isAvailable_ ? "Available" : "Borrowed") << '\n';
}

}  // namespace lms
