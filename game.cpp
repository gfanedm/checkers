#include <iostream>
#include <cstdio>
#include <locale.h>
#include <fstream>
#include <cstring>
#include <cstdlib>

/*
 *  Definição das variáveis usadas para gerar cores no tabuleiro.
 */

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
#define WHITE "\x1B[37m"
#define GRAY "\x1B[40m"

#define LIGHT_RED "\x1B[91m"
#define LIGHT_YELLOW "\x1B[93m"
#define LIGHT_BLUE "\x1B[94m"
#define LIGHT_MAGENTA "\x1B[96m"

#define BACK_BLACK "\x1b[40m"
#define BACK_GRAY "\x1b[100m"
#define BACK_WHITE "\x1b[47m"

/*
 *  Definição das variáveis usadas para gerar o tabuleiro e orientar
 *  o programa nos cálculos necessários.
 */

#define SQR_WIDTH 8
#define SQR_HEIGH 4
#define SQR_QTD 8


// Declaração da namespace utilizada
using namespace std;


/*
 *  Struct Piece feita para indicar a peça do jogador, tendo sua
 *  localização e o valor para identificar se é dama ou uma peça comum,
 *  alem da identificacao do dono da peca.
 */

struct Piece{
    int x, y, value, owner;
};


/*
 *  Struct Move feita para identificar os movimentos dos jogadores,
 *  tendo os valores das localizações iniciais e finais.
 */
struct Move{
    int initialY, initialX, finalY, finalX;
};

const char ABC[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
const char NUM[9] = {'1', '2', '3', '4', '5', '6', '7', '8'};

/*
 *  Declaração dos protótipos das funções utilizadas no jogo.
 */

char* color(int,int);
void draw(Piece [SQR_QTD][SQR_QTD]);
void printTitle(int, char **);
bool canMove(Move, Piece [SQR_QTD][SQR_QTD], bool);
bool move(Move, Piece [SQR_QTD][SQR_QTD], bool);
bool menu(Piece [SQR_QTD][SQR_QTD], char **);
void start(Piece [SQR_QTD][SQR_QTD], char **, bool, bool);
bool readArchive(fstream &, Piece [SQR_QTD][SQR_QTD], char **);
int checkWin(Piece [SQR_QTD][SQR_QTD]);
char convertValue(Piece);
bool saveArchive(Piece [SQR_QTD][SQR_QTD], char **, bool, char *);

/*
 *  Declaração da função principal, ela cria as variaveis na qual o
 *  jogo se fundamenta, além de receber os dados enviados pelo usuário.
 */

int main(){
    setlocale(LC_ALL, "Portuguese");

    char **names = new char*[4];
    Piece pieces[SQR_QTD][SQR_QTD];

    for(int y = 0; y < SQR_QTD; y++){
        names[y] = new char[32];
        
        for(int x = 0; x < SQR_QTD; x++){
            Piece piece;
            piece.x = x;
            piece.y = y;    
            piece.value = 0;
            piece.owner = -1;
            
            //Colocando dono nas peças.
            if(y <= 2){
                if(y % 2 != 0){
                    if(x % 2 == 0)
                        piece.owner = 0;
                }else{
                    if(x % 2 == 1)
                        piece.owner = 0;
                }
            }else if(y >= 5){
                if(y % 2 == 0){
                    if(x % 2 != 0)
                        piece.owner = 1;
                }else{
                    if(x % 2 == 0)
                        piece.owner = 1;
                }
            }
            
            //Inserindo a peça no ponteiro de peças.
            pieces[y][x] = piece; 
        }
    }

    menu(pieces, names);

    return 0;
}

/*
 *  Declaração da função que gera o menu para o jogador, ele também seleciona
 *  a opção correta escolhida pelo jogador
 */
bool menu(Piece pieces[SQR_QTD][SQR_QTD], char **names){

    //Selecionando e validando a primeira opção.
    printTitle(0, names);
    int opt = 0;
    cin >> opt;

    while(!(opt > 0 && opt <= 4)){
        printTitle(0, names);
        cout << "A opção escolhida não é válida." << endl;
        cin >> opt;
    }

    //Opção para sair do jogo.
    if(opt == 3){
        return false;
    }   

    //Selecionando e validando a segunda opção.
    printTitle(1, names);

    int second = 0;
    cin >> second;
    while(!(second > 0 && second < 4)){
        printTitle(1, names);
        cout << "A opção escolhida não é válida." << endl;
        cin >> second;
    }


    bool player = (opt == 1 ? false : true), saved = (second == 2 ? true: false);
    //Switch para a identificação da segunda opção no menu
    switch(second){
        case 3:
            return menu(pieces, names);
            break;
    }  

    start(pieces, names, player, saved);
    return true;
}

void start(Piece pieces[SQR_QTD][SQR_QTD], char **names, bool player, bool save){  

    if(save){
        printTitle(2, names);

        fstream file;
        file.open(names[4], ios::in);

        if(file.fail()){
            names[4] = new char('f');
            file.close();
            start(pieces, names, player, save);
        }

        readArchive(file,pieces,names);

        if(names[3][0] == 1){
            player = true;
        }else{
            player = false;
        }
    }else{
        printTitle(3, names);
    }

    
    if(!player){
        names[1] = "CPU";
    }else if(!save)
            printTitle(4, names);

    bool movetime = true;    
    bool error = false;
    bool win = false;
    int winint;

    char *strmove = new char[8];
    while(strcmp("sair", strmove) != 0 && !win){

        draw(pieces);
        if(error){
            cout << "Erro em validar sua jogada, jogue novamente." << endl; 
        }

        error = false;
        cout << "Vez do jogador " << (movetime ? names[0] : names[1]) << ", digite um movimento." << endl;
        fgets(strmove, 8, stdin);

        Move movectr;    

        if(strcmp("salvar\n", strmove) == 0){
            
            printTitle(7, names);
            saveArchive(pieces, names, player, names[3]);

        }else if(strcmp("sair\n", strmove) == 0){            
            break;
            return;
        }else{
            
            
            for(int i = 0; i < 5; i++){
                if(error){
                    break;
                }
                int c = strmove[i];
        
                if(((c >= 65 && c <= 72) || (c >= 97 && c <= 104)) && i == 0 || i == 3){
                    if(c >= 97){
                        c -= 97; 
                    }else if(c >= 65){
                        c -= 65;
                    }

                    if(i == 0){
                        movectr.initialY = c;
                    }else{ 
                        movectr.finalY = c;
                    }
                }else if((c >= 48 && c <= 56) && i == 1 || i == 4){
                    c -= 49;

                    if(i == 1){
                        movectr.initialX = c;
                    }else{
                        movectr.finalX = c;
                    }
                }else if(c == 32){
                    
                }else{
                    error = true;
                }
            }

            if(!error){ 
                if(move(movectr, pieces, movetime)){
                    movetime = !movetime;
                }else{
                    error = true;
                }
            }
        }
        winint = checkWin(pieces);
        if(winint == 1 || winint == 2){
            win = true;
        }      
    }

    if(win){
        printTitle((winint == 1? 5 : 6), names);
        int tchar = names[3][0];

        cout << "THCA: " << tchar << endl; 
        if(tchar == 1){
            //Jogar dnv
        }else if(tchar == 2){
            //Sair
        }else{
            
        }
    }
}

int checkWin(Piece pieces[SQR_QTD][SQR_QTD]){
    int player1 = 0, player2 = 0;
    
    for(int i = 0; i < SQR_QTD; i++){
        for(int j = 0; j < SQR_QTD; j++){
            Piece piece = pieces[i][j];
            if(piece.value != -1){
                if(piece.owner == 0){
                    player1 += 1;
                }else if(piece.owner == 1){
                    player2 += 1;
                }
            }
        }
    }

    if(player1 > 0 && player2 == 0){
        return 1;
    }else if(player2 > 0 && player1 == 0){
        return 2;
    }else{
        return 0;
    } 
}

char convertValue(Piece piece){
    int value = piece.value;
    if(piece.owner == 0){
        if(value == 1){
            return 'O';
        }else{
            return 'o';
        }
    }else if(piece.owner == 1){
        if(value == 1){
            return 'X';
        }else{
            return 'x';
        }
    }
    return '-';
}

bool saveArchive(Piece pieces[SQR_QTD][SQR_QTD], char **names, bool player, char *archiveName){
    ofstream file;
    file.open(archiveName);
    file.write(names[0], strlen(names[0]));
    file.write((char*)"\n", 1);
    file.write(names[1], strlen(names[1]));
    file.write((char*)"\n", 1);
    for(int i = 0; i < SQR_QTD; i++){
        for(int j = 0; j < SQR_QTD ; j++){
            char converted[] = {convertValue(pieces[i][j])};
            file.write((char*) converted, 1);
        }
        file.write((char*)"\n", 1);
        cout << endl;
    }
    file.write((char*) (player ? "1" : "2"), 1);
    file.close();
    return true;
}

bool readArchive(fstream &file, Piece pieces[SQR_QTD][SQR_QTD], char **names){    
    char *strnames = new char[32];
    file.getline(strnames, 32);
    strcpy(names[0], strnames);

    file.getline(strnames, 32);
    strcpy(names[1], strnames);

    char* strpieces = new char[128];
    for(int i = 0; !file.eof(); i++){
        file.getline(strpieces, 128);
        int n = strlen(strpieces);
        if(n == 1){
            names[3][0] = n;
            continue;
        }
        for(int j = 0; j < n; j++){
            Piece piece = pieces[i][j];

            switch (strpieces[j]){
                case '-':
                    pieces[i][j].owner = -1;
                    pieces[i][j].value = -1;
                    break;
                case 'x':
                    pieces[i][j].owner = 0;
                    pieces[i][j].value = 0;
                    break;         
                case 'X':
                    pieces[i][j].owner = 0; 
                    pieces[i][j].value = 1;
                    break;
                case 'o':
                    pieces[i][j].owner = 1;
                    pieces[i][j].value = 0;
                    break;
                case 'O':
                    pieces[i][j].owner = 1;
                    pieces[i][j].value = 1;
                    break;

                default:
                    break;
            }
        }
    }

    file.close();

    delete[] strnames;
    delete[] strpieces;

    return true;
}

/*
 *  Declaração da função que gera a cor necessária para cada
 *  azulejo do tabuleiro de acordo com as variáveis inteiras y e x.
 */

char* color(int y, int x){
    if(y % 2 == 0){
        if(x % 2 == 0){
            return BACK_WHITE;
        }else{
            return BACK_GRAY;
        }
    }else{
        if(x % 2 == 0){
            return BACK_GRAY;
        }else{
            return BACK_WHITE;
        }
    }
}

/*
 *  Declaração da função que gera o valor booleano para um determinado
 *  movimento descrito pela variável move.
 */
bool canMove(Move move, Piece pieces[SQR_QTD][SQR_QTD], bool player){

    Piece piece = pieces[move.initialY][move.initialX];
    Piece finalPiece = pieces[move.finalY][move.finalX];

    //Se a peça final tiver dono, o movimento é inválido.
    if(finalPiece.owner != -1){
        return false;
    }

    if(piece.y == finalPiece.y){
        return false;
    }

    if(piece.owner == (player ? 0 : 1)){
        return false;
    }

    if(piece.value == 0){
        for(int i = -2; i <= 2; i++){
            if(i == 0)
                continue;
            for(int j = -2; j <= 2; j++){
                if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){

                    int ty = move.finalY - move.initialY;
                    int tx = move.finalX - move.initialX;

                    int yy = ((ty < 1) ? move.initialY - 1 : move.initialY + 1);
                    int xx = ((tx < 1) ? move.initialX - 1 : move.initialX + 1);

                    Piece eaten = pieces[yy][xx];

                    if((eaten.owner == (piece.owner == 1 ? 0 : 1)) && piece.value != 1){
                        return true;
                    }else if(i == 1 || i == -1){
                        return true;
                    }
                }
            }
        }
    }else{
        for(int i = -SQR_QTD; i <= SQR_QTD; i++){
            if(i == 0)
                continue;
            for(int j = -SQR_QTD; j <= SQR_QTD; j++){
                int ty = move.finalY - move.initialY;
                int tx = move.finalX - move.initialX;

                int yy = ((ty < 0) ? move.initialY + ty : move.initialY + ty) + 1;
                int xx = ((tx < 0) ? move.initialX - tx : move.initialX + tx) - 1;

                Piece eaten = pieces[yy][xx];

                if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){
                    
                    if((eaten.owner == (piece.owner == 1 ? 0 : 1))){
                        return true;
                    }else if(abs(i) == abs(j)){
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool move(Move move, Piece pieces[SQR_QTD][SQR_QTD], bool player){

    if(canMove(move, pieces, player)){
        Piece piece = pieces[move.initialY][move.initialX];

        if(piece.value == 1){
            for(int i = -SQR_QTD; i <= SQR_QTD; i++){
                if(i == 0)
                    continue;
                for(int j = -SQR_QTD; j <= SQR_QTD; j++){
                    if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){

                        int ty = move.finalY - move.initialY;
                        int tx = move.finalX - move.initialX;

                        int yy = ((ty < 0) ? move.initialY + ty : move.initialY + ty) + 1;
                        int xx = ((tx < 0) ? move.initialX - tx : move.initialX + tx) - 1;

                        Piece eaten = pieces[yy][xx];

                        if((eaten.owner == (piece.owner == 1 ? 0 : 1))){
                            eaten.owner = -1;

                            pieces[yy][xx] = eaten;
                        }
                    }
                }
            }
        }else{
            if(move.initialY + 2 == move.finalY || move.initialY - 2 == move.finalY){
                
                int ty = move.finalY - move.initialY;
                int tx = move.finalX - move.initialX;

                int yy = ((ty < 1) ? move.initialY - 1 : move.initialY + 1);
                int xx = ((tx < 1) ? move.initialX - 1 : move.initialX + 1);

                Piece eaten = pieces[yy][xx];

                if(eaten.owner == (piece.owner == 1 ? 0 : 1)){
                    eaten.owner = -1;

                    pieces[yy][xx] = eaten;
                }
            }
        }

        int aux = piece.owner;

        piece.owner = -1;
        pieces[move.initialY][move.initialX] = piece;

        if((player && move.finalY == 0) || (!player && move.finalY == 7)){
            piece.value = 1;
        }

        piece.owner = aux;
        piece.y = move.finalY;
        piece.x = move.finalX;

        pieces[move.finalY][move.finalX] = piece;      

        return true;
    }
    
    return false;
}

/*
 *  Declaração da função que desenha e mostra o tabuleiro para o utilizador.
 */

void draw(Piece pieces[SQR_QTD][SQR_QTD]){
    cout << "\033[2J\033[1;1H";
    for(int j = 0; j < SQR_HEIGH * (SQR_QTD) + 1; j++){
        for(int i = 0; i < SQR_WIDTH * (SQR_QTD) + ((SQR_WIDTH / 2) - 2); i++){

            int lin = j / SQR_HEIGH , col = i / SQR_WIDTH;
            char *back = color(col+SQR_WIDTH, lin+SQR_HEIGH);

            cout << back;

            if(i % SQR_WIDTH == 0 || j % SQR_HEIGH == 0){
                
                //Mostrando as letras de posição.
                if((i % SQR_WIDTH >= (SQR_WIDTH / 2) - 1) && (i % SQR_WIDTH <= (SQR_WIDTH / 2)) && (j % SQR_HEIGH <= 2) && j > SQR_HEIGH * (SQR_QTD) - 1){
                    //TODO: trocar, colocar letra
                    cout << LIGHT_BLUE << NUM[col];
                }else{
                    cout << " ";
                }

            } else if((i % SQR_WIDTH >= (SQR_WIDTH / 2) - 2) && (i % SQR_WIDTH <= (SQR_WIDTH / 2) + 1) && (j % SQR_HEIGH <= 2)){
                //Mostra as pecas no quadrado
                Piece piece = pieces[lin][col];

                //Checando se o dono da peça é válido.
                if(piece.owner == -1){
                    cout << back << " " << RESET; 
                    continue;
                }

                char *color = piece.owner == 1 ? BACK_WHITE : BACK_BLACK;

                //Mostrando a peça de acordo com seu dono.
                cout << color << LIGHT_BLUE << (piece.value == 1 ? "D" : " ") << RESET;
            

            } else if((i >= (SQR_WIDTH * (SQR_QTD) + ((SQR_WIDTH / 2) - 2)) - 1) && j % SQR_HEIGH <= 2){
                //Mostrando os números de posição.
                cout << LIGHT_BLUE << ABC[lin];
            } else{
                cout << " ";
            }

            cout << RESET;         
        }
        cout << endl;
    }
}

/*
 *  Declaração da função que gera o menu principal para o jogador,
 *  de acordo com a opção, variável opt, e o estágio do jogo, variável stage.
 */
void printTitle(int opt, char **names){
    cout << "\033[2J\033[1;1H";
    char *es = "             ";
    char *ch = "                     ";

    for(int i = 0; i < 5; i++)
        cout << endl;
    cout << es << LIGHT_YELLOW << "8888888b.                                  " << RESET << endl;
    cout << es << LIGHT_YELLOW << "888   Y88b                                 " << RESET << endl;
    cout << es << YELLOW       << "888    888                                 " << RESET << endl;
    cout << es << YELLOW       << "888    888  8888b.  88888b.d88b.   8888b.  " << RESET << endl; 
    cout << es << LIGHT_RED    << "888    888      88b 888  888  88b      88b " << RESET << endl;
    cout << es << LIGHT_RED    << "888    888 .d888888 888  888  888 .d888888 " << RESET << endl;  
    cout << es << RED          << "888  .d88P 888  888 888  888  888 888  888 " << RESET << endl;
    cout << es << RED          << "8888888P    Y888888 888  888  888  Y888888 " << RESET << endl;
    cout << endl << endl;
    
    if(opt == 0){
        cout << ch << "1 - Jogar contra computador." << endl;
        cout << ch << "2 - Jogar contra outro jogador." << endl;
        cout << ch << "3 - Sair." << endl;
    }else if(opt == 1){
        cout << ch << "1 - Começar novo jogo" << endl;
        cout << ch << "2 - Começar de um jogo salvo" << endl;
        cout << ch << "3 - Voltar" << endl;
    }else if(opt == 2){
        if(names[4][0] == 'f') {
            cout << ch << "Falha em abrir o arquivo desejado." << endl;
        }
        cout << ch << "Digite o nome do arquivo salvo:" << endl;
        cout << ch << "> "; 
        cin >> names[4];
    }else if(opt == 3){
        cout << ch << "Digite o nome do primeiro jogador:" << endl;
        cout << ch << "> "; 
        cin >> names[0];
    }else if(opt == 4){
        cout << ch << "Digite o nome do segundo jogador:" << endl;
        cout << ch << "> "; 
        cin >> names[1]; 
    }else if(opt == 5 || opt == 6){
        cout << ch << "       PARABENS!! "<< endl;
        cout << "      O jogador " << (opt == 5 ? names[0] : names[1]) << " foi o ganhador da partida."<< endl;
        cout << endl;
        cout << ch << "Deseja jogar novamente:" << endl;
        cout << ch << "1 - Sim" << endl;
        cout << ch << "2 - Não" << endl;
        cout << ch << "> ";

        int opt;
        cin >> opt;
        names[3][0] = opt;
    }else if(opt == 7){
        cout << ch << "Digite o nome do arquivo :" << endl;
        cout << ch << "> "; 
        cin >> names[3]; 
    }
    for(int i = 0; i < 8; i++)
        cout << endl;
    //cout << ch << ch << es << "gfanedm" << endl;
}