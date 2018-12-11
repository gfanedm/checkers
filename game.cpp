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
void draw(Piece [SQR_QTD][SQR_QTD], int [2], char **);
void printTitle(int, char **);
bool canMove(Move, Piece [SQR_QTD][SQR_QTD], bool, int &);
bool move(Move, Piece [SQR_QTD][SQR_QTD], bool, int [2]);
bool menu(Piece [SQR_QTD][SQR_QTD], char **);
void start(Piece [SQR_QTD][SQR_QTD], char **, bool, bool);
bool readArchive(fstream &, Piece [SQR_QTD][SQR_QTD], char **);
int checkWin(Piece [SQR_QTD][SQR_QTD]);
char convertValue(Piece);
bool saveArchive(Piece [SQR_QTD][SQR_QTD], char **, bool, char *);
Move findMove(Piece [SQR_QTD][SQR_QTD]);

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

    //Inicia o jogo.
    start(pieces, names, player, saved);
    return true;
}

/*
 *  Declaração da função que inicia o jogo, de acordo com os dados adquiridos
 *  nos menus.
 */
void start(Piece pieces[SQR_QTD][SQR_QTD], char **names, bool player, bool save){  

    //Checando se o jogador quer que leia um arquivo.
    if(save){

        //Mostra a opção para a definição do nome do arquivo.
        printTitle(2, names);

        //Abertura do arquivo definido.
        fstream file;
        file.open(names[4], ios::in);

        //Checando se há alguma falha em abrir o arquivo.
        if(file.fail()){
            names[4] = new char('f');
            file.close();

            //Reiniciando o processo de iniciar.
            start(pieces, names, player, save);
        }

        //Lendo e definindo as peças e nomes dos jogadores. 
        readArchive(file,pieces,names);

        //Definindo qual jogador deverá iniciar.
        if(names[3][0] == 1){
            player = true;
        }else{
            player = false;
        }

    }else{
        //Pedindo para o jogador digitar o nome do primeiro jogador.
        printTitle(3, names);
    }

    //Checando se o jogador deseja jogar contra o computador.
    if(!player){
        names[1] = new char[4];
        strcpy(names[1],"CPU");
    }else if(!save)
        printTitle(4, names);

    //Definindo os pontos dos jogadores.
    int points[2] = {0,0};

    //Definindo variaveis de erro, vez, vitória.
    bool movetime = true;    
    bool error = false;
    bool win = false;
    int winint;

    //Definindo a string a ser adquirida pelo programa.
    char *strmove = new char[8];

    //Checando se o jogador quer sair do jogo ou se a vitória aconteceu.
    while(strcmp("sair", strmove) != 0 && !win){
        
        //Desenha o tabuleiro e checa se há algum erro.
        draw(pieces, points, names);
        if(error){
            cout << "Erro em validar sua jogada, jogue novamente." << endl; 
        }

        error = false;
        
        //Checando se é a vez do jogador, caso seja jogador vs computador.
        if(strcmp("CPU", names[1]) == 0 && !movetime){
            cout << "Vez do computador." << endl;

            //Procura o melhor movimento para o computador.
            Move movectr = findMove(pieces);

            cout << "Move: " << movectr.initialY << "," << movectr.initialX << " to " << movectr.finalY << "," << movectr.finalX << endl;
            
            //Move a peça escolhida pelo algorítimo.
            if(move(movectr, pieces, movetime, points)){
                movetime = !movetime;
            }else{
                cout << "Erro em mover a peça do computador." << endl;  
            }

        }else{

            //Jogador jogando contra outro jogador.
            cout << "Vez do jogador " << (movetime ? names[0] : names[1]) << ", digite um movimento." << endl;

            //Pega o que o usuário digitar.
            fgets(strmove, 8, stdin);

            Move movectr;    

            //Checa se o jogador deseja salvar ou sair do jogo.
            if(strcmp("salvar\n", strmove) == 0){
                
                //Pede para o jogador digitar o nome do arquivo e o salva.
                printTitle(7, names);
                saveArchive(pieces, names, player, names[3]);

            }else if(strcmp("sair\n", strmove) == 0){     
                
                //Sai do jogo.    
                break;
                return;

            }else{
                //Seleciona os caracteres digitados para adquirir o movimento.
                for(int i = 0; i < 5; i++){

                    //Checa se não há erros anteriores.
                    if(error){
                        break;
                    }

                    //Define o inteiro de acordo com a tabela ASCII.
                    int c = strmove[i];

                    //Checa se o inteiro está entre os caracteres aceitos.
                    if(((c >= 65 && c <= 72) || (c >= 97 && c <= 104)) && (i == 0 || i == 3)){
                        
                        //Define o inteiro com o valor do movimento.
                        if(c >= 97){
                            c -= 97; 
                        }else if(c >= 65){
                            c -= 65;
                        }

                        //Define o movimento inicial ou final com o valor do inteiro adiquirido.
                        if(i == 0){
                            movectr.initialY = c;
                        }else{ 
                            movectr.finalY = c;
                        }
                    
                    //Checa se o inteiro está entre os caractéres numéricos aceitos.
                    }else if((c >= 48 && c <= 56) && (i == 1 || i == 4)){
                        
                        //Define o inteiro com o valor do movimento.
                        c -= 49;

                        //Define o movimento inicial ou final com o valor do inteiro adiquirido.
                        if(i == 1){
                            movectr.initialX = c;
                        }else{
                            movectr.finalX = c;
                        }
                    }else if(c == 32){
                        //Nada acontece se o caractére é espaço.
                    }else{
                        //Define erro do movimento inválido.
                        error = true;
                    }
                }

                //Se não tiver nenhum erro o jogo tenta executar o movimento.
                if(!error){ 
                    if(move(movectr, pieces, movetime, points)){
                        movetime = !movetime;
                    }else{
                        error = true;
                    }
                }
            }
        }

        //Define a variável com a função de checar a vitória e encerra a partida.
        winint = checkWin(pieces);
        if(winint == 1 || winint == 2){
            win = true;
        }      
    }

    //Se houver a vitória, mostra ao jogador a parabenização.
    if(win){
        printTitle((winint == 1 ? 5 : 6), names);
    }
}

/*
 *  Declaração da função que checa as peças dos jogadores e vê se há
 *  ou não um vencedor.
 */
int checkWin(Piece pieces[SQR_QTD][SQR_QTD]){
    //Declaração dos contadores de peça de cada jogador.
    int player1 = 0, player2 = 0;
    
    //Verificação de peça por peca do tabuleiro.
    for(int i = 0; i < SQR_QTD; i++){
        for(int j = 0; j < SQR_QTD; j++){

            //Definição da variável da peça.
            Piece piece = pieces[i][j];

            //Checa se o valor da peça é válido.
            if(piece.value != -1){

                //Verificação do dono da peça.
                if(piece.owner == 0){
                    player1 += 1;
                }else if(piece.owner == 1){
                    player2 += 1;
                }
            }
        }
    }

    //Retorna os ganhadores da partida se houver um.
    if(player1 > 0 && player2 == 0){
        return 1;
    }else if(player2 > 0 && player1 == 0){
        return 2;
    }else{
        return 0;
    } 
}

/*
 *  Declaração da função que gera um caractére para o efetuar
 *  o salvamento do arquivo desejado.
 */
char convertValue(Piece piece){

    //Declaração da variável com o valor da peça.
    int value = piece.value;

    //Checando o dono da peça desejada.
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

    //Retornando nada para a peça inválida.
    return '-';
}

/*
 *  Declaração da função que salva o arquivo com o nome especificado
 *  pelo jogador.
 */
bool saveArchive(Piece pieces[SQR_QTD][SQR_QTD], char **names, bool player, char *archiveName){

    //Abertura do arquivo desejado.
    ofstream file;
    file.open(archiveName);

    //Escrevendo os nomes dos jogadores no arquivo.
    file.write(names[0], strlen(names[0]));
    file.write((char*)"\n", 1);
    file.write(names[1], strlen(names[1]));
    file.write((char*)"\n", 1);

    //Escrevendo as linhas e colunas do tabuleiro no arquivo.
    for(int i = 0; i < SQR_QTD; i++){
        for(int j = 0; j < SQR_QTD ; j++){
            
            //Definição do valor da peça.
            char converted[] = {convertValue(pieces[i][j])};
            
            //Escrevendo a peça no arquivo.
            file.write((char*) converted, 1);
        }

        //Pulando uma linha no arquivo.
        file.write((char*)"\n", 1);
        cout << endl;
    }

    //Escrevendo o jogador inicial.
    file.write((char*) (player ? "1" : "2"), 1);

    //Fechando o arquivo.
    file.close();
    return true;
}


/*
 *  Declaração da função que lê o arquivo que o jogador especificou
 *  e retorna uma variavel booleana para sucesso ou falha.
 */

bool readArchive(fstream &file, Piece pieces[SQR_QTD][SQR_QTD], char **names){    
    
    //Leitura dos nomes dos jogadores nas duas primeiras linhas.
    char *strnames = new char[32];
    file.getline(strnames, 32);
    strcpy(names[0], strnames);

    file.getline(strnames, 32);
    strcpy(names[1], strnames);

    //Leitura das peças do arquivo.
    char* strpieces = new char[128];
    for(int i = 0; !file.eof(); i++){
        file.getline(strpieces, 128);
        
        //Leitura do jogador iniciante.
        int n = strlen(strpieces);
        if(n == 1){
            names[3][0] = n;
            continue;
        }

        //Leitura das peças.
        for(int j = 0; j < n; j++){

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

    //Fechando o arquivo e deletando as variáveis.
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
    //Definição da variável de retorno.
    char* name = new char[10];

    //Checando as posições do tabuleiro alternadamente.
    if(y % 2 == 0){
        if(x % 2 == 0){
            strcpy(name, BACK_WHITE);
        }else{
            strcpy(name, BACK_GRAY);
        }
    }else{
        if(x % 2 == 0){
            strcpy(name, BACK_GRAY);
        }else{
            strcpy(name, BACK_WHITE);
        }
    }

    return name;
}

/*
 *  Declaração da função que gera o valor booleano para um determinado
 *  movimento descrito pela variável move.
 */
bool canMove(Move move, Piece pieces[SQR_QTD][SQR_QTD], bool player, int &eat){

    Piece piece = pieces[move.initialY][move.initialX];
    Piece finalPiece = pieces[move.finalY][move.finalX];

    //Se a peça final tiver dono, o movimento é inválido.
    if(finalPiece.owner != -1){
        return false;
    }

    //Se a peça inicial não tiver dono, o movimento é inválido.
    if(piece.owner == -1){
        return false;
    }

    //Se o y inicial for igual ao y final, o movimento é inválido.
    if(piece.y == finalPiece.y){
        return false;
    }

    //Se o dono for diferente do jogador, o movimento é inválido.
    if(piece.owner == (player ? 0 : 1)){
        return false;
    }

    //Se o movimento estiver fora dos limites do tabuleiro, o movimento é inválido.
    if(move.finalY > SQR_QTD || move.finalY < 0){
        return false;
    }

    if(move.finalX > SQR_QTD || move.finalX < 0){
        return false;
    }

    //Checando se a peça é um peão ou uma dama.
    if(piece.value == 0){
        for(int i = -2; i <= 2; i++){
            if(i == 0)
                continue;
            for(int j = -2; j <= 2; j++){
                if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){
                    
                    //Definindo as variáveis adquiridas do movimento.
                    int ty = move.finalY - move.initialY;
                    int tx = move.finalX - move.initialX;
                    
                    //Definindo as variáveis da peça que pode ser comida.
                    int yy = ((ty < 0) ? move.initialY + ty : move.initialY - ty);
                    int xx = ((tx < 0) ? move.initialX - tx : move.initialX + tx);

                    //Definição da variável da peça comida.
                    Piece eaten = pieces[yy][xx];

                    //Checando se a peça pode ser comida e por quem.
                    if(eaten.owner != piece.owner && eaten.owner != -1){
                        if(player){
                            if(ty == -2 && (tx == 2 || tx == -2)){
                                eat += 1;
                                return true;
                            }
                        }else{
                            if(ty == 2 && (tx == 2 || tx == -2)){
                                eat += 1;
                                return true;
                            }
                        }

                    //Checando se a peça pode efetuar um movimento padrao de peão.
                    }else if((tx == 1 || tx == -1)){
                        if(player && ty == -1){
                            return true;
                        }else if(!player && ty == 1){
                            return true;
                        }
                    }
                }
            }
        }
    }else{
        for(int i = -SQR_QTD; i <= SQR_QTD; i++){
            if(i == 0)
                continue;
            for(int j = -SQR_QTD; j <= SQR_QTD; j++){

                //Definindo as variáveis adquiridas do movimento.
                int ty = move.finalY - move.initialY;
                int tx = move.finalX - move.initialX;

                //Definindo as variáveis da peça que pode ser comida.
                int yy = ((ty < 0) ? move.initialY + ty : move.initialY - ty);
                int xx = ((tx < 0) ? move.initialX - tx : move.initialX + tx) - 1;

                //Definição da variável da peça comida.
                Piece eaten = pieces[yy][xx];

                //Checando se o movimento é equivalente ao final.
                if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){

                    //Checando se a peça pode ser comida e por quem.
                    if((eaten.owner == (piece.owner == 1 ? 0 : 1))){
                        if(eaten.owner == -1){
                            return false;
                        }
                        eat += 1;
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

/*
 *  Declaração da função que gera um movimento válido para a vez do
 *  computador.
 */
Move findMove(Piece pieces[SQR_QTD][SQR_QTD]){

    //Variáveis usadas para guardar os movimentos e a quantidade de vezes
    //esta peça come outras.
    Move *moves = new Move[SQR_QTD * SQR_QTD];
    int *eats = new int[SQR_QTD * SQR_QTD];
    Move find;


    //Variável auxiliar.
    int k = 0;

    for(int i = 0; i < SQR_QTD; i++){
        find.initialY = i;
        for(int j = 0; j < SQR_QTD; j++){
            find.initialX = j;
            for(int y = 0; y < SQR_QTD; y++){
                find.finalY = y;
                for(int x = 0; x < SQR_QTD; x++){
                    find.finalX = x;

                    //Declaração da variável para definição de quantas peças podem ser comidas.
                    int eat = 0;
                    
                    //Checando se o movimento é valido.
                    if(canMove(find, pieces, false, eat)){

                        //Adicionando o movimento na matriz de movimentos válidos.
                        eats[k] = eat;
                        moves[k++] = find;

                    }
                }
            }
        }
    }

    //Escolhendo o melhor movimento de acordo com a quantidade de peças comidas.
    int max = 0;
    for(int i = 0; i < k; i++){

        //Definição da variável do movimento para ser analisado.
        Move movectr = moves[i];

        //Checando se o movimento possúi um valor de peças comida maior do que ao anterior.
        if(eats[i] >= max){
            find = movectr;
            max = eats[i];
        }
    }

    //Deletando as variáveis com alocação dinâmica.
    delete[] eats;
    delete[] moves;

    return find;
}



/*
 *  Declaração da função que movimenta uma peça de acordo com o tabuleiro e o
 *  jogador que está efetuando o movimento.
 */
bool move(Move move, Piece pieces[SQR_QTD][SQR_QTD], bool player, int points[2]){
    int eat = 0;

    //Checando se o movimento é valido.
    if(canMove(move, pieces, player, eat)){
        Piece piece = pieces[move.initialY][move.initialX];

        if(piece.value == 1){
            for(int i = -SQR_QTD; i <= SQR_QTD; i++){
                if(i == 0)
                    continue;
                for(int j = -SQR_QTD; j <= SQR_QTD; j++){
                    if(move.initialX + i == move.finalX && move.initialY + j == move.finalY){

                        //Checando se há possibiliadade de comer uma peça.
                        int ty = move.finalY - move.initialY;
                        int tx = move.finalX - move.initialX;

                        int yy = ((ty < 0) ? move.initialY + ty : move.initialY - ty);
                        int xx = ((tx < 0) ? move.initialX - tx : move.initialX + tx);

                        Piece eaten = pieces[yy][xx];

                        //Comendo a peça se for necessário.
                        if((eaten.owner == (piece.owner == 1 ? 0 : 1))){
                            eaten.owner = -1;

                            pieces[yy][xx] = eaten;
                            points[(player ? 0: 1)] += 1; 
                        }
                    }
                }
            }
        }else{
            if(move.initialY + 2 == move.finalY || move.initialY - 2 == move.finalY){

                //Checando se há possibiliadade de comer uma peça.
                int ty = move.finalY - move.initialY;
                int tx = move.finalX - move.initialX;

                int yy = ((ty < 1) ? move.initialY - 1 : move.initialY + 1);
                int xx = ((tx < 1) ? move.initialX - 1 : move.initialX + 1);

                Piece eaten = pieces[yy][xx];

                //Comendo a peça se for necessário.
                if(eaten.owner == (piece.owner == 1 ? 0 : 1)){
                    eaten.owner = -1;

                    pieces[yy][xx] = eaten;
                    points[(player ? 0: 1)] += 1;
                }
            }
        }
        
        //Variável auxiliar que armazena o dono da peça.
        int aux = piece.owner;

        //Colocando o dono da peça como -1 para ela somir do tabuleiro.
        piece.owner = -1;
        pieces[move.initialY][move.initialX] = piece;

        //Definindo uma dama de acordo com o y orientador do jogador.
        if((player && move.finalY == 0) || (!player && move.finalY == 7)){
            piece.value = 1;
        }

        //Definindo os novos valores para a peça final.
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

void draw(Piece pieces[SQR_QTD][SQR_QTD], int points[2], char** names){
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

                const char *color = piece.owner == 1 ? BACK_WHITE : BACK_BLACK;

                //Mostrando a peça de acordo com seu dono.
                cout << color << LIGHT_BLUE << (piece.value == 1 ? "D" : " ") << RESET;
            

            } else if((i >= (SQR_WIDTH * (SQR_QTD) + ((SQR_WIDTH / 2) - 2)) - 1) && j % SQR_HEIGH <= 2){
                //Mostrando os números de posição.
                cout << LIGHT_BLUE << ABC[lin];
            } else{
                cout << " ";
            }

            cout << RESET;
            delete[] back; 
        }
        cout << endl;
    }
    cout << LIGHT_BLUE << "Pontos:" << endl;
    cout << "Jogador " << names[0] << ": " << points[0] << endl;
    cout << "Jogador " << names[1] << ": " << points[1] << RESET<< endl;
}

/*
 *  Declaração da função que gera o menu principal para o jogador,
 *  de acordo com a opção, variável opt, e o estágio do jogo, variável stage.
 */
void printTitle(int opt, char **names){
    //Limpando o console para uma nova mostragem.
    cout << "\033[2J\033[1;1H";

    //Definindo as variáveis para os espaços.
    char *es = new char[20];
    strcpy(es, "             ");
    char *ch = new char[32];
    strcpy(ch, "                     ");

    //Iniciando o titulo.
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
    
    //Colocando as opções para o jogador.
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
    }else if(opt == 7){
        cout << ch << "Digite o nome do arquivo :" << endl;
        cout << ch << "> "; 
        cin >> names[3]; 
    }

    //Finalizando com outros espaços.
    for(int i = 0; i < 8; i++)
        cout << endl;
}