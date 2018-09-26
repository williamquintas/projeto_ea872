#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <ncurses.h>

#include "oo_model.hpp"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>   

#define VELOCIDADE_TIRO 3
#define ACELERACAO_TIRO 3
#define ALTURA_TELA 20
#define LARGURA_TELA 40
#define POSICAO_X_NAVE 3

int total_tiros = 0;
int pontos = 0;

using namespace Audio;
using namespace std::chrono;

Sample::Sample() {

}

Sample::~Sample() {

}

bool Sample::finished() {
  if ( (this->position) >= (this->data.size())) return true;
  else return false;
}

void Sample::load(const char *filename) {
  std::string buffer;
  float fdata;
  std::ifstream input_file;
  unsigned int count = 0;

  input_file.open(filename, std::ios_base::in);
  if (!input_file) {
    std::cerr << "Arquivo " << filename << " nao encontrado" << std::endl;
    return;
  }

  while (std::getline(input_file, buffer) ) {
    std::stringstream(buffer) >> fdata;
    (this->data).push_back(fdata);
    count ++;
  }
  this->position = 0;

}

unsigned int Sample::get_position() {
  return this->position;
}

void Sample::set_position(unsigned int pos) {
  this->position = pos;
}

std::vector<float> Sample::get_data() {
  return this->data;
}

Player::Player() {
  this->playing = false;
  this->audio_sample = NULL;
}

void Player::pause() {
  this->playing = false;
}

Player::~Player() {

}

Sample *Player::get_data() {
  return this->audio_sample;
}


int mix_and_play (const void *inputBuffer, void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData )

{
  Player *player = (Player*) userData;
  float *buffer = (float *) outputBuffer;
  Sample *s;
  s = player->get_data();
  if (s != NULL) {
    std::vector<float> data = s->get_data();
    unsigned int pos = s->get_position();

    // Fill the buffer with samples!
    for (int i=0; (i<framesPerBuffer); i++) {
      if (pos < data.size())
        buffer[i] = data[pos];
      else
        buffer[i] = 0;
      pos+=2;
    }
    s->set_position(pos);
  }
  return 0;
}

void Player::play(Sample *audiosample) {
  this->audio_sample = audiosample;
}

void Player::init() {
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) {
    std::cerr << "Error on Pa_Initialize()" << std::endl;
    return;
  }

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
  if (outputParameters.device == paNoDevice) {
    std::cerr << "Error: No default output device on Pa_GetDefaultOutputDevice()" << std::endl;
    return;
  }

  outputParameters.channelCount = 1;                     /* Mono output. */
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream( &stream,
                         NULL,      /* No input. */
                         &outputParameters,
                         44100,
                         64,       /* Frames per buffer. */
                         paClipOff, /* We won't output out of range samples so don't bother clipping them. */
                         mix_and_play,
                         this );

  if( err != paNoError ) {
    std::cerr << "Error on Pa_OpenStream()" << std::endl;
    return;
  }

  err = Pa_StartStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StartStream()" << std::endl;
    return;
  }


}

void Player::stop() {
  PaError err;
  err = Pa_StopStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  err = Pa_CloseStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  Pa_Terminate();
}

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
Alvo::Alvo(float posicao_x, float posicao_y) {
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
Fisica::Fisica(Alvo *alvo, ListaDeNaves *ldn, ListaDeTiros *ldt) {
  this->alvo = alvo;
  this->lista_nave = ldn;
  this->lista_tiro = ldt;
  this->asample = new Audio::Sample();
  this->asample->load("assets/explosion.dat");
  this->player = new Audio::Player();
  this->player->init();
  //Espera
  while (1) {
    int t1, t0;
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    t1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    if (t1-t0 > 500) break;
  }
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
  this->asample->set_position(0);
  this->player->play(this->asample);    
}


void Fisica::update_tiro(float deltaT) {
  // Atualiza parametros dos tiros!
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  for (int i = 0; i < (*t).size(); i++) {
    float new_vel = (*t)[i]->get_velocidade() + (float)deltaT * (ACELERACAO_TIRO)/1000;
    float new_pos = (*t)[i]->get_posicao_x() + (float)deltaT * new_vel/1000;
    if (
      new_pos <= this->alvo->get_posicao_x() + 0.5 && 
      new_pos >= this->alvo->get_posicao_x() - 0.5 && 
      (*t)[i]-> get_posicao_y() <= this->alvo->get_posicao_y() + 0.5 &&
      (*t)[i]-> get_posicao_y() >= this->alvo->get_posicao_y() - 0.5
      ) {
        this->destruir_tiro(i);
        srand (time(NULL));
        this->alvo->update((float)(rand() % (LARGURA_TELA/2) + LARGURA_TELA/2), (float)(rand() % (ALTURA_TELA - 1)) + 1.0);
        pontos++;
    }
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
  printw("%d Tiros Restantes / %d Pontos", out_tiros, pontos);

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
