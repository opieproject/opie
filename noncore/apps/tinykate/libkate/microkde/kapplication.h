#ifndef MINIKDE_KAPPLICATION_H
#define MINIKDE_KAPPLICATION_H

class KApplication
{
  public:
    static int random();
    static int cursorFlashTime() { return 1000; }
};

#endif
