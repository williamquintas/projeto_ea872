#include "fisica.hpp"
#include <thread>

using namespace std::chrono;

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

void Fisica::disparar_tiro(int i_tiro, int *total_tiros){
  (*total_tiros)++;
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

void Fisica::update_tiro(float deltaT, int *pontos) {
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
        (*pontos)++;
    }
    (*t)[i]->update(new_pos, (*t)[i]->get_posicao_y(), new_vel);
  }
}
