
# 🇫🇷 NYU - Langage de Programmation Français

NYU est un langage de programmation généraliste entièrement en français, inspiré de Ruby et Python. Il permet de créer facilement des sites web multi-pages avec liens externes, gérer des bases de données avancées et effectuer des calculs mathématiques complexes.

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

## 🌐 Sites Web Avancés

### Configuration de base
```nyu
site port 5000                # Port du serveur
site fond "#e6f3ff"           # Couleur de fond (hex/nom)
site titre "Mon Site"         # Titre de la page
```

### Contenu
```nyu
site contenu "Bienvenue!"     # Ajouter du texte
site bouton "Cliquez ici"     # Ajouter un bouton
```

### Pages Multiples
```nyu
page créer accueil            # Créer une nouvelle page
page créer contact            # Créer page contact
page changer accueil          # Changer la page active
page titre "Accueil"          # Titre de la page active
page contenu "Bienvenue"      # Contenu de la page active
page bouton "Contact" "/contact"  # Bouton vers autre page
```

### Liens Externes
```nyu
# Créer un lien externe
lien externe github https://github.com "Mon GitHub"
lien externe google https://google.com "Google"

# Ajouter le lien à la page courante
lien ajouter github
lien ajouter google
```

### Styles Avancés
```nyu
style police "Georgia, serif"          # Changer la police
style couleur_texte "#2c3e50"         # Couleur du texte
style css "h1 { color: blue; }"       # CSS personnalisé global
style page_css "p { font-size: 18px; }" # CSS pour page actuelle
```

## 🗄️ Base de Données Avancée

### Sauvegarder avec types
```nyu
bd sauver nom "Jean Dupont" texte
bd sauver age "30" nombre
bd sauver date "2024-01-15" date
bd sauver email "jean@mail.com" texte
```

### Lire et gérer
```nyu
bd lire nom                   # Affiche: nom = Jean Dupont (texte)
bd lister                     # Liste toutes les entrées avec types
bd supprimer nom              # Supprimer une entrée
bd web_afficher               # Afficher BD sur site web en tableau
```

## 🎨 Personnalisation Complète

### Couleurs supportées
- **Noms CSS**: `red`, `blue`, `green`, `orange`, `purple`, `lightblue`
- **Hexadécimal**: `#ff0000`, `#00ff00`, `#0000ff`, `#e6f3ff`
- **Dégradés CSS**: Support complet dans `style css`

### Polices supportées
- **Systèmes**: `Arial`, `Georgia`, `Times New Roman`, `Courier New`
- **Stacks**: `"Arial, sans-serif"`, `"Georgia, serif"`
- **Web fonts**: Via CSS personnalisé

## 🌟 Exemple Site Complet

```nyu
# Configuration principale
site port 5000
site fond "#e6f3ff"
site titre "Mon Site NYU"

# Styles
style police "Georgia, serif"
style couleur_texte "#2c3e50"
style css "h1 { text-align: center; border-bottom: 3px solid #3498db; }"

# Page d'accueil
site contenu "Bienvenue sur mon site!"

# Liens externes
lien externe github https://github.com "Mon GitHub"
lien externe linkedin https://linkedin.com "LinkedIn"
lien ajouter github
lien ajouter linkedin

# Créer des pages
page créer apropos
page créer contact

# Page À propos
page changer apropos
page titre "À Propos"
page contenu "Je suis développeur NYU!"
style page_css "body { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }"
page bouton "Accueil" "/"

# Page Contact
page changer contact
page titre "Contact"
page contenu "Email: contact@monsite.com"
page bouton "À_Propos" "/apropos"

# Navigation sur accueil
page changer accueil
site bouton "À_Propos" "/apropos"
site bouton "Contact" "/contact"

# Base de données
bd sauver visiteurs "1250" nombre
bd sauver derniere_visite "2024-01-15" date
bd web_afficher
```

## 🔧 Fonctionnalités Avancées

- **🌐 Sites Multi-Pages**: Navigation fluide entre pages
- **🔗 Liens Externes**: Intégration de liens vers sites externes
- **🎨 CSS Personnalisé**: Contrôle total du design
- **📊 Base de Données Typée**: Stockage avec types (texte, nombre, date)
- **📱 Design Responsive**: Optimisé pour tous les écrans
- **🚀 Serveur HTTP**: Gestion des routes automatique
- **💾 Persistance**: Base de données en mémoire pendant exécution

## 📁 Structure de Navigation

NYU génère automatiquement:
- **Menu de navigation** sur chaque page
- **Routes automatiques**: `/`, `/page1`, `/page2`, etc.
- **Liens de retour** entre les pages
- **Gestion d'erreurs 404** pour pages inexistantes

## 🎯 Cas d'Usage

### Site Portfolio
```nyu
page créer portfolio
page changer portfolio
page titre "Mon Portfolio"
page contenu "Mes projets réalisés avec NYU"
lien externe github https://github.com/monprofil "Code Source"
lien ajouter github
```

### Site E-commerce Simple
```nyu
bd sauver produit1 "Ordinateur" texte
bd sauver prix1 "899.99" nombre
bd sauver stock1 "5" nombre
bd web_afficher
```

### Blog Personnel
```nyu
page créer blog
page changer blog
page titre "Mon Blog"
bd sauver article1 "Premier article NYU" texte
bd sauver date1 "2024-01-15" date
```

## 🏆 Avantages NYU 2.0

1. **🇫🇷 100% Français**: Parfait pour l'apprentissage
2. **🌐 Multi-Pages**: Sites web complexes
3. **🔗 Liens Externes**: Connexion avec l'internet
4. **🎨 Design Libre**: CSS personnalisé illimité
5. **📊 Base de Données**: Stockage structuré avec types
6. **⚡ Temps Réel**: Changements instantanés
7. **📱 Moderne**: Standards web actuels
8. **🛠️ Extensible**: Architecture modulaire

## 🚧 Fonctionnalités Futures

### En développement:
- [ ] Formulaires web interactifs
- [ ] Upload de fichiers/images
- [ ] Base de données persistante (fichiers)
- [ ] API REST automatique
- [ ] Authentification utilisateurs
- [ ] Templates de pages
- [ ] Système de commentaires
- [ ] Intégration réseaux sociaux

## 📞 Support et Documentation

### Commandes rapides:
```bash
./main aide                   # Aide complète
./main démarrer exemple.nyu   # Exemple simple
./main démarrer site_avance.nyu # Exemple avancé
```

### Dépannage:
1. **Site ne se charge pas**: Vérifiez le port avec `site port 5000`
2. **Page vide**: Utilisez `page contenu "texte"` après `page changer`
3. **Liens ne marchent pas**: Format: `lien externe nom url texte`
4. **Styles pas appliqués**: Utilisez `style css "règles CSS"`

---

**NYU 2.0** - *Le langage web français nouvelle génération!* 🇫🇷✨🌐

Créez des sites web professionnels entièrement en français avec NYU!
