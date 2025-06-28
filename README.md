
# 🇫🇷 NYU - Langage de Programmation Français

NYU est un langage de programmation généraliste entièrement en français, inspiré de Ruby et Python. Il permet de créer facilement des sites web, gérer des bases de données et effectuer des calculs mathématiques avancés.

## 🚀 Installation et Utilisation

### Compilation
```bash
make
```

### Exécution d'un programme NYU
```bash
./main démarrer fichier.nyu
```

### Aide
```bash
./main aide
```

## 📝 Syntaxe NYU

### Variables
```nyu
variable nom = valeur
variable age = 25
variable prix = 19.99
```

### Affichage
```nyu
afficher "Bonjour le monde!"
afficher nom_variable
```

### Calculs Mathématiques

#### Opérations de base
```nyu
calculer 5 + 3
calculer 10 - 4
calculer 6 * 7
calculer 15 / 3
```

#### Mathématiques avancées
```nyu
calculer puissance 2 8        # 2^8 = 256
calculer pythagore 3 4        # √(3² + 4²) = 5
calculer modulo 17 5          # 17 % 5 = 2
```

### Sites Web

#### Configuration de base
```nyu
site port 5000                # Port du serveur
site fond lightblue          # Couleur de fond
site titre "Mon Site"        # Titre de la page
```

#### Contenu
```nyu
site contenu "Bienvenue!"     # Ajouter du texte
site bouton "Cliquez ici"     # Ajouter un bouton
```

### Base de Données

#### Sauvegarder des données
```nyu
bd sauver nom "Jean"
bd sauver age "30"
bd sauver ville "Paris"
```

#### Lire des données
```nyu
bd lire nom                   # Affiche: nom = Jean
bd lister                     # Liste toutes les entrées
```

### Structures de Contrôle

#### Conditions (en développement)
```nyu
si age > 18
    afficher "Majeur"
```

#### Boucles (en développement)
```nyu
répéter 5
    afficher "Bonjour!"
```

## 🌐 Création de Sites Web

NYU peut créer des sites web interactifs qui s'ouvrent automatiquement dans Replit:

```nyu
# Configuration du site
site port 5000
site fond #f0f8ff
site titre "Ma Page Web NYU"

# Contenu de la page
site contenu "Bienvenue sur mon site!"
site contenu "Créé avec le langage NYU"
site bouton "Accueil"
site bouton "À propos"
site bouton "Contact"
```

Le site sera automatiquement accessible sur `https://votre-repl.replit.dev` quand le port 5000 est utilisé.

## 🗄️ Gestion de Base de Données

NYU inclut un système de base de données simple pour persister les données:

```nyu
# Sauvegarder
bd sauver utilisateur "Alice"
bd sauver score "1500"
bd sauver niveau "expert"

# Lire
bd lire utilisateur          # Affiche: utilisateur = Alice

# Lister tout
bd lister                    # Affiche toutes les entrées
```

## 🧮 Mathématiques Avancées

### Théorème de Pythagore
```nyu
calculer pythagore 3 4       # Résultat: 5
calculer pythagore 5 12      # Résultat: 13
```

### Puissances
```nyu
calculer puissance 2 10      # 2^10 = 1024
calculer puissance 3 4       # 3^4 = 81
```

### Modulo
```nyu
calculer modulo 17 5         # 17 % 5 = 2
calculer modulo 100 7        # 100 % 7 = 2
```

## 📁 Extension de Fichier

Tous les programmes NYU utilisent l'extension `.nyu`:
- `mon_site.nyu`
- `calculatrice.nyu`
- `application.nyu`

## 🎯 Exemples Complets

### Site Web Simple
```nyu
# mon_site.nyu
site port 5000
site fond lightgreen
site titre "Ma Première Page NYU"
site contenu "Bienvenue sur mon site!"
site contenu "NYU rend la programmation accessible en français"
site bouton "Accueil"
site bouton "Services"
```

### Programme de Calcul
```nyu
# calculatrice.nyu
variable a = 10
variable b = 5

afficher "Calculs avec a=10 et b=5:"
calculer 10 + 5
calculer 10 * 5
calculer puissance 10 2

bd sauver resultat_addition "15"
bd sauver resultat_multiplication "50"
```

### Application Mixte
```nyu
# app_complete.nyu
# Site web avec calculs et base de données

variable prix_unitaire = 25.50
variable quantite = 3

site port 5000
site fond #e6f3ff
site titre "Calculateur de Prix"
site contenu "Application de calcul de prix"

calculer 25.50 * 3
bd sauver prix_total "76.50"

site bouton "Calculer"
site bouton "Historique"

afficher "Application prête!"
```

## 🔧 Fonctionnalités Techniques

- **Serveur Web Intégré**: Démarre automatiquement sur le port configuré
- **Base de Données Persistante**: Stockage en mémoire pendant l'exécution
- **Calculs Mathématiques**: Opérations de base et avancées
- **Interface Française**: Toutes les commandes en français
- **Syntaxe Intuitive**: Inspirée de Ruby/Python mais simplifiée

## 🎨 Personnalisation Web

### Couleurs de Fond Supportées
- `lightblue`, `lightgreen`, `lightyellow`
- Codes hexadécimaux: `#ff0000`, `#00ff00`, `#0000ff`
- Noms CSS: `red`, `blue`, `green`, `orange`, `purple`

### Éléments HTML Générés
- Paragraphes pour le contenu
- Boutons interactifs
- Styles CSS automatiques
- Design responsive

## 🚧 Développement Futur

### Fonctionnalités à venir:
- [ ] Conditions `si/sinon`
- [ ] Boucles `répéter/tant_que`
- [ ] Fonctions personnalisées
- [ ] Import/export de modules
- [ ] Base de données persistante sur fichier
- [ ] Formulaires web interactifs
- [ ] API REST intégrée

## 🏆 Avantages de NYU

1. **100% en Français**: Idéal pour l'apprentissage
2. **Simplicité**: Syntaxe claire et intuitive
3. **Polyvalent**: Web, calculs, base de données
4. **Rapide**: Démarrage instantané des projets
5. **Intégré**: Serveur web et BD inclus
6. **Replit-Compatible**: Fonctionne parfaitement sur Replit

## 📞 Support

NYU est conçu pour être simple et intuitif. En cas de problème:
1. Vérifiez la syntaxe avec `./main aide`
2. Consultez les exemples fournis
3. Testez avec des programmes simples d'abord

---

**NYU** - *Le langage de programmation qui parle français!* 🇫🇷✨
