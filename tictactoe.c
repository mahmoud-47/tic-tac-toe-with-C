#include<stdio.h>
#include<stdlib.h>

typedef struct node{
    char t[3][3];
    int nbEnfants;
    struct node* parent;
    struct node** enfants;
} *Arbre, arbre;

typedef struct {
    int statut; //1, -1 ou 0
    int depth; //profondeur
    Arbre node; //le noeud 
} Score;

char tab[3][3]; //table de jeu

Score* score = NULL; //tableau pour stocker, au finish les noeuds gagnants, perdants ou nuls
int nbScore = 0;//taille du tableau

void init(){
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            tab[i][j] = ' ';
}

void copy(char dest[][3], char src[][3]){
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            dest[i][j] = src[i][j];
}

Arbre CreerNoeud(char t[][3]){
    Arbre node = (Arbre)malloc(sizeof(arbre));
    copy(node -> t, t);
    node -> nbEnfants = 0;
    node -> parent = NULL;
    node -> enfants = NULL;
    return node;
}

void ajoutEnfant(Arbre a, Arbre node){
    a -> enfants = (Arbre*)realloc(a -> enfants, (a -> nbEnfants + 1)*sizeof(Arbre));
    a -> enfants[a -> nbEnfants++] = node;
}

int aligne_3(char liste[][3], char *c){
    //verifier si on a 3 cases alignées et renvoyer par adresse x ou o
    struct{
        int x,y;
    } t[8][3] = { //t contient les lignes alignées
        { {0,0}, {0,1}, {0,2} } ,
        { {1,0}, {1,1}, {1,2} } ,
        { {2,0}, {2,1}, {2,2} } ,
        { {0,0}, {1,0}, {2,0} } ,
        { {0,1}, {1,1}, {2,1} } ,
        { {0,2}, {1,2}, {2,2} } ,
        { {0,0}, {1,1}, {2,2} } ,
        { {0,2}, {1,1}, {2,0} } 
    };
    
    for(int i = 0; i<8; i++){
        char car = liste[t[i][0].x][t[i][0].y];
        int count = 0;
        for(int j = 0; j<3; j++)
            if(car!=' ' && liste[t[i][j].x][t[i][j].y] == car)
                count++;
        if(count == 3){
            *c = car;
            return 1;
        }
    }
    return 0;
}

void display(char tab[][3]){
    printf("------------\n");
    for (int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf(" %c |",tab[i][j]);
            if(j == 2)
                printf("\n");
        }
        printf("------------\n");
    }   
}

int full(char c[][3]){//verifie si le tableau est plein
    for (int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(c[i][j]==' ')
                return 0;
    return 1;
}

int egal(char t1[][3], char t2[][3]){//verifie si deux tableaux sont egaux
    for (int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if(t1[i][j]!=t2[i][j])
                return 0;
    return 1;
}

void human_play(){ 
    int choix;
    do{
        printf("Choisissez une case entre 1 et 9 >>> ");
        scanf("%d",&choix);
    }while(choix<1 || choix>9);
    int x = (choix - 1) / 3;
    int y = (choix - 1) % 3;
    if(tab[x][y] != ' ')
        do{
            printf("Choix invalide, choisissez une autre >>> ");
            scanf("%d",&choix);
            x = (choix - 1) / 3;
            y = (choix - 1) % 3;
        }while(choix<1 || choix>9 || tab[x][y] != ' ');
    tab[x][y] = 'o';
}

Arbre *eviter = NULL; //tableau de toutes les combinaisons à eviter, qui comportent des risques
int nevit = 0; //taille du tableau

void Paths(Arbre a, int depth){//rechercher tous les chemins possibles qui mènent à 1,0 ou -1
    char c = '-';
    if(aligne_3(a -> t, &c) || full(a -> t)){//si on a un noeud gagnat perdant ou plein
        score = (Score*)realloc(score, sizeof(Score)*(nbScore+1));
        if(c == 'x')
            score[nbScore].statut = 1;
        else if(c == 'o'){
            score[nbScore].statut = -1;
            if(depth == 2 || depth == 4){
                eviter = realloc(eviter, (nevit+1)*sizeof(arbre));
                eviter[nevit] = a -> parent;
                nevit ++;
            }
        }else
            score[nbScore].statut = 0;
        score[nbScore].depth = depth;
        score[nbScore].node = a;
        nbScore++;
    }else{
        for(int i = 0; i<3; i++)
            for(int j = 0; j<3; j++)
                if(a -> t[i][j] == ' '){
                    Arbre node = CreerNoeud(a -> t);
                    node -> parent = a;
                    ajoutEnfant(a, node);
                    if(depth%2 == 0)
                        node -> t[i][j] = 'x';
                    else
                        node -> t[i][j] = 'o';
                    Paths(node, depth+1);
                }
    }
}

Arbre liberer(Arbre a){
    if(a == NULL)
        return NULL;
    for(int i=0; i<a->nbEnfants; i++)
        liberer(a->enfants[i]);
    free(a);
}

void machine_play(){
    if(tab[1][1]==' '){//privilegier la case centrale
        tab[1][1]='x';
        return;
    }
    Arbre root = CreerNoeud(tab), minNode = NULL;
    Paths(root,0);       
    
    /*for(int i=0; i<nbScore; i++){
        printf("Stat %d deep %d\n",score[i].statut,score[i].depth); 
        display(score[i].node -> t);
    }*/
    int min = -1, i=0;
    
    while(i<nbScore){
        
        for(int j = 0; j<nevit;j++){
            Arbre a = score[i].node;
            while(a -> parent != NULL && egal(eviter[j]->t,a->t)==0)
                    a = a -> parent;
            if(a -> parent!= NULL)
                score[i].statut = -9;
        }
            
        if(score[i].statut==1)
            if(min == -1){
                min = score[i].depth;
                minNode = score[i].node;
            }else{
                if(score[i].depth<min){
                    min = score[i].depth;
                    minNode = score[i].node;
                }
            }

        i++;
    } 
    nevit = 0;
    eviter = NULL;
    if(min==-1){
        Paths(root,0);
        i = 0;
        while(i<nbScore){
            for(int j = 0; j<nevit;j++){
                Arbre a = score[i].node;
                while(a -> parent != NULL && egal(eviter[j]->t,a->t)==0)
                        a = a -> parent;
                if(a -> parent!= NULL)
                    score[i].statut = -9;
            }            
            if(score[i].statut==0)
                if(min == -1){
                    min = score[i].depth;
                    minNode = score[i].node;
                }else{
                    if(score[i].depth<min){
                        min = score[i].depth;
                        minNode = score[i].node;
                    }
                }
            i++;
        } 
    }
    if(min==-1){
        Paths(root,0);
        i = 0;
        while(i<nbScore){
            for(int j = 0; j<nevit;j++){
                Arbre a = score[i].node;
                while(a -> parent != NULL && egal(eviter[j]->t,a->t)==0)
                        a = a -> parent;
                if(a -> parent!= NULL)
                    score[i].statut = -9;
            }   
            if(score[i].statut==-1)
                if(min == -1){
                    min = score[i].depth;
                    minNode = score[i].node;
                }else{
                    if(score[i].depth<min){
                        min = score[i].depth;
                        minNode = score[i].node;
                    }
                }
            i++;
        } 
    }
    
    while(minNode -> parent && egal(minNode -> parent -> t, tab)==0)
        minNode = minNode -> parent;
    
    copy(tab, minNode->t);
    free(score);
    score = NULL;
    nbScore = 0;
    nevit = 0;
    liberer(root);
}

void effacer(){
    system("cls");
}

int gagner(){
    char c;
    if(aligne_3(tab, &c)){
        if(c=='x')
            printf("\aL'ordi a gagné !!!\n");
        else
            printf("Joueur a gagné !!!\n");
        return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]){
    init();
    int numJoueur = 0;
    do{
        display(tab);
        if(numJoueur%2==0)
            human_play();
        else
            machine_play();
        effacer();
        numJoueur++;
    }while(!gagner() &&numJoueur<9);
    display(tab);  
    printf("La partie est terminée\n");
    //display(tab);
    return 0;
}