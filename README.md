# Construire le projet
Vous pouvez utiliser un dev container de base C++ de VScode.
Le projet utilise cmake, pensez à l'inclure dans votre dev container.

Voici les lignes de commandes pour compiler le projet:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Répertoire data

Il contient 2 fichiers `books.txt`et `users.txt` que vous pouvez utilisez pour tester votre code.
Pour ca il suffit de donner chemin vers le repertoire data avec l'application `bibliotheque -d <chemin vers data>`

# Mohamad El-Hajj

## Fonctionnalités choisi

1. numérotation automatique des ID utilisateurs
2. Tri des résultats par titre, auteur pour l’affichage (utilisation de la fonction de tri de la STL).

## Questions

### Question 1 : C++

```
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
```

1. La bibliothèque chrono
- Cette bibliothèque sert à récupérer la date exacte incluant le temps. je l'ai utilisé pour mettre la date exacte pour chaque action enregistrée dans le journal d'activités.

2. La bibliothèque iomanip
- Cette bibliothèque est utilisé pour formatter le temps pour un affichage propre avec put_time dans le journal d'activités.

3. ios::app
- Elle garantit le fichier de journal d'activités s'ouvre pour ajouter les nouvelles données à la du fichier sans écraser le fichier. De plus, si le fichier n'existe pas, il est créé automatiquement.

### Question 2 : Options de développement possible

Pour une gestion à plus grande échelle et plus performante de la bibliothèque, on pourrait utiliser un système de gestion de base de données relationnelle (SGBDR) comme MySQL. Il serait possible de s'interfacer avec le code C++ en utilisant un connecteur officiel tel que MySQL Connector/C++ pour exécuter des requêtes directement depuis l'application.