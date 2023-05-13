/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   move.h
 * Author: adi
 *
 * Created on September 8, 2017, 11:32 AM
 */

#ifndef MOVE_H
#define MOVE_H

#include <string>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

struct Move {
  friend std::ostream &operator<<(std::ostream &, const Move &); 

    int slotX; // 0 .. 12
    int slotY; // 0 .. 12
    int direction; // horizontal (false) or vertical (true)
    int wordId; // any word in the word list
    string word; // the word whose id is given in field wordId
};

  inline  std::ostream &operator<<(std::ostream &os, const Move &m){
    os<<"slotX "<<m.slotX<<" slotY "<<m.slotY<<" direction "<<m.direction
      <<" wordid "<<m.wordId<<" word "<< m.word <<endl;
    return os; 
  }


#ifdef __cplusplus
}
#endif

#endif /* MOVE_H */

