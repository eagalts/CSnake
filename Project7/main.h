#pragma once


void change_direction(); //Отслеживание нажатой клавиши и изменение направления

void next_step(); //Отслеживает шаг змейки

void place_food(); //расстановка фруктов рандомно

void show_table(); //Расчертить поле игрока

void clear_snake_on_table(); //Убираем змейку после проигрыша

void check_coordination(); //проверка координат 

void show_snake_on_table(); //показываем змейку

void standart_settings(); // стандартные настройки

int game_over(); //змейка скушала сама себя

int food_check(); //проверяем еду