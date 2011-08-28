#ifndef Chronometer_H
#define Chronometer_H

#ifdef WIN32
    #include <Windows.h>
    #include <Winbase.h>
#else
    #include <sys/time.h>
#endif
#include <vector>


struct Chronometer {

#ifdef WIN32
    static LARGE_INTEGER m_freq;
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_end;
#else
    timeval m_start;
    timeval m_end;
#endif

    float m_lapse;	
    std::string m_label;

    int m_counter;

    Chronometer (const std::string& label);
    static void init();

    void reset();
    void start();
    void stop();
    void lapse();
  
//    void output(class view* view, rgUint16 x, rgUint16 y);
};

struct Chronos 
{
    std::vector<Chronometer*> m_Swatches;

    static Chronos& Singleton();
    ~Chronos();
    Chronometer* add(const std::string& label);
    Chronometer* get(const std::string& label);
    //void Output(class view* view, rgUint16 x, rgUint16 y);
private:
    Chronos(){;}
};


#endif