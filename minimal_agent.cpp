#include <iostream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include <fstream>
#include <string.h>
#include <sstream>
// Global vars
const int maxSteps = 7500;
int lastLives;
float totalReward;
ALEInterface alei;

///////////////////////////////////////////////////////////////////////////////
/// Get info from RAM
///////////////////////////////////////////////////////////////////////////////
void cls(){std::printf("\033[H\033[J");}
void showRAM(){
   auto* RAM{alei.getRAM().array()};
   std::size_t i{};
   std::printf("\033[H");
   std::printf("\nADDR || 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
   std::printf("\n=======================================================\n");
   for(std::uint16_t row{}; row < 8; ++row){
      std::printf("  %02X || ",16*row);
      for(std::uint16_t col{};col < 16; ++col,++i){
         std::printf("%02X ",RAM[i]);
      }
      std::puts("");
   }
   std::printf("\n=======================================================");
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
std::string agentStep(std::string lastline) {
   static int wide = 9;
   float reward = 0;
   auto* RAM{alei.getRAM().array()};
   //positions to save in csv file
   std::stringstream ss;
   std::string s;
   ss << 1 << ";" <<(float)RAM[6] <<";"<< (float)RAM[85] <<";"<< (float)RAM[26] <<";"<< (float)RAM[81] <<";"<< (float)RAM[82] 
            << ";"<< (float)RAM[83] <<";"<< (float)RAM[84] <<";"<< (float)RAM[30] <<";"<< (float)RAM[28]<<";";

   auto* KEYS = SDL_GetKeyState(nullptr);
   if(KEYS[SDLK_SPACE] && KEYS[SDLK_RIGHT]){
      ss << 1 << ";" << 1 << ";" << 0;
      alei.act(PLAYER_A_RIGHTFIRE);
   }else
   if(KEYS[SDLK_SPACE] && KEYS[SDLK_LEFT]){
      ss << 1 << ";" << 0 << ";" << 1;
      alei.act(PLAYER_A_LEFTFIRE);
   }else
   if(KEYS[SDLK_SPACE]){
      ss << 1 << ";" << 0 << ";" << 0;
      alei.act(PLAYER_A_FIRE);
   }else
   if(KEYS[SDLK_RIGHT]){
      ss << 0 << ";" << 1 << ";" << 0;
      alei.act(PLAYER_A_RIGHT);
   }else if(KEYS[SDLK_LEFT]){
      ss << 0 << ";" << 0 << ";" << 1;
      alei.act(PLAYER_A_LEFT);
   }else{
      ss << 0 << ";" << 0 << ";" << 0;
      alei.act(PLAYER_A_NOOP);
   }
   s = ss.str();
   if(s.compare(lastline) != 0){
      //saving positions in csv file
      std::ofstream my_file;
      my_file.open("data.csv",std::ios::app);
      if(!my_file){
         std::printf("file not found\n");
         return(lastline);
      }else{
         my_file << ss.rdbuf();;
         my_file << "\n";
         return(s);
      }
   }else{
      return(lastline);
   }
}

///////////////////////////////////////////////////////////////////////////////
/// Print usage and exit
///////////////////////////////////////////////////////////////////////////////
void usage(char* pname) {
   std::cerr
      << "\nUSAGE:\n" 
      << "   " << pname << " <romfile>\n";
   exit(-1);
}

///////////////////////////////////////////////////////////////////////////////
/// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

   // Check input parameter
   if (argc != 2)
      usage(argv[0]);

   // Create alei object.
   alei.setInt("random_seed", 0);
   alei.setFloat("repeat_action_probability", 0);
   alei.setBool("display_screen", true);
   alei.setBool("sound", false);
   alei.loadROM(argv[1]);
   std::string last_line = "";
   // Init
   srand(time(NULL));
   lastLives = alei.lives();
   totalReward = .0f;

   // Main loop
   alei.act(PLAYER_A_FIRE);
   int step;
   std::ofstream ofs;
   ofs.open("data.csv", std::ofstream::out | std::ofstream::trunc);
   ofs.close();
   cls();
   for (step = 0; 
        !alei.game_over() && step < maxSteps; 
        ++step) 
   {
      //showRAM();
      last_line = agentStep(last_line);
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}
