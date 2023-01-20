#include <iostream>
#include <cmath>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include "Matrix.hpp"
#include "RANet.hpp"

// Global vars
const int maxSteps = 7500;
int lastLives;
float totalReward;
ALEInterface alei;

void play(){
   auto* RAM{alei.getRAM().array()};
   Matrix X = {{1, (float)RAM[6], (float)RAM[85], (float)RAM[26],(float)RAM[81], (float)RAM[82], (float)RAM[83],(float)RAM[84] ,(float)RAM[30], (float)RAM[28]}};
   RANet net({9, 9, 3});   
   net.setWeights(Matrix{{0.0536307, -0.981177, 0.282775, 0.385805, 0.244212, 0.0933811, -0.867564, -0.576487, 0.485852},
                         {-0.350331, 0.825374, 0.794923, 0.516568, 0.241684, 0.574206, -0.298815, 0.409221, -0.571367 },
                         {0.182976, 0.944162, 1.34004, 0.958971, -0.28126, 1.21449, 0.868603, 0.434293, -1.2118 },
                         {0.569939, 0.0136375, 1.15428, 0.385617, -0.843738, 0.97306, -0.830006, 0.0297179, -1.11627 },
                         {0.877926, 0.985363, 0.527592, 0.2726, 0.967689, 0.190643, 0.895818, 1.09408, 0.367316 },
                         {1.33285, -0.753581, 0.808112, -0.909632, -0.950731, 1.33102, 0.190099, 0.0127621, -1.40483 },
                         {-0.270859, 0.446752, -0.166883, 0.0394374, -0.490882, 0.161609, 0.369026, 1.05562, -0.90773 },
                         {0.755162, -0.627145, 1.22385, 0.471945, -0.0190783, 0.63987, 0.491441, 0.803908, -1.30744 },
                         {1.27804, 0.445254, 1.59051, 0.790204, -0.679947, 1.18392, 0.646191, 1.07922, -2.02426 },
                         {-0.118234, -0.38124, 0.852508, 0.723987, -0.654434, 0.15262, -0.28483, -0.570772, 0.185885 }}, 0);
   net.setWeights(Matrix{{2.66185, 2.66862 ,-1.0124 },
                         {3.25068, -1.23334, 0.210561 },
                         {2.34496, 2.10896 ,-0.414738} ,
                         {2.21755, 0.900531, -0.561325} ,
                         {2.33518, 1.74002 ,0.188403},
                         {-2.67396, -2.38764, 1.42443} ,
                         {2.29816, 1.8997 ,-1.35072}, 
                         {3.61868, 1.49309, 0.387046}, 
                         {2.83195, 2.30407, -1.11569}, 
                         {-2.48127, -0.807404, 1.056}}, 1);

   auto Y = net.predict(X);
   std::cout << Y << "\n";
   if(Y[0,0] == 1 && Y[0,1] == 1){
      alei.act(PLAYER_A_RIGHTFIRE);
   }else
   if(Y[0,0] == 1 && Y[0,2] == 1){
      alei.act(PLAYER_A_LEFTFIRE);
   }else
   if(Y[0,0] == 1){
      alei.act(PLAYER_A_FIRE);
   }else
   if(Y[0,1] == 1){
      alei.act(PLAYER_A_RIGHT);
   }else if(Y[0,2] == 1){
      alei.act(PLAYER_A_LEFT);
   }else{
      alei.act(PLAYER_A_NOOP);
   }

}

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
      play();
      //showRAM();
      //last_line = agentStep(last_line);
   }

   std::cout << "Steps: " << step << std::endl;
   std::cout << "Reward: " << totalReward << std::endl;

   return 0;
}
