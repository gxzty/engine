#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "Interface.hpp"

namespace My {
  Interface IRuntimeModule {
public:
  virtual ~IRuntimeModule() {};
  virtual int Initialize() = 0;
  virtual void Finalize() = 0;
  virtual void Tick() = 0;
  };
}


#endif  // __INTERFACE_H__
