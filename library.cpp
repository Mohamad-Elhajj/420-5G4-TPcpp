#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "library.h"

using namespace std;

// Constructor
Library::Library() {}

// Fonction utilitaire pour écrire dans le journal d'activités (activity.log)
void logActivity(const string& action, const string& details) {
    ofstream logFile("../data/activity.log", ios::app);
    if (logFile.is_open()) {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        
        logFile << ss.str() << " - [" << action << "] " << details << "\n";
    }
}

// Add book to library
void Library::addBook(const Book& book) {
    books.push_back(make_unique<Book>(book));

    string details = book.getTitle() + "|" + book.getAuthor() + "|" + book.getISBN() + "|1|";
    logActivity("AJOUT LIVRE", details);
}

// Remove book from library
bool Library::removeBook(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    if (it != books.end()) {
        string title = (*it)->getTitle();
        string author = (*it)->getAuthor();
        string bookIsbn = (*it)->getISBN();

        books.erase(it);

        string details = title + "|" + author + "|" + bookIsbn;
        logActivity("SUPPRESSION LIVRE", details);

        return true;
    }
    return false;
}

// Find book by ISBN
Book* Library::findBookByISBN(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    return (it != books.end()) ? it->get() : nullptr;
}

// Search books by title (case-insensitive partial match)
vector<Book*> Library::searchBooksByTitle(const string& title) {
    vector<Book*> results;
    string lowerTitle = title;
    transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookTitle = book->getTitle();
        transform(bookTitle.begin(), bookTitle.end(), bookTitle.begin(), ::tolower);
        
        if (bookTitle.find(lowerTitle) != string::npos) {
            results.push_back(book.get());
        }
    }

    // Tri STL par titre
    sort(results.begin(), results.end(), [](Book* a, Book* b) {
        return a->getTitle() < b->getTitle();
    });

    return results;
}

// Search books by author (case-insensitive partial match)
vector<Book*> Library::searchBooksByAuthor(const string& author) {
    vector<Book*> results;
    string lowerAuthor = author;
    transform(lowerAuthor.begin(), lowerAuthor.end(), lowerAuthor.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookAuthor = book->getAuthor();
        transform(bookAuthor.begin(), bookAuthor.end(), bookAuthor.begin(), ::tolower);
        
        if (bookAuthor.find(lowerAuthor) != string::npos) {
            results.push_back(book.get());
        }
    }

    // Tri STL par ordre alphabétique de l'auteur (et par titre en cas d'égalité)
    sort(results.begin(), results.end(), [](Book* a, Book* b) {
        if (a->getAuthor() == b->getAuthor()) {
            return a->getTitle() < b->getTitle();
        }
        return a->getAuthor() < b->getAuthor();
    });

    return results;
}

// Get all available books
vector<Book*> Library::getAvailableBooks() {
    vector<Book*> available;
    for (auto& book : books) {
        if (book->getAvailability()) {
            available.push_back(book.get());
        }
    }

    // Tri STL par titre
    sort(available.begin(), available.end(), [](Book* a, Book* b) {
        return a->getTitle() < b->getTitle();
    });
    
    return available;
}

// Get all books
vector<Book*> Library::getAllBooks() {
    vector<Book*> allBooks;
    for (auto& book : books) {
        allBooks.push_back(book.get());
    }

    // Tri STL par titre pour s'assurer que l'affichage global est toujours ordonné
    sort(allBooks.begin(), allBooks.end(), [](Book* a, Book* b) {
        return a->getTitle() < b->getTitle();
    });

    return allBooks;
}

// Add user to library
void Library::addUser(User& user) {
    if (user.getUserId().empty()) {
        int nextIdNum = users.size() + 1;
        string autoId = "USR" + string(3 - min(2, (int)to_string(nextIdNum).length()), '0') + to_string(nextIdNum);
        user.setUserId(autoId);
    }
    
    users.push_back(make_unique<User>(user));

    string details = user.getName() + "|" + user.getUserId();
    logActivity("AJOUT UTILISATEUR", details);
}

// Find user by ID
User* Library::findUserById(const string& userId) {
    auto it = find_if(users.begin(), users.end(),
        [&userId](const unique_ptr<User>& user) {
            return user->getUserId() == userId;
        });
    
    return (it != users.end()) ? it->get() : nullptr;
}

// Get all users
vector<User*> Library::getAllUsers() {
    vector<User*> allUsers;
    for (auto& user : users) {
        allUsers.push_back(user.get());
    }

    return allUsers;
}

// Check out book
bool Library::checkOutBook(const string& isbn, const string& userId) {
    Book* book = findBookByISBN(isbn);
    User* user = findUserById(userId);
    
    if (book && user && book->getAvailability()) {
        string borrowerName = user->getName();
        book->checkOut(user->getUserId());
        user->borrowBook(isbn);

        // Format requis pour le journal d'activités
        string details = book->getTitle() + "|" + book->getAuthor() + "|" + book->getISBN() + "|0|" + borrowerName;
        logActivity("EMPRUNT", details);

        return true;
    }
    return false;
}

// Return book
bool Library::returnBook(const string& isbn) {
    Book* book = findBookByISBN(isbn);
    
    if (book && !book->getAvailability()) {
        // Find the user who borrowed this book
        string borrowerId = book->getBorrowerId();
        string borrowerName = "Inconnu";

        for (auto& user : users) {
            if (user->hasBorrowedBook(isbn)) {
                user->returnBook(isbn);
                break;
            }
        }

        string title = book->getTitle();
        string author = book->getAuthor();
        string bookIsbn = book->getISBN();

        book->returnBook();

        string details = title + "|" + author + "|" + bookIsbn + "|1|" + borrowerName;
        logActivity("RETOUR", details);

        return true;
    }
    return false;
}

// Display all books
void Library::displayAllBooks() {
    auto booksList = getAllBooks();

    if (booksList.empty()) {
        cout << "Aucun livre dans la bibliothèque.\n";
        return;
    }
    
    cout << "\n=== TOUS LES LIVRES ===\n";
    for (size_t i = 0; i < booksList.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << booksList[i]->toString() << "\n";
        cout << "-------------------------\n";
    }
}

// Display available books
void Library::displayAvailableBooks() {
    auto available = getAvailableBooks();
    
    if (available.empty()) {
        cout << "Aucun livre disponible pour emprunt.\n";
        return;
    }
    
    cout << "\n=== LIVRES DISPONIBLES ===\n";
    for (size_t i = 0; i < available.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << available[i]->toString() << "\n";
        cout << "---------------------------\n";
    }
}

// Display all users
void Library::displayAllUsers() {
    auto usersList = getAllUsers();

    if (usersList.empty()) {
        cout << "Aucun utilisateur enregistré.\n";
        return;
    }
    
    cout << "\n=== TOUS LES UTILISATEURS ===\n";
    for (size_t i = 0; i < usersList.size(); ++i) {
        cout << "\nUtilisateur " << (i + 1) << " :\n";
        cout << usersList[i]->toString() << "\n";
        cout << "------------------------------\n";
    }
}

// Statistics
int Library::getTotalBooks() const { return books.size(); }
int Library::getAvailableBookCount() const {
    return count_if(books.begin(), books.end(),
        [](const unique_ptr<Book>& book) {
            return book->getAvailability();
        });
}
int Library::getCheckedOutBookCount() const { return getTotalBooks() - getAvailableBookCount(); }