
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
#define MAX_HTML 10000
#define MAX_DB_ENTRIES 1000

// Structure pour les variables
typedef struct {
    char nom[50];
    double valeur;
} Variable;

// Structure pour la base de données
typedef struct {
    char cle[50];
    char valeur[200];
} EntreeBD;

// Structure pour le serveur web
typedef struct {
    int port;
    char contenu_html[MAX_HTML];
    char couleur_fond[20];
    char titre[100];
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
        executer_nyu(argv[2]);
    } else {
        printf("Commande inconnue. Utilisez 'démarrer' ou 'aide'\n");
    }
    
    return 0;
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
            contenu[strlen(contenu)-1] = 0; // Supprimer la dernière guillemet
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
    
    // Base de données
    else if (strncmp(ligne, "bd ", 3) == 0) {
        gerer_base_donnees(ligne + 3);
    }
    
    // Boucles
    else if (strncmp(ligne, "répéter ", 8) == 0) {
        int fois;
        sscanf(ligne + 8, "%d", &fois);
        printf("🔄 Répétition %d fois\n", fois);
    }
    
    // Conditions
    else if (strncmp(ligne, "si ", 3) == 0) {
        printf("🤔 Condition: %s\n", ligne + 3);
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
        strcpy(serveur.titre, ligne + 6);
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
        char cle[50], valeur[200];
        if (sscanf(ligne + 7, "%s %s", cle, valeur) == 2) {
            if (nb_entrees_bd < MAX_DB_ENTRIES) {
                strcpy(base_donnees[nb_entrees_bd].cle, cle);
                strcpy(base_donnees[nb_entrees_bd].valeur, valeur);
                nb_entrees_bd++;
                printf("💾 Sauvé: %s = %s\n", cle, valeur);
            }
        }
    }
    else if (strncmp(ligne, "lire ", 5) == 0) {
        char cle[50];
        strcpy(cle, ligne + 5);
        for (int i = 0; i < nb_entrees_bd; i++) {
            if (strcmp(base_donnees[i].cle, cle) == 0) {
                printf("📖 %s = %s\n", cle, base_donnees[i].valeur);
                return;
            }
        }
        printf("❌ Clé '%s' non trouvée\n", cle);
    }
    else if (strcmp(ligne, "lister") == 0) {
        printf("📚 Base de données:\n");
        for (int i = 0; i < nb_entrees_bd; i++) {
            printf("  %s = %s\n", base_donnees[i].cle, base_donnees[i].valeur);
        }
    }
}

void* serveur_thread(void* arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
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
        
        char response[MAX_HTML + 1000];
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <title>%s</title>\n"
            "    <meta charset='utf-8'>\n"
            "    <style>\n"
            "        body { background-color: %s; font-family: Arial; margin: 40px; }\n"
            "        h1 { color: #333; }\n"
            "        button { padding: 10px 20px; margin: 5px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; }\n"
            "        button:hover { background: #0056b3; }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>%s</h1>\n"
            "    <p>🎉 Bienvenue sur votre site créé avec NYU!</p>\n"
            "    %s\n"
            "    <hr>\n"
            "    <small>Propulsé par NYU - Le langage de programmation français</small>\n"
            "</body>\n"
            "</html>",
            serveur.titre[0] ? serveur.titre : "Site NYU",
            serveur.couleur_fond[0] ? serveur.couleur_fond : "#f0f8ff",
            serveur.titre[0] ? serveur.titre : "Mon Site NYU",
            serveur.contenu_html
        );
        
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
    printf("BASE DE DONNÉES:\n");
    printf("  bd sauver nom Jean       - Sauvegarder une entrée\n");
    printf("  bd lire nom              - Lire une entrée\n");
    printf("  bd lister                - Lister toutes les entrées\n\n");
    printf("EXEMPLE:\n");
    printf("  ./main démarrer exemple.nyu\n\n");
}
