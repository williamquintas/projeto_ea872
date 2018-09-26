#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

#include "oo_model.hpp"

#include <ncurses.h>

#define VELOCIDADE_TIRO 3
#define ACELERACAO_TIRO 3
#define ALTURA_TELA 20
#define LARGURA_TELA 40
#define POSICAO_X_NAVE 3

int total_tiros = 0;

using namespace std::chrono;

//Nave
Nave::Nave(float posicao) {
  this->posicao = posicao;
}

void Nave::update(float nova_posicao) {
  this->posicao = nova_posicao;
}

float Nave::get_posicao() {
  return this->posicao;
}

//Alvo
Alvo::Alvo(float posicao_x, float posicao_y){
  this->posicao_x = posicao_x;
  this->posicao_y = posicao_y;
}

float Alvo::get_posicao_x() {
  return this->posicao_x;
}

float Alvo::get_posicao_y() {
  return this->posicao_y;
}

void Alvo::update(float nova_posicao_x, float nova_posicao_y) {
  this->posicao_x = nova_posicao_x;
  this->posicao_y = nova_posicao_y;
}

//Tiro
Tiro::Tiro(float posicao_x, float posicao_y, float velocidade, int existe) {
  this->posicao_x = posicao_x;
  this->posicao_y = posicao_y;
  this->velocidade = velocidade;
  this->existe = existe;
}

float Tiro::get_posicao_x() {
  return this->posicao_x;
}

float Tiro::get_posicao_y() {
  return this->posicao_y;
}

float Tiro::get_velocidade() {
  return this->velocidade;
}

int Tiro::get_existe() {
  return this->existe;
}

void Tiro::update_existe(int e) {
  this->existe = e;
}

void Tiro::update(float nova_posicao_x, float nova_posicao_y, float nova_velocidade) {
  this->posicao_x = nova_posicao_x;
  this->posicao_y = nova_posicao_y;
  this->velocidade = nova_velocidade;
}

//Lista de Naves
ListaDeNaves::ListaDeNaves() {
  this->naves = new std::vector<Nave *>(0);
}

void ListaDeNaves::hard_copy(ListaDeNaves *ldn) {
  std::vector<Nave *> *naves = ldn->get_naves();
  for (int k=0; k<naves->size(); k++) {
    Nave *n = new Nave( (*naves)[k]->get_posicao());
    this->add_nave(n);
  }
}

void ListaDeNaves::add_nave(Nave *n) {
  (this->naves)->push_back(n);
}

std::vector<Nave*> *ListaDeNaves::get_naves() {
  return (this->naves);
}


//Lista de Tiros
ListaDeTiros::ListaDeTiros() {
  this->tiros = new std::vector<Tiro *>(0);
}

void ListaDeTiros::hard_copy(ListaDeTiros *ldt) {
  std::vector<Tiro *> *tiros = ldt->get_tiros();

  for (int k=0; k<tiros->size(); k++) {
    Tiro *t = new Tiro( (*tiros)[k]->get_posicao_x(), 
                        (*tiros)[k]->get_posicao_y(), 
                        (*tiros)[k]->get_velocidade(), 
                        (*tiros)[k]->get_existe()     );
    this->add_tiro(t);
  }

}

void ListaDeTiros::add_tiro(Tiro *t) {
  (this->tiros)->push_back(t);
}

std::vector<Tiro*> *ListaDeTiros::get_tiros() {
  return (this->tiros);
}


//Fisica
Fisica::Fisica(ListaDeNaves *ldn, ListaDeTiros *ldt) {
  this->lista_nave = ldn;
  this->lista_tiro = ldt;
}

void Fisica::andar_nave(int deslocamento) {
  std::vector<Nave *> *n = this->lista_nave->get_naves();
  if ((*n)[0]->get_posicao() + deslocamento > 0 && (*n)[0]->get_posicao() + deslocamento < ALTURA_TELA) {
    (*n)[0]->update((*n)[0]->get_posicao() + deslocamento);
  }
}


void Fisica::disparar_tiro(int i_tiro){
  total_tiros++;
  std::vector<Nave *> *n = this->lista_nave->get_naves();
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  (*t)[i_tiro]->update_existe(1);
  (*t)[i_tiro]->update(POSICAO_X_NAVE+2, (*n)[0]->get_posicao(), VELOCIDADE_TIRO);
}

void Fisica::destruir_tiro(int i_tiro){
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  (*t)[i_tiro]->update_existe(0);
}


void Fisica::update_tiro(float deltaT) {
  // Atualiza parametros dos tiros!
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  for (int i = 0; i < (*t).size(); i++) {
    float new_vel = (*t)[i]->get_velocidade() + (float)deltaT * (ACELERACAO_TIRO)/1000;
    float new_pos = (*t)[i]->get_posicao_x() + (float)deltaT * new_vel/1000;
    (*t)[i]->update(new_pos, (*t)[i]->get_posicao_y(), new_vel);
  }
}

//Tela
Tela::Tela(Alvo *alvo, ListaDeNaves *ldn, ListaDeTiros *ldt, int maxI, int maxJ, float maxX, float maxY) {

  this->alvo = alvo;
  this->alvo_antigo = new Alvo(this->alvo->get_posicao_x(), this->alvo->get_posicao_y()); 

  this->lista_nave = ldn;
  this->lista_anterior_nave = new ListaDeNaves();
  this->lista_anterior_nave->hard_copy(this->lista_nave);

  this->lista_tiro = ldt;
  this->lista_anterior_tiro = new ListaDeTiros();
  this->lista_anterior_tiro->hard_copy(this->lista_tiro);

  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();		 /* Start curses mode 		*/
	raw();		 /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
}

void Tela::draw() {
    int l, a;
    
    for (a=0; a<ALTURA_TELA; a++){
        l = 0;
        //desenha em l = 0
        move(a, l);
        echochar('+');
    }    
    for (l=0; l<LARGURA_TELA; l++){
        //desenha em a = 0 e a = altura_tela
        a = 0;
        move (a, l);
        echochar('+');
        a = ALTURA_TELA;
        move(a, l);
        echochar('+');
        
    } 
    for (a=0; a<ALTURA_TELA; a++){
        l = LARGURA_TELA;
        //desenha em l = largura_tela
        move(a, l);
        echochar('+');
    }       
    
    // Atualiza tela
    refresh();

}

void Tela::update() {
  int x, y;

  //para deixar do tamanho do terminal, trocar "altura_tela" por essa
  //int altura, largura;
  //altura = getmaxy(stdscr);
  //largura = getmaxx(stdscr);

  //Apaga o Alvo antigo
  Alvo *alvo_antigo = this->alvo_antigo;
  y = (int) alvo_antigo->get_posicao_y() * (this->maxI / this->maxX);
  x = (int) alvo_antigo->get_posicao_x();
  move(y, x);   /* Move cursor to position */
  if (y<ALTURA_TELA && y>0){
      echochar(' ');  /* Prints character, advances a position */
  }
  // Desenha o alvo na tela
  Alvo *alvo = this->alvo;
  y = (int) alvo->get_posicao_y() * (this->maxI / this->maxX);
  x = (int) alvo->get_posicao_x();
  move(y, x);   /* Move cursor to position */
  if (y<ALTURA_TELA && y>0){
      echochar('O');  /* Prints character, advances a position */
  }
  // Atualiza o alvo antigo
  alvo_antigo->update(alvo->get_posicao_x(), alvo->get_posicao_y());
  

  std::vector<Nave *> *naves_old = this->lista_anterior_nave->get_naves();

  std::vector<Tiro *> *tiros_old = this->lista_anterior_tiro->get_tiros();

  // Apaga tiros na tela
  for (int k=0; k<tiros_old->size(); k++){
     y = (int) ((*tiros_old)[k]->get_posicao_y()) * (this->maxI / this->maxX);
     x = (int) ((*tiros_old)[k]->get_posicao_x()) * (this->maxI / this->maxX);
     move(y, x);   /* Move cursor to position */
     if (x<LARGURA_TELA && x>0){
         echochar(' ');  /* Prints character, advances a position */
     }
  }

  // Desenha tiros na tela
  std::vector<Tiro *> *tiros = this->lista_tiro->get_tiros();

  for (int k=0; k<tiros->size(); k++){
    if((*tiros)[k]->get_existe() == 1){
       y = (int) ((*tiros)[k]->get_posicao_y()) * (this->maxI / this->maxX);
       x = (int) ((*tiros)[k]->get_posicao_x()) * (this->maxI / this->maxX);
       move(y, x);   /* Move cursor to position */
         if (x<LARGURA_TELA && x>0){
            echochar('*');  /* Prints character, advances a position */
       }

      // Atualiza tiros antigos
      (*tiros_old)[k]->update((*tiros)[k]->get_posicao_x(), (*tiros)[k]->get_posicao_y(), (*tiros)[k]->get_velocidade());
    }
  }

  // Apaga a nave na tela
  for (int k=0; k<naves_old->size(); k++){
     y = (int) ((*naves_old)[k]->get_posicao()) * (this->maxI / this->maxX);
     x = POSICAO_X_NAVE;
     move(y, x);   /* Move cursor to position */
     if (y<ALTURA_TELA && y>0){
         echochar(' ');  /* Prints character, advances a position */
     }
  }

  // Desenha a nave na tela
  std::vector<Nave *> *naves = this->lista_nave->get_naves();

  for (int k=0; k<naves->size(); k++){
     y = (int) ((*naves)[k]->get_posicao()) * (this->maxI / this->maxX);
     x = POSICAO_X_NAVE;
     move(y, x);   /* Move cursor to position */
     if (y<ALTURA_TELA && y>0){
         echochar('>');  /* Prints character, advances a position */
     }

    // Atualiza a nave antiga
    (*naves_old)[k]->update((*naves)[k]->get_posicao());
  }
 

  // Atualizando os pontos e o restante de tiros
  int out_tiros = tiros->size()-total_tiros;
  move(ALTURA_TELA+1, 1);
  printw("%d Tiros Restantes / 0 Pontos", out_tiros);

  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}

void threadfun (char *keybuffer, int *control)
{
  char c;
  while ((*control) == 1) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
    else (*keybuffer) = 0;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses
  raw();				         /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
	noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */

  this->rodando = 1;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}
