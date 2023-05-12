/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CrosswordsBoard.cpp
 * Author: adi
 * 
 * Created on 23 December 2017, 19:02
 */

#include "CrosswordsBoard.h"

atomic_int Board::best_score(0);
int Board::scoring = 0;
int Board::minscore = -1;
