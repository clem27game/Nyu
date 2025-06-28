
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_VARS 100
#define MAX_HTML 50000
#define MAX_DB_ENTRIES 1000
#define MAX_PAGES 50
#define MAX_LINKS 100
#define MAX_STYLES 500

// Structure pour les variables
typedef struct {
    char nom[50];
    double valeur;
} Variable;

// Structure pour la base de données
typedef struct {
    char cle[50];
    char valeur[500];
    char type[20]; // texte, nombre, date
} EntreeBD;

// Structure pour les pages
typedef struct {
    char nom[50];
    char titre[100];
    char contenu[MAX_HTML];
    char couleur_fond[50];
    char style_personnalise[MAX_STYLES];
} Page;

// Structure pour les liens externes
typedef struct {
    char nom[100];
    char url[500];
    char texte[100];
} LienExterne;

// Structure pour le serveur web
typedef struct {
    int port;
    char contenu_html[MAX_HTML];
    char couleur_fond[50];
    char titre[100];
    char police[50];
    char couleur_texte[50];
    char style_personnalise[MAX_STYLES];
    Page pages[MAX_PAGES];
    int nb_pages;
    LienExterne liens[MAX_LINKS];
    int nb_liens;
    char page_actuelle[50];
} ServeurWeb;

// Variables globales
Variable variables[MAX_VARS];
int nb_variables = 0;
EntreeBD base_donnees[MAX_DB_ENTRIES];
int nb_entrees_bd = 0;
ServeurWeb serveur;
int serveur_actif = 0;

// Prototypes
void executer_nyu(const char* fichier);
void traiter_ligne(char* ligne);
double evaluer_expression(char* expr);
void creer_variable(char* nom, double valeur);
double obtenir_variable(char* nom);
void gerer_site_web(char* ligne);
void demarrer_serveur();
void* serveur_thread(void* arg);
void gerer_base_donnees(char* ligne);
void calculer_mathematiques(char* ligne);
void afficher_aide();
void gerer_pages(char* ligne);
void gerer_liens(char* ligne);
void gerer_styles(char* ligne);
char* generer_html_page(const char* nom_page);
void initialiser_serveur();

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s démarrer <fichier.nyu>\n", argv[0]);
        printf("Ou: %s aide\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "aide") == 0) {
        afficher_aide();
        return 0;
    }
    
    if (strcmp(argv[1], "démarrer") == 0) {
        initialiser_serveur();
        executer_nyu(argv[2]);
    } else {
        printf("Commande inconnue. Utilisez 'démarrer' ou 'aide'\n");
    }
    
    return 0;
}

void initialiser_serveur() {
    serveur.port = 5000;
    strcpy(serveur.couleur_fond, "#f0f8ff");
    strcpy(serveur.titre, "Site NYU");
    strcpy(serveur.police, "Arial, sans-serif");
    strcpy(serveur.couleur_texte, "#333");
    serveur.nb_pages = 0;
    serveur.nb_liens = 0;
    strcpy(serveur.page_actuelle, "accueil");
    serveur.contenu_html[0] = '\0';
    serveur.style_personnalise[0] = '\0';
}

void executer_nyu(const char* fichier) {
    FILE* file = fopen(fichier, "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", fichier);
        return;
    }
    
    printf("🚀 Démarrage du programme NYU: %s\n", fichier);
    
    char ligne[MAX_LINE];
    while (fgets(ligne, sizeof(ligne), file)) {
        // Supprimer le retour à la ligne
        ligne[strcspn(ligne, "\n")] = 0;
        
        // Ignorer les lignes vides et les commentaires
        if (strlen(ligne) == 0 || ligne[0] == '#') continue;
        
        traiter_ligne(ligne);
    }
    
    fclose(file);
    
    // Si un serveur web a été configuré, le démarrer
    if (serveur_actif) {
        printf("🌐 Démarrage du serveur web sur le port %d...\n", serveur.port);
        demarrer_serveur();
    }
}

void traiter_ligne(char* ligne) {
    // Supprimer les espaces en début
    while (*ligne == ' ') ligne++;
    
    // Variables
    if (strncmp(ligne, "variable ", 9) == 0) {
        char nom[50];
        double valeur;
        if (sscanf(ligne + 9, "%s = %lf", nom, &valeur) == 2) {
            creer_variable(nom, valeur);
            printf("✓ Variable '%s' créée avec la valeur %g\n", nom, valeur);
        }
    }
    
    // Affichage
    else if (strncmp(ligne, "afficher ", 9) == 0) {
        char* contenu = ligne + 9;
        if (contenu[0] == '"') {
            // Chaîne de caractères
            contenu++;
            contenu[strlen(contenu)-1] = 0; 
            printf("%s\n", contenu);
        } else {
            // Variable
            double val = obtenir_variable(contenu);
            printf("%g\n", val);
        }
    }
    
    // Calculs mathématiques
    else if (strncmp(ligne, "calculer ", 9) == 0) {
        calculer_mathematiques(ligne + 9);
    }
    
    // Site web
    else if (strncmp(ligne, "site ", 5) == 0) {
        gerer_site_web(ligne + 5);
    }
    
    // Pages
    else if (strncmp(ligne, "page ", 5) == 0) {
        gerer_pages(ligne + 5);
    }
    
    // Liens externes
    else if (strncmp(ligne, "lien ", 5) == 0) {
        gerer_liens(ligne + 5);
    }
    
    // Styles
    else if (strncmp(ligne, "style ", 6) == 0) {
        gerer_styles(ligne + 6);
    }
    
    // Base de données
    else if (strncmp(ligne, "bd ", 3) == 0) {
        gerer_base_donnees(ligne + 3);
    }
}

void gerer_pages(char* ligne) {
    if (strncmp(ligne, "créer ", 6) == 0) {
        char nom[50];
        sscanf(ligne + 6, "%s", nom);
        if (serveur.nb_pages < MAX_PAGES) {
            strcpy(serveur.pages[serveur.nb_pages].nom, nom);
            strcpy(serveur.pages[serveur.nb_pages].titre, nom);
            strcpy(serveur.pages[serveur.nb_pages].couleur_fond, serveur.couleur_fond);
            serveur.pages[serveur.nb_pages].contenu[0] = '\0';
            serveur.pages[serveur.nb_pages].style_personnalise[0] = '\0';
            serveur.nb_pages++;
            printf("📄 Page '%s' créée\n", nom);
        }
    }
    else if (strncmp(ligne, "changer ", 8) == 0) {
        char nom[50];
        sscanf(ligne + 8, "%s", nom);
        strcpy(serveur.page_actuelle, nom);
        printf("📋 Page active: %s\n", nom);
    }
    else if (strncmp(ligne, "titre ", 6) == 0) {
        char* titre = ligne + 6;
        if (titre[0] == '"') {
            titre++;
            titre[strlen(titre)-1] = 0;
        }
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcpy(serveur.pages[i].titre, titre);
                printf("📝 Titre de la page '%s': %s\n", serveur.page_actuelle, titre);
                break;
            }
        }
    }
    else if (strncmp(ligne, "contenu ", 8) == 0) {
        char* contenu = ligne + 8;
        if (contenu[0] == '"') {
            contenu++;
            contenu[strlen(contenu)-1] = 0;
        }
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].contenu, "<p>");
                strcat(serveur.pages[i].contenu, contenu);
                strcat(serveur.pages[i].contenu, "</p>");
                printf("📄 Contenu ajouté à la page '%s'\n", serveur.page_actuelle);
                break;
            }
        }
    }
    else if (strncmp(ligne, "bouton ", 7) == 0) {
        char texte[100], lien[100];
        if (sscanf(ligne + 7, "%s %s", texte, lien) == 2) {
            for (int i = 0; i < serveur.nb_pages; i++) {
                if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                    strcat(serveur.pages[i].contenu, "<button onclick=\"window.location.href='");
                    strcat(serveur.pages[i].contenu, lien);
                    strcat(serveur.pages[i].contenu, "'\">");
                    strcat(serveur.pages[i].contenu, texte);
                    strcat(serveur.pages[i].contenu, "</button><br>");
                    printf("🔲 Bouton '%s' -> '%s' ajouté\n", texte, lien);
                    break;
                }
            }
        }
    }
}

void gerer_liens(char* ligne) {
    if (strncmp(ligne, "externe ", 8) == 0) {
        char nom[100], url[500], texte[100];
        if (sscanf(ligne + 8, "%s %s %s", nom, url, texte) == 3) {
            if (serveur.nb_liens < MAX_LINKS) {
                strcpy(serveur.liens[serveur.nb_liens].nom, nom);
                strcpy(serveur.liens[serveur.nb_liens].url, url);
                strcpy(serveur.liens[serveur.nb_liens].texte, texte);
                serveur.nb_liens++;
                printf("🔗 Lien externe '%s' créé: %s\n", nom, url);
            }
        }
    }
    else if (strncmp(ligne, "ajouter ", 8) == 0) {
        char nom[100];
        sscanf(ligne + 8, "%s", nom);
        
        // Trouver le lien
        for (int i = 0; i < serveur.nb_liens; i++) {
            if (strcmp(serveur.liens[i].nom, nom) == 0) {
                // Ajouter à la page actuelle
                for (int j = 0; j < serveur.nb_pages; j++) {
                    if (strcmp(serveur.pages[j].nom, serveur.page_actuelle) == 0) {
                        strcat(serveur.pages[j].contenu, "<a href=\"");
                        strcat(serveur.pages[j].contenu, serveur.liens[i].url);
                        strcat(serveur.pages[j].contenu, "\" target=\"_blank\">");
                        strcat(serveur.pages[j].contenu, serveur.liens[i].texte);
                        strcat(serveur.pages[j].contenu, "</a><br>");
                        printf("🔗 Lien externe '%s' ajouté à la page\n", nom);
                        return;
                    }
                }
                // Si pas de page active, ajouter au contenu principal
                strcat(serveur.contenu_html, "<a href=\"");
                strcat(serveur.contenu_html, serveur.liens[i].url);
                strcat(serveur.contenu_html, "\" target=\"_blank\">");
                strcat(serveur.contenu_html, serveur.liens[i].texte);
                strcat(serveur.contenu_html, "</a><br>");
                printf("🔗 Lien externe '%s' ajouté\n", nom);
                return;
            }
        }
    }
}

void gerer_styles(char* ligne) {
    if (strncmp(ligne, "police ", 7) == 0) {
        char police[50];
        sscanf(ligne + 7, "%s", police);
        strcpy(serveur.police, police);
        printf("🎨 Police: %s\n", police);
    }
    else if (strncmp(ligne, "couleur_texte ", 14) == 0) {
        char couleur[50];
        sscanf(ligne + 14, "%s", couleur);
        strcpy(serveur.couleur_texte, couleur);
        printf("🎨 Couleur texte: %s\n", couleur);
    }
    else if (strncmp(ligne, "css ", 4) == 0) {
        char* css = ligne + 4;
        if (css[0] == '"') {
            css++;
            css[strlen(css)-1] = 0;
        }
        strcat(serveur.style_personnalise, css);
        strcat(serveur.style_personnalise, " ");
        printf("🎨 CSS personnalisé ajouté\n");
    }
    else if (strncmp(ligne, "page_css ", 9) == 0) {
        char* css = ligne + 9;
        if (css[0] == '"') {
            css++;
            css[strlen(css)-1] = 0;
        }
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].style_personnalise, css);
                strcat(serveur.pages[i].style_personnalise, " ");
                printf("🎨 CSS ajouté à la page '%s'\n", serveur.page_actuelle);
                break;
            }
        }
    }
}

void creer_variable(char* nom, double valeur) {
    // Vérifier si la variable existe déjà
    for (int i = 0; i < nb_variables; i++) {
        if (strcmp(variables[i].nom, nom) == 0) {
            variables[i].valeur = valeur;
            return;
        }
    }
    
    // Créer une nouvelle variable
    if (nb_variables < MAX_VARS) {
        strcpy(variables[nb_variables].nom, nom);
        variables[nb_variables].valeur = valeur;
        nb_variables++;
    }
}

double obtenir_variable(char* nom) {
    for (int i = 0; i < nb_variables; i++) {
        if (strcmp(variables[i].nom, nom) == 0) {
            return variables[i].valeur;
        }
    }
    return 0.0;
}

void calculer_mathematiques(char* ligne) {
    if (strncmp(ligne, "pythagore ", 10) == 0) {
        double a, b;
        if (sscanf(ligne + 10, "%lf %lf", &a, &b) == 2) {
            double c = sqrt(a*a + b*b);
            printf("📐 Théorème de Pythagore: √(%g² + %g²) = %g\n", a, b, c);
        }
    }
    else if (strncmp(ligne, "puissance ", 10) == 0) {
        double base, exp;
        if (sscanf(ligne + 10, "%lf %lf", &base, &exp) == 2) {
            double resultat = pow(base, exp);
            printf("⚡ %g^%g = %g\n", base, exp, resultat);
        }
    }
    else if (strncmp(ligne, "modulo ", 7) == 0) {
        int a, b;
        if (sscanf(ligne + 7, "%d %d", &a, &b) == 2) {
            int resultat = a % b;
            printf("🔢 %d modulo %d = %d\n", a, b, resultat);
        }
    }
    else {
        // Calcul simple
        double resultat = evaluer_expression(ligne);
        printf("🧮 Résultat: %g\n", resultat);
    }
}

double evaluer_expression(char* expr) {
    // Simple évaluateur d'expressions
    double a, b;
    char op;
    if (sscanf(expr, "%lf %c %lf", &a, &op, &b) == 3) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0 ? a / b : 0;
            default: return 0;
        }
    }
    return atof(expr);
}

void gerer_site_web(char* ligne) {
    if (strncmp(ligne, "port ", 5) == 0) {
        serveur.port = atoi(ligne + 5);
        serveur_actif = 1;
        printf("🌐 Port configuré: %d\n", serveur.port);
    }
    else if (strncmp(ligne, "fond ", 5) == 0) {
        strcpy(serveur.couleur_fond, ligne + 5);
        printf("🎨 Couleur de fond: %s\n", serveur.couleur_fond);
    }
    else if (strncmp(ligne, "titre ", 6) == 0) {
        char* titre = ligne + 6;
        if (titre[0] == '"') {
            titre++;
            titre[strlen(titre)-1] = 0;
        }
        strcpy(serveur.titre, titre);
        printf("📝 Titre: %s\n", serveur.titre);
    }
    else if (strncmp(ligne, "contenu ", 8) == 0) {
        char* contenu = ligne + 8;
        if (contenu[0] == '"') {
            contenu++;
            contenu[strlen(contenu)-1] = 0;
        }
        strcat(serveur.contenu_html, "<p>");
        strcat(serveur.contenu_html, contenu);
        strcat(serveur.contenu_html, "</p>");
        printf("📄 Contenu ajouté: %s\n", contenu);
    }
    else if (strncmp(ligne, "bouton ", 7) == 0) {
        char* texte = ligne + 7;
        if (texte[0] == '"') {
            texte++;
            texte[strlen(texte)-1] = 0;
        }
        strcat(serveur.contenu_html, "<button onclick='alert(\"");
        strcat(serveur.contenu_html, texte);
        strcat(serveur.contenu_html, "\")'>");
        strcat(serveur.contenu_html, texte);
        strcat(serveur.contenu_html, "</button><br>");
        printf("🔲 Bouton ajouté: %s\n", texte);
    }
}

void gerer_base_donnees(char* ligne) {
    if (strncmp(ligne, "sauver ", 7) == 0) {
        char cle[50], valeur[500], type[20] = "texte";
        if (sscanf(ligne + 7, "%s %s %s", cle, valeur, type) >= 2) {
            // Vérifier si l'entrée existe déjà
            for (int i = 0; i < nb_entrees_bd; i++) {
                if (strcmp(base_donnees[i].cle, cle) == 0) {
                    strcpy(base_donnees[i].valeur, valeur);
                    strcpy(base_donnees[i].type, type);
                    printf("💾 Mis à jour: %s = %s (%s)\n", cle, valeur, type);
                    return;
                }
            }
            
            // Créer nouvelle entrée
            if (nb_entrees_bd < MAX_DB_ENTRIES) {
                strcpy(base_donnees[nb_entrees_bd].cle, cle);
                strcpy(base_donnees[nb_entrees_bd].valeur, valeur);
                strcpy(base_donnees[nb_entrees_bd].type, type);
                nb_entrees_bd++;
                printf("💾 Sauvé: %s = %s (%s)\n", cle, valeur, type);
            }
        }
    }
    else if (strncmp(ligne, "lire ", 5) == 0) {
        char cle[50];
        strcpy(cle, ligne + 5);
        for (int i = 0; i < nb_entrees_bd; i++) {
            if (strcmp(base_donnees[i].cle, cle) == 0) {
                printf("📖 %s = %s (%s)\n", cle, base_donnees[i].valeur, base_donnees[i].type);
                return;
            }
        }
        printf("❌ Clé '%s' non trouvée\n", cle);
    }
    else if (strcmp(ligne, "lister") == 0) {
        printf("📚 Base de données (%d entrées):\n", nb_entrees_bd);
        for (int i = 0; i < nb_entrees_bd; i++) {
            printf("  %s = %s (%s)\n", base_donnees[i].cle, base_donnees[i].valeur, base_donnees[i].type);
        }
    }
    else if (strncmp(ligne, "supprimer ", 10) == 0) {
        char cle[50];
        strcpy(cle, ligne + 10);
        for (int i = 0; i < nb_entrees_bd; i++) {
            if (strcmp(base_donnees[i].cle, cle) == 0) {
                // Décaler les éléments
                for (int j = i; j < nb_entrees_bd - 1; j++) {
                    base_donnees[j] = base_donnees[j + 1];
                }
                nb_entrees_bd--;
                printf("🗑️ Entrée '%s' supprimée\n", cle);
                return;
            }
        }
        printf("❌ Clé '%s' non trouvée\n", cle);
    }
    else if (strncmp(ligne, "web_afficher", 12) == 0) {
        // Afficher la BD sur le site web
        strcat(serveur.contenu_html, "<h3>Base de Données</h3><table border='1'>");
        strcat(serveur.contenu_html, "<tr><th>Clé</th><th>Valeur</th><th>Type</th></tr>");
        for (int i = 0; i < nb_entrees_bd; i++) {
            strcat(serveur.contenu_html, "<tr><td>");
            strcat(serveur.contenu_html, base_donnees[i].cle);
            strcat(serveur.contenu_html, "</td><td>");
            strcat(serveur.contenu_html, base_donnees[i].valeur);
            strcat(serveur.contenu_html, "</td><td>");
            strcat(serveur.contenu_html, base_donnees[i].type);
            strcat(serveur.contenu_html, "</td></tr>");
        }
        strcat(serveur.contenu_html, "</table>");
        printf("📊 Base de données ajoutée au site web\n");
    }
}

char* generer_html_page(const char* nom_page) {
    static char html[MAX_HTML * 2];
    
    // Trouver la page
    Page* page = NULL;
    for (int i = 0; i < serveur.nb_pages; i++) {
        if (strcmp(serveur.pages[i].nom, nom_page) == 0) {
            page = &serveur.pages[i];
            break;
        }
    }
    
    if (!page) {
        // Page par défaut
        snprintf(html, sizeof(html), 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <title>%s</title>\n"
            "    <meta charset='utf-8'>\n"
            "    <style>\n"
            "        body { background-color: %s; font-family: %s; color: %s; margin: 40px; }\n"
            "        h1 { color: #333; }\n"
            "        button { padding: 10px 20px; margin: 5px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; }\n"
            "        button:hover { background: #0056b3; }\n"
            "        a { color: #007bff; text-decoration: none; }\n"
            "        a:hover { text-decoration: underline; }\n"
            "        table { border-collapse: collapse; width: 100%%; }\n"
            "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
            "        th { background-color: #f2f2f2; }\n"
            "        %s\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>%s</h1>\n"
            "    <p>🎉 Bienvenue sur votre site créé avec NYU!</p>\n"
            "    %s\n",
            serveur.titre,
            serveur.couleur_fond,
            serveur.police,
            serveur.couleur_texte,
            serveur.style_personnalise,
            serveur.titre,
            serveur.contenu_html
        );
        
        // Ajouter menu de navigation
        if (serveur.nb_pages > 0) {
            strcat(html, "<hr><h3>Navigation:</h3>");
            for (int i = 0; i < serveur.nb_pages; i++) {
                strcat(html, "<a href=\"/");
                strcat(html, serveur.pages[i].nom);
                strcat(html, "\">");
                strcat(html, serveur.pages[i].titre);
                strcat(html, "</a> | ");
            }
        }
        
        strcat(html, "<hr><small>Propulsé par NYU - Le langage de programmation français</small></body></html>");
    } else {
        // Page spécifique
        snprintf(html, sizeof(html), 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <title>%s</title>\n"
            "    <meta charset='utf-8'>\n"
            "    <style>\n"
            "        body { background-color: %s; font-family: %s; color: %s; margin: 40px; }\n"
            "        h1 { color: #333; }\n"
            "        button { padding: 10px 20px; margin: 5px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; }\n"
            "        button:hover { background: #0056b3; }\n"
            "        a { color: #007bff; text-decoration: none; }\n"
            "        a:hover { text-decoration: underline; }\n"
            "        table { border-collapse: collapse; width: 100%%; }\n"
            "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
            "        th { background-color: #f2f2f2; }\n"
            "        %s\n"
            "        %s\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>%s</h1>\n"
            "    %s\n",
            page->titre,
            page->couleur_fond[0] ? page->couleur_fond : serveur.couleur_fond,
            serveur.police,
            serveur.couleur_texte,
            serveur.style_personnalise,
            page->style_personnalise,
            page->titre,
            page->contenu
        );
        
        // Ajouter menu de navigation
        strcat(html, "<hr><h3>Navigation:</h3>");
        strcat(html, "<a href=\"/\">Accueil</a> | ");
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, nom_page) != 0) {
                strcat(html, "<a href=\"/");
                strcat(html, serveur.pages[i].nom);
                strcat(html, "\">");
                strcat(html, serveur.pages[i].titre);
                strcat(html, "</a> | ");
            }
        }
        
        strcat(html, "<hr><small>Propulsé par NYU - Le langage de programmation français</small></body></html>");
    }
    
    return html;
}

void* serveur_thread(void* arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return NULL;
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return NULL;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(serveur.port);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return NULL;
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return NULL;
    }
    
    printf("✅ Serveur NYU démarré sur 0.0.0.0:%d\n", serveur.port);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        
        read(new_socket, buffer, 1024);
        
        // Parser l'URL
        char* url = strstr(buffer, "GET ");
        char page_demandee[100] = "";
        if (url) {
            url += 4; // Skip "GET "
            char* end = strchr(url, ' ');
            if (end) {
                *end = '\0';
                if (strcmp(url, "/") == 0) {
                    strcpy(page_demandee, "accueil");
                } else if (url[0] == '/') {
                    strcpy(page_demandee, url + 1);
                }
            }
        }
        
        char* response = generer_html_page(page_demandee[0] ? page_demandee : "accueil");
        
        char header[500];
        snprintf(header, sizeof(header), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n\r\n");
        
        send(new_socket, header, strlen(header), 0);
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }
    
    return NULL;
}

void demarrer_serveur() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, serveur_thread, NULL);
    
    // Garder le programme en vie
    printf("📱 Serveur en cours d'exécution. Appuyez sur Ctrl+C pour arrêter.\n");
    while (1) {
        sleep(1);
    }
}

void afficher_aide() {
    printf("🇫🇷 NYU - Langage de Programmation Français\n");
    printf("===========================================\n\n");
    printf("COMMANDES PRINCIPALES:\n");
    printf("  variable nom = valeur     - Créer une variable\n");
    printf("  afficher \"texte\"          - Afficher du texte\n");
    printf("  afficher nom_variable     - Afficher une variable\n\n");
    printf("MATHÉMATIQUES:\n");
    printf("  calculer 5 + 3           - Calculs de base\n");
    printf("  calculer puissance 2 8   - Puissance (2^8)\n");
    printf("  calculer pythagore 3 4   - Théorème de Pythagore\n");
    printf("  calculer modulo 10 3     - Modulo\n\n");
    printf("SITE WEB:\n");
    printf("  site port 5000           - Définir le port\n");
    printf("  site fond lightblue      - Couleur de fond\n");
    printf("  site titre \"Mon Site\"     - Titre de la page\n");
    printf("  site contenu \"Bonjour!\"   - Ajouter du contenu\n");
    printf("  site bouton \"Cliquez\"     - Ajouter un bouton\n\n");
    printf("PAGES MULTIPLES:\n");
    printf("  page créer accueil       - Créer une page\n");
    printf("  page changer accueil     - Changer de page active\n");
    printf("  page titre \"Accueil\"     - Titre de la page\n");
    printf("  page contenu \"Bonjour\"   - Contenu de la page\n");
    printf("  page bouton Contactez /contact - Bouton vers page\n\n");
    printf("LIENS EXTERNES:\n");
    printf("  lien externe google https://google.com Google - Créer un lien\n");
    printf("  lien ajouter google      - Ajouter le lien à la page\n\n");
    printf("STYLES AVANCÉS:\n");
    printf("  style police Georgia     - Changer la police\n");
    printf("  style couleur_texte red  - Couleur du texte\n");
    printf("  style css \"h1{color:blue;}\" - CSS personnalisé\n");
    printf("  style page_css \"p{font-size:18px;}\" - CSS pour page\n\n");
    printf("BASE DE DONNÉES AVANCÉE:\n");
    printf("  bd sauver nom Jean texte - Sauvegarder avec type\n");
    printf("  bd lire nom              - Lire une entrée\n");
    printf("  bd lister                - Lister toutes les entrées\n");
    printf("  bd supprimer nom         - Supprimer une entrée\n");
    printf("  bd web_afficher          - Afficher BD sur site web\n\n");
    printf("EXEMPLE COMPLET:\n");
    printf("  ./main démarrer site_avance.nyu\n\n");
}
