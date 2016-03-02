#include "configuration.h"

using namespace std;

Configuration::Configuration() :
    instance()
{

}

Configuration::Configuration(const Instance* _instance) :
    instance(_instance)
{}

Configuration::Configuration(const string& fileName)
{
    Instance* inst= new Instance();

    if (!inst->tryLoadFile(fileName)){
        cerr << "Impossible de charger le fichier " << fileName << endl;
        exit(EXIT_FAILURE);
    }
    else { instance = inst; }
}

const pair<int, int>& Configuration::getPair(int x, int y) const {
    const pair<int, int>& position = positions[x + y * instance->width()];
    return position;
}

const Piece & Configuration::getPiece(int x, int y) const {
    const Piece & piece = instance->get_vectPieces()->at(getPair(x, y).first-1);
    return piece;
}

int * Configuration::getRotatedMotif(int x, int y) const {
    pair<int, int> position = getPair(x, y);
    Piece P = getPiece(x, y);
    return P.rotate(position.second);
}

int Configuration::getPosition(const Piece& p) const {
    bool found = false;
    int ind = 0;
    while(!found) {
        if(positions[ind].first == p.get_id())
            found = true;
        else ind++;
    }
    return ind;
}

ostream& Configuration::print(ostream& out){
    for(int i=0 ; i<instance->width() ; ++i) {
        for(int j=0 ; j<instance->height() ; ++j) {
            pair<int, int> p = getPair(i, j);

            out << "Case (" << i << "," << j << ") : \tID = " << p.first << "\tRotation = " << p.second << "\tSONE = ";

            const Piece& piece = getPiece(i, j);

            const int* swne ;
            if(p.second != 0) {
                swne = piece.rotate(p.second );
            } else {
                swne = piece.get_motif();
            }

            for(int l=0; l<MAX_CARD; ++l){
                if(l==0)
                    out << "[" << swne[l] << ",";
                else if(l==MAX_CARD-1)
                    out << swne[l] << "]";
                else
                    out << swne[l] << ",";
            }
            out << endl;
        }
    }

    for(int i=0 ; i<instance->width(); i++) {
        for(int j=0 ; j<instance->height() ; j++) {
            pair<int, int> p = getPair(j, i);
            cout << "\t" << p.first << " ";
        }
        cout << endl;
    }

    return out;
}

bool Configuration::tryLoadFile(const string &fileName){
    if( instance->width() * instance->height() == 0){
        cerr << "Aucune instance n'est chargée" << endl;
        return false;
    }else{
        ifstream f(fileName.c_str());

        if(!f){
            cerr << "Erreur pendant l'ouverture du fichier" << endl;
            return false;
        } else {

            string line;

            while(getline(f,line)){

                vector<string>& tokens = explode(line);

                positions.push_back( pair<int,int>( atoi(tokens[0].c_str()) , atoi(tokens[1].c_str()) ) );
            }

            if(positions.size() != (unsigned)(instance->width() * instance->height()) ){
                cerr << "Fichier de configuration incomplet" << endl;
                return false;
            } else {
                return true;
            }
        }
    }
}

void Configuration::randomConfiguration() {
    int i_rot;
    vector<int> pieces_id;

    // Travail uniquement sur l'id des pieces
    for(const Piece & p : *(instance->get_vectPieces()) ) {
        pieces_id.push_back(p.get_id());
    }
    // Mélange aléatoire
    random_shuffle(pieces_id.begin(), pieces_id.end());

    while(!pieces_id.empty()) {
        // Récupération de la pièce (id)
        int p = pieces_id.back();
        pieces_id.pop_back();
        // Choix aléatoire de la rotation
        i_rot = rand() % 4;
        pair<int, int> piece;
            piece.first = p;
            piece.second = i_rot;
        // Ajout de la pair
        placePiece(piece);
    }
}

vector<Configuration*>&  Configuration::generateRandomConfigurations(const Instance * instance, int limit) {
    vector<Configuration*>& configurations= *(new vector<Configuration*>);

    for(int ind_conf=0 ; ind_conf < limit ; ind_conf++) {
        Configuration* configuration= new Configuration(instance);
        configuration->randomConfiguration();

        configurations.push_back(configuration);
    }

    return configurations;
}

int Configuration::constraintRows() {
    int errors = 0;
    const int * swne;
    // Vérification de la première ligne (Contrainte Ligne Nord)
    for(int i=0 ; i<instance->width() ; i++) {
        const pair<int, int> & pair = getPair(i, 0);
        const Piece & piece = getPiece(i, 0);
        swne = piece.rotate(pair.second);
        if(swne[2] != 0) {
            cout << "\t(*)" << piece;
            errors++;
        }
    }
    // Vérification de la seconde ligne (Contrainte Ligne Sud)
    for(int i=0 ; i<instance->width() ; i++) {
        const pair<int, int> & pair = getPair(i, instance->height()-1);
        const Piece & piece = getPiece(i, instance->height()-1);
        swne = piece.rotate(pair.second);
        if(swne[0] != 0) {
            cout << "\t(*)" << piece;
            errors++;
        }
    }
    return errors;
}

int Configuration::constraintCols() {
    int errors = 0;
    const int * swne;
    // Vérification de la première colonne (Contrainte Colonne Ouest)
    for(int i=0 ; i<instance->height() ; i++) {
        const pair<int, int> & pair = getPair(0, i);
        const Piece & piece = getPiece(0, i);
        swne = piece.rotate(pair.second);
        if(swne[1] != 0) {
            cout << "\t(*)" << piece;
            errors++;
        }
    }
    // Vérification de la seconde colonne (Contrainte Colonne Est)
    for(int i=0 ; i<instance->height() ; i++) {
        const pair<int, int> & pair = getPair(instance->width()-1, i);
        const Piece & piece = getPiece(instance->width()-1, i);
        swne = piece.rotate(pair.second);
        if(swne[3] != 0) {
            cout << "\t(*)" << piece;
            errors++;
        }
    }
    return errors;

}

int Configuration::constraintEdges() {
    int errors = 0;
    const int * swne;
    pair<int, int> pair;
    // Coin Nord-Ouest
    pair = getPair(0,0);
    Piece piece = getPiece(0,0);
    swne = piece.rotate(pair.second);
    if(swne[1] != 0 || swne[2] != 0) {
        cout << "\t(*)" << piece;
        errors++;
    }
    // Coin Nord-Est
    pair = getPair(0,instance->width()-1);
    piece = getPiece(0,instance->width()-1);
    swne = piece.rotate(pair.second);
    if(swne[3] != 0 || swne[2] != 0) {
        cout << "\t(*)" << piece;
        errors++;
    }
    // Coin Sud-Ouest
    pair = getPair(instance->height()-1,0);
    piece = getPiece(instance->height()-1,0);
    swne = piece.rotate(pair.second);
    if(swne[1] != 0 || swne[0] != 0) {
        cout << "\t(*)" << piece;
        errors++;
    }
    // Coin Sud-Est
    pair = getPair(instance->height()-1, instance->height()-1);
    piece = getPiece(instance->height()-1, instance->height()-1);
    swne = piece.rotate(pair.second);
    if(swne[3] != 0 || swne[0] != 0) {
        cout << "\t(*)" << piece;
        errors++;
    }

    return errors;
}

int Configuration::checkPieces(){
    /**
      Algo :
            Initialiser un vector de bool (vect[i] = 1 si la pièce i a déjà été traité)
            Tant qu'il reste des pièces à vérifier faire
                Prendre une pièce P qui n'est pas dans vect
                Comparer P à ses pièces adjacentes (S et E ayant subit la rotation)
                Ajouter P à vect (indice de la pièce P à 1 dans vect)
            Fin TantQue
    */
    int nb_errors = 0;

    for(int i= 0; i< height()*width()-1; ++i){
        pair<int, int> current_piece= positions[i];
        int id_piece = current_piece.first;

        int* id_motifs= instance->getPiece(id_piece).rotate(current_piece.second);
        int south_motif = id_motifs[0];
        int east_motif = id_motifs[3];

        //derniere ligne du puzzle - est seulement
        if(i > width()*height() - width()){
            int tmp_rot = positions[getPosition(instance->getPiece(id_piece+1))].second;
            int piece_tmp_ouest = instance->getPiece(id_piece+1).rotate(tmp_rot)[1];
            //si la piece est noire, pas de comparaison
            if(piece_tmp_ouest != 0){
                if(piece_tmp_ouest != east_motif) ++nb_errors;
            }

        } else
        if(i % width() == 0){
            //derniere case d'une ligne - comparaison sud seulement
            int tmp_rot = positions[getPosition(instance->getPiece(id_piece+width()))].second;
            int piece_tmp_nord = instance->getPiece(id_piece+width()).rotate(tmp_rot)[2];
            //si la piece est noire, pas de comparaison
            if(piece_tmp_nord != 0){
                if(piece_tmp_nord != south_motif) ++nb_errors;
            }

        } else {
            //comparaison sud et est
            int tmp_rot_est = positions[getPosition(instance->getPiece(id_piece+1/*Changer par la valeur de position de la piece à l'est*/))].second;
            int tmp_rot_sud = positions[getPosition(instance->getPiece(id_piece+width()/*Changer par la valeur de position de la piece au sud*/))].second;

            int piece_tmp_ouest = instance->getPiece(id_piece+1).rotate(tmp_rot_est)[1];
            int piece_tmp_nord = instance->getPiece(id_piece+width()).rotate(tmp_rot_sud)[2];
            //si la piece est noire, pas de comparaison
            if(piece_tmp_ouest != 0){
                if(piece_tmp_ouest != east_motif) ++nb_errors;
            }
            //si la piece est noire, pas de comparaison
            if(piece_tmp_nord != 0){
                if(piece_tmp_nord != south_motif) ++nb_errors;
            }
        }
    }


    return nb_errors;
}
