#include "algorithm.h"

using namespace std;

int Algorithm::vicinity(const Configuration & C1, const Configuration & C2) {
    assert(C1.get_ids_and_rots().size() == C2.get_ids_and_rots().size());
    int vicinity = 0;

    for(unsigned int i=0 ; i<C1.get_ids_and_rots().size() ; i++) {
        if(C1.get_ids_and_rots().at(i).first == C2.get_ids_and_rots().at(i).first)
            vicinity++;
    }
    return vicinity;
}

Configuration * Algorithm::getNeighbour(const Configuration & config) {
    Configuration * voisin = new Configuration(config);

    int x, y;
    do {
        x = rand() % (config.get_width()-1);
        y = rand() % (config.get_height()-1);
    } while ( x==0 && y==0 );

    int rotation = config.getRotation(x, y);
    int rot_add =((rand() % (4)) + rotation) % 4;

    voisin->rotatePiece(x, y, rot_add);
    return voisin;
}

Configuration * Algorithm::getFirstBetterNeighbour(const Configuration &config) {
    Configuration  * cpy = new Configuration(config);
    for(int j=0 ; j<config.get_height() ; j++) {
        for(int i=0 ; i<config.get_width() ; i++) {
            for(int l=0 ; l<config.get_height() ; l++) {
                for(int k=0 ; k<config.get_width() ; k++) {
                    bool res = cpy->better_permutation_two_pieces(i, j, k, l);
                    if(res) {
                       // cout << "Premier meilleur voisin : " << cpy->countNbErrors() << endl;
                       return cpy;
                    } else {
                        free(cpy);
                        cpy = new Configuration(config);
                    }
                }
            }
        }
    }
    // Pas de meilleur voisin
    return cpy;
}

vector<Configuration *> Algorithm::get_neighbours(Configuration & C, vector<Configuration *> configurations) {
    vector<Configuration *> neightbours;
    int i = 0;
    int vic_max = 0;
    for(auto config : configurations) {
        int vic = Algorithm::vicinity(C, (*config));
        if(vic != C.get_width()*C.get_height()) {
            if(vic > vic_max) {
                vic_max = vic;
                neightbours.clear();
                neightbours.push_back(config);
            } else if(vic == vic_max)
                    neightbours.push_back(config);
        }
        i++;
    }
    return neightbours;
}

void Algorithm::add_child(std::vector<Configuration *> &children, Configuration *parent1, Configuration *parent2){
    pair<Configuration*, Configuration*> new_children= make_children(parent1, parent2);
    children.push_back(new_children.first);
    children.push_back(new_children.second);

}

pair<Configuration*, Configuration*> Algorithm::make_children(Configuration* parent1, Configuration* parent2){
    Configuration* son= new Configuration(*parent1);
    Configuration* daughter= new Configuration(*parent2);
    //critere de croisement = pieces du parent 1 ayant le moins d'erreurs
    int erreurs_tolereees= 2;
    vector<int> ids_of_son, ids_of_daughter;

    for(int i= 0; i<parent1->get_height(); ++i){

        for(int j= 0; j<parent1->get_height(); ++j){
            const Piece& piece_parent1= parent1->getPiece(i,j);
            pair<int,int> id_rot_p1= parent1->getPair(i,j);
            int errors_piece_ij= parent1->getPieceNbErrors(piece_parent1);

            if(erreurs_tolereees >= errors_piece_ij) //piece de 2 erreurs max = copie dans fils
            {
                //on retient l'id de la piece ajoutee dans le fils
                ids_of_son.push_back(id_rot_p1.first);
                son->setPiece(i, j, id_rot_p1);
            } else //la piece a trop d'erreurs dans parent1, on la prendra dans parent2
            {
                //on retient l'id de la piece ajoutee dans la fille
                ids_of_daughter.push_back(id_rot_p1.first);
                //les pieces non voulues vont dans la config fille
                daughter->setPiece(i, j, id_rot_p1);
            }
        }
    }

    //Ici, le fils est rempli des meilleurs pieces de parent1 et la fille des pires pieces de parent1
    //on doit completer son & daughter avec les pieces de parent2
    for(int i= 0; i<parent2->get_height(); ++i){
        for(int j= 0; j<parent2->get_height(); ++j){
            pair<int,int> id_rot_p2= parent2->getPair(i,j);
            int id_p2= id_rot_p2.first;

            //la piece parent2[i][j] est-elle deja presente dans son ou dans daughter ?
            if (find(ids_of_son.begin(), ids_of_son.end(), id_p2) != ids_of_son.end()) //id_p2 est un id deja present dans le fils
            {
                //on ajoute donc la piece dans daughter
                daughter->setPiece(i, j, id_rot_p2);

            } else //id_p2 n'est pas present dans le fils - ajout de la piece
            {
                //on ajoute la piece dans son
                son->setPiece(i, j, id_rot_p2);
            }
        }
    }

    pair<Configuration*, Configuration*> children= make_pair(son, daughter);
    return children;
}

int Algorithm::evaluation(const Configuration & C) {
    return C.countNbErrors();
}

Configuration * Algorithm::local_search(const Configuration * config) {
    int nb_eval = 0;
//    int x = 0;
//    int y = 0;
    // 1. Sélectionner une solution initiale x0 € X
    Configuration * c0 = new Configuration(*config);
    // 2. x <- x0 (x est la solution courante)
    Configuration * c = c0;
    // 3. x* <- x (x* est la meilleure solution rencontrée au sens de f)
    Configuration * cStar = c;
    int eval_cStar = Algorithm::evaluation(*cStar);
    // 4. Tant que le critère d'arret n'est pas respecté faire
    Configuration * cprime;
    while(nb_eval < 100) {
        // 5. Sélectionner une solution voisine x' ∈ N(x)
        cprime = getFirstBetterNeighbour(*c);
        // 6. x <- x'
        c = cprime;
        nb_eval++;
        // 7. si f(x) > f(x*) alors
        int eval_c = Algorithm::evaluation(*c);
        if(eval_c < eval_cStar) {
            // 8. x* <- x
            cStar = c;  // Nouvelle meilleure solution
            eval_cStar = eval_c;
            nb_eval = 0;
        }
//        if(nb_eval > 93 && eval_cStar > 2) {
//            c->random_permutation_two_pieces();
//            cout << "Permutation aléatoire" << endl;
//        }
        // 9. fin
    } // 10. fin
    // 11. retourner x*
    return cStar;
}


// ###########################################
//      PROGRAMMATION PAR CONTRAINTES
// ###########################################

list<Piece>* Algorithm::initCSP(Configuration * config)
{
    bool has_first_corner= false;

    list<Piece>* available_pieces= new list<Piece>;

    for(const Piece p : *(config->getPieces())) {
        if (!has_first_corner && p.isCorner()){
            config->addPieceAsCorner(p.get_id(), 0,0);
            has_first_corner= true;
#if DEBUG_CSP
            cout << p<< " placée au coin"<< endl;
#endif
        }
        else {
            available_pieces->push_back(p);
        }
    }

    return available_pieces;
}

void Algorithm::placeWithoutConstraints(Configuration* config, list<Piece>* remainingPieces)
{
//    for (Piece piece : available_pieces){

    unsigned ever_placed= config->get_ids_and_rots().size();

    int y= ever_placed / config->get_height();
    int x= ever_placed % config->get_width();
    for ( ; y < config->get_height(); ++y){
        for ( ; x < config->get_width(); ++x){
            assert(remainingPieces->size() > 0);

            // Placement de la pièce à la fin de la configuration
            Piece piece= remainingPieces->front();
            config->placePiece(piece, 0);
            remainingPieces->pop_front();

            // Tourne la pièce si une meilleure rotation est disponible
            int best_rot= config->rotationForBestPlace(x, y);
            config->rotatePiece(x, y, best_rot);
        }
    }
}

Configuration* Algorithm::resolveWithCSP(const Instance *instance)
{
    Configuration* solution = new Configuration(instance);
    list<Piece>& available_pieces= *(initCSP(solution));

#if DEBUG_CSP
    cout << "Nombre de pièces disponibles : "<< available_pieces.size()<< endl;
#endif

//    Debut de l'algo

    // Parcours du tableau @SEE si on l'améliore en escargot
    for(int i= 2; i<= solution->get_width() * solution->get_height(); ++i) {
            Piece current_piece= available_pieces.front();
            available_pieces.pop_front();

            unsigned int num_try= 0;
            bool placed= false;
            do {
                placed= solution->tryPlaceAtEnd(current_piece);
                 ++num_try;
                // La pièce ne peut pas être ajoutée, donc on la replace à la fin
                //  de la liste des pièces disponibles
                if (!placed){
                    available_pieces.push_back(current_piece);
                    current_piece= available_pieces.front();
                    available_pieces.pop_front();

#if DEBUG_CSP
                    cout << "Essai num "<< num_try<< " : placer "<< current_piece<< " en "
                         << i<< " eme case (placé : "<< placed<< ")"<< endl;
                    cout << solution->get_ids_and_rots().size()<< " déjà placées, "<<
                         solution->get_width() * solution->get_height()-i +1<< " à placer"<< endl;
                    cout << available_pieces.size() +1<< " disponibles"<< endl;
#endif
                }
            }while (!placed && num_try <= available_pieces.size());

            // En cas d'echec de placement de la pièce suivante
            if (!placed){
                available_pieces.push_front(current_piece);
                clog << "IL FAUT PLACER LES PIECES SUIVANTES SANS RESPECTER LES CONTRAINTES"
                        " OU FAIRE DU BACKTRACKING"<< endl;
            }

//        }
    }

    // Pose des pièces restantes, sans vérifier les contraintes
    placeWithoutConstraints(solution, &available_pieces);

    /** Fin de l'algo **/
    delete &available_pieces;

    return solution;
}

// ###########################################
//              GENETIQUE
// ###########################################

std::vector<Configuration*> Algorithm::genetic_search(std::vector<Configuration*> configs){
    cout << "DEBUT ALGO GENETIC" << endl;

    std::vector<Configuration*> new_generation, final_generation;
    //selection des configurations 2 à 2 dans "configs" (2 parents)

    int taille_config= configs.size();
    //croisement (sur quel critere croiser ? Nombre d'erreurs par piece ?)
    cout << "HERE IS THE PROBLEM" << endl;
    if(0 == taille_config % 2) //nombre paire de parents
    {
        for(int k= 0; k < taille_config; k+=2){
            add_child(new_generation, configs[k], configs[k+1]);
        }
    } else //nombre impaire de parents
    {
        if(1 != taille_config) //population d'un seul individu
        {
            for(int k= 0; k < taille_config-1; k+=2){
                add_child(new_generation, configs[k], configs[k+1]);
            }
        }
        //le parent qu'il reste est ajoute a la new generation
        new_generation.push_back(configs[taille_config-1]);

    }

    cout << "STADE DE LA MUTATION" << endl;

    for(int m= 0; m < new_generation.size(); ++m) {
        //mutation (s'arranger pour tourner les pieces si besoin)
        new_generation[m]= Configuration::getBestRotatedConfig(new_generation[m]);

    }

    //injection (fils = meilleure solution ?)
    for(unsigned int i= 0; i < configs.size(); ++i){
        if(Algorithm::evaluation(*configs[i]) > Algorithm::evaluation(*new_generation[i])) //ancienne generation meilleure
        {
            final_generation.push_back(configs[i]);
        } else //nouvelle generation meilleure (si autant d'erreurs, place a la jeunesse !
        {
            final_generation.push_back(new_generation[i]);
        }

    }

    return final_generation;
}
