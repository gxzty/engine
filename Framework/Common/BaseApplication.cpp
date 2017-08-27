#include "BaseApplication.hpp"

// Parse command line, read configuration, initialize all sub modules

int My::BaseApplication::Initialize() {
  m_bQuit = false;
  return 0;
}

void My::BaseApplication::Finalize() {

}

void My::BaseApplication::Tick(){
}

bool My::BaseApplication::IsQuit(){
  return m_bQuit;
}
