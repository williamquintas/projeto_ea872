#include "tela.hpp"
#include <ncurses.h>

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

void Tela::update(int *total_tiros, int *pontos) {
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
     if (k==0){
        y = (int) ((*naves_old)[k]->get_posicao()) * (this->maxI / this->maxX);
        x = POSICAO_X_NAVE_0;
        move(y, x);   /* Move cursor to position */
        if (y<ALTURA_TELA && y>0){
            echochar(' ');  /* Prints character, advances a position */
        }
     }

     if (k==1){
        y = (int) ((*naves_old)[k]->get_posicao()) * (this->maxI / this->maxX);
        x = POSICAO_X_NAVE_1;
        move(y, x);
        if (y<ALTURA_TELA && y>0){
            echochar(' ');
        }
     }
  }
  // Desenha a nave na tela
  std::vector<Nave *> *naves = this->lista_nave->get_naves();
  for (int k=0; k<naves->size(); k++){
     if(k==0){
        y = (int) ((*naves)[k]->get_posicao()) * (this->maxI / this->maxX);
        x = POSICAO_X_NAVE_0;
        move(y, x);   /* Move cursor to position */
        if (y<ALTURA_TELA && y>0){
            echochar('>');  /* Prints character, advances a position */
        }
        // Atualiza a nave antiga
       (*naves_old)[k]->update((*naves)[k]->get_posicao());
     }

     if(k==1){
        y = (int) ((*naves)[k]->get_posicao()) * (this->maxI / this->maxX);
        x = POSICAO_X_NAVE_1;
        move(y, x);
        if (y<ALTURA_TELA && y>0){
            echochar('<');
        }
        // Atualiza a nave antiga
       (*naves_old)[k]->update((*naves)[k]->get_posicao());
     }
  }



  // Atualizando os pontos e o restante de tiros
  int out_tiros = tiros->size()-(*total_tiros);
  move(ALTURA_TELA+1, 1);
  printw("%d Tiros Restantes / %d Pontos", out_tiros, (*pontos));
  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}
