#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct BNode {
	char label;
	int frequence;
	struct BNode *left, *right;
} BNode;

void insert_BNode(BNode new_node, BNode array[], int *array_size) {
    int i = 0;

    // Parcourir le tableau jusqu'à trouver l'emplacement où insérer le nouveau nœud
    while (i < *array_size && new_node.frequence > array[i].frequence) i++;
    
    // Décaler tous les nœuds situés après l'emplacement d'insertion vers la droite
    int j;
    for (j = *array_size; j > i; j--) array[j] = array[j-1];
    
    // Insérer le nouveau nœud à l'emplacement trouvé
    array[i] = new_node;
    (*array_size)++;
}

void show_BNode_array(BNode array[], int array_size) {
	BNode* p;
	for(p = array; p < array + array_size; p++) {
		printf("label: ");
		if (p->label == '\0') printf("null");
		else printf("%c", p->label);
		printf(", frequence: %d", p->frequence);
		printf(", left: %p, right: %p", p->left, p->right);
		printf(", pointeur: %p\n", p);
	};
}

void show_tree(BNode* node, int niveau) {
	// Affichage des indentations
	int i;
	for (i=0; i < niveau; i++) printf("\t");
	
	// Affichage des propriétés
	printf("frequence: %d", node->frequence);
	if(node->label != '\0') printf(", label: %c", node->label);
	printf("\n");
	
	// Affichage récursif des enfants
	if (node->left != NULL) show_tree(node->left, niveau+1);
	if (node->right != NULL) show_tree(node->right, niveau+1);	
}

void show_bits(FILE* binaryFile, unsigned char buffer) {
	// Affichage de chaque bit du fichier binaire
	rewind(binaryFile);
	int i;
    while (fread(&buffer, 1, 1, binaryFile) == 1) {
        // Affichage de chaque bit de l'octet
        for (i = 7; i >= 0; i--) {
			printf("%d", (buffer >> i) & 1);
			if(i % 4 == 0) printf(" ");
        }
    }
    printf("\n");
}

unsigned char get_next_label(BNode **pCurrentNode, int nextTurn, BNode *pRoot) {
	// Erreur de valeur sur nextTurn
	if (nextTurn != 0 && nextTurn != 1) {
		printf("[Erreur] Le sens est %d au lieu de 0 ou 1", nextTurn);
		exit(0);
	}
	
	// Parcours de l'arbre à gauche ou à droite
	*pCurrentNode = (nextTurn==0) ? (*pCurrentNode)->left : (*pCurrentNode)->right;
	
	unsigned char label = (*pCurrentNode)->label;
	
	// Retour à la racine si on a atteint une feuille
	if(label != '\0') *pCurrentNode = pRoot;
	
	return label;
}

int main() {
	//	Ouverture du fichier fréquence
	char freqFileName[] = "exemple_freq.txt";
	FILE *freqFile = fopen(freqFileName, "r");
	if (freqFile == NULL) printf("Impossible d'ouvrir le fichier %s", freqFileName);

	//	Lecture du fichier de fréquence
	int nbLabels, i;
	fscanf(freqFile, "%d\n", &nbLabels);
	
	//	Taille d'une ligne: 1 char + 1 espace + 1 char + 1 retour à la ligne + 1 caractère de fin de fichier (pour la dernière ligne)
	const int LINESIZE = 5;
	char* line = malloc((LINESIZE) * sizeof(char));	
	
	//	On alloue 2n-1 BNode car se sera le nombre de noeuds dans l'arbre final
	BNode* nodes = calloc((nbLabels*2 - 1), sizeof(BNode));
	if (nodes == NULL) exit(0);
	
	//  On initialise les nbLabels-premiers BNode
	for (i = 0; i < nbLabels; i++) {
		// End of file
		if (fgets(line, LINESIZE, freqFile) == NULL) break;
		// Stockage des données du noeud
		sscanf(line, "%c %d", &nodes[i].label, &nodes[i].frequence);
	}
	
	free(line);
    fclose(freqFile);
	
// ------------ CREATION DE L'ARBRE ------------

	//	nbNodes est le nombre de noeuds dans l'arbre, tandis que nbLabels et le nombre de caractères
	// 	nbNodes va donc augmenter au fur et à mesure qu'on créé des noeuds
	int nbNodes = nbLabels;
	
	//	A ce point, le tableau est déjà trier
	//	Il reste nbLabels-1 opérations à faire pour obtenir l'arbre final
	for (i = 1; i < 2*(nbLabels-1); i += 2) {
		BNode *leftChild, *rightChild;
		
		// Déterminer les 2 prochains noeud à fusionner
		leftChild = &nodes[i-1];
		rightChild = &nodes[i];
		
		//Création du noeud
		BNode newNode = {
			.frequence = leftChild->frequence + rightChild->frequence,
			.left = leftChild,
			.right = rightChild
		};
		
		// Insertion à la bonne place dans le tableau
		insert_BNode(newNode, nodes, &nbNodes);
	}
	
	// Récupération de la racine et affichage de l'arbre
	BNode* pRoot = &nodes[nbNodes-1];
	printf("Arbre de Huffman\n");
	show_tree(pRoot, 0);
	
//	------------ LECTURE DU FICHIER COMPRESSE ET ECRITURE DU FICHIER DECOMPRESSE ------------

	char fileName[] = "exemple_comp.bin";
	FILE *binaryFile = fopen(fileName, "rb");
	if (binaryFile == NULL) printf("Impossible d'ouvrir le fichier binaire %s", fileName);
	
	unsigned char buffer;
	
	// Affichage des bits du fichier	
    //show_bits(binaryFile, buffer);
	
	// pCurrentNode est un "curseur" qui donne la position sur l'arbre
	BNode *pCurrentNode = pRoot;
	
    FILE *outputFile = fopen("output.txt", "w");
	if (outputFile == NULL) printf("Impossible d'ouvrir le fichier output.txt");
	
    // Lecture octet par octet 
    while(fread(&buffer, 1, 1, binaryFile) == 1) {
    	// Isolation de chaque bit
    	for (i = 7; i >= 0; i--) {
    		// Obtention du label
    		unsigned char label = get_next_label(&pCurrentNode, (buffer>>i)&1, pRoot);
			// Si le label n'est pas vide, on l'ajoute dans le texte décompressé
			if(label != '\0') fprintf(outputFile, "%c", label);
		}
	}
	
	free(nodes);
	
// ------ CALCUL DU TAUX DE COMPRESSION -------
	
	fseek(binaryFile, 0L, SEEK_END);
	fseek(outputFile, 0L, SEEK_END);
	
	float sizeBinaryFile = ftell(binaryFile);
	float sizeOutputFile = ftell(outputFile);
	
	fclose(outputFile);
	fclose(binaryFile);
	
	printf("Taille des fichiers: compresse %.0f octets | decompresse %.0f octets\n", sizeBinaryFile, sizeOutputFile);
	printf("Taux de compression: %.2f\n", 1-(sizeBinaryFile/sizeOutputFile));
	
// ------ CALCUL DU NB DE BITS MOYEN -------

	printf("Nombre de caractere: %d\n", nbLabels);
	printf("Nombre de bits: %.0f\n", 8.0*sizeBinaryFile);
	printf("Nombre de bit moyen par caractere: %.2f", sizeBinaryFile/nbLabels*8);
	return 0;
}

