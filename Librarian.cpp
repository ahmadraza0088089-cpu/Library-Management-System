#include "Librarian.hpp"

#include "Book.hpp"
#include "Library.hpp"

#include <iostream>

namespace lms {

Librarian::Librarian(int memberId, std::string name, std::string email, std::string staffId)
    : Member(memberId, std::move(name), std::move(email)), staffId_(std::move(staffId)) {}

const std::string& Librarian::getStaffId() const {
    return staffId_;
}

std::string Librarian::getRole() const {
    return "Librarian";
}

bool Librarian::addBook(Library& library, Book book) {
    return library.addBook(std::move(book));
}

bool Librarian::removeBook(Library& library, const std::string& isbn) {
    return library.removeBook(isbn);
}

std::vector<const Book*> Librarian::searchBook(const Library& library, const std::string& query) const {
    return library.searchBook(query);
}

void Librarian::viewAllMembers(const Library& library) const {
    const auto& members = library.getMembers();

    std::cout << "\nRegistered Members\n";
    std::cout << std::string(52, '-') << '\n';

    if (members.empty()) {
        std::cout << "  No members registered.\n";
        return;
    }

    for (const auto& member : members) {
        std::cout << "  ID: " << member->getId() << " | " << member->getName()
                  << " | " << member->getEmail() << " | Role: " << member->getRole() << '\n';
    }
}

}  // namespace lms
