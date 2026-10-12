#include <iostream>
#include <sstream>

#include "book.h"

using namespace std;

// Constructeur par défaut
Book::Book() : title(""), author(""), isbn(""), isAvailable(true), borrowerId("") {}

// Constructeur paramétré
Book::Book(const string& title, const string& author, const string& isbn)
    : title(title), author(author), isbn(isbn), isAvailable(true), borrowerId("") {}

// Getters
string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getISBN() const { return isbn; }
bool Book::getAvailability() const { return isAvailable; }
string Book::getBorrowerId() const { return borrowerId; }

// Setters
void Book::setTitle(const string& title) { this->title = title; }
void Book::setAuthor(const string& author) { this->author = author; }
void Book::setISBN(const string& isbn) { this->isbn = isbn; }
void Book::setAvailability(bool available) { this->isAvailable = available; }
void Book::setBorrowerId(const string& id) { this->borrowerId = id; }

// Emprunter le livre
void Book::checkOut(const string& borrowerId) {
    isAvailable = false;
    this->borrowerId = borrowerId;
}

// Retourner le livre
void Book::returnBook() {
    isAvailable = true;
    borrowerId = "";
}

// Affichage textuel des informations du livre
string Book::toString() const {
    string status = isAvailable ? "Disponible" : "Emprunte (ID Emprunteur : " + borrowerId + ")";
    return "Titre : " + title + "\nAuteur : " + author + "\nISBN : " + isbn + "\nStatut : " + status;
}

// Format pour l'enregistrement dans le fichier (ex: Titre|Auteur|ISBN|Dispo|BorrowerId)
string Book::toFileFormat() const {
    return title + "|" + author + "|" + isbn + "|" + (isAvailable ? "1" : "0") + "|" + borrowerId;
}

// Lecture depuis le format de fichier
void Book::fromFileFormat(const string& line) {
    stringstream ss(line);
    string availStr;

    getline(ss, title, '|');
    getline(ss, author, '|');
    getline(ss, isbn, '|');
    getline(ss, availStr, '|');
    getline(ss, borrowerId, '|');
    isAvailable = (availStr == "1");
}