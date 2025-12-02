#pragma once

//les bits sont en base 16 et on veut activer seulement 1 bit (en base 2)
//NAME, BITS (base 16), (base 2)
//chaque bit représente un flag
#define WORLD_STATIC 0x0001     //0000 0000 0000 0001
#define WORLD_DYNAMIC 0x0002    //0000 0000 0000 0010
#define PLAYER 0x0004           //0000 0000 0000 0100
#define AI_CHARACTER 0x0008     //0000 0000 0000 1000
#define PARTICLE 0x0010         //0000 0000 0001 0000

#define ALL 0xFFFF              //1111 1111 1111 1111