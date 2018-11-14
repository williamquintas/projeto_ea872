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

void Fisica::andar_nave(int deslocamento, int i_nave) {
  std::vector<Nave *> *n = this->lista_nave->get_naves();

  //Naves na vertical
  if (i_nave == 0 || i_nave == 1){
    if ((*n)[i_nave]->get_posicao() + deslocamento > MIN_Y && (*n)[i_nave]->get_posicao() + deslocamento < MAX_Y) {
      (*n)[i_nave]->update((*n)[i_nave]->get_posicao() + deslocamento);
    } 
  }

  //Naves na horizontal
  if (i_nave == 2 || i_nave == 3){
    if ((*n)[i_nave]->get_posicao() + deslocamento > MIN_X && (*n)[i_nave]->get_posicao() + deslocamento < MAX_X) {
      (*n)[i_nave]->update((*n)[i_nave]->get_posicao() + deslocamento);
    } 
  }

}

void Fisica::disparar_tiro(int i_tiro, int *total_tiros, int quem_atirou){
  (*total_tiros)++;
  std::vector<Nave *> *n = this->lista_nave->get_naves();
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  (*t)[i_tiro]->update_existe(1);
  (*t)[i_tiro]->update_quem_atirou(quem_atirou);
  switch (quem_atirou){
    case 0:
    default:
      (*t)[i_tiro]->update(POSICAO_X_NAVE_0+2, (*n)[0]->get_posicao(), VELOCIDADE_TIRO);
      break;
    case 1:
      (*t)[i_tiro]->update(POSICAO_X_NAVE_1-2, (*n)[1]->get_posicao(), VELOCIDADE_TIRO);
      break;
    case 2:
      (*t)[i_tiro]->update((*n)[2]->get_posicao(), POSICAO_Y_NAVE_2+1, VELOCIDADE_TIRO);
      break;
    case 3:
      (*t)[i_tiro]->update((*n)[3]->get_posicao(), POSICAO_Y_NAVE_3-1, VELOCIDADE_TIRO);
      break;
  }
  
}

void Fisica::destruir_tiro(int i_tiro){
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  (*t)[i_tiro]->update_existe(0);
  this->asample->set_position(0);
  this->player->play(this->asample);    
}

void Fisica::update_tiro(float deltaT, int *pontos1, int *pontos2, int *pontos3, int *pontos4) {
  // Atualiza parametros dos tiros!
  std::vector<Tiro *> *t = this->lista_tiro->get_tiros();
  for (int i = 0; i < (*t).size(); i++) {
    float new_vel = (*t)[i]->get_velocidade() + (float)deltaT * (ACELERACAO_TIRO)/1000;
    float new_pos;
    switch ((*t)[i]->get_quem_atirou())
    {
      case 0:
      default:
        new_pos = (*t)[i]->get_posicao_x() + (float)deltaT * new_vel/1000;
        (*t)[i]->update(new_pos, (*t)[i]->get_posicao_y(), new_vel);
        break;
      case 1:
        new_pos = (*t)[i]->get_posicao_x() - (float)deltaT * new_vel/1000;
        (*t)[i]->update(new_pos, (*t)[i]->get_posicao_y(), new_vel);
        break;
      case 2:
        new_pos = (*t)[i]->get_posicao_y() + (float)deltaT * new_vel/1000;
        (*t)[i]->update((*t)[i]->get_posicao_x(), new_pos, new_vel);
        break;
      case 3:
        new_pos = (*t)[i]->get_posicao_y() - (float)deltaT * new_vel/1000;
        (*t)[i]->update((*t)[i]->get_posicao_x(), new_pos, new_vel);
        break;
    }
    if (
      (((*t)[i]->get_quem_atirou() == 0) ||
      (*t)[i]->get_quem_atirou() == 1) &&
      new_pos <= this->alvo->get_posicao_x() + 0.5 && 
      new_pos >= this->alvo->get_posicao_x() - 0.5 && 
      (*t)[i]-> get_posicao_y() <= this->alvo->get_posicao_y() + 0.5 &&
      (*t)[i]-> get_posicao_y() >= this->alvo->get_posicao_y() - 0.5
      ) {
        this->destruir_tiro(i);
        srand (time(NULL));
        this->alvo->update((float)(rand() % (LARGURA_TELA - 5) + 3.0), (float)(rand() % (ALTURA_TELA - 5)) + 3.0);
        if ((*t)[i]->get_quem_atirou() == 0)  (*pontos1)++;
        else (*pontos2)++;
    }
    else if (
      (((*t)[i]->get_quem_atirou() == 2) ||
      (*t)[i]->get_quem_atirou() == 3) &&
      new_pos <= this->alvo->get_posicao_y() + 0.5 && 
      new_pos >= this->alvo->get_posicao_y() - 0.5 && 
      (*t)[i]-> get_posicao_x() <= this->alvo->get_posicao_x() + 0.5 &&
      (*t)[i]-> get_posicao_x() >= this->alvo->get_posicao_x() - 0.5
      ) {
        this->destruir_tiro(i);
        srand (time(NULL));
        this->alvo->update((float)(rand() % (LARGURA_TELA - 5) + 3.0), (float)(rand() % (ALTURA_TELA - 5)) + 3.0);
        if ((*t)[i]->get_quem_atirou() == 2)  (*pontos3)++;
        else (*pontos4)++;
    }
  }
}
