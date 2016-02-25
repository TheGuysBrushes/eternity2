#include "piece_model.h"

using namespace std;

Piece::Piece(int _id, int tab[]) :
    id(_id)
{
    for(int i=0; i<MAX_CARD; ++i){
        motif[i] = tab[i];
    }
}

void Piece::rotate(){
    int tmp[MAX_CARD];

    tmp[0] = motif[MAX_CARD-1];

    for(int i=1; i<MAX_CARD;++i){
        tmp[i] = motif[i-1];
    }

    for(int j= 0; j < MAX_CARD; ++j){
        motif[j] = tmp[j];
    }
}

ostream& Piece::print(ostream& out){
    out << "Pièce n° " << id << " : ";

    // Affichage des n-1 premières couleurs chacune suivie d'un espace
    for(int i = 0; i < MAX_CARD-1; ++i)
        out << motif[i] << " ";

    // Affichage de la n-ième couleur suivie d'un ; et d'un saut de ligne
    out << motif[MAX_CARD-1] << ";" << endl;

    return out;
}