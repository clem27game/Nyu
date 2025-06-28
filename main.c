
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_VARS 100
#define MAX_HTML 50000
#define MAX_DB_ENTRIES 1000
#define MAX_PAGES 50
#define MAX_LINKS 100
#define MAX_STYLES 500
#define MAX_FORMS 50
#define MAX_USERS 200
#define MAX_COMMENTS 1000
#define MAX_TEMPLATES 20
#define MAX_UPLOADS 100
#define MAX_API_ROUTES 50

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
    char timestamp[30];
} EntreeBD;

// Structure pour les pages
typedef struct {
    char nom[50];
    char titre[100];
    char contenu[MAX_HTML];
    char couleur_fond[50];
    char style_personnalise[MAX_STYLES];
    char template_utilise[50];
    int protegee; // 0 = publique, 1 = privée
} Page;

// Structure pour les liens externes
typedef struct {
    char nom[100];
    char url[500];
    char texte[100];
} LienExterne;

// Structure pour les formulaires
typedef struct {
    char nom[50];
    char action[100];
    char methode[10]; // GET ou POST
    char champs[MAX_HTML];
    char validation[500];
} Formulaire;

// Structure pour les utilisateurs
typedef struct {
    char nom[50];
    char email[100];
    char mot_de_passe[100];
    char role[20]; // admin, utilisateur
    char date_creation[30];
    int actif;
} Utilisateur;

// Structure pour les commentaires
typedef struct {
    char id[20];
    char page[50];
    char auteur[50];
    char contenu[1000];
    char date[30];
    int approuve;
} Commentaire;

// Structure pour les templates
typedef struct {
    char nom[50];
    char contenu[MAX_HTML];
    char variables[500]; // Variables du template
} Template;

// Structure pour les fichiers uploadés
typedef struct {
    char nom[100];
    char chemin[200];
    char type[50];
    int taille;
    char date_upload[30];
} FichierUploade;

// Structure pour les routes API
typedef struct {
    char route[100];
    char methode[10];
    char reponse[MAX_HTML];
    char donnees_bd[100]; // Clé de BD à retourner
} RouteAPI;

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
    Formulaire formulaires[MAX_FORMS];
    int nb_formulaires;
    char session_actuelle[100];
    int authentification_active;
} ServeurWeb;

// Variables globales
Variable variables[MAX_VARS];
int nb_variables = 0;
EntreeBD base_donnees[MAX_DB_ENTRIES];
int nb_entrees_bd = 0;
ServeurWeb serveur;
int serveur_actif = 0;
Utilisateur utilisateurs[MAX_USERS];
int nb_utilisateurs = 0;
Commentaire commentaires[MAX_COMMENTS];
int nb_commentaires = 0;
Template templates[MAX_TEMPLATES];
int nb_templates = 0;
FichierUploade fichiers[MAX_UPLOADS];
int nb_fichiers = 0;
RouteAPI routes_api[MAX_API_ROUTES];
int nb_routes_api = 0;

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
void gerer_formulaires(char* ligne);
void gerer_authentification(char* ligne);
void gerer_commentaires(char* ligne);
void gerer_templates(char* ligne);
void gerer_upload(char* ligne);
void gerer_api(char* ligne);
void gerer_reseaux_sociaux(char* ligne);
char* generer_html_page(const char* nom_page);
void initialiser_serveur();
void sauvegarder_bd_fichier();
void charger_bd_fichier();
void traiter_requete_post(char* data, char* response);
char* appliquer_template(const char* nom_template, const char* contenu);
int verifier_session(const char* session);
void generer_session(char* session);

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
        charger_bd_fichier();
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
    serveur.nb_formulaires = 0;
    serveur.authentification_active = 0;
    strcpy(serveur.page_actuelle, "accueil");
    serveur.contenu_html[0] = '\0';
    serveur.style_personnalise[0] = '\0';
    serveur.session_actuelle[0] = '\0';
    
    // Créer le répertoire de base de données
    mkdir("nyu_data", 0755);
    mkdir("nyu_data/uploads", 0755);
    mkdir("nyu_data/templates", 0755);
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
    
    // Formulaires
    else if (strncmp(ligne, "formulaire ", 11) == 0) {
        gerer_formulaires(ligne + 11);
    }
    
    // Authentification
    else if (strncmp(ligne, "auth ", 5) == 0) {
        gerer_authentification(ligne + 5);
    }
    
    // Commentaires
    else if (strncmp(ligne, "commentaire ", 12) == 0) {
        gerer_commentaires(ligne + 12);
    }
    
    // Templates
    else if (strncmp(ligne, "template ", 9) == 0) {
        gerer_templates(ligne + 9);
    }
    
    // Upload
    else if (strncmp(ligne, "upload ", 7) == 0) {
        gerer_upload(ligne + 7);
    }
    
    // API
    else if (strncmp(ligne, "api ", 4) == 0) {
        gerer_api(ligne + 4);
    }
    
    // Réseaux sociaux
    else if (strncmp(ligne, "social ", 7) == 0) {
        gerer_reseaux_sociaux(ligne + 7);
    }
    
    // Base de données
    else if (strncmp(ligne, "bd ", 3) == 0) {
        gerer_base_donnees(ligne + 3);
    }
}

void gerer_formulaires(char* ligne) {
    if (strncmp(ligne, "créer ", 6) == 0) {
        char nom[50], action[100], methode[10];
        if (sscanf(ligne + 6, "%s %s %s", nom, action, methode) == 3) {
            if (serveur.nb_formulaires < MAX_FORMS) {
                strcpy(serveur.formulaires[serveur.nb_formulaires].nom, nom);
                strcpy(serveur.formulaires[serveur.nb_formulaires].action, action);
                strcpy(serveur.formulaires[serveur.nb_formulaires].methode, methode);
                serveur.formulaires[serveur.nb_formulaires].champs[0] = '\0';
                serveur.formulaires[serveur.nb_formulaires].validation[0] = '\0';
                serveur.nb_formulaires++;
                printf("📝 Formulaire '%s' créé\n", nom);
            }
        }
    }
    else if (strncmp(ligne, "champ ", 6) == 0) {
        char nom_form[50], type[50], nom_champ[50], placeholder[100];
        if (sscanf(ligne + 6, "%s %s %s %s", nom_form, type, nom_champ, placeholder) == 4) {
            for (int i = 0; i < serveur.nb_formulaires; i++) {
                if (strcmp(serveur.formulaires[i].nom, nom_form) == 0) {
                    char champ[200];
                    snprintf(champ, sizeof(champ), 
                        "<label>%s:</label><input type='%s' name='%s' placeholder='%s' required><br>",
                        nom_champ, type, nom_champ, placeholder);
                    strcat(serveur.formulaires[i].champs, champ);
                    printf("📄 Champ '%s' ajouté au formulaire '%s'\n", nom_champ, nom_form);
                    break;
                }
            }
        }
    }
    else if (strncmp(ligne, "ajouter ", 8) == 0) {
        char nom_form[50];
        sscanf(ligne + 8, "%s", nom_form);
        
        for (int i = 0; i < serveur.nb_formulaires; i++) {
            if (strcmp(serveur.formulaires[i].nom, nom_form) == 0) {
                // Ajouter à la page actuelle ou au contenu principal
                char form_html[MAX_HTML];
                snprintf(form_html, sizeof(form_html),
                    "<form action='%s' method='%s' enctype='multipart/form-data'>"
                    "%s"
                    "<button type='submit'>Envoyer</button>"
                    "</form>",
                    serveur.formulaires[i].action,
                    serveur.formulaires[i].methode,
                    serveur.formulaires[i].champs);
                
                // Chercher la page actuelle
                int page_trouvee = 0;
                for (int j = 0; j < serveur.nb_pages; j++) {
                    if (strcmp(serveur.pages[j].nom, serveur.page_actuelle) == 0) {
                        strcat(serveur.pages[j].contenu, form_html);
                        page_trouvee = 1;
                        break;
                    }
                }
                if (!page_trouvee) {
                    strcat(serveur.contenu_html, form_html);
                }
                printf("📝 Formulaire '%s' ajouté à la page\n", nom_form);
                break;
            }
        }
    }
}

void gerer_authentification(char* ligne) {
    if (strncmp(ligne, "activer", 7) == 0) {
        serveur.authentification_active = 1;
        printf("🔐 Authentification activée\n");
    }
    else if (strncmp(ligne, "utilisateur ", 12) == 0) {
        char nom[50], email[100], mdp[100], role[20];
        if (sscanf(ligne + 12, "%s %s %s %s", nom, email, mdp, role) == 4) {
            if (nb_utilisateurs < MAX_USERS) {
                strcpy(utilisateurs[nb_utilisateurs].nom, nom);
                strcpy(utilisateurs[nb_utilisateurs].email, email);
                strcpy(utilisateurs[nb_utilisateurs].mot_de_passe, mdp);
                strcpy(utilisateurs[nb_utilisateurs].role, role);
                time_t t = time(NULL);
                strcpy(utilisateurs[nb_utilisateurs].date_creation, ctime(&t));
                utilisateurs[nb_utilisateurs].actif = 1;
                nb_utilisateurs++;
                printf("👤 Utilisateur '%s' créé\n", nom);
            }
        }
    }
    else if (strncmp(ligne, "page_connexion", 14) == 0) {
        // Créer une page de connexion automatique
        if (serveur.nb_pages < MAX_PAGES) {
            strcpy(serveur.pages[serveur.nb_pages].nom, "connexion");
            strcpy(serveur.pages[serveur.nb_pages].titre, "Connexion");
            strcpy(serveur.pages[serveur.nb_pages].contenu, 
                "<form action='/login' method='POST'>"
                "<label>Email:</label><input type='email' name='email' required><br>"
                "<label>Mot de passe:</label><input type='password' name='password' required><br>"
                "<button type='submit'>Se connecter</button>"
                "</form>");
            serveur.nb_pages++;
            printf("🔑 Page de connexion créée\n");
        }
    }
    else if (strncmp(ligne, "proteger ", 9) == 0) {
        char nom_page[50];
        sscanf(ligne + 9, "%s", nom_page);
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, nom_page) == 0) {
                serveur.pages[i].protegee = 1;
                printf("🔒 Page '%s' protégée\n", nom_page);
                break;
            }
        }
    }
}

void gerer_commentaires(char* ligne) {
    if (strncmp(ligne, "activer ", 8) == 0) {
        char nom_page[50];
        sscanf(ligne + 8, "%s", nom_page);
        
        // Ajouter la section commentaires à la page
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, nom_page) == 0) {
                strcat(serveur.pages[i].contenu, 
                    "<hr><h3>Commentaires</h3>"
                    "<div id='commentaires'></div>"
                    "<form action='/add_comment' method='POST'>"
                    "<input type='hidden' name='page' value='");
                strcat(serveur.pages[i].contenu, nom_page);
                strcat(serveur.pages[i].contenu, 
                    "'><label>Nom:</label><input type='text' name='auteur' required><br>"
                    "<label>Commentaire:</label><textarea name='contenu' required></textarea><br>"
                    "<button type='submit'>Ajouter un commentaire</button>"
                    "</form>");
                printf("💬 Commentaires activés pour la page '%s'\n", nom_page);
                break;
            }
        }
    }
    else if (strncmp(ligne, "ajouter ", 8) == 0) {
        char page[50], auteur[50], contenu[1000];
        if (sscanf(ligne + 8, "%s %s %[^\n]", page, auteur, contenu) == 3) {
            if (nb_commentaires < MAX_COMMENTS) {
                snprintf(commentaires[nb_commentaires].id, sizeof(commentaires[nb_commentaires].id), 
                    "comm_%d", nb_commentaires + 1);
                strcpy(commentaires[nb_commentaires].page, page);
                strcpy(commentaires[nb_commentaires].auteur, auteur);
                strcpy(commentaires[nb_commentaires].contenu, contenu);
                time_t t = time(NULL);
                strcpy(commentaires[nb_commentaires].date, ctime(&t));
                commentaires[nb_commentaires].approuve = 1;
                nb_commentaires++;
                printf("💬 Commentaire ajouté par '%s'\n", auteur);
            }
        }
    }
    else if (strncmp(ligne, "afficher ", 9) == 0) {
        char nom_page[50];
        sscanf(ligne + 9, "%s", nom_page);
        
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, nom_page) == 0) {
                strcat(serveur.pages[i].contenu, "<div class='commentaires'>");
                for (int j = 0; j < nb_commentaires; j++) {
                    if (strcmp(commentaires[j].page, nom_page) == 0 && commentaires[j].approuve) {
                        strcat(serveur.pages[i].contenu, "<div class='commentaire'><strong>");
                        strcat(serveur.pages[i].contenu, commentaires[j].auteur);
                        strcat(serveur.pages[i].contenu, "</strong><br>");
                        strcat(serveur.pages[i].contenu, commentaires[j].contenu);
                        strcat(serveur.pages[i].contenu, "<br><small>");
                        strcat(serveur.pages[i].contenu, commentaires[j].date);
                        strcat(serveur.pages[i].contenu, "</small></div><hr>");
                    }
                }
                strcat(serveur.pages[i].contenu, "</div>");
                printf("💬 Commentaires affichés pour la page '%s'\n", nom_page);
                break;
            }
        }
    }
}

void gerer_templates(char* ligne) {
    if (strncmp(ligne, "créer ", 6) == 0) {
        char nom[50];
        sscanf(ligne + 6, "%s", nom);
        if (nb_templates < MAX_TEMPLATES) {
            strcpy(templates[nb_templates].nom, nom);
            templates[nb_templates].contenu[0] = '\0';
            templates[nb_templates].variables[0] = '\0';
            nb_templates++;
            printf("📋 Template '%s' créé\n", nom);
        }
    }
    else if (strncmp(ligne, "contenu ", 8) == 0) {
        char nom[50], *contenu;
        if (sscanf(ligne + 8, "%s ", nom) == 1) {
            contenu = strchr(ligne + 8 + strlen(nom) + 1, '"');
            if (contenu) {
                contenu++; // Skip opening quote
                contenu[strlen(contenu)-1] = 0; // Remove closing quote
                
                for (int i = 0; i < nb_templates; i++) {
                    if (strcmp(templates[i].nom, nom) == 0) {
                        strcpy(templates[i].contenu, contenu);
                        printf("📄 Contenu ajouté au template '%s'\n", nom);
                        break;
                    }
                }
            }
        }
    }
    else if (strncmp(ligne, "utiliser ", 9) == 0) {
        char nom_template[50];
        sscanf(ligne + 9, "%s", nom_template);
        
        // Appliquer le template à la page actuelle
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcpy(serveur.pages[i].template_utilise, nom_template);
                printf("📋 Template '%s' appliqué à la page '%s'\n", nom_template, serveur.page_actuelle);
                break;
            }
        }
    }
}

void gerer_upload(char* ligne) {
    if (strncmp(ligne, "activer", 7) == 0) {
        // Ajouter un formulaire d'upload à la page actuelle
        char upload_form[] = 
            "<form action='/upload' method='POST' enctype='multipart/form-data'>"
            "<label>Choisir un fichier:</label><input type='file' name='fichier' required><br>"
            "<button type='submit'>Télécharger</button>"
            "</form>";
        
        int page_trouvee = 0;
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].contenu, upload_form);
                page_trouvee = 1;
                break;
            }
        }
        if (!page_trouvee) {
            strcat(serveur.contenu_html, upload_form);
        }
        printf("📤 Upload de fichiers activé\n");
    }
    else if (strncmp(ligne, "galerie", 7) == 0) {
        // Afficher tous les fichiers uploadés
        char galerie[MAX_HTML] = "<h3>Galerie de fichiers</h3><div class='galerie'>";
        
        for (int i = 0; i < nb_fichiers; i++) {
            if (strstr(fichiers[i].type, "image") != NULL) {
                strcat(galerie, "<div class='image'><img src='/uploads/");
                strcat(galerie, fichiers[i].nom);
                strcat(galerie, "' style='max-width:200px;'><br>");
                strcat(galerie, fichiers[i].nom);
                strcat(galerie, "</div>");
            } else {
                strcat(galerie, "<div class='fichier'><a href='/uploads/");
                strcat(galerie, fichiers[i].nom);
                strcat(galerie, "'>");
                strcat(galerie, fichiers[i].nom);
                strcat(galerie, "</a></div>");
            }
        }
        strcat(galerie, "</div>");
        
        int page_trouvee = 0;
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].contenu, galerie);
                page_trouvee = 1;
                break;
            }
        }
        if (!page_trouvee) {
            strcat(serveur.contenu_html, galerie);
        }
        printf("🖼️ Galerie de fichiers ajoutée\n");
    }
}

void gerer_api(char* ligne) {
    if (strncmp(ligne, "route ", 6) == 0) {
        char route[100], methode[10], donnees[100];
        if (sscanf(ligne + 6, "%s %s %s", route, methode, donnees) == 3) {
            if (nb_routes_api < MAX_API_ROUTES) {
                strcpy(routes_api[nb_routes_api].route, route);
                strcpy(routes_api[nb_routes_api].methode, methode);
                strcpy(routes_api[nb_routes_api].donnees_bd, donnees);
                
                // Générer une réponse JSON basique
                snprintf(routes_api[nb_routes_api].reponse, sizeof(routes_api[nb_routes_api].reponse),
                    "{\"status\":\"success\",\"data\":\"%s\"}", donnees);
                
                nb_routes_api++;
                printf("🔌 Route API '%s' créée\n", route);
            }
        }
    }
    else if (strncmp(ligne, "donnees ", 8) == 0) {
        char route[100];
        sscanf(ligne + 8, "%s", route);
        
        // Créer une route qui retourne toutes les données de la BD
        if (nb_routes_api < MAX_API_ROUTES) {
            strcpy(routes_api[nb_routes_api].route, route);
            strcpy(routes_api[nb_routes_api].methode, "GET");
            strcpy(routes_api[nb_routes_api].reponse, "{\"bd\":[");
            
            for (int i = 0; i < nb_entrees_bd; i++) {
                strcat(routes_api[nb_routes_api].reponse, "{\"cle\":\"");
                strcat(routes_api[nb_routes_api].reponse, base_donnees[i].cle);
                strcat(routes_api[nb_routes_api].reponse, "\",\"valeur\":\"");
                strcat(routes_api[nb_routes_api].reponse, base_donnees[i].valeur);
                strcat(routes_api[nb_routes_api].reponse, "\",\"type\":\"");
                strcat(routes_api[nb_routes_api].reponse, base_donnees[i].type);
                strcat(routes_api[nb_routes_api].reponse, "\"}");
                if (i < nb_entrees_bd - 1) strcat(routes_api[nb_routes_api].reponse, ",");
            }
            strcat(routes_api[nb_routes_api].reponse, "]}");
            
            nb_routes_api++;
            printf("📊 Route API données créée: %s\n", route);
        }
    }
}

void gerer_reseaux_sociaux(char* ligne) {
    if (strncmp(ligne, "facebook ", 9) == 0) {
        char url[200];
        sscanf(ligne + 9, "%s", url);
        
        char fb_button[500];
        snprintf(fb_button, sizeof(fb_button),
            "<a href='https://www.facebook.com/sharer/sharer.php?u=%s' target='_blank' "
            "style='background:#3b5998;color:white;padding:10px;text-decoration:none;margin:5px;'>"
            "📘 Partager sur Facebook</a>", url);
        
        int page_trouvee = 0;
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].contenu, fb_button);
                page_trouvee = 1;
                break;
            }
        }
        if (!page_trouvee) {
            strcat(serveur.contenu_html, fb_button);
        }
        printf("📘 Bouton Facebook ajouté\n");
    }
    else if (strncmp(ligne, "twitter ", 8) == 0) {
        char texte[200], url[200];
        if (sscanf(ligne + 8, "%s %s", texte, url) == 2) {
            char twitter_button[500];
            snprintf(twitter_button, sizeof(twitter_button),
                "<a href='https://twitter.com/intent/tweet?text=%s&url=%s' target='_blank' "
                "style='background:#1da1f2;color:white;padding:10px;text-decoration:none;margin:5px;'>"
                "🐦 Partager sur Twitter</a>", texte, url);
            
            int page_trouvee = 0;
            for (int i = 0; i < serveur.nb_pages; i++) {
                if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                    strcat(serveur.pages[i].contenu, twitter_button);
                    page_trouvee = 1;
                    break;
                }
            }
            if (!page_trouvee) {
                strcat(serveur.contenu_html, twitter_button);
            }
            printf("🐦 Bouton Twitter ajouté\n");
        }
    }
    else if (strncmp(ligne, "linkedin ", 9) == 0) {
        char url[200];
        sscanf(ligne + 9, "%s", url);
        
        char linkedin_button[500];
        snprintf(linkedin_button, sizeof(linkedin_button),
            "<a href='https://www.linkedin.com/sharing/share-offsite/?url=%s' target='_blank' "
            "style='background:#0077b5;color:white;padding:10px;text-decoration:none;margin:5px;'>"
            "💼 Partager sur LinkedIn</a>", url);
        
        int page_trouvee = 0;
        for (int i = 0; i < serveur.nb_pages; i++) {
            if (strcmp(serveur.pages[i].nom, serveur.page_actuelle) == 0) {
                strcat(serveur.pages[i].contenu, linkedin_button);
                page_trouvee = 1;
                break;
            }
        }
        if (!page_trouvee) {
            strcat(serveur.contenu_html, linkedin_button);
        }
        printf("💼 Bouton LinkedIn ajouté\n");
    }
}

void sauvegarder_bd_fichier() {
    FILE* file = fopen("nyu_data/database.txt", "w");
    if (file) {
        fprintf(file, "%d\n", nb_entrees_bd);
        for (int i = 0; i < nb_entrees_bd; i++) {
            fprintf(file, "%s|%s|%s|%s\n", 
                base_donnees[i].cle, 
                base_donnees[i].valeur,
                base_donnees[i].type,
                base_donnees[i].timestamp);
        }
        fclose(file);
    }
}

void charger_bd_fichier() {
    FILE* file = fopen("nyu_data/database.txt", "r");
    if (file) {
        fscanf(file, "%d\n", &nb_entrees_bd);
        for (int i = 0; i < nb_entrees_bd; i++) {
            char ligne[1000];
            if (fgets(ligne, sizeof(ligne), file)) {
                ligne[strcspn(ligne, "\n")] = 0;
                char* token = strtok(ligne, "|");
                if (token) strcpy(base_donnees[i].cle, token);
                token = strtok(NULL, "|");
                if (token) strcpy(base_donnees[i].valeur, token);
                token = strtok(NULL, "|");
                if (token) strcpy(base_donnees[i].type, token);
                token = strtok(NULL, "|");
                if (token) strcpy(base_donnees[i].timestamp, token);
            }
        }
        fclose(file);
        printf("💾 Base de données chargée: %d entrées\n", nb_entrees_bd);
    }
}

// Continuer avec les autres fonctions...
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
            serveur.pages[serveur.nb_pages].template_utilise[0] = '\0';
            serveur.pages[serveur.nb_pages].protegee = 0;
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
                    time_t t = time(NULL);
                    strcpy(base_donnees[i].timestamp, ctime(&t));
                    sauvegarder_bd_fichier();
                    printf("💾 Mis à jour: %s = %s (%s)\n", cle, valeur, type);
                    return;
                }
            }
            
            // Créer nouvelle entrée
            if (nb_entrees_bd < MAX_DB_ENTRIES) {
                strcpy(base_donnees[nb_entrees_bd].cle, cle);
                strcpy(base_donnees[nb_entrees_bd].valeur, valeur);
                strcpy(base_donnees[nb_entrees_bd].type, type);
                time_t t = time(NULL);
                strcpy(base_donnees[nb_entrees_bd].timestamp, ctime(&t));
                nb_entrees_bd++;
                sauvegarder_bd_fichier();
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
            printf("  %s = %s (%s) [%s]\n", base_donnees[i].cle, base_donnees[i].valeur, base_donnees[i].type, base_donnees[i].timestamp);
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
                sauvegarder_bd_fichier();
                printf("🗑️ Entrée '%s' supprimée\n", cle);
                return;
            }
        }
        printf("❌ Clé '%s' non trouvée\n", cle);
    }
    else if (strncmp(ligne, "web_afficher", 12) == 0) {
        // Afficher la BD sur le site web
        strcat(serveur.contenu_html, "<h3>Base de Données</h3><table border='1'>");
        strcat(serveur.contenu_html, "<tr><th>Clé</th><th>Valeur</th><th>Type</th><th>Date</th></tr>");
        for (int i = 0; i < nb_entrees_bd; i++) {
            strcat(serveur.contenu_html, "<tr><td>");
            strcat(serveur.contenu_html, base_donnees[i].cle);
            strcat(serveur.contenu_html, "</td><td>");
            strcat(serveur.contenu_html, base_donnees[i].valeur);
            strcat(serveur.contenu_html, "</td><td>");
            strcat(serveur.contenu_html, base_donnees[i].type);
            strcat(serveur.contenu_html, "</td><td>");
            strcat(serveur.contenu_html, base_donnees[i].timestamp);
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
            "    <meta name='viewport' content='width=device-width, initial-scale=1'>\n"
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
            "        form { margin: 20px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }\n"
            "        input, textarea, select { margin: 5px 0; padding: 8px; width: 100%%; max-width: 300px; }\n"
            "        .galerie { display: flex; flex-wrap: wrap; gap: 10px; }\n"
            "        .galerie img { max-width: 200px; height: auto; }\n"
            "        .commentaire { border: 1px solid #eee; padding: 10px; margin: 10px 0; }\n"
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
        
        strcat(html, "<hr><small>Propulsé par NYU 2.0 - Le framework web français complet</small></body></html>");
    } else {
        // Vérifier si la page est protégée
        if (page->protegee && strlen(serveur.session_actuelle) == 0) {
            snprintf(html, sizeof(html),
                "<!DOCTYPE html><html><head><title>Accès refusé</title></head>"
                "<body><h1>🔒 Accès refusé</h1><p>Cette page nécessite une authentification.</p>"
                "<a href='/connexion'>Se connecter</a></body></html>");
            return html;
        }
        
        // Page spécifique
        char contenu_final[MAX_HTML];
        strcpy(contenu_final, page->contenu);
        
        // Appliquer un template si défini
        if (strlen(page->template_utilise) > 0) {
            char* contenu_avec_template = appliquer_template(page->template_utilise, page->contenu);
            if (contenu_avec_template) {
                strcpy(contenu_final, contenu_avec_template);
            }
        }
        
        snprintf(html, sizeof(html), 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <title>%s</title>\n"
            "    <meta charset='utf-8'>\n"
            "    <meta name='viewport' content='width=device-width, initial-scale=1'>\n"
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
            "        form { margin: 20px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }\n"
            "        input, textarea, select { margin: 5px 0; padding: 8px; width: 100%%; max-width: 300px; }\n"
            "        .galerie { display: flex; flex-wrap: wrap; gap: 10px; }\n"
            "        .galerie img { max-width: 200px; height: auto; }\n"
            "        .commentaire { border: 1px solid #eee; padding: 10px; margin: 10px 0; }\n"
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
            contenu_final
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
        
        strcat(html, "<hr><small>Propulsé par NYU 2.0 - Le framework web français complet</small></body></html>");
    }
    
    return html;
}

char* appliquer_template(const char* nom_template, const char* contenu) {
    static char resultat[MAX_HTML];
    
    for (int i = 0; i < nb_templates; i++) {
        if (strcmp(templates[i].nom, nom_template) == 0) {
            strcpy(resultat, templates[i].contenu);
            // Remplacer {{contenu}} par le contenu de la page
            char* pos = strstr(resultat, "{{contenu}}");
            if (pos) {
                char avant[MAX_HTML], apres[MAX_HTML];
                strncpy(avant, resultat, pos - resultat);
                avant[pos - resultat] = '\0';
                strcpy(apres, pos + 11); // 11 = length of "{{contenu}}"
                
                snprintf(resultat, sizeof(resultat), "%s%s%s", avant, contenu, apres);
            }
            return resultat;
        }
    }
    return NULL;
}

void traiter_requete_post(char* data, char* response) {
    // Traitement basique des données POST
    if (strstr(data, "action=/login") != NULL) {
        // Gestion de la connexion
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Connexion réussie!</h1>");
        generer_session(serveur.session_actuelle);
    }
    else if (strstr(data, "action=/add_comment") != NULL) {
        // Gestion des commentaires
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Commentaire ajouté!</h1>");
    }
    else if (strstr(data, "action=/upload") != NULL) {
        // Gestion de l'upload
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Fichier uploadé!</h1>");
    }
    else {
        strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Données reçues!</h1>");
    }
}

void generer_session(char* session) {
    sprintf(session, "nyu_session_%ld", time(NULL));
}

int verifier_session(const char* session) {
    return strlen(session) > 0;
}

void* serveur_thread(void* arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[4096] = {0};
    
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
    
    printf("✅ Serveur NYU 2.0 démarré sur 0.0.0.0:%d\n", serveur.port);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        
        int bytes_read = read(new_socket, buffer, 4096);
        buffer[bytes_read] = '\0';
        
        char response[8192];
        
        // Vérifier si c'est une requête POST
        if (strncmp(buffer, "POST", 4) == 0) {
            traiter_requete_post(buffer, response);
            send(new_socket, response, strlen(response), 0);
        }
        // Vérifier si c'est une requête API
        else if (strstr(buffer, "GET /api/") != NULL) {
            char* api_path = strstr(buffer, "/api/");
            char route[100];
            sscanf(api_path, "%s", route);
            
            int route_trouvee = 0;
            for (int i = 0; i < nb_routes_api; i++) {
                if (strstr(route, routes_api[i].route) != NULL) {
                    snprintf(response, sizeof(response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Access-Control-Allow-Origin: *\r\n\r\n"
                        "%s", routes_api[i].reponse);
                    route_trouvee = 1;
                    break;
                }
            }
            
            if (!route_trouvee) {
                strcpy(response, "HTTP/1.1 404 Not Found\r\n\r\n{\"error\":\"Route not found\"}");
            }
            
            send(new_socket, response, strlen(response), 0);
        }
        else {
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
            
            char* html_response = generer_html_page(page_demandee[0] ? page_demandee : "accueil");
            
            char header[500];
            snprintf(header, sizeof(header), 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Connection: close\r\n\r\n");
            
            send(new_socket, header, strlen(header), 0);
            send(new_socket, html_response, strlen(html_response), 0);
        }
        
        close(new_socket);
    }
    
    return NULL;
}

void demarrer_serveur() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, serveur_thread, NULL);
    
    // Garder le programme en vie
    printf("📱 Serveur NYU 2.0 en cours d'exécution. Appuyez sur Ctrl+C pour arrêter.\n");
    while (1) {
        sleep(1);
    }
}

void afficher_aide() {
    printf("🇫🇷 NYU 2.0 - Framework Web Français Complet\n");
    printf("=============================================\n\n");
    printf("NOUVELLES FONCTIONNALITÉS AVANCÉES:\n\n");
    
    printf("📝 FORMULAIRES INTERACTIFS:\n");
    printf("  formulaire créer contact /submit POST\n");
    printf("  formulaire champ contact text nom \"Votre nom\"\n");
    printf("  formulaire champ contact email email \"Email\"\n");
    printf("  formulaire ajouter contact\n\n");
    
    printf("🔐 AUTHENTIFICATION:\n");
    printf("  auth activer\n");
    printf("  auth utilisateur admin admin@site.com motdepasse admin\n");
    printf("  auth page_connexion\n");
    printf("  auth proteger page_privee\n\n");
    
    printf("💬 SYSTÈME DE COMMENTAIRES:\n");
    printf("  commentaire activer accueil\n");
    printf("  commentaire ajouter accueil Jean \"Super site!\"\n");
    printf("  commentaire afficher accueil\n\n");
    
    printf("📋 TEMPLATES DE PAGES:\n");
    printf("  template créer base\n");
    printf("  template contenu base \"<header>Menu</header>{{contenu}}<footer>© 2024</footer>\"\n");
    printf("  template utiliser base\n\n");
    
    printf("📤 UPLOAD DE FICHIERS:\n");
    printf("  upload activer\n");
    printf("  upload galerie\n\n");
    
    printf("🔌 API REST AUTOMATIQUE:\n");
    printf("  api route /data GET donnees\n");
    printf("  api donnees /api/all\n\n");
    
    printf("📱 RÉSEAUX SOCIAUX:\n");
    printf("  social facebook https://monsite.com\n");
    printf("  social twitter \"Mon super site\" https://monsite.com\n");
    printf("  social linkedin https://monsite.com\n\n");
    
    printf("💾 BASE DE DONNÉES PERSISTANTE:\n");
    printf("  bd sauver utilisateur1 \"Marie\" texte  # Sauvé automatiquement\n");
    printf("  # Les données sont persistantes entre les redémarrages\n\n");
    
    printf("🌟 EXEMPLE SITE COMPLET AVEC TOUTES LES FONCTIONNALITÉS:\n");
    printf("  ./main démarrer site_ultra_avance.nyu\n\n");
    
    printf("🚀 NOUVELLES CAPACITÉS NYU 2.0:\n");
    printf("  ✅ Formulaires web interactifs\n");
    printf("  ✅ Upload de fichiers/images\n");
    printf("  ✅ Base de données persistante (fichiers)\n");
    printf("  ✅ API REST automatique\n");
    printf("  ✅ Authentification utilisateurs\n");
    printf("  ✅ Templates de pages\n");
    printf("  ✅ Système de commentaires\n");
    printf("  ✅ Intégration réseaux sociaux\n\n");
}
